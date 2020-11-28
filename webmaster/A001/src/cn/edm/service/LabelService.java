package cn.edm.service;

import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import cn.edm.domain.Label;
import cn.edm.domain.LabelCategory;
import cn.edm.vo.PropertyVo;
import cn.edm.vo.PropertyVoStat;

public interface LabelService {
	
	void save(Label label);
	
	List<Label> getByCategoryId(Integer categoryId);
	
	String getLabelId(List<Label> labelList);
	
	List<PropertyVo> getPropertyVo(String content,List<Label> labelList);
	
	PropertyVoStat stat(LabelCategory lc,List<PropertyVo> propertyVoList,int labelNum);
	
	boolean isLabelExist(String labelName);
	
	Map<String, Object> verifyXss(Label label);
	
	List<Label> getAll();
	
	void importPropLib(Map<String,Object> paramsMap);
	
	List<PropertyVo> getPropertyIndexVo(String content,List<Label> labelList);
	
	List<String> getLabelIds(String includeIds,String excludeIds);
	
	List<Label> getLabelByIds(Map<String,Object> params);
	
	Map<String,Object> generateFile(HttpServletRequest request,List<Label> labelList,String[] date,String[] score,String cnd,String excludeTagIds);
	
}
