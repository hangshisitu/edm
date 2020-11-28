package cn.edm.web.action;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.app.jms.NotifyMessageProducer;
import cn.edm.app.logger.FormLogger;
import cn.edm.consts.Config;
import cn.edm.consts.Mail;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Form;
import cn.edm.model.Prop;
import cn.edm.model.Recipient;
import cn.edm.model.Tag;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.service.EmailService;
import cn.edm.service.FormService;
import cn.edm.service.PropService;
import cn.edm.service.TagService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Concurrencys;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.facade.Forms;
import cn.edm.web.facade.Recipients;

import com.google.common.collect.Maps;

@Controller("Form")
@RequestMapping("/form/**")
public class FormAction {

    private static final Logger logger = LoggerFactory.getLogger(FormAction.class);

    @Autowired
    private EmailService emailService;
    @Autowired
    private FormService formService;
    @Autowired
    private PropService propService;
    @Autowired
    private TagService tagService;

    @Autowired
    private Ehcache ehcache;
    
    private NotifyMessageProducer producer;
    
    @Autowired(required = false)
    public void setProducer(NotifyMessageProducer producer) {
        this.producer = producer;
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String input(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            Integer id = Https.getInt(request, "id", R.CLEAN, R.REQUIRED, R.INTEGER);
            Asserts.isNull(id, "表单");
            String r = Https.getStr(request, "r", R.CLEAN, R.REQUIRED, R.LENGTH, "{32,32}", "验证码");
            String email = Https.getStr(request, "m", R.CLEAN, R.MAIL);
            
            Form form = formService.get(id, r);
            Asserts.isNull(form, "表单");
            Asserts.notExists(Webs.rootPath(), form.getInputPath(), "表单填写界面");
            
            Document doc = Jsoup.parse(Files.get(Webs.rootPath(), form.getInputPath()));
            if (StringUtils.isNotBlank(email)) {
                doc.select("input[name=email]").attr("value", email);
            }
            Modulars.filter(doc);
            ObjectMappers.renderHtml(response, XSS.safety4(doc).toString());
            return null;
        } catch (Exception e) {
            logger.error("(Form:input) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.POST)
    public String submit(HttpServletRequest request, HttpServletResponse response) {
        Counter counter = null;
        Form form = null;
        
        try {
            Integer id = Https.getInt(request, "id", R.CLEAN, R.REQUIRED, R.INTEGER);
            Asserts.isNull(id, "表单");
            String r = Https.getStr(request, "r", R.CLEAN, R.REQUIRED, R.LENGTH, "{32,32}", "验证码");
            form = formService.get(id, r);
            Asserts.isNull(form, "表单");
            Asserts.notExists(Webs.rootPath(), form.getSubmitPath(), "表单提交界面");
            
            int confirm = Values.get(form.getEmailConfirm(), Value.F);
            int prompt = Values.get(form.getEmailPrompt(), Value.F);
            
            Map<String, Object> paramMap = Servlets.getParametersStartingWith(request, "");
            Asserts.isEmpty(paramMap, "表单填写");

            List<Prop> propList = propService.find(form.getCorpId());
            Asserts.isEmpty(propList, "属性");
            Map<String, Prop> propMap = Maps.newHashMap();
            for (Prop prop : propList) {
                propMap.put(prop.getPropName(), prop);
            }
            
            StringBuffer sbff = new StringBuffer();
            String email = null;
            boolean hasParameter = false;
            for (Entry<String, Object> entry : paramMap.entrySet()) {
                String key = entry.getKey();
                String value = (String) entry.getValue();
                if (StringUtils.isBlank(key) || StringUtils.isBlank(value))
                    continue;
                Prop prop = propMap.get(key);
                if (prop == null)
                    continue;
                if (prop.getPropName().equals(PropMap.EMAIL.getId())) {
                    email = (String) Validator.validate(value, R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,60}");
                    continue;
                }
                if (Imports.isParameter(prop, value)) {
                    sbff.append(prop.getPropName()).append("=").append(Values.get(value)).append(",");
                    hasParameter = true;
                }
            }
            
            if (StringUtils.isNotBlank(email)) {
                if (confirm == Value.T) {
                    Asserts.notExists(Webs.rootPath(), form.getConfirmPath(), "邮箱验证界面");
                    FormLogger.ok(email, sbff.toString());
                    confirmNotify(form, email);
                } else {
                    Tag tag = tagService.get(form.getCorpId(), form.getUserId(), form.getTagId());
                    Asserts.isNull(tag, "标签");

                    Recipient recipient = new Recipient();
                    recipient.setCorpId(form.getCorpId());
                    recipient.setEmail(email);
                    recipient.setParameterList(Values.get(recipient.getParameterList()) + Values.get(sbff.toString()));
                    
                    counter = Concurrencys.getCounter(ehcache, false, form.getUserId());
                    emailService.save(tag, recipient, hasParameter);
                    counter.setCode(Counter.COMPLETED);
                    
                    if (prompt == Value.T) {
                        Asserts.notExists(Webs.rootPath(), form.getPromptPath(), "邮件提示界面");
                        Map<String, String> parameterMap = Maps.newHashMap();
                        Recipients.parameterMap(parameterMap, emailService.get(tag, email, true));
                        promptNotify(form, email, parameterMap);
                    }
                }
            }
            Document doc = Jsoup.parse(Files.get(Webs.rootPath(), form.getSubmitPath()));
            Modulars.filter(doc);
            ObjectMappers.renderHtml(response, XSS.safety4(doc).toString());
            return null;
        } catch (Exception e) {
            logger.error("(Form:submit) error: ", e);
            if (form != null) {
                Concurrencys.clearCounter(ehcache, true, form.getUserId());                
            }
            return Views._404();
        } finally {
            if (form != null) {
                Concurrencys.clearCounter(ehcache, false, form.getUserId());
            }
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String confirm(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        Counter counter = null;
        try {
            Integer id = Https.getInt(request, "id", R.CLEAN, R.REQUIRED, R.INTEGER);
            Asserts.isNull(id, "表单");
            String r = Https.getStr(request, "r", R.CLEAN, R.REQUIRED, R.LENGTH, "{32,32}", "验证码");
            Form form = formService.get(id, r);
            Asserts.isNull(form, "表单");

            int prompt = Values.get(form.getEmailPrompt(), Value.F);
            
            String email = Https.getStr(request, "e", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "e");
            email = new String(Encodes.decodeBase64(email));
            email = (String) Validator.validate(email, R.REQUIRED, R.MAIL, "{1,64}", "邮箱");
            
            Tag tag = tagService.get(form.getCorpId(), form.getUserId(), form.getTagId());
            Asserts.isNull(tag, "标签");

            Map<String, String> recipientMap = Forms.recipient(email);
            String mailbox = recipientMap.get("email");
            logger.error("[ mailbox ] : 表单二次验证的收件人 "+mailbox.toString());
            String parameter_list = recipientMap.get("parameter_list");
            
            Recipient recipient = new Recipient();
            recipient.setCorpId(form.getCorpId());
            recipient.setEmail(mailbox);
            recipient.setParameterList(Values.get(parameter_list));

            boolean hasParameter = false;
            if (StringUtils.isNotBlank(parameter_list)) {
                hasParameter = true;
            }
            
            counter = Concurrencys.getCounter(ehcache, false, form.getUserId());
            emailService.save(tag, recipient, hasParameter);
            counter.setCode(Counter.COMPLETED);
            
            if (prompt == Value.T) {
                Asserts.notExists(Webs.rootPath(), form.getPromptPath(), "邮件提示界面");
                Map<String, String> parameterMap = Maps.newHashMap();
                Recipients.parameterMap(parameterMap, emailService.get(tag, email, true));
                promptNotify(form, email, parameterMap);
            }
            
            return "confirm";
        } catch (Exception e) {
            logger.error("(Form:confirm) error: ", e);
            return Views._404();
        }
    }
    
    private void confirmNotify(Form form, String email) {
        if (producer != null) {
            try {
                Map<String, Object> map = Maps.newHashMap();
                map.put(Mail.TEMPLATE_FILE_NAME, form.getConfirmPath());
                map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
                map.put(Mail.TO, email);
                map.put(Mail.SUBJECT, form.getConfirmSubject());
                map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
                map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(email.getBytes()));
                producer.sendTopic(map);
            } catch (Exception e) {
                throw new Errors("(Form:confirmNotify) error: ", e);
            }
        }
    }
    
    private void promptNotify(Form form, String email, Map<String, String> parameterMap) {
        if (producer != null) {
            try {
                Map<String, Object> map = Maps.newHashMap();
                map.put(Mail.TEMPLATE_FILE_NAME, form.getPromptPath());
                map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
                map.put(Mail.TO, email);
                map.put(Mail.SUBJECT, form.getPromptSubject());
                map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
                map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(email.getBytes()));
                for (Entry<String, String> entry : parameterMap.entrySet()) {
                    if (StringUtils.isBlank(entry.getKey())) continue;
                    if (map.containsKey(entry.getKey())) continue;
                    map.put(entry.getKey(), entry.getValue());
                }
                producer.sendTopic(map);
            } catch (Exception e) {
                throw new Errors("(Form:promptNotify) error: ", e);
            }
        }
    }
}
