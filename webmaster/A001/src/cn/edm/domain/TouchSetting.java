package cn.edm.domain;

public class TouchSetting {

	private Integer taskId;
	
	private Integer parentId;
	
	private Integer templateId;
	
	private String touchUrls;
	
	private String subject;
	
	private String templateName;
	
    private String parentName;
    
    private Integer corpId;
    
    private Integer afterMinutes;
    
    private Integer deviceType; // 设备类型

	public Integer getTaskId() {
		return taskId;
	}

	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
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

	public String getParentName() {
		return parentName;
	}

	public void setParentName(String parentName) {
		this.parentName = parentName;
	}

	public Integer getCorpId() {
		return corpId;
	}

	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
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
    
}
