package cn.edm.domain;

public class Setting {

	private Integer taskId;
	private String ad;
	private String sendCode;
	private String sms;
	private String replier;
	private String rand;
	private String robot;
	private String tagIds;
	private Integer unsubscribeId;
	private String labelIds;
	private String unsubscribe;
	private String campaign;
	private String template;
	
	public String getCampaign() {
		return campaign;
	}

	public void setCampaign(String campaign) {
		this.campaign = campaign;
	}

	public String getUnsubscribe() {
		return unsubscribe;
	}

	public void setUnsubscribe(String unsubscribe) {
		this.unsubscribe = unsubscribe;
	}

	public Integer getTaskId() {
		return taskId;
	}

	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
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

	public String getTagIds() {
		return tagIds;
	}

	public void setTagIds(String tagIds) {
		this.tagIds = tagIds;
	}

	public Integer getUnsubscribeId() {
		return unsubscribeId;
	}

	public void setUnsubscribeId(Integer unsubscribeId) {
		this.unsubscribeId = unsubscribeId;
	}

	public String getLabelIds() {
		return labelIds;
	}

	public void setLabelIds(String labelIds) {
		this.labelIds = labelIds;
	}

    public String getTemplate() {
        return template;
    }

    public void setTemplate(String template) {
        this.template = template;
    }

}
