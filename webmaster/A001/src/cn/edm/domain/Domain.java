package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;
import java.util.List;

import cn.edm.utils.ArithUtil;

/**
 * 域名统计.
 * 
 * @author yjli
 */
public class Domain implements Serializable{

	private static final long serialVersionUID = -7742458371052626864L;
	private Integer corpId;
	private Integer taskId;
	private Integer templateId;
	private String emailDomain;
	private Integer sentCount = 0;
	private Integer reachCount = 0;
	private Integer readCount = 0;
	private Integer readUserCount = 0;
	private Integer clickCount = 0;
	private Integer clickUserCount = 0;
	private Integer unsubscribeCount = 0;
	private Integer forwardCount = 0;
	private Integer softBounceCount = 0;
	private Integer hardBounceCount = 0;
	private Integer beforeNoneCount = 0;
	private Integer afterNoneCount = 0;
	private Integer dnsFailCount = 0;
	private Integer spamGarbageCount = 0;
	
	private String taskName;
	
	private String subject;
	
	private Date createTime;
	
	private String senderDomain;
	
	private String triggerName;
	
	private Integer labelStatus;
	
	private Date deliveryTime;
	
	private Integer status;
	
	private String isCanImport;
	
	private String type;
	
	private Integer campaignId;
	
	private Integer planId;
	
	private String planReferer;
	
	private Integer taskCount = 0;
	
	private Integer taskType; // 0为单项任务，1为测试任务，2为周期任务父任务（无投递效果），3为周期任务子任务（有投递效果），4为API群发任务
	
	private String templateName;
	
	private Integer touchCount = 0; // 触发总数
	
	private Integer templateCount = 0;
	
	private List<String> urlList;
	
	private Integer touchId;
	
	private Integer parentReadUserCount = 0;
	/**
     * 成功数百分比.
     */
	private Double resultReach = 0d;
	/**
     * 打开百分比.
     */
	private Double resultRead = 0d;
	/**
     * 点击百分比.
     */
	private Double resultClick = 0d;
	/**
	 * 触发率：该触点链接的点击人数 / 主模板任务的打开人数  = touchCount / readUserCount
	 */
	private Double touchRate = 0d; 
	
	private String corpPath;//机构全称
		
	public String getCorpPath() {
		return corpPath;
	}

	public void setCorpPath(String corpPath) {
		this.corpPath = corpPath;
	}

	public Integer getDnsFailCount() {
		return dnsFailCount;
	}

	public void setDnsFailCount(Integer dnsFailCount) {
		this.dnsFailCount = dnsFailCount;
	}

	public Integer getSpamGarbageCount() {
		return spamGarbageCount;
	}

	public void setSpamGarbageCount(Integer spamGarbageCount) {
		this.spamGarbageCount = spamGarbageCount;
	}

	private String userId;
	private String companyName;
	
	private String successRate;
	private String openRate;
	private String clickRate;

	public String getSuccessRate() {
		if(successRate==null) {
			successRate = "0.00%";
		}
		return successRate;
	}

	public void setSuccessRate(String successRate) {
		this.successRate = successRate;
	}

	public String getOpenRate() {
		if(openRate==null) {
			openRate = "0.00%";
		}
		return openRate;
	}

	public void setOpenRate(String openRate) {
		this.openRate = openRate;
	}

	public String getClickRate() {
		if(clickRate==null) {
			clickRate = "0.00%";
		}
		return clickRate;
	}

	public void setClickRate(String clickRate) {
		this.clickRate = clickRate;
	}

	public String getUserId() {
		return userId;
	}

	public void setUserId(String userId) {
		this.userId = userId;
	}

	public String getCompanyName() {
		return companyName;
	}

	public void setCompanyName(String companyName) {
		this.companyName = companyName;
	}

