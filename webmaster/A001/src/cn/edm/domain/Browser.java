package cn.edm.domain;

public class Browser {
	private Integer corpId;
	private Integer taskId;
	private Integer templateId;
	private Integer browser;
	private Integer openCount;
	private Integer clickCount;
	
	private float openRate;
	private Integer total;
	private String name;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	
	public float getOpenRate() {
		return openRate;
	}
	public void setOpenRate(float openRate) {
		this.openRate = openRate;
	}
	public Integer getTotal() {
		return total;
	}
	public void setTotal(Integer total) {
		this.total = total;
	}
	public Integer getCorpId() {
		return corpId;
	}
	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}
	public Integer getTaskId() {
		return taskId;
	}
	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}
	public Integer getTemplateId() {
		return templateId;
	}
	public void setTemplateId(Integer templateId) {
		this.templateId = templateId;
	}
	public Integer getBrowser() {
		return browser;
	}
	public void setBrowser(Integer browser) {
		this.browser = browser;
	}
	public Integer getOpenCount() {
		return openCount;
	}
	public void setOpenCount(Integer openCount) {
		this.openCount = openCount;
	}
	public Integer getClickCount() {
		return clickCount;
	}
	public void setClickCount(Integer clickCount) {
		this.clickCount = clickCount;
	}
}
