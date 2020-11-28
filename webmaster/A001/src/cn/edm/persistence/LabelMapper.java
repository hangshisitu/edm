package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Label;




public interface LabelMapper {
	
	void insert(Label label);
	
	List<Label> selectByCategoryId(Integer categoryId);
	
	List<Label> selectByName(String labelName);
	
	List<Label> selectAll();
	
	List<Label> selectByLabelIds(Map<String,Object> params);
}
