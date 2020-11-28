package cn.edm.model;

import java.util.Date;

/**
 * 任务.
 * 
 * email_cnds(筛选条件, eg.
 * tag_id=1,tag_name=TEST,unsubscribe_id=2,filter_id=1,filter_name
 * =TEST,props=(email:end=qq.com&birthday:range=1990-01-01,1995-12-31) )
 * send_settings(发送设置, eg.
 * ad=false,sms=1,rand=true,robot=[edm1@139.com,edm2@139.com] )
 * 
 * plan_referer(引用. 任务ID#2013/01/01)
 * 
 * @author yjli
 */
public class Task {

    private Integer taskId;
    private Integer corpId;
    private Integer bindCorpId;
    private String userId;
    private Integer campaignId;
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
    private Integer planId;
    private String planReferer;
    private Integer labelStatus;
    private Integer status;
    private Date createTime;
    private Date modifyTime;
    private Date deliveryTime;

    private String campaignName;
    private String templateName;
    private String fileId;
    private String includes;
    private String excludes;
    private String ad;
    private String unsubscribe;
    private String sendCode;
    private String replier;
    private String rand;
    private String robot;
    private Integer sentCount;

    private Integer taskCount;
    private Integer touchCount;
    private Integer bindTouchCount;
    
    private String company;
    
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

    public Integer getBindCorpId() {
        return bindCorpId;
    }

    public void setBindCorpId(Integer bindCorpId) {
        this.bindCorpId = bindCorpId;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public Integer getCampaignId() {
        return campaignId;
    }

    public void setCampaignId(Integer campaignId) {
        this.campaignId = campaignId;
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

    public Integer getLabelStatus() {
        return labelStatus;
    }

    public void setLabelStatus(Integer labelStatus) {
        this.labelStatus = labelStatus;
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

    public String getCampaignName() {
        return campaignName;
    }

    public void setCampaignName(String campaignName) {
        this.campaignName = campaignName;
    }

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

    public String getFileId() {
        return fileId;
    }

    public void setFileId(String fileId) {
        this.fileId = fileId;
    }

    public String getIncludes() {
        return includes;
    }

    public void setIncludes(String includes) {
        this.includes = includes;
    }

    public String getExcludes() {
        return excludes;
    }

    public void setExcludes(String excludes) {
        this.excludes = excludes;
    }

    public String getAd() {
        return ad;
    }

    public void setAd(String ad) {
        this.ad = ad;
    }

    public String getUnsubscribe() {
        return unsubscribe;
    }

    public void setUnsubscribe(String unsubscribe) {
        this.unsubscribe = unsubscribe;
    }

    public String getSendCode() {
        return sendCode;
    }

    public void setSendCode(String sendCode) {
        this.sendCode = sendCode;
    }

    public String getReplier() {
        return replier;
    }

    public void setReplier(String replier) {
        this.replier = replier;
    }

    public String getRand() {
        return rand;
    }

    public void setRand(String rand) {
        this.rand = rand;
    }

    public String getRobot() {
        return robot;
    }

    public void setRobot(String robot) {
        this.robot = robot;
    }

    public Integer getSentCount() {
        return sentCount;
    }

    public void setSentCount(Integer sentCount) {
        this.sentCount = sentCount;
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

    public Integer getBindTouchCount() {
        return bindTouchCount;
    }

    public void setBindTouchCount(Integer bindTouchCount) {
        this.bindTouchCount = bindTouchCount;
    }

    public String getCompany() {
        return company;
    }

    public void setCompany(String company) {
        this.company = company;
    }

}
