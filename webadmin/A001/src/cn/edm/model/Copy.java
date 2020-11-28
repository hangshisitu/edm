package cn.edm.model;

import java.util.Date;

/**
 * 复制备份.
 * Active-Slave策略备份MongoDB邮箱地址
 * 
 * action(触发动作. modify|clear)
 * 
 * @author yjli
 */
public class Copy {

    public static final String TAG = "Tag";
    public static final String RECIPIENT = "Recipient";

    public static final Integer MODIFY = 1;
    public static final Integer CLEAR = 0;
    
    private Integer corpId;
    private Integer dbId;
    private Integer coll;
    private Integer action;
    private Date modifyTime;

    public Copy(Integer corpId, Integer dbId, Integer coll, Integer action) {
        this.corpId = corpId;
        this.dbId = dbId;
        this.coll = coll;
        this.action = action;
    }

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

    public Integer getColl() {
        return coll;
    }

    public void setColl(Integer coll) {
        this.coll = coll;
    }

    public Integer getAction() {
        return action;
    }

    public void setAction(Integer action) {
        this.action = action;
    }

    public Date getModifyTime() {
        return modifyTime;
    }

    public void setModifyTime(Date modifyTime) {
        this.modifyTime = modifyTime;
    }

}
