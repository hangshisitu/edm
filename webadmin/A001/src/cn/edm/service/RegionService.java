package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Type;
import cn.edm.model.Region;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class RegionService {

    @Autowired
    private Dao dao;

    public List<Region> find(Integer[] corpIds, Integer corpId, Integer taskId) {
    	MapBean mb = new MapBean();
    	Pages.put(mb, "corpIds", corpIds);
    	Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
        return dao.find("Region.query", mb);
    }
    
	public List<Region> findTriger(Integer[] corpIds, Integer corpId, Integer taskId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
		return dao.find("Region.trigerQuery", mb);
	}
    
    public List<Region> group(Integer[] corpIds, Integer corpId, Integer taskId) {
    	MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
        return dao.find("Region.group", mb);
    }
    
    public List<Region> campaignFind(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.find("Region.querySum", mb);
    }
    
    public List<Region> campaignGroup(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.find("Region.group", mb);
    }
    
    public List<Region> trigerGroup(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        mb.put("corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("taskId", taskId);
        return dao.find("Region.trigerGroup", mb);
    }
}
