package cn.edm.web.facade;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.springframework.ui.ModelMap;

import cn.edm.model.Adapter;
import cn.edm.model.Category;
import cn.edm.model.Setting;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Safety;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;

import com.google.common.collect.Lists;

public class Templates {
    
    /**
     * 设置模板属性.
     */
    public static void attrs(Template template, MapBean attrs) {
        attrs.put("editor", "WYSIWYG");
        attrs.put("personal", "false");
        attrs.put("touch", "false");
        if (StringUtils.isNotBlank(template.getAttrs())) {
            for (String str : StringUtils.splitPreserveAllTokens(template.getAttrs(), "&")) {
                String key = StringUtils.substringBefore(str, "=");
                String val = StringUtils.substringAfter(str, "=");
                if (StringUtils.isBlank(key) || StringUtils.isBlank(val)) {
                    continue;
                }
                attrs.put(key, val);
            }
        }
    }
    
    public static void attrs(Adapter adapter, MapBean phoneAttrs) {
        phoneAttrs.put("editor", "WYSIWYG");
        phoneAttrs.put("personal", "false");
        phoneAttrs.put("touch", "false");
        if (StringUtils.isNotBlank(adapter.getPhoneAttrs())) {
            for (String str : StringUtils.splitPreserveAllTokens(adapter.getPhoneAttrs(), "&")) {
                String key = StringUtils.substringBefore(str, "=");
                String val = StringUtils.substringAfter(str, "=");
                if (StringUtils.isBlank(key) || StringUtils.isBlank(val)) {
                    continue;
                }
                phoneAttrs.put(key, val);
            }
        }
    }
    
    /**
     * 获取模板属性.
     */
    public static String attrs(String editor, String personal, String touch) {
        StringBuffer sbff = new StringBuffer();
        sbff.append("editor=").append(Values.get(editor, "WYSIWYG"))
            .append("&")
            .append("personal=").append(StringUtils.isNotBlank(personal) && StringUtils.equals(personal, "true") ? "true" : "false")
            .append("&")
            .append("touch=").append(StringUtils.isNotBlank(touch) && StringUtils.equals(touch, "true") ? "true" : "false");
        return sbff.toString();
    }
    
    /**
     * 设置模板页眉页脚.
     */
    public static void page(HttpServletRequest request, ModelMap map, Template template) {
        String content = Files.get(Webs.rootPath(), template.getFilePath());
        template.setTemplateContent(Values.get(content));
        
        map.put("page_header", StringUtils.contains(content, "page_header") ? "on" : "off");
        map.put("page_footer", StringUtils.contains(content, "page_footer") ? "on" : "off");
    }
    
    public static void page(HttpServletRequest request, ModelMap map, Adapter adapter) {
        String phoneContent = Files.get(Webs.rootPath(), adapter.getPhonePath());
        adapter.setPhoneContent(Values.get(phoneContent));
        
        map.put("phone_page_header", StringUtils.contains(phoneContent, "phone_page_header") ? "on" : "off");
        map.put("phone_page_footer", StringUtils.contains(phoneContent, "phone_page_footer") ? "on" : "off");

        String smsContent = Files.get(Webs.rootPath(), adapter.getSmsPath());
        adapter.setSmsContent(Values.get(smsContent));
    }
    
    public static String filter(boolean script, boolean blacklist) {
        StringBuilder buff = new StringBuilder();
        buff.append("邮件模板禁止");
        if (script) buff.append("JavaScript脚本、");
        if (blacklist) buff.append("HTML非白名单标签、");
        
        String error = buff.toString();
        if (StringUtils.endsWith(error, "、")) error = StringUtils.removeEnd(error, "、");
        error = error + "，系统已自动过滤";
        return error;
    }
    
    public static List<Setting> templateSetting(HttpServletRequest request, boolean isTouch, String pref, int deviceType) {
        Map<String, Object> touchUrlMap = Servlets.getParametersStartingWith(request, pref + "touchUrls_");
        Map<String, Object> templateIdMap = Servlets.getParametersStartingWith(request, pref + "templateIds_");
        Map<String, Object> afterMinuteMap = Servlets.getParametersStartingWith(request, pref + "afterMinutes_");
        Map<String, Object> subjectMap = Servlets.getParametersStartingWith(request, pref + "subjects_");
        
        List<Setting> templateSets = Lists.newArrayList();
        for (Entry<String, Object> entry : templateIdMap.entrySet()) {
            String index = entry.getKey();
            Integer id = Validator.getInt(entry.getValue(), R.CLEAN, R.INTEGER, "子模板ID");
            if (id == null) continue;
            
            String urls = Validator.getStr(touchUrlMap.get(index), R.REQUIRED, "触点链接");
            Integer minute = Validator.getInt(afterMinuteMap.get(index), R.CLEAN, R.INTEGER, R.REQUIRED, R.SIZE, "{0,120}", "触发投递时间");
            String sub = Validator.getStr(subjectMap.get(index), R.CLEAN, R.REQUIRED, R.LENGTH, "{1,200}", "邮件主题");
            
            Setting setting = new Setting();
            setting.setCorpId(UserSession.getCorpId());
            setting.setUserId(UserSession.getUserId());
            setting.setTemplateId(id);
            setting.setTouchUrls(urls);
            setting.setAfterMinutes(minute);
            setting.setSubject(sub);
            setting.setDeviceType(deviceType);
            
            templateSets.add(setting);
        }
        
        if (!Asserts.empty(templateSets) && isTouch) {
//            throw new Errors("触发计划类别禁止设置触发条件");
            templateSets.clear();
        }
        
        return templateSets;
    }
    
    public static void touch(Template template, Category touchCategory, Integer categoryId) {
        if (template.getCategoryId().equals(touchCategory.getCategoryId())) {
            if (!categoryId.equals(touchCategory.getCategoryId())) {
                throw new Errors("模板类别不能修改为非触发计划类型");
            }
        } else {
            if (categoryId.equals(touchCategory.getCategoryId())) {
                throw new Errors("模板类别不能修改为触发计划类型");
            }
        }
    }
    
    public static Document filter(HttpServletResponse response, MapBean mb, String content, String next) {
        Document doc = Jsoup.parse(content);
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
                return null;
            }
        }
        Modulars.filter(doc);
        return doc;
    }
}
