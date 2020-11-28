package cn.edm.web.action.datasource;

import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.safety.Whitelist;
import org.jsoup.select.Elements;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Id;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.FormMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Category;
import cn.edm.model.Corp;
import cn.edm.model.Form;
import cn.edm.model.Prop;
import cn.edm.model.Tag;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Randoms;
import cn.edm.utils.Asserts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Readers;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Safety;
import cn.edm.utils.web.Safety4;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.api.Snatchs;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Forms;
import cn.edm.web.facade.Templates;

@Controller
@RequestMapping("/datasource/form/**")
public class FormAction extends Action {
    
    private static final Logger logger = LoggerFactory.getLogger(FormAction.class);

    @RequestMapping(value = "page", method = RequestMethod.GET)
    public String page(HttpServletRequest request, ModelMap map) {
        try {
            Page<Form> page = new Page<Form>();
            Pages.page(request, page);

            String formName = StreamToString.getStringByUTF8(Https.getStr(request, "formName", R.CLEAN, R.LENGTH, "{1,20}"));
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);

            MapBean mb = new MapBean();
            page = formService.search(page, mb, UserSession.getCorpId(), formName, beginTime, endTime);

            map.put("page", page);
            map.put("mb", mb);

            return "default/datasource/form_page";
        } catch (Exception e) {
            logger.error("(Form:page) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "add", method = RequestMethod.GET)
    public String add(HttpServletRequest request, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/form/preview", token);
            CSRF.generate(request, "/datasource/form/recommend", token);

            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER);

            Form form = null;

            if (formId != null) {
                form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            }
            if (form == null) {
                form = new Form();
            }

            Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.FORM.getName(), Category.TAG);
            Asserts.isNull(category, "类别");
            List<Tag> tagList = tagService.find(null,category.getCategoryId());
            
            map.put("form", form);
            map.put("category", category);
            map.put("tagList", tagList);

            return "default/datasource/form_add";
        } catch (Exception e) {
            logger.error("(Form:add) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "add", method = RequestMethod.POST)
    public void add(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer formId = null;
        
        try {
            CSRF.validate(request);
            
            formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER);
            String formName = Https.getStr(request, "formName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "表单名称");
            String formDesc = Https.getStr(request, "formDesc", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}", "表单用途");
            Integer type = Https.getInt(request, "type", R.CLEAN, R.REQUIRED, R.INTEGER, "表单类型");
            Integer tagId = Https.getInt(request, "tagId", R.REQUIRED, "数据储存");

            Integer REQUIRED = R.REQUIRED;
            if (type.equals(FormMap.SUBSCRIBE.getId())) {
                REQUIRED = R.CLEAN;
            }
            Integer emailConfirm = Https.getInt(request, "emailConfirm", R.CLEAN, REQUIRED, "邮件验证");
            Integer emailPrompt = Https.getInt(request, "emailPrompt", R.CLEAN, R.REQUIRED, "邮件提示");
            
            Form form = null;
            if (formId != null) {
                form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
                Asserts.isNull(form, "表单");
                if (!type.equals(form.getType())) {
                    throw new Errors("禁止修改表单类型");
                }
            } else {
                form = new Form();
                form.setCorpId(UserSession.getCorpId());
                form.setUserId(UserSession.getUserId());
                form.setRandCode(Randoms.getRandomString(32));
            }

            form.setFormName(formName);
            form.setFormDesc(formDesc);
            form.setType(type);
            form.setEmailConfirm(emailConfirm);
            form.setEmailPrompt(emailPrompt);
            form.setTagId(tagId);
            
            boolean ok = Forms.ok(form.getEmailConfirm(), form.getEmailPrompt(),
                    form.getInputPath(), form.getSubmitPath(), form.getConfirmPath(), form.getPromptPath());
            form.setStatus(ok ? Status.ENABLED : Status.DISABLED);
            
            formService.save(form);
            
            mb.put("formId", form.getFormId());
            
            Views.ok(mb, (formId != null ? "修改" : "新建") + "表单成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, (formId != null ? "修改" : "新建") + "表单失败");
            logger.error("(Form:add) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    @RequestMapping(value = "del", method = RequestMethod.GET)
    public String del(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);

            Integer formId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            
            map.put("checkedName", form.getFormName());
			Views.map(map, "del", "", "删除", "所选表单", "若邮件中已使用了该表单，对应的链接将会无效。", "checkedIds", formId);
			return "default/common_form";
        } catch (Exception e) {
            logger.error("(Form:del) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "del", method = RequestMethod.POST)
    public void del(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        String checkedIds = null;
        try {
            CSRF.validate(request);

            Integer formId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");

            checkedIds = String.valueOf(formId);
            formService.delete(form.getUserId(), formId);
            Files.delete(Webs.rootPath(), form.getInputPath(), PathMap.FORM.getPath());
            Files.delete(Webs.rootPath(), form.getSubmitPath(), PathMap.FORM.getPath());
            Files.delete(Webs.rootPath(), form.getConfirmPath(), PathMap.FORM.getPath());
            Files.delete(Webs.rootPath(), form.getPromptPath(), PathMap.FORM.getPath());
            
            Views.ok(mb, "删除表单成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "删除表单失败");
            logger.error("(Form:del) error: ", e);
        }

        mb.put("checkedIds", checkedIds);
        ObjectMappers.renderJson(response, mb);
    }

    @RequestMapping(value = "preview", method = RequestMethod.POST)
    public String preview(HttpServletRequest request, HttpServletResponse response) {
        try {
            CSRF.auth(request);
            
            String content = request.getParameter("content");
            if (StringUtils.isBlank(content)) {
                content = Value.S;
            }
            
            Document doc = Jsoup.parse(content);
            Modulars.filter(doc);
            
            ObjectMappers.renderHtml(response, XSS.safety4(doc).toString());

            return null;
        } catch (Exception e) {
            logger.error("(Form:preview) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "view", method = RequestMethod.GET)
    public String view(HttpServletRequest request, HttpServletResponse response) {
        try {
            CSRF.auth(request);

            String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^input|submit|confirm|prompt$");
            t = Values.get(t, "input");
            
            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), null, formId);
            Asserts.isNull(form, "表单");
            
            String content = request.getParameter("content");
            if (t.equals("input")) {
                content = Files.get(Webs.rootPath(), form.getInputPath());
            } else if (t.equals("submit")) {
                content = Files.get(Webs.rootPath(), form.getSubmitPath());
            } else if (t.equals("confirm")) {
                content = Files.get(Webs.rootPath(), form.getConfirmPath());
            } else if (t.equals("prompt")) {
                content = Files.get(Webs.rootPath(), form.getPromptPath());
            }
            content = Values.get(content);

            Document doc = Jsoup.parse(content);
            Modulars.filter(doc);
            if (t.equals("input")) {
                doc.select("form").attr("action", "");
            }
            ObjectMappers.renderHtml(response, XSS.safety4(doc).toString());

            return null;
        } catch (Exception e) {
            logger.error("(Form:view) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "1", method = RequestMethod.GET)
    public String _1(HttpServletRequest request, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/form/preview", token);
            CSRF.generate(request, "/datasource/form/recommend", token);
            
            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            String templateContent = Files.get(Webs.rootPath(), form.getInputPath());

            List<Prop> propList = propService.find(UserSession.getCorpId());

            String templateName = "注册";
            if (form.getType().equals(FormMap.SUBSCRIBE.getId())) {
                templateName = "订阅";
            }
            
            List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
            Map<String, List<Template>> recommendMap = Categorys.groupMap(
                    categoryService.find(corpIds, null, Category.FORM, CategoryMap.INPUT.getName(), Cnds.EQ),
                    templateService.find(corpIds, null, templateName, Cnds.LIKE));

            map.put("form", form);
            map.put("propList", propList);
            map.put("recommendMap", recommendMap);
            map.put("templateContent", templateContent);

            return "default/datasource/form_1";
        } catch (Exception e) {
            logger.error("(Form:1) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "1", method = RequestMethod.POST)
    public void _1(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer formId = null;
        String orgInputPath = null;
        
        try {
            CSRF.validate(request);

            formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            
            String templateContent = request.getParameter("templateContent");

            orgInputPath = form.getInputPath();
            
            Document doc = Jsoup.parse(templateContent);
            Modulars.filter(doc);
            
            String action = Property.getStr(Config.APP_URL) + "/form/submit?id=" + formId + "&r=" + form.getRandCode();
            doc.select("form").attr("action", action);
            
            form.setInputPath(Uploads.upload(UserSession.getCorpId(), XSS.safety4(doc).toString(), Webs.rootPath(), PathMap.FORM.getPath()));

            boolean ok = Forms.ok(form.getEmailConfirm(), form.getEmailPrompt(),
                    form.getInputPath(), form.getSubmitPath(), form.getConfirmPath(), form.getPromptPath());
            form.setStatus(ok ? Status.ENABLED : Status.DISABLED);
            
            formService.save(form);
            Files.delete(Webs.rootPath(), orgInputPath, PathMap.FORM.getPath());

            mb.put("formId", form.getFormId());
            Views.ok(mb, "编辑表单填写界面成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "编辑表单填写界面失败");
            if (formId == null) {
                Files.delete(Webs.rootPath(), orgInputPath, PathMap.FORM.getPath());
            }
            logger.error("(Form:1) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }

    @RequestMapping(value = "2", method = RequestMethod.GET)
    public String _2(HttpServletRequest request, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/form/preview", token);
            CSRF.generate(request, "/datasource/form/recommend", token);

            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            String templateContent = Files.get(Webs.rootPath(), form.getSubmitPath());

            String templateName = "注册";
            if (form.getType().equals(FormMap.SUBSCRIBE.getId())) {
                templateName = "订阅";
            }
            
            List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
            
            Map<String, List<Template>> recommendMap = Categorys.groupMap(
                    categoryService.find(corpIds, null, Category.FORM, CategoryMap.SUBMIT.getName(), Cnds.EQ),
                    templateService.find(corpIds, null, templateName, Cnds.LIKE));

            map.put("form", form);
            map.put("templateContent", templateContent);
            map.put("recommendMap", recommendMap);
            
            return "default/datasource/form_2";
        } catch (Exception e) {
            logger.error("(Form:2) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "2", method = RequestMethod.POST)
    public void _2(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer formId = null;
        String orgSubmitPath = null;

        try {
            CSRF.validate(request);

            formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            String templateContent = request.getParameter("templateContent");
            
            orgSubmitPath = form.getSubmitPath();
            Document doc = Jsoup.parse(templateContent);
            Modulars.filter(doc);
            form.setSubmitPath(Uploads.upload(UserSession.getCorpId(), XSS.safety4(doc).toString(), Webs.rootPath(),  PathMap.FORM.getPath()));

            boolean ok = Forms.ok(form.getEmailConfirm(), form.getEmailPrompt(),
                    form.getInputPath(), form.getSubmitPath(), form.getConfirmPath(), form.getPromptPath());
            form.setStatus(ok ? Status.ENABLED : Status.DISABLED);
            
            formService.save(form);
            Files.delete(Webs.rootPath(), orgSubmitPath, PathMap.FORM.getPath());

            mb.put("formId", form.getFormId());
            mb.put("emailConfirm", form.getEmailConfirm());
            mb.put("emailPrompt", form.getEmailPrompt());

            Views.ok(mb, "编辑表单提交界面成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "编辑表单提交界面失败");
            if (formId == null) {
                Files.delete(Webs.rootPath(), orgSubmitPath, PathMap.FORM.getPath());
            }
            logger.error("(Form:2) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    @RequestMapping(value = "3", method = RequestMethod.GET)
    public String _3(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);

            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            String templateContent = Files.get(Webs.rootPath(), form.getConfirmPath());

            map.put("form", form);
            map.put("templateContent", templateContent);
            
            return "default/datasource/form_3";
        } catch (Exception e) {
            logger.error("(Form:3) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "3", method = RequestMethod.POST)
    public void _3(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer formId = null;
        String orgConfirmPath = null;
        try {
            CSRF.validate(request);

            formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            
            String confirmSubject = Https.getStr(request, "confirmSubject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "");
            String templateContent = request.getParameter("templateContent");

            orgConfirmPath = form.getConfirmPath();
            Document doc = Jsoup.parse(templateContent);
            Modulars.filter(doc);

            form.setConfirmPath(Uploads.upload(UserSession.getCorpId(), XSS.safety4(doc).toString(), Webs.rootPath(), PathMap.FORM.getPath()));
            form.setConfirmSubject(confirmSubject);
            
            boolean ok = Forms.ok(form.getEmailConfirm(), form.getEmailPrompt(),
                    form.getInputPath(), form.getSubmitPath(), form.getConfirmPath(), form.getPromptPath());
            form.setStatus(ok ? Status.ENABLED : Status.DISABLED);

            formService.save(form);
            Files.delete(Webs.rootPath(), orgConfirmPath, PathMap.FORM.getPath());

            mb.put("formId", form.getFormId());
            mb.put("emailPrompt", form.getEmailPrompt());

            Views.ok(mb, "编辑邮件验证确认成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "编辑邮件验证确认失败");
            if (formId == null) {
                Files.delete(Webs.rootPath(), orgConfirmPath, PathMap.FORM.getPath());
            }
            logger.error("(Form:3) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    @RequestMapping(value = "4", method = RequestMethod.GET)
    public String _4(HttpServletRequest request, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/form/preview", token);
            CSRF.generate(request, "/datasource/form/recommend", token);
            
            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            String templateContent = Files.get(Webs.rootPath(), form.getPromptPath());
            
            List<Form> formList = formService.find(UserSession.getCorpId(), null, Value.T);
            List<Prop> propList = propService.find(UserSession.getCorpId());
            List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.FORM, null, null);

            String templateName = "注册";
            if (form.getType().equals(FormMap.SUBSCRIBE.getId())) {
                templateName = "订阅";
            }
            
            List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
            
            Map<String, List<Template>> recommendMap = Categorys.groupMap(
                    categoryService.find(corpIds, null, Category.FORM, CategoryMap.PROMPT.getName(), Cnds.EQ),
                    templateService.find(corpIds, null, templateName, Cnds.LIKE));

            map.put("form", form);
            map.put("templateContent", templateContent);
            map.put("formList", formList);
            map.put("propList", propList);
            map.put("categoryList", categoryList);
            map.put("recommendMap", recommendMap);

            return "default/datasource/form_4";
        } catch (Exception e) {
            logger.error("(Form:4) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "4", method = RequestMethod.POST)
    public void _4(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer formId = null;
        String orgPromptPath = null;
        try {
            CSRF.validate(request);

            formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), UserSession.getUserId(), formId);
            Asserts.isNull(form, "表单");
            
            String promptSubject = Https.getStr(request, "promptSubject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
            String templateContent = request.getParameter("templateContent");
            String next = Values.get(Https.getStr(request, "next", R.CLEAN, R.REGEX, "regex:^on|off$"), "on");
            
            orgPromptPath = form.getPromptPath();
            Document doc = Jsoup.parse(templateContent);
            
            if (next.equals("off")) {
                boolean script = false;
                boolean blacklist = false;
                if (doc.select("script").size() > 0) {
                    script = true;
                }
                if (!(Jsoup.isValid(doc.toString(), Safety.whitelist()))) {
                    blacklist = true;
                }
                if (script || blacklist) {
                    mb.put("code", 2);
                    mb.put("message", "该模板不符合设计规范，如直接投递将导致效果差异，请修改后再使用。");
                    ObjectMappers.renderJson(response, mb);
                    return;
                }
            }
            
            Modulars.filter(doc);

            form.setPromptPath(Uploads.upload(UserSession.getCorpId(), XSS.safety(doc).toString(), Webs.rootPath(), PathMap.FORM.getPath()));
            form.setPromptSubject(promptSubject);
            
            boolean ok = Forms.ok(form.getEmailConfirm(), form.getEmailPrompt(),
                    form.getInputPath(), form.getSubmitPath(), form.getConfirmPath(), form.getPromptPath());
            form.setStatus(ok ? Status.ENABLED : Status.DISABLED);

            formService.save(form);
            Files.delete(Webs.rootPath(), orgPromptPath, PathMap.FORM.getPath());

            mb.put("formId", form.getFormId());

            Views.ok(mb, "编辑成功邮件提示成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "编辑成功邮件提示失败");
            if (formId == null) {
                Files.delete(Webs.rootPath(), orgPromptPath, PathMap.FORM.getPath());
            }
            logger.error("(Form:4) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }

    @RequestMapping(value = "5", method = RequestMethod.GET)
    public String _5(HttpServletRequest request, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/form/view", token);
            CSRF.generate(request, "/datasource/form/recommend", token);
            
            Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER, R.REQUIRED);
            Asserts.isNull(formId, "表单");
            Form form = formService.get(UserSession.getCorpId(), null, formId);
            Asserts.isNull(form, "表单");
            Tag tag = tagService.get(form.getTagId());
            Asserts.isNull(tag, "标签");
            
            map.put("tag", tag);
            map.put("form", form);
            
            return "default/datasource/form_5";
        } catch (Exception e) {
            logger.error("(Form:5) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "snatch", method = RequestMethod.GET)
    public String snatch(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);
            String t = Https.getStr(request, "t", R.REGEX, "regex:^safety|safety4$");
            t = Values.get(t, "safety");
            map.put("t", t);
            return "default/datasource/form_snatch";
        } catch (Exception e) {
            logger.error("(Form:snatch) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "snatch", method = RequestMethod.POST)
    public void snatch(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);

            String url = Https.getStr(request, "snatchUrl", R.REQUIRED, "网页地址");
            String t = Https.getStr(request, "t", R.REGEX, "regex:^safety|safety4$");
            t = Values.get(t, "safety");
            
            boolean script = false;
            boolean blacklist = false;
            
            String s1 = Snatchs.get(url, "GB2312");
            String s2 = Snatchs.get(url, "UTF-8");
            Uploads.maxSize(s1);
            Uploads.maxSize(s2);

            Document doc1 = Jsoup.parse(s1, url);
            Document doc2 = Jsoup.parse(s2, url);
            
            Whitelist whitelist = t.equals("safety") ? Safety.whitelist() : Safety4.whitelist();
            
            // 1) JavaScript
            if (doc1.select("script").size() > 0 || doc2.select("script").size() > 0) {
                script = true;
            }
            
            // 2) HTML WhiteList
            if (!(Jsoup.isValid(doc1.toString(), whitelist) && Jsoup.isValid(doc2.toString(), whitelist))) {
                blacklist = true;
            }

            filter(doc1.select("[href]"), "abs:href", "href");
            filter(doc1.select("[src]"), "abs:src", "src");
            filter(doc2.select("[href]"), "abs:href", "href");
            filter(doc2.select("[src]"), "abs:src", "src");

            Modulars.filter(doc1);
            Modulars.filter(doc2);

            mb.put("code", script || blacklist ? 2 : Value.T);
            mb.put("message", script || blacklist ? Templates.filter(script, blacklist) : "抓取网页成功");

            mb.put("GB2312", (t.equals("safety") ? XSS.safety(doc1).toString() : XSS.safety4(doc1).toString()));
            mb.put("UTF8", (t.equals("safety") ? XSS.safety(doc2).toString() : XSS.safety4(doc2).toString()));
        } catch (Errors e) {
            mb.put("code", Value.F, "message", e.getMessage());
        } catch (Exception e) {
            mb.put("code", Value.F, "message", "抓取网页失败");
            logger.error("(Form:snatch) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }

    private void filter(Elements links, String abs, String attr) {
        for (Element link : links) {
            String src = link.attr(abs);
            link.attr(attr, src);
        }
    }

    @RequestMapping(value = "recommend", method = RequestMethod.POST)
    public void recommend(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);

            Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
            Asserts.isNull(templateId, "推荐模板");
            
            List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
            Template template = templateService.get(corpIds, null, null, templateId);
            Asserts.isNull(template, "推荐模板");
            String content = Files.get(Webs.rootPath(), template.getFilePath());

            Document doc = Jsoup.parse(content);
            Modulars.filter(doc);

            mb.put("content", XSS.safety4(doc).toString());
            mb.put("page_header", StringUtils.contains(content, "page_header") ? "on" : "off");
            mb.put("page_footer", StringUtils.contains(content, "page_footer") ? "on" : "off");

            Views.ok(mb, "导入模板成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "导入模板失败");
            logger.error("(Form:recommend) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }

    @RequestMapping(value = "file", method = RequestMethod.GET)
    public String file(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);
            String t = Https.getStr(request, "t", R.REGEX, "regex:^safety|safety4$");
            t = Values.get(t, "safety");
            map.put("t", t);
            return "default/datasource/form_file";
        } catch (Exception e) {
            logger.error("(Form:file) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "file", method = RequestMethod.POST)
    public void file(HttpServletRequest request, HttpServletResponse response, 
                    @RequestParam(required = false, value = "upload") MultipartFile upload) {
        Integer code = Value.I;
        String message = Value.S;
        String GB2312 = Value.S;
        String UTF8 = Value.S;
        try {
            CSRF.auth(request);

            String t = Https.getStr(request, "t", R.REGEX, "regex:^safety|safety4$");
            t = Values.get(t, "safety");
            
            Files.valid(upload, new MapBean("file", "html,htm"));
            boolean script = false;
            boolean blacklist = false;

            Document doc1 = Jsoup.parse(Readers.html(upload.getInputStream(), "GB2312"));
            Document doc2 = Jsoup.parse(Readers.html(upload.getInputStream(), "UTF-8"));

            Whitelist whitelist = t.equals("safety") ? Safety.whitelist() : Safety4.whitelist();
            
            Uploads.maxSize(doc1.toString());
            Uploads.maxSize(doc2.toString());

            if (doc1.select("script").size() > 0 || doc2.select("script").size() > 0) {
                script = true;
            }

            // 2) HTML WhiteList
            if (!(Jsoup.isValid(doc1.toString(), whitelist) && Jsoup.isValid(doc2.toString(), whitelist))) {
                blacklist = true;
            }

            Modulars.filter(doc1);
            Modulars.filter(doc2);

            GB2312 =(t.equals("safety") ? XSS.safety(doc1).toString() : XSS.safety4(doc1).toString());
            UTF8 = (t.equals("safety") ? XSS.safety(doc2).toString() : XSS.safety4(doc2).toString());

            code = script || blacklist ? 2 : Value.T;
            message = script || blacklist ? Templates.filter(script, blacklist) : "导入文件成功";
        } catch (Errors e) {
            code = Value.F;
            message = e.getMessage();
        } catch (Exception e) {
            code = Value.F;
            message = "导入文件失败";
            logger.error("(Form:file) error: ", e);
        }

        StringBuffer sbff = new StringBuffer();
        sbff.append("code:")
            .append(code)
            .append("---------- NextPart ----------")
            .append("message:")
            .append(message)
            .append("---------- NextPart ----------")
            .append(GB2312)
            .append("---------- NextPart ----------")
            .append(UTF8);

        ObjectMappers.renderHtml(response, sbff.toString());
    }
}
