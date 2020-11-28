package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Value;
import cn.edm.model.Copy;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.repository.TagDao;
import cn.edm.utils.Values;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class TagService {

	@Autowired
	private Dao dao;
	@Autowired
	private TagDao tagDao;
   
    public void save(Tag tag) {
        if (tag.getTagId() != null) {
            dao.update("Tag.update", tag);
        } else {
            dao.save("Tag.save", tag);
        }
    }
    
    public void delete(String userId, Integer tagId) {
        dao.delete("Tag.delete", new MapBean("userId", userId, "tagId", tagId));
    }

    public void delete(String userId, Integer[] tagIds) {
        dao.delete("Tag.delete", new MapBean("userId", userId, "tagIds", tagIds));
    }
    
    public void delete(Tag tag) {
        int dbId = Values.get(tag.getDbId());
    	tagDao.clear(tag);
		delete(tag.getUserId(), tag.getTagId());
		
		// CopyTrigger触发
        if (dbId > 0) {
            dao.save("CopyTag.save", new Copy(tag.getCorpId(), tag.getDbId(), tag.getTagId(), Copy.CLEAR));
        }
	}
    
	public void clear(Tag tag) {
	    int dbId = Values.get(tag.getDbId());
		tagDao.clear(tag);
		tag.setDbId(Value.I);
		tag.setEmailCount(Value.I);
		save(tag);
		
		// CopyTrigger触发
		if (dbId > 0) {
		    dao.save("CopyTag.save", new Copy(tag.getCorpId(), tag.getDbId(), tag.getTagId(), Copy.CLEAR));
		}
	}
	
    public Tag get(Integer tagId) {
        return dao.get("Tag.query", new MapBean("tagId", tagId));
    }
	
    public Tag get(Integer corpId, String userId, Integer tagId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("tagId", tagId);
        return dao.get("Tag.query", mb);
    }
    
    public Tag get(Integer corpId, String userId, String tagName) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("tagName", tagName);
        mb.put("nameCnd", Cnds.EQ);
        return dao.get("Tag.query", mb);
    }
    
    public List<Tag> find(Integer corpId) {
        return dao.find("Tag.query", new MapBean("corpId", corpId));
    }
    
    public List<Tag> find(Integer corpId, String userId) {
        return dao.find("Tag.query", new MapBean("corpId", corpId, "userId", userId));
    }
    
    public List<Tag> find(Integer corpId, String categoryName, String categoryCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "categoryName", categoryName);
        Pages.put(mb, "categoryCnd", categoryCnd);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Tag.query", mb);
    }

	public List<Tag> find(Integer corpId, String userId, Integer[] tagIds) {
	    MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "tagIds", tagIds);
		return dao.find("Tag.query", mb);
	}
	
	public List<Tag> find(Integer corpId, Integer categoryId) {
		return dao.find("Tag.query", new MapBean("corpId", corpId, "categoryId", categoryId));
	}
	
	public List<Tag> find(String userId, Integer categoryId, int day) {
		return dao.find("Tag.query", new MapBean("userId", userId, "categoryId", categoryId, "day", day));
	}
    public Page<Tag> search(Page<Tag> page, MapBean mb, Integer corpId, String tagName, String beginTime, String endTime) {
        Pages.put(mb, "tagName", tagName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
		Pages.search(mb, page);
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Tag.count", "Tag.index");
    }
    
    public boolean unique(Integer corpId, String tagName, String orgTagName) {
        if (tagName == null || tagName.equals(orgTagName)) {
            return true;
        }
        
        long count = dao.countResult("Tag.count", new MapBean("corpId", corpId, "tagName", tagName, "nameCnd", Cnds.EQ));
        return count == 0;
    }
    
    public boolean lockForm(int corpId, Integer[] tagIds, String categoryName, String categoryCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        mb.put("tagIds", tagIds);
        mb.put("categoryName", categoryName);
        mb.put("categoryCnd", categoryCnd);
        long count = dao.countResult("Tag.count", mb);
        return count > 0;
    }
    
    public int maxDbId() {
    	int maxDbId = Values.get((Integer) dao.get("Tag.maxDbId", Value.I));
		return maxDbId > 0 ? maxDbId : 1;
	}
}
