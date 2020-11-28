package cn.edm.model;

import java.util.Date;
import java.util.List;

/**
 * 域名统计.
 * 
 * @author yjli
 */
public class Domain {

    private Integer corpId;
    private Integer taskId;
    private Integer templateId;
    private String emailDomain;
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

    private String taskName;
    private Integer status;
    private Date deliveryTime;

    private Integer planId;
    private Integer campaignId;
    private String compareName;
    
    private String templateName;
    private Integer touchCount;
    private Integer templateCount;
    private Integer touchId;
    private String parentTaskName;
    private List<String> urlList;
    private Integer parentReadUserCount;

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

    public Integer getCampaignId() {
        return campaignId;
    }

    public void setCampaignId(Integer campaignId) {
        this.campaignId = campaignId;
    }

    public String getCompareName() {
        return compareName;
    }

    public void setCompareName(String compareName) {
        this.compareName = compareName;
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

	public String getParentTaskName() {
		return parentTaskName;
	}
	
	public void setParentTaskName(String parentTaskName) {
		this.parentTaskName = parentTaskName;
	}

	public List<String> getUrlList() {
		return urlList;
	}

	public void setUrlList(List<String> urlList) {
		this.urlList = urlList;
	}

	public Integer getParentReadUserCount() {
		return parentReadUserCount;
	}

	public void setParentReadUserCount(Integer parentReadUserCount) {
		this.parentReadUserCount = parentReadUserCount;
	}

}
