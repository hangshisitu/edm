package cn.edm.domain;

public class Os {
	private Integer corpId;
	private Integer taskId;
	private Integer templateId;
	private Integer os;
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
	
	public Integer getTotal() {
		return total;
	}
	public void setTotal(Integer total) {
		this.total = total;
	}
	public float getOpenRate() {
		return openRate;
	}
	public void setOpenRate(float openRate) {
		this.openRate = openRate;
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
	
	public Integer getOs() {
		return os;
	}
	public void setOs(Integer os) {
		this.os = os;
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
