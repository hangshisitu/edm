package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Type;
import cn.edm.model.Hour;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class HourService {

    @Autowired
    private Dao dao;

    public List<Hour> find(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("taskId", taskId);
        return dao.find("Hour.query", mb);
    }

    public List<Hour> campaignFind(Integer[] corpIds, Integer corpId,
            Integer campaignId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignId", campaignId);
        mb.put("types", new Integer[] { Type.FORMAL, Type.JOIN });
        return dao.find("Hour.querySum", mb);
    }
}
