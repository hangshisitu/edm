package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.model.Form;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class FormService {

    @Autowired
    private Dao dao;

    public void save(Form form) {
        if (form.getFormId() != null) {
            dao.update("Form.update", form);
        } else {
            dao.save("Form.save", form);
        }
    }

    public Form get(Integer formId, String randCode) {
        return dao.get("Form.query", new MapBean("formId", formId, "randCode", randCode));
    }

    public Form get(Integer corpId, String userId, Integer formId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("formId", formId);
        return dao.get("Form.query", mb);
    }

    public void delete(String userId, Integer formId) {
        dao.delete("Form.delete", new MapBean("userId", userId, "formId", formId));
    }
    
    public Page<Form> search(Page<Form> page, MapBean mb, Integer corpId, String formName, String beginTime, String endTime) {
        Pages.put(mb, "formName", formName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Form.count", "Form.index");
    }

    public List<Form> find(int corpId, Integer tagId, Integer status) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        Pages.put(mb, "tagId", tagId);
        Pages.put(mb, "status", status);
        return dao.find("Form.query", mb);
    }
    
    public boolean lockTag(int corpId, String userId, int tagId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        mb.put("userId", userId);
        mb.put("tagId", tagId);
        return (int) dao.countResult("Form.count", mb) > 0;
    }
}
