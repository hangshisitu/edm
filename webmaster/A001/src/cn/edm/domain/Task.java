package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

/**
 * 任务.
 * 
 * @author yjli
 */
public class Task implements Serializable{

	private static final long serialVersionUID = 527877157695503917L;
	private Integer taskId;
	private Integer corpId;
	private String userId;
	private Integer templateId;
	private String taskName;
	private String subject;
	private String senderName;
	private String senderEmail;
	private String receiver;
	private Integer emailCount;

	private String auditPath;
	private String moderatePath;
	private String suggestion;
	private String resourceId;
	private Integer type;
	private Date jobTime;
	private Integer isCallback;
	private Integer status;
	private Date createTime;
	private Date modifyTime;
	private Date deliveryTime;
	
	private Integer labelStatus; 
	
	private Integer campaignId;
	
	private String campaignName;
	
	private Integer priority; // 任务优先级，默认为：0 ，优先级：1

	private Integer planId; // 周期任务ID
	/**
	 * 当planReferer为投递效果的任务时，值为
	 * taskId#yyy-MM-dd
	 * 如任务ID为110，投递时间为2014-06-11
	 * 则 planReferer = 110#2014-06-11
	 */
	private String planReferer; // 周期任务引用
	
	private Integer touchCount;
	
	private String corpPath;//机构全称
	
	public String getCorpPath() {
		return corpPath;
	}

	public void setCorpPath(String corpPath) {
		this.corpPath = corpPath;
	}

	public String getCampaignName() {
		return campaignName;
	}

	public void setCampaignName(String campaignName) {
		this.campaignName = campaignName;
	}

	public Integer getCampaignId() {
		return campaignId;
	}

	public void setCampaignId(Integer campaignId) {
		this.campaignId = campaignId;
	}

	public Integer getLabelStatus() {
		return labelStatus;
	}

	public void setLabelStatus(Integer labelStatus) {
		this.labelStatus = labelStatus;
	}

	private String companyName;

	public String getCompanyName() {
		return companyName;
	}

	public void setCompanyName(String companyName) {
		this.companyName = companyName;
	}

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

	public String getUserId() {
		return userId;
	}

	public void setUserId(String userId) {
		this.userId = userId;
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

	public String getReceiver() {
		return receiver;
	}

	public void setReceiver(String receiver) {
		this.receiver = receiver;
	}

	public Integer getEmailCount() {
		return emailCount;
	}

	public void setEmailCount(Integer emailCount) {
		this.emailCount = emailCount;
	}

	

	public String getAuditPath() {
		return auditPath;
	}

	public void setAuditPath(String auditPath) {
		this.auditPath = auditPath;
	}

	public String getModeratePath() {
		return moderatePath;
	}

	public void setModeratePath(String moderatePath) {
		this.moderatePath = moderatePath;
	}

	public String getSuggestion() {
		return suggestion;
	}

	public void setSuggestion(String suggestion) {
		this.suggestion = suggestion;
	}

	public String getResourceId() {
		return resourceId;
	}

	public void setResourceId(String resourceId) {
		this.resourceId = resourceId;
	}

	public Integer getType() {
		return type;
	}

	public void setType(Integer type) {
		this.type = type;
	}
	
	public Date getJobTime() {
		return jobTime;
	}

	public void setJobTime(Date jobTime) {
		this.jobTime = jobTime;
	}

	public Integer getIsCallback() {
		return isCallback;
	}

	public void setIsCallback(Integer isCallback) {
		this.isCallback = isCallback;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
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

	public Date getDeliveryTime() {
		return deliveryTime;
	}

	public void setDeliveryTime(Date deliveryTime) {
		this.deliveryTime = deliveryTime;
	}

	public Integer getPriority() {
		return priority;
	}

	public void setPriority(Integer priority) {
		this.priority = priority;
	}

	public Integer getPlanId() {
		return planId;
	}

	public void setPlanId(Integer planId) {
		this.planId = planId;
	}

	public String getPlanReferer() {
		return planReferer;
	}

	public void setPlanReferer(String planReferer) {
		this.planReferer = planReferer;
	}

	public Integer getTouchCount() {
		return touchCount;
	}

	public void setTouchCount(Integer touchCount) {
		this.touchCount = touchCount;
	}
}
