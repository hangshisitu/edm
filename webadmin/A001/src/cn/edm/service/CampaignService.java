package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Status;
import cn.edm.model.Campaign;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class CampaignService {

    @Autowired
    private Dao dao;

    public void save(Campaign campaign) {
        if (campaign.getCampaignId() != null) {
            dao.update("Campaign.update", campaign);
        } else {
            dao.save("Campaign.save", campaign);
        }
    }
    
    public void delete(String userId, Integer campaignId) {
        dao.delete("Campaign.delete", new MapBean("userId", userId, "campaignId", campaignId));
        dao.update("Task.clearCampaign", new MapBean("userId", userId, "campaignId", campaignId));
    }
    
    public void bind(int corpId, String userId, Integer campaignId, Integer[] taskIds) {
        dao.update("Task.bindCampaign", new MapBean("corpId", corpId, "userId", userId, "campaignId", campaignId, "taskIds", taskIds));
    }

    public Campaign get(Integer corpId, String userId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("campaignId", campaignId);
        Pages.put(mb, "statuses", Status.task("all"));
        return dao.get("Campaign.query", mb);
    }
       
    public Campaign get(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        Pages.put(mb, "statuses", Status.task("all"));
        return dao.get("Campaign.query", mb);
    }

    public List<Campaign> find(int corpId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "statuses", Status.task("all"));
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Campaign.query", mb);
    }

    public Page<Campaign> search(Page<Campaign> page, MapBean mb, Integer corpId, String campaignName, String beginTime, String endTime, Integer[] statuses) {
        Pages.put(mb, "campaignName", campaignName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "statuses", statuses);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Campaign.count", "Campaign.index");
    }
    
    public boolean unique(Integer corpId, String campaignName, String orgCampaignName) {
        if (campaignName == null || campaignName.equals(orgCampaignName)) {
            return true;
        }
        
        long count = dao.countResult("Campaign.count", new MapBean("corpId", corpId, "campaignName", campaignName, "nameCnd", Cnds.EQ));
        return count == 0;
    }
}
