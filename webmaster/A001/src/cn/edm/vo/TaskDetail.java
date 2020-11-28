package cn.edm.vo;

public class TaskDetail {
	private Integer taskId;
	public Integer getTaskId() {
		return taskId;
	}
	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}
	public String getTaskName() {
		if(taskName==null||"".equals(taskName.trim()))
			taskName = "无";
		return taskName;
	}
	public void setTaskName(String taskName) {
		this.taskName = taskName;
	}
	public String getTemplateName() {
		if(templateName==null||"".equals(templateName.trim()))
			templateName = "无";
		return templateName;
	}
	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}
	public String getSubject() {
		if(subject==null||"".equals(subject.trim()))
			subject = "无";
		return subject;
	}
	public void setSubject(String subject) {
		this.subject = subject;
	}
	public String getSenderEmail() {
		if(senderEmail==null||"".equals(senderEmail.trim()))
			senderEmail = "无";
		return senderEmail;
	}
	public void setSenderEmail(String senderEmail) {
		this.senderEmail = senderEmail;
	}
	
	public String getSenderName() {
		if(senderName==null||"".equals(senderName.trim()))
			senderName = "无";
		return senderName;
	}
	public void setSenderName(String senderName) {
		this.senderName = senderName;
	}
	public String getTag() {
		if(tag==null||"".equals(tag.trim()))
			tag = "无";
		return tag;
	}
	public void setTag(String tag) {
		this.tag = tag;
	}
	public String getExculde() {
		if(exculde==null||"".equals(exculde.trim()))
			exculde = "无";
		return exculde;
	}
	public void setExculde(String exculde) {
		this.exculde = exculde;
	}
	public String getReceiverName() {
		if(receiverName==null||"".equals(receiverName.trim()))
			receiverName = "无";
		return receiverName;
	}
	public void setReceiverName(String receiverName) {
		this.receiverName = receiverName;
	}
	public String getSetting() {
		if(setting==null||"".equals(setting.trim()))
			setting = "无";
		return setting;
	}
	public void setSetting(String setting) {
		this.setting = setting;
	}
	public String getWay() {
		if(way==null||"".equals(way.trim()))
			way = "无";
		return way;
	}
	public void setWay(String way) {
		this.way = way;
	}
	private String taskName;
	private String templateName;
	private String subject;
	private String senderEmail;
	private String senderName;
	private String tag;
	private String exculde;
	private String receiverName;
	private String campaignName;
	public String getCampaignName() {
		return campaignName;
	}
	public void setCampaignName(String campaignName) {
		this.campaignName = campaignName;
	}
	public String getRobot() {
		if(robot==null||"".equals(robot.trim()))
			robot = "无";
		return robot;
	}
	public void setRobot(String robot) {
		this.robot = robot;
	}
	public String getReplier() {
		if(replier==null||"".equals(replier.trim()))
			replier = "无";
		return replier;
	}
	public void setReplier(String replier) {
		this.replier = replier;
	}
	private String setting;
	private String way;
	private String robot;
	private String replier;
}
