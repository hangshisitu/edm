package cn.edm.model;

import java.util.Date;

/**
 * 表单.
 * 
 * @author yjli
 */
public class Form {

    private Integer formId;
    private Integer corpId;
    private String userId;
    private String formName;
    private String formDesc;
    private Integer type;
    private Integer tagId;
    private Integer emailConfirm;
    private Integer emailPrompt;
    private String promptSubject;
    private String confirmSubject;
    private String inputPath;
    private String submitPath;
    private String confirmPath;
    private String promptPath;
    private String randCode;
    private Integer status;
    private Date createTime;
    private Date modifyTime;

    public Integer getFormId() {
        return formId;
    }
    
    public void setFormId(Integer formId) {
        this.formId = formId;
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

    public String getFormName() {
        return formName;
    }
    
    public void setFormName(String formName) {
        this.formName = formName;
    }

    public String getFormDesc() {
        return formDesc;
    }
    
    public void setFormDesc(String formDesc) {
        this.formDesc = formDesc;
    }

    public Integer getType() {
        return type;
    }
    
    public void setType(Integer type) {
        this.type = type;
    }

    public Integer getTagId() {
        return tagId;
    }
    
    public void setTagId(Integer tagId) {
        this.tagId = tagId;
    }

    public Integer getEmailConfirm() {
        return emailConfirm;
    }
    
    public void setEmailConfirm(Integer emailConfirm) {
        this.emailConfirm = emailConfirm;
    }

    public Integer getEmailPrompt() {
        return emailPrompt;
    }
    
    public void setEmailPrompt(Integer emailPrompt) {
        this.emailPrompt = emailPrompt;
    }

    public String getPromptSubject() {
        return promptSubject;
    }
    
    public void setPromptSubject(String promptSubject) {
        this.promptSubject = promptSubject;
    }

    public String getConfirmSubject() {
        return confirmSubject;
    }
    
    public void setConfirmSubject(String confirmSubject) {
        this.confirmSubject = confirmSubject;
    }

    public String getInputPath() {
        return inputPath;
    }
    
    public void setInputPath(String inputPath) {
        this.inputPath = inputPath;
    }

    public String getSubmitPath() {
        return submitPath;
    }
    
    public void setSubmitPath(String submitPath) {
        this.submitPath = submitPath;
    }

    public String getConfirmPath() {
        return confirmPath;
    }
    
    public void setConfirmPath(String confirmPath) {
        this.confirmPath = confirmPath;
    }

    public String getPromptPath() {
        return promptPath;
    }
    
    public void setPromptPath(String promptPath) {
        this.promptPath = promptPath;
    }

    public String getRandCode() {
        return randCode;
    }
    
    public void setRandCode(String randCode) {
        this.randCode = randCode;
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

}
