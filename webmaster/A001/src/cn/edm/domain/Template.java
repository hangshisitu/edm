package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

public class Template implements Serializable{
	private static final long serialVersionUID = 7545622577107067639L;
	private Integer templateId;
	private Integer corpId;
	private String userId;
	private Integer categoryId;
	private String templateName;
	private String filePath;
	private String subject;
	private String senderName;
	private String senderEmail;
	private String reveiver;
	private String robotEmail;
	private Integer isRand;
	private String attrs;
	private Date createTime;
	private Date modifyTime;

	private String content;
	
	private String categoryName;

	public String getCategoryName() {
		return categoryName;
	}

	public void setCategoryName(String categoryName) {
		this.categoryName = categoryName;
	}

	public Integer getTemplateId() {
		return templateId;
	}

	public void setTemplateId(Integer templateId) {
		this.templateId = templateId;
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

	public Integer getCategoryId() {
		return categoryId;
	}

	public void setCategoryId(Integer categoryId) {
		this.categoryId = categoryId;
	}

	public String getTemplateName() {
		return templateName;
	}

	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}

	public String getFilePath() {
		return filePath;
	}

	public void setFilePath(String filePath) {
		this.filePath = filePath;
	}

	public String getSubject() {
		return subject;
	}

	public void setSubject(String subject) {
		this.subject = subject;
	}

	public String getSenderName() {
		return senderName;
	}

	public void setSenderName(String senderName) {
		this.senderName = senderName;
	}

	public String getSenderEmail() {
		return senderEmail;
	}

	public void setSenderEmail(String senderEmail) {
		this.senderEmail = senderEmail;
	}

	public String getReveiver() {
		return reveiver;
	}

	public void setReveiver(String reveiver) {
		this.reveiver = reveiver;
	}

	public String getRobotEmail() {
		return robotEmail;
	}

	public void setRobotEmail(String robotEmail) {
		this.robotEmail = robotEmail;
	}

	public Integer getIsRand() {
		return isRand;
	}

	public void setIsRand(Integer isRand) {
		this.isRand = isRand;
	}

	public String getAttrs() {
		return attrs;
	}

	public void setAttrs(String attrs) {
		this.attrs = attrs;
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

	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}
}
