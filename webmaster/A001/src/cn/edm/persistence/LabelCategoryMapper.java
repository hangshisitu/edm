package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.LabelCategory;




public interface LabelCategoryMapper {
	
	List<LabelCategory> selectAll();
	
	LabelCategory selectById(Integer id);
}
