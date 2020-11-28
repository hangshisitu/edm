package cn.edm.model;

import java.io.Serializable;

public class Step implements Serializable {

    private static final long serialVersionUID = 3393039989850888377L;

    private String id;
    private String userId;
    private String _1;
    private String _2;

    // step 1
    private Integer campaignId;
    private Integer templateId;
    private String subject;

    // step 2
    private String fileId;
    private String fileName;
    private String activeCnd;
    private String inCnd;
    private String includeIds;
    private String excludeIds;
    private String includes;
    private String excludes;
    private Integer emailCount;

    // step 3
    private Integer taskId;
    private Integer bindCorpId;
    private String bindResourceId;
    private String taskName;
    private String senderName;
    private String senderEmail;
    private String receiver;
    private String jobTime;
    private Integer unsubscribeId;
    private String ad;
    private String sendCode;
    private String sms;
    private String replier;
    private String rand;
    private String robot;
    private String labelIds;

    private Integer planId;
    private String cron;
    private String beginTime;
    private String endTime;

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String get_1() {
        return _1;
    }

    public void set_1(String _1) {
        this._1 = _1;
    }

    public String get_2() {
        return _2;
    }

    public void set_2(String _2) {
        this._2 = _2;
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

    public String getSubject() {
        return subject;
    }

    public void setSubject(String subject) {
        this.subject = subject;
    }

    public String getFileId() {
        return fileId;
    }

    public void setFileId(String fileId) {
        this.fileId = fileId;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public String getActiveCnd() {
        return activeCnd;
    }

    public void setActiveCnd(String activeCnd) {
        this.activeCnd = activeCnd;
    }

    public String getInCnd() {
        return inCnd;
    }

    public void setInCnd(String inCnd) {
        this.inCnd = inCnd;
    }

    public String getIncludeIds() {
        return includeIds;
    }

    public void setIncludeIds(String includeIds) {
        this.includeIds = includeIds;
    }

    public String getExcludeIds() {
        return excludeIds;
    }

    public void setExcludeIds(String excludeIds) {
        this.excludeIds = excludeIds;
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

    public Integer getEmailCount() {
        return emailCount;
    }

    public void setEmailCount(Integer emailCount) {
        this.emailCount = emailCount;
    }

    public Integer getTaskId() {
        return taskId;
    }

    public void setTaskId(Integer taskId) {
        this.taskId = taskId;
    }

    public Integer getBindCorpId() {
        return bindCorpId;
    }

    public void setBindCorpId(Integer bindCorpId) {
        this.bindCorpId = bindCorpId;
    }

    public String getBindResourceId() {
        return bindResourceId;
    }

    public void setBindResourceId(String bindResourceId) {
        this.bindResourceId = bindResourceId;
    }

    public String getTaskName() {
        return taskName;
    }

    public void setTaskName(String taskName) {
        this.taskName = taskName;
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

    public String getJobTime() {
        return jobTime;
    }

    public void setJobTime(String jobTime) {
        this.jobTime = jobTime;
    }

    public Integer getUnsubscribeId() {
        return unsubscribeId;
    }

    public void setUnsubscribeId(Integer unsubscribeId) {
        this.unsubscribeId = unsubscribeId;
    }

    public String getAd() {
        return ad;
    }

    public void setAd(String ad) {
        this.ad = ad;
    }

    public String getSendCode() {
        return sendCode;
    }

    public void setSendCode(String sendCode) {
        this.sendCode = sendCode;
    }

    public String getSms() {
        return sms;
    }

    public void setSms(String sms) {
        this.sms = sms;
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

    public String getLabelIds() {
        return labelIds;
    }

    public void setLabelIds(String labelIds) {
        this.labelIds = labelIds;
    }

    public Integer getPlanId() {
        return planId;
    }

    public void setPlanId(Integer planId) {
        this.planId = planId;
    }

    public String getCron() {
        return cron;
    }

    public void setCron(String cron) {
        this.cron = cron;
    }

    public String getBeginTime() {
        return beginTime;
    }

    public void setBeginTime(String beginTime) {
        this.beginTime = beginTime;
    }

    public String getEndTime() {
        return endTime;
    }

    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }

}
