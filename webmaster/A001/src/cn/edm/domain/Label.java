package cn.edm.domain;

import java.io.Serializable;

public class Label implements Serializable{

	private static final long serialVersionUID = -1933704801401192124L;
	
	private Integer lableId;
	private Integer parentId;
	private Integer categoryId;
	private String labelName;
	private String labelDesc;
	
	public Integer getLableId() {
		return lableId;
	}
	public void setLableId(Integer lableId) {
		this.lableId = lableId;
	}
	public Integer getParentId() {
		return parentId;
	}
	public void setParentId(Integer parentId) {
		this.parentId = parentId;
	}
	public Integer getCategoryId() {
		return categoryId;
	}
	public void setCategoryId(Integer categoryId) {
		this.categoryId = categoryId;
	}
	public String getLabelName() {
		return labelName;
	}
	public void setLabelName(String labelName) {
		this.labelName = labelName;
	}
	public String getLabelDesc() {
		return labelDesc;
	}
	public void setLabelDesc(String labelDesc) {
		this.labelDesc = labelDesc;
	}
	
}
