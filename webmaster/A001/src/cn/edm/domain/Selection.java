package cn.edm.domain;

import java.util.Date;

public class Selection {

    private Integer selectionId;
    private Integer corpId;
    private String userId;
    private String includeIds;
    private String activeCnd;
    private String inCnd;
    private String includes;
    private String excludeIds;
    private String excludes;
    private Integer filterId;
    private String filter;
    private Integer emailCount;
    private Integer type;
    private String fileId;
    private Integer tagId;
    private Integer status;
    private Date createTime;
    private Date modifyTime;

    public Integer getSelectionId() {
        return selectionId;
    }
    
    public void setSelectionId(Integer selectionId) {
        this.selectionId = selectionId;
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

    public String getIncludeIds() {
        return includeIds;
    }
    
    public void setIncludeIds(String includeIds) {
        this.includeIds = includeIds;
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

    public String getIncludes() {
        return includes;
    }
    
    public void setIncludes(String includes) {
        this.includes = includes;
    }

    public String getExcludeIds() {
        return excludeIds;
    }
    
    public void setExcludeIds(String excludeIds) {
        this.excludeIds = excludeIds;
    }

    public String getExcludes() {
        return excludes;
    }
    
    public void setExcludes(String excludes) {
        this.excludes = excludes;
    }

    public Integer getFilterId() {
        return filterId;
    }
    
    public void setFilterId(Integer filterId) {
        this.filterId = filterId;
    }

    public String getFilter() {
        return filter;
    }
    
    public void setFilter(String filter) {
        this.filter = filter;
    }

    public Integer getEmailCount() {
        return emailCount;
    }
    
    public void setEmailCount(Integer emailCount) {
        this.emailCount = emailCount;
    }

    public Integer getType() {
        return type;
    }
    
    public void setType(Integer type) {
        this.type = type;
    }

    public String getFileId() {
        return fileId;
    }
    
    public void setFileId(String fileId) {
        this.fileId = fileId;
    }

    public Integer getTagId() {
        return tagId;
    }

    public void setTagId(Integer tagId) {
        this.tagId = tagId;
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
