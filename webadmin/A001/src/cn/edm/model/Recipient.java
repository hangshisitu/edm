package cn.edm.model;

import java.util.Date;

/**
 * 收件人.
 * 
 * @author yjli
 */
public class Recipient {

    private Integer corpId;
    private Integer dbId;

    private String email;
    private String parameterList;
    private Date createTime;
    private Date modifyTime;
    private Date activeTime;

    private String desc;

    public Integer getCorpId() {
        return corpId;
    }

    public void setCorpId(Integer corpId) {
        this.corpId = corpId;
    }

    public Integer getDbId() {
        return dbId;
    }

    public void setDbId(Integer dbId) {
        this.dbId = dbId;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getParameterList() {
        return parameterList;
    }

    public void setParameterList(String parameterList) {
        this.parameterList = parameterList;
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

    public Date getActiveTime() {
        return activeTime;
    }

    public void setActiveTime(Date activeTime) {
        this.activeTime = activeTime;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

}
