package cn.edm.service;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.constants.Cnds;
import cn.edm.constants.mapper.PropMap;
import cn.edm.domain.Prop;
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

    public void delete(int corpId, String userId, Integer propId) {
        dao.delete("Prop.delete", new MapBean("corpId", corpId, "userId", userId, "propId", propId));
    }

    public void delete(int corpId, String userId, Integer[] propIds) {
        dao.delete("Prop.delete", new MapBean("corpId", corpId, "userId", userId, "propIds", propIds));
    }

    public Prop get(int corpId, String userId, Integer propId) {
        return dao.get("Prop.query", new MapBean("corpId", corpId, "userId", userId, "propId", propId));
    }
    
    public Prop get(int corpId, String userId, String propName) {
        return dao.get("Prop.query", new MapBean("corpId", corpId, "userId", userId, "propName", propName, "nameCnd", Cnds.EQ));
    }

	public List<Prop> find(int corpId, String userId) {
		return dao.find("Prop.query", new MapBean("corpId", corpId, "userId", userId));
	}
    
    public List<Prop> find(int corpId, String userId, Integer[] propIds) {
        return dao.find("Prop.query", new MapBean("corpId", corpId, "userId", userId, "propIds", propIds));
    } 
    
	public List<Prop> find(int corpId, String userId, String[] propNames) {
		return dao.find("Prop.query", new MapBean("corpId", corpId, "userId", userId, "propNames", propNames));
	}

    public Page<Prop> search(Page<Prop> page, MapBean mb, int corpId, String userId, String propName, String beginTime, String endTime) {
        Pages.put(mb, "propName", propName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        mapper(mb, propName);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
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
    
    public boolean unique(int corpId, String userId, String propName, String orgPropName) {
        if (propName == null || propName.equals(orgPropName)) {
            return true;
        }
        
        long count = dao.countResult("Prop.count", new MapBean("corpId", corpId, "userId", userId, "propName", propName, "nameCnd", Cnds.EQ));
        return count == 0;
    }
}
