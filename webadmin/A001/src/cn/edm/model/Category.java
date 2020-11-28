package cn.edm.model;

import java.util.Date;

/**
 * 类别.
 * type(类型. 0:标签, 1:模板 ,2:表单)
 * 
 * @author yjli
 */
public class Category {

	public static final int TAG = 0;
	public static final int TEMPLATE = 1;
	public static final int FORM = 2; 

	private Integer categoryId;
	private Integer corpId;
	private String userId;
	private String categoryName;
	private String categoryDesc;
	private Integer type;
	private Date createTime;
	private Date modifyTime;
	
	private Integer count;
	private boolean isKeyword; 

	public Integer getCategoryId() {
		return categoryId;
	}

	public void setCategoryId(Integer categoryId) {
		this.categoryId = categoryId;
	}

	public Integer getCorpId() {
		return corpId;
	}

	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}

	public String getUserId() {
		return userId;
	}

	public void setUserId(String userId) {
		this.userId = userId;
	}

	public String getCategoryName() {
		return categoryName;
	}

	public void setCategoryName(String categoryName) {
		this.categoryName = categoryName;
	}

	public String getCategoryDesc() {
		return categoryDesc;
	}

	public void setCategoryDesc(String categoryDesc) {
		this.categoryDesc = categoryDesc;
	}

	public Integer getType() {
		return type;
	}

	public void setType(Integer type) {
		this.type = type;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}

	public Integer getCount() {
		return count;
	}

	public void setCount(Integer count) {
		this.count = count;
	}

	public boolean isKeyword() {
		return isKeyword;
	}

	public void setKeyword(boolean isKeyword) {
		this.isKeyword = isKeyword;
	}
}
