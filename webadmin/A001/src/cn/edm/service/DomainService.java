package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Type;
import cn.edm.model.Domain;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class DomainService {

	@Autowired
	private Dao dao;

	public Domain sum(MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId,
	        String[] domains, String beginTime, String endTime, Integer[] types) {
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "bindCorpId", bindCorpId);
		Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.put(mb, "domains", domains);
		Pages.put(mb, "domainCnd", Cnds.IN);
        Pages.put(mb, "types", types);
		return dao.get("Domain.sum", mb);
	}
	
    public Domain sum(MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] taskIds) {
    	Pages.put(mb, "taskIds", taskIds);
    	Pages.put(mb, "corpIds", corpIds);
    	Pages.put(mb, "corpId", corpId);
    	Pages.put(mb, "bindCorpId", bindCorpId);
    	Pages.put(mb, "domainCnd", Cnds.IN);
    	return dao.get("Domain.sum", mb);
    }
	
    public Domain sum(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "taskId", taskId);
        return dao.get("Domain.sum", mb);
    }
    
    public List<Domain> find(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "taskId", taskId);
        return dao.find("Domain.query", mb);
    }
    
    public List<String> top5(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("taskId", taskId);
        return dao.find("Domain.top5", mb);
    }
    
    public List<Domain> group(Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "taskIds", taskIds);
        return dao.find("Domain.group", mb);
    }
    
    public List<Domain> trigerGroup(Integer[] corpIds, Integer corpId, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        return dao.find("Domain.trigerGroup", mb);
    }

    public Domain planSum(Integer[] corpIds, Integer corpId, Integer planId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "planId", planId);
        mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        return dao.get("Domain.sum", mb);
    }
    
    public Domain planSum(MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] planIds) {
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "planIds", planIds);
        return dao.get("Domain.sum", mb);
    }
    
    public List<Domain> planGroup(Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] planIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "planIds", planIds);
        return dao.find("Domain.planGroup", mb);
    }
    
    public Domain campaignSum(MapBean mb, Integer[] corpIds, Integer corpId, Integer campaignId, String beginTime, String endTime) {
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "campaignId", campaignId);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        return dao.get("Domain.sum", mb);
    }
    
    public Domain campaignSum(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.get("Domain.sum", mb);
    }
    
    public List<Domain> campaignSum(Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] campaignIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "campaignIds", campaignIds);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.find("Domain.campaignSum", mb);
    }
    
    public List<Domain> campaignFind(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.find("Domain.querySum", mb);
    }
    
    public List<String> campaignTop5(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("type", Type.FORMAL, "typeCnd", Cnds.EQ);
        return dao.find("Domain.top5", mb);
    }
    
	public Domain trigerSum(MapBean mb, Integer[] corpIds, Integer corpId, String beginTime, String endTime) {
		Pages.put(mb, "beginTime", beginTime);
		Pages.put(mb, "endTime", endTime);
		Pages.search(mb);
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		return dao.get("Domain.trigerSum", mb);
	}
	
	public Domain trigerSum(Integer[] corpIds, Integer corpId, Integer taskId) {
		MapBean mb = new MapBean();
		mb.put("corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
		return dao.get("Domain.trigerSum", mb);
	}
	
	public List<Domain> trigerFind(Integer[] corpIds, Integer corpId, Integer taskId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "taskId", taskId);
		return dao.find("Domain.trigerQuery", mb);
	}
	
	public List<Domain> touchGroup(Integer[] corpIds, Integer corpId, Integer[] templateIds, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "taskIds", taskIds);
        Pages.put(mb, "templateIds", templateIds);
        return dao.find("Domain.touchGroup", mb);
    }
	
	public List<Domain> touchCampaignGroup(Integer[] corpIds, Integer corpId, Integer[] taskIds) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "taskIds", taskIds);
		return dao.find("Domain.touchCampaignGroup", mb);
	}
	
	public List<Domain> touchQuery(Integer[] corpIds, Integer corpId, Integer templateId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "templateId", templateId);
		return dao.find("Domain.touchQuery", mb);
	}
	
	public List<Domain> touchSum(Integer[] corpIds, Integer corpId, Integer templateId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "templateId", templateId);
		return dao.find("Domain.touchSum", mb);
	}
	
	public Domain touchTotal(Integer[] corpIds, Integer corpId, Integer templateId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "templateId", templateId);
		return dao.get("Domain.touchTotal", mb);
	}
	
}
