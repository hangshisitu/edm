package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Permissions;
import cn.edm.consts.Status;
import cn.edm.model.Corp;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.CategoryDao;
import cn.edm.repository.Dao;
import cn.edm.repository.PropDao;
import cn.edm.repository.TagDao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class CorpService {

    @Autowired
    private Dao dao;
    @Autowired
    private CategoryDao categoryDao;
    @Autowired
    private PropDao propDao;
    @Autowired
    private TagDao tagDao;
    
    public void save(Corp corp) {
        if (corp.getCorpId() != null) {
            dao.update("Corp.update", corp);
        } else {
            dao.save("Corp.save", corp);
            categoryDao.save(corp.getCorpId());
            propDao.save(corp.getCorpId());
            tagDao.save(corp.getCorpId());
        }
    }
    
    public Corp get(Integer corpId) {
        return dao.get("Corp.query", new MapBean("corpId", corpId));
    }
    
    public Corp getChild(Integer corpId, Integer childCorpId) {
        return dao.get("Corp.query", new MapBean("parentId", corpId, "corpId", childCorpId));
    }

    public List<Integer> getCorpId(Integer cosId) {
    	MapBean mb = new MapBean();
        Pages.put(mb, "cosId",cosId);
        return dao.find("Corp.select",mb);
    }
    
    public void delete(Integer[] corpIds) {
        dao.delete("Corp.delete", new MapBean("corpIds", corpIds));
    }
    
	public List<Corp> find() {
		return dao.find("Corp.query");
	}

	public List<Corp> findChildren(Integer corpId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "parentId", corpId);
        Pages.order(mb, "createTime", Page.DESC);
        return dao.find("Corp.index", mb);
    }
	
	public List<Corp> findChildren(MapBean mb, Integer corpId, String company, String beginTime, String endTime) {
        Pages.put(mb, "company", company);        
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb);
        Pages.put(mb, "parentId", corpId);
        Pages.put(mb, "companyCnd", Cnds.LIKE);
        Pages.order(mb, "createTime", Page.DESC);
        return dao.find("Corp.index", mb);
    }
	
    public void cross(List<Corp> corpList, Integer corpId, List<Integer> children) {
        for (Corp corp : corpList) {
            if (corp.getParentId().equals(corpId)) {
                children.add(corp.getCorpId());
                cross(corpList, corp.getCorpId(), children);
            }
        }
    }
	
    public boolean unique(String key, String value, String orgValue) {
        if (value == null || value.equals(orgValue)) {
            return true;
        }
        
        long count = dao.countResult("Corp.count", new MapBean(key, value, key + "Cnd", Cnds.EQ));
        return count == 0;
    }
    
    public List<Corp> sees() {
        Integer[] statusArr = { Status.ENABLED };
        return dao.find("Corp.group", new MapBean("permissionId", Permissions.RDR.getId(), "statuses", statusArr));
    }
}
