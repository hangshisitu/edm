package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Value;
import cn.edm.model.Adapter;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class AdapterService {

    @Autowired
    private Dao dao;

    public void save(Adapter adapter) {
        dao.save("Adapter.save", adapter);
    }

    public void delete(Integer templateId) {
        dao.delete("Adapter.delete", new MapBean("templateId", templateId));
    }

    public Adapter get(Integer templateId) {
        return dao.get("Adapter.query", new MapBean("templateId", templateId));
    }

    public List<Adapter> find(Integer corpId, String userId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId, "userId", userId, "joinPhone", Value.T);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Adapter.query", mb);
    }
}
