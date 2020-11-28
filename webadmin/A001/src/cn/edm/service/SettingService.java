package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Setting;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class SettingService {

	@Autowired
	private Dao dao;

	public void delete(int corpId, String userId, Integer[] parentIds) {
        dao.delete("TemplateSetting.delete", new MapBean("corpId", corpId, "userId", userId, "parentIds", parentIds));
    }
	
	public Setting get(Integer taskId) {
		return dao.get("TaskSetting.query", new MapBean("taskId", taskId));
	}
	
	public Setting get(Integer corpId, Integer taskId, Integer templateId) {
		return dao.get("TaskSetting.query", new MapBean("corpId", corpId, "taskId", taskId, "templateId", templateId));
	}
	
    public Setting get(Integer parentId, Integer templateId) {
        return dao.get("TemplateSetting.query", new MapBean("parentId", parentId, "templateId", templateId));
    }
    
    public List<Setting> find(Integer corpId, String userId, Integer parentId, Integer deviceType) {
        return dao.find("TemplateSetting.query", new MapBean("corpId", corpId, "userId", userId, "parentId", parentId, "deviceType", deviceType));
	}
    
    public List<Integer> lockTemplate(Integer corpId, String userId, Integer[] templateIds) {
        return dao.find("TemplateSetting.lockTemplate", new MapBean("corpId", corpId, "userId", userId, "templateIds", templateIds));
    }
    
    public List<Setting> touchFind(Integer corpId, String userId, Integer parentId, Integer deviceType) {
        return dao.find("TouchSetting.query", new MapBean("corpId", corpId, "userId", userId, "parentId", parentId, "deviceType", deviceType));
    }
    
    public Setting touchGet(Integer taskId, Integer templateId, Integer deviceType) {
        return dao.get("TouchSetting.query", new MapBean("taskId", taskId, "templateId", templateId, "deviceType", deviceType));
    }
    
    public List<Setting> touchQuery(Integer[] corpIds, Integer corpId, Integer parentId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "parentId", parentId);
        Pages.put(mb, "touchParentId", taskId);
        return dao.find("TouchSetting.touchQuery", mb);
    }
    
    public List<Setting> touchQuery(Integer[] corpIds, Integer corpId,
            Integer parentId, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "parentId", parentId);
        Pages.put(mb, "touchParentIds", taskIds);
        return dao.find("TouchSetting.touchQuery", mb);
    }
    
    public Setting get(Integer[] corpIds, Integer corpId, Integer templateId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "templateId", templateId);
        Pages.put(mb, "taskId", taskId);
    	return dao.get("TouchSetting.query", mb);
    }
    
}
