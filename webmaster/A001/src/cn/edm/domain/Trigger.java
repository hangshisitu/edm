package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

/**
 * 触发任务
 * 
 */
public class Trigger implements Serializable{

	private static final long serialVersionUID = 527877157695503917L;
	private Integer taskId;
	private Integer corpId; // 不是企业ID，是与triger_api表的triger_corp_id与result表的corp_id对应
	private Integer templateId;
	private String taskName;
	private String subject;
	private String senderName;
	private String senderEmail;
	private Integer emailCount;
	private String resourceId;
//	private Integer type;
	private Date createTime;
	public Integer getTaskId() {
		return taskId;
	}
	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}
	public Integer getCorpId() {
		return corpId;
	}
	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}
	public Integer getTemplateId() {
		return templateId;
	}
	public void setTemplateId(Integer templateId) {
		this.templateId = templateId;
	}
	public String getTaskName() {
		return taskName;
	}
	public void setTaskName(String taskName) {
		this.taskName = taskName;
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
	public Integer getEmailCount() {
		return emailCount;
	}
	public void setEmailCount(Integer emailCount) {
		this.emailCount = emailCount;
	}
	public String getResourceId() {
		return resourceId;
	}
	public void setResourceId(String resourceId) {
		this.resourceId = resourceId;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
//	public Integer getType() {
//		return type;
//	}
//	public void setType(Integer type) {
//		this.type = type;
//	}
	
}