	public Integer getCorpId() {
		return corpId;
	}

	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}

	public Integer getTaskId() {
		return taskId;
	}

	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}

	public Integer getTemplateId() {
		return templateId;
	}

	public void setTemplateId(Integer templateId) {
		this.templateId = templateId;
	}

	public String getEmailDomain() {
		return emailDomain;
	}

	public void setEmailDomain(String emailDomain) {
		this.emailDomain = emailDomain;
	}

	public Integer getSentCount() {
		if(sentCount==null)
			sentCount = 0;
		return sentCount;
	}

	public void setSentCount(Integer sentCount) {
		this.sentCount = sentCount;
	}

	public Integer getReachCount() {
		if(reachCount==null)
			reachCount = 0;
		return reachCount;
	}

	public void setReachCount(Integer reachCount) {
		this.reachCount = reachCount;
	}

	public Integer getReadCount() {
		if(readCount==null)
			readCount = 0;
		return readCount;
	}

	public void setReadCount(Integer readCount) {
		this.readCount = readCount;
	}

	public Integer getReadUserCount() {
		if(readUserCount==null)
			readUserCount = 0;
		return readUserCount;
	}

	public void setReadUserCount(Integer readUserCount) {
		this.readUserCount = readUserCount;
	}

	public Integer getClickCount() {
		if(clickCount==null)
			clickCount = 0;
		return clickCount;
	}

	public void setClickCount(Integer clickCount) {
		this.clickCount = clickCount;
	}

	public Integer getClickUserCount() {
		if(clickUserCount==null)
			clickUserCount = 0;
		return clickUserCount;
	}

	public void setClickUserCount(Integer clickUserCount) {
		this.clickUserCount = clickUserCount;
	}

	public Integer getUnsubscribeCount() {
		if(unsubscribeCount==null)
			unsubscribeCount = 0;
		return unsubscribeCount;
	}

	public void setUnsubscribeCount(Integer unsubscribeCount) {
		this.unsubscribeCount = unsubscribeCount;
	}

	public Integer getForwardCount() {
		if(forwardCount==null)
			forwardCount = 0;
		return forwardCount;
	}

	public void setForwardCount(Integer forwardCount) {
		this.forwardCount = forwardCount;
	}

	public Integer getSoftBounceCount() {
		if(softBounceCount==null)
			softBounceCount = 0;
		return softBounceCount;
	}

	public void setSoftBounceCount(Integer softBounceCount) {
		this.softBounceCount = softBounceCount;
	}

	public Integer getHardBounceCount() {
		if(hardBounceCount==null)
			hardBounceCount = 0;
		return hardBounceCount;
	}

	public void setHardBounceCount(Integer hardBounceCount) {
		this.hardBounceCount = hardBounceCount;
	}

	public Integer getBeforeNoneCount() {
		if(beforeNoneCount==null)
			beforeNoneCount = 0;
		return 0;
	}

	public void setBeforeNoneCount(Integer beforeNoneCount) {
		this.beforeNoneCount = 0;
	}

	public Integer getAfterNoneCount() {
		if(afterNoneCount==null)
			afterNoneCount = 0;
		return afterNoneCount;
	}

	public void setAfterNoneCount(Integer afterNoneCount) {
		this.afterNoneCount = afterNoneCount;
	}

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
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

	public Integer getTaskCount() {
		return taskCount;
	}

	public void setTaskCount(Integer taskCount) {
		this.taskCount = taskCount;
	}

	public Integer getTaskType() {
		return taskType;
	}

	public void setTaskType(Integer taskType) {
		this.taskType = taskType;
	}

	public void setResultReach(Double resultReach) {
		this.resultReach = resultReach;
	}

	public void setResultRead(Double resultRead) {
		this.resultRead = resultRead;
	}

	public Double getResultReach() {
        int sum = this.sentCount - this.afterNoneCount;
		resultReach = (sum==0 ? 0 : ArithUtil.div(this.reachCount * 100, sum));
		return resultReach;
	}

	public Double getResultRead() {
		resultRead = (reachCount==0 ? 0 : ArithUtil.div(this.readUserCount * 100, reachCount));
		return resultRead;
	}

	public Double getResultClick() {
		resultClick = (this.readUserCount==0 ? 0 : ArithUtil.div(this.clickUserCount * 100, readUserCount));
		return resultClick;
	}

	public void setResultClick(Double resultClick) {
		this.resultClick = resultClick;
	}

	public String getIsCanImport() {
		return isCanImport;
	}

	public void setIsCanImport(String isCanImport) {
		this.isCanImport = isCanImport;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
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

	public Date getDeliveryTime() {
		return deliveryTime;
	}

	public void setDeliveryTime(Date deliveryTime) {
		this.deliveryTime = deliveryTime;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}

	public String getSenderDomain() {
		return senderDomain;
	}

	public void setSenderDomain(String senderDomain) {
		this.senderDomain = senderDomain;
	}

	public String getTriggerName() {
		return triggerName;
	}

	public void setTriggerName(String triggerName) {
		this.triggerName = triggerName;
	}

	public Integer getLabelStatus() {
		return labelStatus;
	}

	public void setLabelStatus(Integer labelStatus) {
		this.labelStatus = labelStatus;
	}

	public String getTemplateName() {
		return templateName;
	}

	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}

	public Integer getTouchCount() {
		return touchCount;
	}

	public void setTouchCount(Integer touchCount) {
		this.touchCount = touchCount;
	}

	public Double getTouchRate() {
		touchRate = (this.parentReadUserCount==0 ? 0 : ArithUtil.div(this.touchCount * 100, parentReadUserCount));
		return touchRate;
	}

	public void setTouchRate(Double touchRate) {
		this.touchRate = touchRate;
	}

	public List<String> getUrlList() {
		return urlList;
	}

	public void setUrlList(List<String> urlList) {
		this.urlList = urlList;
	}

	public Integer getTouchId() {
		return touchId;
	}

	public void setTouchId(Integer touchId) {
		this.touchId = touchId;
	}

	public Integer getCampaignId() {
		return campaignId;
	}

	public void setCampaignId(Integer campaignId) {
		this.campaignId = campaignId;
	}

	public Integer getTemplateCount() {
		return templateCount;
	}

	public void setTemplateCount(Integer templateCount) {
		this.templateCount = templateCount;
	}

	public Integer getParentReadUserCount() {
		return parentReadUserCount;
	}

	public void setParentReadUserCount(Integer parentReadUserCount) {
		this.parentReadUserCount = parentReadUserCount;
	}

}
