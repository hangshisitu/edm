package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.model.Category;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class CategoryService {

    @Autowired
    private Dao dao;

    public void save(Category category) {
        if (category.getCategoryId() != null) {
            dao.update("Category.update", category);
        } else {
            dao.save("Category.save", category);
        }
	}

    public void delete(String userId, Integer categoryId, Integer type) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        mb.put("categoryId", categoryId);
        mb.put("type", type);
        dao.delete("Category.delete",mb);
    }

    public void delete(String userId, Integer[] categoryIds, Integer type) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        mb.put("categoryIds", categoryIds);
        mb.put("type", type);
        dao.delete("Category.delete", mb);
    }

    public Category get(Integer corpId, String userId, Integer categoryId, Integer type) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("categoryId", categoryId);
        mb.put("type", type);
        return dao.get("Category.query", mb);
    }
    
	public Category get(Integer corpId, String userId, String categoryName, Integer type) {
	    MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("categoryName", categoryName);
        mb.put("type", type);
        mb.put("nameCnd", Cnds.EQ);
		return dao.get("Category.query", mb);
	}

    public List<Category> find(Integer[] corpIds, Integer corpId, Integer type, String categoryName, String nameCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("categoryName", categoryName);
        mb.put("type", type);
        mb.put("nameCnd", nameCnd);
		Pages.order(mb, "modifyTime", Page.DESC);
		return dao.find("Category.query", mb);
	}
    
    public List<Category> find(Integer[] corpIds, Integer corpId, String userId, Integer type, String categoryName, String nameCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("categoryName", categoryName);
        mb.put("type", type);
        mb.put("nameCnd", nameCnd);
		Pages.order(mb, "modifyTime", Page.DESC);
		return dao.find("Category.query", mb);
	}
    
    public List<Category> find(Integer corpId, String userId, Integer type, String categoryName, String nameCnd) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "corpId", corpId);
        mb.put("categoryName", categoryName);
        mb.put("type", type);
        mb.put("nameCnd", nameCnd);
		Pages.order(mb, "modifyTime", Page.DESC);
		return dao.find("Category.query", mb);
	}
    
    
    public List<Category> find(String userId, Integer[] categoryIds, Integer type) {
        MapBean mb = new MapBean();
        Pages.put(mb, "userId", userId);
        mb.put("categoryIds", categoryIds);
        mb.put("type", type);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Category.query", mb);
    }
    
    public Page<Category> search(Page<Category> page, MapBean mb, Integer[] corpIds, Integer corpId, String categoryName, Integer type, String beginTime, String endTime) {
        Pages.put(mb, "categoryName", categoryName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.put(mb, "type", type);
        
        if (Property.getStr(Config.CATEGORY_BILL).equals("off")) {
            Pages.put(mb, "categoryName", CategoryMap.BILL.getName());
            Pages.put(mb, "nameCnd", Cnds.NOT_EQ);
        }
        
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Category.count", "Category.index");
    }
    
    public boolean unique(Integer corpId, String categoryName, String orgCategoryName, Integer type) {
        if (categoryName == null || categoryName.equals(orgCategoryName)) {
            return true;
        }
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        mb.put("categoryName", categoryName);
        mb.put("type", type);
        mb.put("nameCnd", Cnds.EQ);
        long count = dao.countResult("Category.count", mb);
        return count == 0;
    }
    
	public List<Category> tagCount(Integer[] corpIds, Integer corpId, Integer[] categoryIds) {
	    MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("categoryIds", categoryIds);
		return dao.find("Category.tagCount", mb);
	}
	
	public List<Category> templateCount(Integer[] corpIds, Integer corpId, Integer[] categoryIds) {
	    MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("categoryIds", categoryIds);
		return dao.find("Category.templateCount", mb);
	}
	
	public List<Category> labelFind() {
        MapBean mb = new MapBean();
        Pages.order(mb, "categoryId", Page.ASC);
		return dao.find("Category.labelQuery", mb);
	}
}
