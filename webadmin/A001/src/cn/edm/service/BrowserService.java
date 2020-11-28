package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Type;
import cn.edm.model.Browser;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class BrowserService {

    @Autowired
    private Dao dao;
    
    public List<Browser> sum(Integer[] corpIds, Integer corpId, Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "campaignId", campaignId);
        mb.put("type", Type.TEST, "typeCnd", Cnds.NOT_EQ);
        return dao.find("Browser.sum", mb);
    }
}
