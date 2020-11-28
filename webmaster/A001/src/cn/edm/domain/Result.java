package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;
import java.util.List;

import cn.edm.utils.ArithUtil;

/**
 * 任务结果.
 * 
 * @author yjli
 */
public class Result implements Serializable{

	private static final long serialVersionUID = -8342904246765069395L;
	private Integer corpId;  // 不是企业ID，与其它表关联的ID
	private Integer taskId;
	private Integer templateId;
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
	private Date beginSendTime;
	private Date endSendTime;

	private String taskName;
	private Integer status;
	private Date deliveryTime;
	
	private String userId;
	private String companyName;
	private Date createTime;
	
	private String senderDomain;
	
	private String triggerName;
	
	private Integer labelStatus;
	
	private String isCanImport;
	
	private String subject;
	
	private String campaignName;
	private Integer taskNum;
	private Integer campaignId;
	
	private double successRate;
	private double openRate;
	private double clickRate;
	private String type;
	
	private Integer planId;
	
	private String planReferer;
	
	private Integer taskCount = 0;
	
	private Integer taskType; // 0为单项任务，1为测试任务，2为周期任务父任务（无投递效果），3为周期任务子任务（有投递效果），4为API群发任务
	
	private Integer touchCount = 0;
	
	private Integer templateCount = 0;
	
	private Integer touchId;
	
	private List<String> urlList;
	
	private String templateName;
	
	private String parentTaskName;
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
	
	private Double touchRate = 0d; // 触发率：该触点链接的点击人数 / 主模板任务的打开人数  = touchCount / readUserCount
	
	private String corpPath;//机构全称
	
	
	public String getCorpPath() {
		return corpPath;
	}

	public void setCorpPath(String corpPath) {
		this.corpPath = corpPath;
	}

	public void setResultReach(Double resultReach) {
		this.resultReach = resultReach;
	}
	
	public void setResultRead(Double resultRead) {
		this.resultRead = resultRead;
	}

	public void setResultClick(Double resultClick) {
		this.resultClick = resultClick;
	}
	
	public String getType() {
		return type;
	}

	public double getSuccessRate() {
		return successRate;
	}

	public void setSuccessRate(double successRate) {
		this.successRate = successRate;
	}

	public double getOpenRate() {
		return openRate;
	}

	public void setOpenRate(double openRate) {
		this.openRate = openRate;
	}

	public double getClickRate() {
		return clickRate;
	}

	public void setClickRate(double clickRate) {
		this.clickRate = clickRate;
	}

	public void setType(String type) {
		this.type = type;
	}

	public Integer getCampaignId() {
		return campaignId;
	}

	public void setCampaignId(Integer campaignId) {
		this.campaignId = campaignId;
	}

	public String getCampaignName() {
		return campaignName;
	}

	public void setCampaignName(String campaignName) {
		this.campaignName = campaignName;
	}

	public Integer getTaskNum() {
		return taskNum;
	}

	public void setTaskNum(Integer taskNum) {
		this.taskNum = taskNum;
	}

	public static long getSerialversionuid() {
		return serialVersionUID;
	}

	public String getSubject() {
		return subject;
	}

	public void setSubject(String subject) {
		this.subject = subject;
	}

	public String getIsCanImport() {
		return isCanImport;
	}

	public void setIsCanImport(String isCanImport) {
		this.isCanImport = isCanImport;
	}

	public Integer getLabelStatus() {
		return labelStatus;
	}

	public void setLabelStatus(Integer labelStatus) {
		this.labelStatus = labelStatus;
	}

	public String getTriggerName() {
		return triggerName;
	}

	public void setTriggerName(String triggerName) {
		this.triggerName = triggerName;
	}

	public String getSenderDomain() {
		return senderDomain;
	}

