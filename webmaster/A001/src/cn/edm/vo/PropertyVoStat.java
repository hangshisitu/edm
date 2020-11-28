package cn.edm.vo;

import java.util.List;

import cn.edm.domain.LabelCategory;


public class PropertyVoStat {
	
	private LabelCategory labelCategory;
	
	private int labelNum;
	
	private int emailNum;
	
	
	
	private List<PropertyVo> propertyVoList;

	
	public LabelCategory getLabelCategory() {
		return labelCategory;
	}

	public void setLabelCategory(LabelCategory labelCategory) {
		this.labelCategory = labelCategory;
	}
	public int getLabelNum() {
		return labelNum;
	}

	public void setLabelNum(int labelNum) {
		this.labelNum = labelNum;
	}

	public int getEmailNum() {
		return emailNum;
	}

	public void setEmailNum(int emailNum) {
		this.emailNum = emailNum;
	}

	public List<PropertyVo> getPropertyVoList() {
		return propertyVoList;
	}

	public void setPropertyVoList(List<PropertyVo> propertyVoList) {
		this.propertyVoList = propertyVoList;
	}
	
}
