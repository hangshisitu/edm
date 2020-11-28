package cn.edm.model;

import java.util.Date;

public class Touch {

    private Integer taskId;
    private Integer corpId;
    private String userId;
    private Integer templateId;
    private Integer parentId;
    private String templateName;
    private String subject;
    private String senderName;
    private String senderEmail;
    private String receiver;
    private String resourceId;
    private Integer touchCount;
    private Date createTime;

    private Integer parentTemplateId;
    private String parentTemplateName;
    private String touchUrls;
    private Integer afterMinutes;
    private Integer deviceType;

    private Integer joinPhone;
    private Integer joinSms;
    private String filePath;
    private String phonePath;
    private String smsPath;
    private String phoneAttrs;
    
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

    public Integer getParentId() {
        return parentId;
    }

    public void setParentId(Integer parentId) {
        this.parentId = parentId;
    }

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
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

    public String getResourceId() {
        return resourceId;
    }

    public void setResourceId(String resourceId) {
        this.resourceId = resourceId;
    }

    public Integer getTouchCount() {
        return touchCount;
    }

    public void setTouchCount(Integer touchCount) {
        this.touchCount = touchCount;
    }

    public Date getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }

    public Integer getParentTemplateId() {
        return parentTemplateId;
    }

    public void setParentTemplateId(Integer parentTemplateId) {
        this.parentTemplateId = parentTemplateId;
    }

    public String getParentTemplateName() {
        return parentTemplateName;
    }

    public void setParentTemplateName(String parentTemplateName) {
        this.parentTemplateName = parentTemplateName;
    }

    public String getTouchUrls() {
        return touchUrls;
    }

    public void setTouchUrls(String touchUrls) {
        this.touchUrls = touchUrls;
    }

    public Integer getAfterMinutes() {
        return afterMinutes;
    }

    public void setAfterMinutes(Integer afterMinutes) {
        this.afterMinutes = afterMinutes;
    }

    public Integer getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(Integer deviceType) {
        this.deviceType = deviceType;
    }

    public Integer getJoinPhone() {
        return joinPhone;
    }

    public void setJoinPhone(Integer joinPhone) {
        this.joinPhone = joinPhone;
    }

    public Integer getJoinSms() {
        return joinSms;
    }

    public void setJoinSms(Integer joinSms) {
        this.joinSms = joinSms;
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public String getPhonePath() {
        return phonePath;
    }

    public void setPhonePath(String phonePath) {
        this.phonePath = phonePath;
    }

    public String getSmsPath() {
        return smsPath;
    }

    public void setSmsPath(String smsPath) {
        this.smsPath = smsPath;
    }

    public String getPhoneAttrs() {
        return phoneAttrs;
    }

    public void setPhoneAttrs(String phoneAttrs) {
        this.phoneAttrs = phoneAttrs;
    }

}
