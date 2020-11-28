package cn.edm.service;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Prop;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Pages;

import com.google.common.collect.Lists;

@Transactional
@Service
public class PropService {

    @Autowired
    private Dao dao;

    public void save(Prop prop) {
        if (prop.getPropId() != null) {
            dao.update("Prop.update", prop);
        } else {
            dao.save("Prop.save", prop);
        }
    }

    public void delete(String userId, Integer propId) {
        dao.delete("Prop.delete", new MapBean("userId", userId, "propId", propId));
    }

    public void delete(String userId, Integer[] propIds) {
        dao.delete("Prop.delete", new MapBean("userId", userId, "propIds", propIds));
    }

    public Prop get(Integer corpId, String userId, Integer propId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("propId", propId);
        return dao.get("Prop.query", mb);
    }
    
    public Prop get(Integer corpId, String userId, String propName) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("propName", propName);
        mb.put("nameCnd", Cnds.EQ);
        return dao.get("Prop.query", mb);
    }

	public List<Prop> find(Integer corpId) {
		return dao.find("Prop.query", new MapBean("corpId", corpId));
	}
    
    public List<Prop> find(Integer corpId, String userId, Integer[] propIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "propIds", propIds);
        return dao.find("Prop.query", mb);
    } 
    
	public List<Prop> find(Integer corpId, String[] propNames) {
	    MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "propNames", propNames);
		return dao.find("Prop.query", mb);
	}

    public Page<Prop> search(Page<Prop> page, MapBean mb, Integer corpId, String propName, String beginTime, String endTime) {
        Pages.put(mb, "propName", propName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        mapper(mb, propName);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Prop.count", "Prop.index");
    }
    
	private void mapper(MapBean mb, String propName) {
		if (StringUtils.isNotBlank(propName)) {
			propName = StringUtils.lowerCase(propName);
			List<String> propKeys = Lists.newArrayList();
			List<String> propVals = Lists.newArrayList();
			for (PropMap mapper : PropMap.values()) {
				if (StringUtils.contains(mapper.getId(), propName)) {
					propKeys.add(mapper.getId());
				}

				if (StringUtils.contains(mapper.getName(), propName)) {
					propVals.add(mapper.getId());
				}
			}
			Pages.put(mb, "propKeys", Converts._toStrings(propKeys));
			Pages.put(mb, "propVals", Converts._toStrings(propVals));
		}
	}
    
    public boolean unique(Integer corpId, String propName, String orgPropName) {
        if (propName == null || propName.equals(orgPropName)) {
            return true;
        }
        
        long count = dao.countResult("Prop.count", new MapBean("corpId", corpId, "propName", propName, "nameCnd", Cnds.EQ));
        return count == 0;
    }
}
