package cn.edm.persistence;


import java.util.List;
import java.util.Map;

import cn.edm.model.Category;

public interface CategoryMapper {
	
	void insert(Category category);
	
	void insertBatch(List<Category> categoryList);
	
	Category selectByUserIdAndCatalogName(Map<String,Object> paramsMap);
	
	void deleteCategoryByUserId(String userId);
}
