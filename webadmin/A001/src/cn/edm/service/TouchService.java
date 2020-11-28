package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class TouchService {

    @Autowired
    private Dao dao;

    public void save(Touch touch) {
        dao.save("Touch.save", touch);
    }
    
    public Touch get(Integer corpId, Integer taskId, Integer templateId) {
        return dao.get("Touch.query", new MapBean("corpId", corpId, "taskId", taskId, "templateId", templateId));
    }
    
    public Touch find(Integer corpId, Integer parentId, Integer templateId) {
    	return dao.get("Touch.query", new MapBean("corpId", corpId, "parentId", parentId, "templateId", templateId));
    }
    
    public List<Touch> find(Integer parentId) {
    	return dao.find("Touch.query", new MapBean("parentId", parentId));
    }
    
    public List<Touch> find(Integer[] parentIds) {
    	return dao.find("Touch.query", new MapBean("parentIds", parentIds));
    }
    
    public Integer touchSum(Integer[] corpIds, Integer corpId, Integer templateId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "templateId", templateId);
    	return dao.get("Touch.touchSum", mb);
    }
    
}
