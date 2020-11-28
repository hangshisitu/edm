package cn.edm.model;

import java.util.Date;

/**
 * 任务结果.
 * 
 * @author yjli
 */
public class Result {

    private Integer corpId;
    private Integer taskId;
    private Integer templateId;
    private Integer sentCount;
    private Integer reachCount;
    private Integer readCount;
    private Integer readUserCount;
    private Integer clickCount;
    private Integer clickUserCount;
    private Integer unsubscribeCount;
    private Integer forwardCount;
    private Integer softBounceCount;
    private Integer hardBounceCount;
    private Integer beforeNoneCount;
    private Integer afterNoneCount;
    private Integer dnsFailCount;
    private Integer spamGarbageCount;
    private Date beginSendTime;
    private Date endSendTime;

    private String userId;
    private String taskName;
    private String trigerName;
    private String subject;
    private Integer status;
    private Date createTime;
    private Date deliveryTime;

    private Integer planId;
    private Integer campaignId;
    private String campaignName;
    private Integer taskCount;
    private Integer touchCount;
    private String templateName;
    private Integer templateCount;
    private Integer touchId;
    private Integer bindTouchCount;
    
    private String parentTaskName;
    private String company;
    
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
        return sentCount;
    }

    public void setSentCount(Integer sentCount) {
        this.sentCount = sentCount;
    }

    public Integer getReachCount() {
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
        return softBounceCount;
    }

    public void setSoftBounceCount(Integer softBounceCount) {
        this.softBounceCount = softBounceCount;
    }

    public Integer getHardBounceCount() {
        return hardBounceCount;
    }

    public void setHardBounceCount(Integer hardBounceCount) {
        this.hardBounceCount = hardBounceCount;
    }

    public Integer getBeforeNoneCount() {
        return beforeNoneCount;
    }

    public void setBeforeNoneCount(Integer beforeNoneCount) {
        this.beforeNoneCount = beforeNoneCount;
    }

    public Integer getAfterNoneCount() {
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

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getTaskName() {
        return taskName;
    }

    public void setTaskName(String taskName) {
        this.taskName = taskName;
    }

    public String getTrigerName() {
        return trigerName;
    }

    public void setTrigerName(String trigerName) {
        this.trigerName = trigerName;
    }

    public String getSubject() {
        return subject;
    }

    public void setSubject(String subject) {
        this.subject = subject;
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

    public Integer getTaskCount() {
        return taskCount;
    }

    public void setTaskCount(Integer taskCount) {
        this.taskCount = taskCount;
    }

    public Integer getTouchCount() {
        return touchCount;
    }

    public void setTouchCount(Integer touchCount) {
        this.touchCount = touchCount;
    }

	public String getTemplateName() {
		return templateName;
	}

	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}

	public Integer getTouchId() {
		return touchId;
	}

	public void setTouchId(Integer touchId) {
		this.touchId = touchId;
	}

	public Integer getTemplateCount() {
		return templateCount;
	}

	public void setTemplateCount(Integer templateCount) {
		this.templateCount = templateCount;
	}

    public Integer getBindTouchCount() {
        return bindTouchCount;
    }

    public void setBindTouchCount(Integer bindTouchCount) {
        this.bindTouchCount = bindTouchCount;
    }

    public String getParentTaskName() {
        return parentTaskName;
    }

    public void setParentTaskName(String parentTaskName) {
        this.parentTaskName = parentTaskName;
    }

    public String getCompany() {
        return company;
    }

    public void setCompany(String company) {
        this.company = company;
    }

}
