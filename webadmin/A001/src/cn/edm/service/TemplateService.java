package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Device;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.model.Adapter;
import cn.edm.model.Category;
import cn.edm.model.Setting;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.repository.Dao;
import cn.edm.utils.Asserts;
import cn.edm.utils.web.Pages;
import cn.edm.web.UserSession;

@Transactional
@Service
public class TemplateService {

    @Autowired
    private Dao dao;
    @Autowired
    private CategoryService categoryService;

    public void save(Template template) {
        if (template.getTemplateId() != null) {
            dao.update("Template.update", template);
        } else {
            dao.save("Template.save", template);
        }
    }

    public void save(Template template, Adapter adapter, List<Setting> templateSettingList, List<Setting> phoneSettingList) {
        save(template);
        if (!Asserts.empty(templateSettingList)) {
            for (Setting setting : templateSettingList) {
                setting.setParentId(template.getTemplateId());
            }
            dao.delete("TemplateSetting.delete", new MapBean("parentId", template.getTemplateId(), "deviceType", Device.PC));
            dao.save("TemplateSetting.batchSave", templateSettingList);
        }
        if (!Asserts.empty(phoneSettingList)) {
            for (Setting setting : phoneSettingList) {
                setting.setParentId(template.getTemplateId());
            }
            dao.delete("TemplateSetting.delete", new MapBean("parentId", template.getTemplateId(), "deviceType", Device.PHONE));
            dao.save("TemplateSetting.batchSave", phoneSettingList);
        }
        if (adapter != null) {
            adapter.setTemplateId(template.getTemplateId());
            dao.save("Adapter.save", adapter);
        }
    }
    
    public void delete(String userId, Integer templateId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        mb.put("templateId", templateId);
        dao.delete("Template.delete", mb);
    }

    public void delete(String userId, Integer[] templateIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        mb.put("templateIds", templateIds);
        dao.delete("Template.delete", mb);
    }

    public Template get(Integer templateId) {
        return dao.get("Template.query", new MapBean("templateId", templateId));
    }
    
    public Template get(Integer[] corpIds, Integer corpId, String userId, Integer templateId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("templateId", templateId);
        return dao.get("Template.query", mb);
    }

    public List<Template> find(Integer corpId, Integer[] templateIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        mb.put("templateIds", templateIds);
        return dao.find("Template.query", mb);
    } 
    
    public List<Template> find(Integer corpId, Integer categoryId, String categoryCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "categoryId", categoryId);
        Pages.put(mb, "categoryCnd", categoryCnd);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Template.query", mb);
    } 
    
    public List<Template> find(Integer[] corpIds, Integer corpId, String templateName, String nameCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "templateName", templateName);
        Pages.put(mb, "nameCnd", nameCnd);
        return dao.find("Template.query", mb);
    } 

    public Page<Template> search(Page<Template> page, MapBean mb, Integer[] corpIds, Integer corpId, String templateName, String beginTime, String endTime) {
        Pages.put(mb, "templateName", templateName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        
        if (Property.getStr(Config.CATEGORY_BILL).equals("off")) {
            Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.BILL.getName(), Category.TEMPLATE);
            if (category != null) {
                Pages.put(mb, "categoryId", category.getCategoryId());
                Pages.put(mb, "categoryCnd", Cnds.NOT_EQ);
            }
        }
        
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Template.count", "Template.index");
    }
    
    public boolean unique(Integer corpId, String templateName, String orgTemplateName) {
        if (templateName == null || templateName.equals(orgTemplateName)) {
            return true;
        }
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        mb.put("templateName", templateName);
        mb.put("nameCnd", Cnds.EQ);
        long count = dao.countResult("Template.count", mb);
        return count == 0;
    }
    
    public Template md5(String templateId) {
        return dao.get("Template.md5", new MapBean("templateId", templateId));
    }
    
    public long count(Integer corpId, Integer categoryId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "categoryId", categoryId);
        Pages.put(mb, "categoryCnd", Cnds.EQ);
        return dao.countResult("Template.count", mb);
    }
}
