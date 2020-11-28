package cn.edm.domain;

import java.util.Date;

public class Campaign {
	private Integer campaignId;
	private Integer corpId;
	private String userId;
	private String campaignName;
	private String campaignDesc;
	private Date createTime;
	
	private Integer touchCount;
	private Integer taskCount;
	private String latestTime;
	
	public Integer getCampaignId() {
		return campaignId;
	}
	public void setCampaignId(Integer campaignId) {
		this.campaignId = campaignId;
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
	public String getCampaignName() {
		return campaignName;
	}
	public void setCampaignName(String campaignName) {
		this.campaignName = campaignName;
	}
	public String getCampaignDesc() {
		return campaignDesc;
	}
	public void setCampaignDesc(String campaignDesc) {
		this.campaignDesc = campaignDesc;
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
	private Date modifyTime;

	public Integer getTouchCount() {
		return touchCount;
	}
	public void setTouchCount(Integer touchCount) {
		this.touchCount = touchCount;
	}
	public Integer getTaskCount() {
		return taskCount;
	}
	public void setTaskCount(Integer taskCount) {
		this.taskCount = taskCount;
	}
	public String getLatestTime() {
		return latestTime;
	}
	public void setLatestTime(String latestTime) {
		this.latestTime = latestTime;
	}
}
