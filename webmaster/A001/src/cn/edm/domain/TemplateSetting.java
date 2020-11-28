package cn.edm.domain;

public class TemplateSetting {

	private Integer corpId;
	
	private String userId;
	
	private Integer parentId;
	
	private Integer templateId;
	
	private String touchUrls;
	
	private Integer afterMinutes;
	
	private String subject;
	
	private Integer taskId;
	
	private String templateName;
	
	private Integer deviceType; // 设备类型

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

	public Integer getParentId() {
		return parentId;
	}

	public void setParentId(Integer parentId) {
		this.parentId = parentId;
	}

	public Integer getTemplateId() {
		return templateId;
	}

	public void setTemplateId(Integer templateId) {
		this.templateId = templateId;
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

	public String getSubject() {
		return subject;
	}

	public void setSubject(String subject) {
		this.subject = subject;
	}

	public String getTemplateName() {
		return templateName;
	}

	public void setTemplateName(String templateName) {
		this.templateName = templateName;
	}

	public Integer getTaskId() {
		return taskId;
	}

	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}

	public Integer getDeviceType() {
		return deviceType;
	}

	public void setDeviceType(Integer deviceType) {
		this.deviceType = deviceType;
	}

}
