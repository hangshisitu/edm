package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.model.Filter;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class FilterService {

    @Autowired
    private Dao dao;

    public void save(Filter filter) {
        if (filter.getFilterId() != null) {
            dao.update("Filter.update", filter);
        } else {
            dao.save("Filter.save", filter);
        }
    }

    public void delete(String userId, Integer filterId) {
        dao.delete("Filter.delete", new MapBean("userId", userId, "filterId", filterId));
    }

    public void delete(String userId, Integer[] filterIds) {
        dao.delete("Filter.delete", new MapBean("userId", userId, "filterIds", filterIds));
    }

    public Filter get(Integer corpId, String userId, Integer filterId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("filterId", filterId);
        return dao.get("Filter.query", mb);
    }

    public List<Filter> find(int corpId) {
        return dao.find("Filter.query", new MapBean("corpId", corpId));
    }
    
    public List<Filter> find(String userId, Integer[] filterIds) {
        return dao.find("Filter.query", new MapBean("userId", userId, "filterIds", filterIds));
    } 

    public Page<Filter> search(Page<Filter> page, MapBean mb, int corpId, String filterName, String beginTime, String endTime) {
        Pages.put(mb, "filterName", filterName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Filter.count", "Filter.index");
    }
    
    public boolean unique(int corpId, String filterName, String orgFilterName) {
        if (filterName == null || filterName.equals(orgFilterName)) {
            return true;
        }
        
        long count = dao.countResult("Filter.count", new MapBean("corpId", corpId, "filterName", filterName, "nameCnd", Cnds.EQ));
        return count == 0;
    }
}
