package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Type;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class UrlService {

    @Autowired
    private Dao dao;

    public List<Url> find(Integer[] corpIds, Integer corpId, Integer taskId) {
    	MapBean mb = new MapBean();
    	Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
        return dao.find("Url.query", mb);
    }
    
    public Url sum(Integer[] corpIds, Integer corpId, Integer taskId) {
    	MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
		mb.put("taskId", taskId);
        return dao.get("Url.sum", mb);
    }
    
    public List<Url> campaignFind(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.find("Url.querySum", mb);
    }
    
    public Url campaignSum(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[]{ Type.FORMAL, Type.JOIN });
        return dao.get("Url.sum", mb);
    }
}
