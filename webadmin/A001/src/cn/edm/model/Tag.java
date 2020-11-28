package cn.edm.model;

import java.util.Date;

/**
 * 标签.
 * dbId(MongoDB库, 库名格式:tag_n)
 * 
 * @author yjli
 */
public class Tag {

	private Integer tagId;
	private Integer dbId;
	private Integer categoryId;
	private Integer corpId;
	private String userId;
	private String tagName;
	private String tagDesc;
	private Integer emailCount;
	private Date createTime;
	private Date modifyTime;
	
	private String categoryName;
	private boolean isKeyword; 

	public Integer getTagId() {
		return tagId;
	}

	public void setTagId(Integer tagId) {
		this.tagId = tagId;
	}

	public Integer getDbId() {
		return dbId;
	}

	public void setDbId(Integer dbId) {
		this.dbId = dbId;
	}

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

	public String getTagName() {
		return tagName;
	}

	public void setTagName(String tagName) {
		this.tagName = tagName;
	}

	public String getTagDesc() {
		return tagDesc;
	}

	public void setTagDesc(String tagDesc) {
		this.tagDesc = tagDesc;
	}

	public Integer getEmailCount() {
		return emailCount;
	}

	public void setEmailCount(Integer emailCount) {
		this.emailCount = emailCount;
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

	public String getCategoryName() {
		return categoryName;
	}

	public void setCategoryName(String categoryName) {
		this.categoryName = categoryName;
	}

	public boolean isKeyword() {
		return isKeyword;
	}

	public void setKeyword(boolean isKeyword) {
		this.isKeyword = isKeyword;
	}

}
