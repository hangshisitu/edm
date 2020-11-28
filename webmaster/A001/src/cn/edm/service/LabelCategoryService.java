package cn.edm.service;

import java.util.List;

import cn.edm.domain.LabelCategory;

public interface LabelCategoryService {
	
	List<LabelCategory> getAll();
	
	LabelCategory getById(Integer id);
}
