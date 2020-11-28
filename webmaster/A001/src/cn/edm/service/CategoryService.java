package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.constant.Cnds;
import cn.edm.model.Category;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
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

    public void delete(int corpId, String userId, Integer categoryId, Integer type) {
        dao.delete("Category.delete", new MapBean("corpId", corpId, "userId", userId, "categoryId", categoryId, "type", type));
    }

    public void delete(int corpId, String userId, Integer[] categoryIds, Integer type) {
        dao.delete("Category.delete", new MapBean("corpId", corpId, "userId", userId, "categoryIds", categoryIds, "type", type));
    }

    public Category get(int corpId, String userId, Integer categoryId, Integer type) {
        return dao.get("Category.query", new MapBean("corpId", corpId, "userId", userId, "categoryId", categoryId, "type", type));
    }
    
	public Category get(int corpId, String userId, String categoryName, Integer type) {
		return dao.get("Category.query", new MapBean("corpId", corpId, "userId", userId, "categoryName", categoryName, "type", type, "nameCnd", Cnds.EQ));
	}

    public List<Category> find(int corpId, String userId, Integer type, String categoryName, String nameCnd) {
		MapBean mb = new MapBean();
		mb.put("corpId", corpId);
		mb.put("userId", userId);
		mb.put("type", type);
		Pages.put(mb, "categoryName", categoryName);
		Pages.put(mb, "nameCnd", nameCnd);
		Pages.order(mb, "modifyTime", Page.DESC);
		return dao.find("Category.query", mb);
	}
    
    public List<Category> find(int corpId, String userId, Integer[] categoryIds, Integer type) {
		MapBean mb = new MapBean();
		mb.put("corpId", corpId);
		mb.put("userId", userId);
		mb.put("categoryIds", categoryIds);
		mb.put("type", type);
		Pages.order(mb, "modifyTime", Page.DESC);
		return dao.find("Category.query", mb);
	}
    
    public List<Category> find(int corpId, String userId, Integer type) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        mb.put("userId", userId);
        mb.put("type", type);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Category.query", mb);
    }

    public Page<Category> search(Page<Category> page, MapBean mb, int corpId, String userId, String categoryName, Integer type, String beginTime, String endTime) {
        Pages.put(mb, "categoryName", categoryName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.put(mb, "type", type);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Category.count", "Category.index");
    }
    
    public boolean unique(int corpId, String userId, String categoryName, String orgCategoryName, Integer type) {
        if (categoryName == null || categoryName.equals(orgCategoryName)) {
            return true;
        }
        
        long count = dao.countResult("Category.count", new MapBean("corpId", corpId, "userId", userId, "categoryName", categoryName, "type", type, "nameCnd", Cnds.EQ));
        return count == 0;
    }
    
	public List<Category> tagCount(int corpId, String userId, Integer[] categoryIds) {
		return dao.find("Category.tagCount", new MapBean("corpId", corpId, "userId", userId, "categoryIds", categoryIds));
	}
	
	public List<Category> templateCount(int corpId, String userId, Integer[] categoryIds) {
		return dao.find("Category.templateCount", new MapBean("corpId", corpId, "userId", userId, "categoryIds", categoryIds));
	}
	
	public List<Category> labelFind() {
        MapBean mb = new MapBean();
        Pages.order(mb, "categoryId", Page.ASC);
		return dao.find("Category.labelQuery", mb);
	}
}