	public void setSenderDomain(String senderDomain) {
		this.senderDomain = senderDomain;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
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

	public Integer getSentCount() {
		if(sentCount==null) {
			sentCount = 0;
		}
		return sentCount;
	}

	public void setSentCount(Integer sentCount) {
		this.sentCount = sentCount;
	}

	public Integer getReachCount() {
		if(reachCount==null) {
			reachCount = 0;
		}
		return reachCount;
	}

	public void setReachCount(Integer reachCount) {
		this.reachCount = reachCount;
	}

	public Integer getReadCount() {
		return readCount;
	}

	public void setReadCount(Integer readCount) {
		this.readCount = readCount;
	}

	public Integer getReadUserCount() {
		if(readUserCount==null) {
			readUserCount = 0;
		}
		return readUserCount;
	}

	public void setReadUserCount(Integer readUserCount) {
		this.readUserCount = readUserCount;
	}

	public Integer getClickCount() {
		return clickCount;
	}

	public void setClickCount(Integer clickCount) {
		this.clickCount = clickCount;
	}

	public Integer getClickUserCount() {
		if(clickUserCount==null) {
			clickUserCount = 0;
		}
		return clickUserCount;
	}

	public void setClickUserCount(Integer clickUserCount) {
		this.clickUserCount = clickUserCount;
	}

	public Integer getUnsubscribeCount() {
		return unsubscribeCount;
	}

	public void setUnsubscribeCount(Integer unsubscribeCount) {
		this.unsubscribeCount = unsubscribeCount;
	}

	public Integer getForwardCount() {
		return forwardCount;
	}

	public void setForwardCount(Integer forwardCount) {
		this.forwardCount = forwardCount;
	}

	public Integer getSoftBounceCount() {
		if(softBounceCount==null) {
			softBounceCount = 0;
		}
		return softBounceCount;
	}

	public void setSoftBounceCount(Integer softBounceCount) {
		this.softBounceCount = softBounceCount;
	}

	public Integer getHardBounceCount() {
		if(hardBounceCount==null) {
			hardBounceCount = 0;
		}
		return hardBounceCount;
	}

	public void setHardBounceCount(Integer hardBounceCount) {
		this.hardBounceCount = hardBounceCount;
	}

	public Integer getBeforeNoneCount() {
		return beforeNoneCount;
	}

	public void setBeforeNoneCount(Integer beforeNoneCount) {
		this.beforeNoneCount = 0;
	}

	public Integer getAfterNoneCount() {
		if(afterNoneCount==null) {
			afterNoneCount = 0;
		}
		return afterNoneCount;
	}

	public void setAfterNoneCount(Integer afterNoneCount) {
		this.afterNoneCount = afterNoneCount;
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

	public Date getBeginSendTime() {
		return beginSendTime;
	}

	public void setBeginSendTime(Date beginSendTime) {
		this.beginSendTime = beginSendTime;
	}

	public Date getEndSendTime() {
		return endSendTime;
	}

	public void setEndSendTime(Date endSendTime) {
		this.endSendTime = endSendTime;
	}

	public String getTaskName() {
		return taskName;
	}

	public void setTaskName(String taskName) {
		this.taskName = taskName;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}

	public Date getDeliveryTime() {
		return deliveryTime;
	}

	public void setDeliveryTime(Date deliveryTime) {
		this.deliveryTime = deliveryTime;
	}

	public Integer getPlanId() {
		return planId;
	}

	public void setPlanId(Integer planId) {
		this.planId = planId;
	}

	public Integer getTaskCount() {
		return taskCount;
	}

	public void setTaskCount(Integer taskCount) {
		this.taskCount = taskCount;
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

	public String getPlanReferer() {
		return planReferer;
	}

	public void setPlanReferer(String planReferer) {
		this.planReferer = planReferer;
	}

	public Integer getTaskType() {
		return taskType;
	}

	public void setTaskType(Integer taskType) {
		this.taskType = taskType;
	}

	public Integer getTouchCount() {
		return touchCount;
	}

	public void setTouchCount(Integer touchCount) {
		this.touchCount = touchCount;
	}

	public Integer getTemplateCount() {
		return templateCount;
	}

	public void setTemplateCount(Integer templateCount) {
		this.templateCount = templateCount;
	}

	public Integer getTouchId() {
		return touchId;
	}

	public void setTouchId(Integer touchId) {
		this.touchId = touchId;
	}

	public List<String> getUrlList() {
		return urlList;
	}

	public void setUrlList(List<String> urlList) {
		this.urlList = urlList;
	}

	public String getTemplateName() {
		return templateName;
	}

	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}

	public Double getTouchRate() {
		touchRate = (this.readUserCount==0 ? 0 : ArithUtil.div(this.touchCount * 100, readUserCount));
		return touchRate;
	}

	public void setTouchRate(Double touchRate) {
		this.touchRate = touchRate;
	}

	public String getParentTaskName() {
		return parentTaskName;
	}

	public void setParentTaskName(String parentTaskName) {
		this.parentTaskName = parentTaskName;
	}

}
