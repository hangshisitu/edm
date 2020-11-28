package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.model.Result;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class ResultService {

    @Autowired
    private Dao dao;

    public Result get(Integer[] corpIds, Integer corpId, Integer taskId, Integer templateId) {
        MapBean mb = new MapBean();
        mb.put("corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("taskId", taskId);
        mb.put("templateId", templateId);
        return dao.get("Result.query", mb);
    }

    public Result get(Integer[] corpIds, Integer taskId) {
        MapBean mb = new MapBean();
        mb.put("corpIds", corpIds);
        mb.put("taskId", taskId);
        return dao.get("Result.query", mb);
    }

    public Result get(Integer[] corpIds, Integer corpId, Integer taskId) {
    	MapBean mb = new MapBean();
		mb.put("corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
        return dao.get("Result.trigerQuery", mb);
    }
    
    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId,
            String taskName, String beginTime, String endTime, Integer[] types, String nameCnd) {
        Pages.put(mb, "taskName", taskName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "types", types);
        Pages.put(mb, "nameCnd", nameCnd);
        Pages.order(page, "deliveryTime", Page.DESC);
        return dao.find(page, mb, "Result.count", "Result.index");
    }
    
    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId, String campaignName) {
        Pages.put(mb, "campaignName", campaignName);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "statuses", Status.task("completed"));
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "deliveryTime", Page.DESC);
        return dao.find(page, mb, "Result.campaignCount", "Result.campaignIndex");
    }

    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId,
            Integer campaignId, Integer[] statuses, String orderBy) {
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        Pages.put(mb, "statuses", statuses);
        Pages.order(page, orderBy, Page.DESC);
        return dao.find(page, mb, "Result.count", "Result.index");
    }
    
    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, String trigerName, String beginTime, String endTime) {
        Pages.put(mb, "trigerName", trigerName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.order(page, "createTime", Page.DESC);
        return dao.find(page, mb, "Result.trigerCount", "Result.trigerIndex");
    }
    
    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId, String taskName, String nameCnd) {
        Pages.put(mb, "taskName", taskName);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "nameCnd", nameCnd);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Result.planCount", "Result.planIndex");
    }

    public Page<Result> search(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer planId) {
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("planId", planId);
        mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        Pages.order(page, "deliveryTime", Page.DESC);
        return dao.find(page, mb, "Result.count", "Result.index");
    }
    
    public List<Result> find(Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        mb.put("taskIds", taskIds);
        Pages.order(mb, "deliveryTime", Page.DESC);
        return dao.find("Result.query", mb);
    }
    
    public List<Result> referers(Integer[] corpIds, Integer corpId, Integer bindCorpId, Integer[] planIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        mb.put("planIds", planIds);
        mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Result.query", mb);
    }
    
    public List<Result> find(Integer[] corpIds, Integer corpId, Integer campaignId, String orderBy) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        mb.put("statuses", new Integer[] { Status.CANCELLED, Status.COMPLETED });
        Pages.order(mb, orderBy, Page.DESC);
        return dao.find("Result.query", mb);
    }
    
    public List<Result> find(Integer[] corpIds, Integer corpId, Integer planId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("planId", planId);
        mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        Pages.order(mb, "deliveryTime", Page.DESC);
        return dao.find("Result.query", mb);
    }
    
    public MapBean recent(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        Pages.put(mb, "statuses", Status.task("all"));
        return dao.get("Result.recent", mb);
    }
    
    public Page<Result> searchTouch(Page<Result> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer bindCorpId,
            String templateName, String nameCnd) {
    	Pages.put(mb, "templateName", templateName);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "bindCorpId", bindCorpId);
        Pages.put(mb, "nameCnd", nameCnd);
        return dao.find(page, mb, "Result.touchCount", "Result.touchIndex");
    }
    
    public Result get(Integer corpId, Integer taskId, Integer templateId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        mb.put("taskId", taskId);
        mb.put("templateId", templateId);
        return dao.get("Result.touchQuery", mb);
    }
    
    public List<Result> findTouch(Integer corpId, Integer templateId) {
    	 MapBean mb = new MapBean();
         mb.put("corpId", corpId);
         mb.put("templateId", templateId);
         return dao.find("Result.touchQuery", mb);
    }
    
}
