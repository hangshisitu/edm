package cn.edm.model;

/**
 * 适配终端.
 * 
 * @author yjli
 */
public class Adapter {

    private Integer templateId;
    private Integer joinPhone;
    private Integer joinSms;
    private String phonePath;
    private String smsPath;
    private String phoneAttrs;

    private String phoneContent;
    private String smsContent;
    
    private String templateName;

    public Integer getTemplateId() {
        return templateId;
    }

    public void setTemplateId(Integer templateId) {
        this.templateId = templateId;
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

    public String getPhoneContent() {
        return phoneContent;
    }

    public void setPhoneContent(String phoneContent) {
        this.phoneContent = phoneContent;
    }

    public String getSmsContent() {
        return smsContent;
    }

    public void setSmsContent(String smsContent) {
        this.smsContent = smsContent;
    }

    public String getTemplateName() {
        return templateName;
    }

    public void setTemplateName(String templateName) {
        this.templateName = templateName;
    }

}
