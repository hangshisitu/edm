package cn.edm.model;

/**
 * 链接统计.
 * 
 * @author yjli
 */
public class Url {

	private Integer corpId;
	private Integer taskId;
	private Integer templateId;
	private String url;
	private Integer clickCount;

	private Integer sumClickCount;

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

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public Integer getClickCount() {
		return clickCount;
	}

	public void setClickCount(Integer clickCount) {
		this.clickCount = clickCount;
	}

	public Integer getSumClickCount() {
		return sumClickCount;
	}

	public void setSumClickCount(Integer sumClickCount) {
		this.sumClickCount = sumClickCount;
	}

}
