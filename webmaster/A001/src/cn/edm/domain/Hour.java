package cn.edm.domain;

import java.io.Serializable;

/**
 * 小时统计.
 * 
 * @author yjli
 */
public class Hour implements Serializable{

	private static final long serialVersionUID = -6675188372843665935L;
	private Integer corpId;
	private Integer taskId;
	private Integer templateId;
	private Integer hour;
	private Integer readCount;
	private Integer readUserCount;
	private Integer clickCount;
	private Integer clickUserCount;

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

	public Integer getHour() {
		return hour;
	}

	public void setHour(Integer hour) {
		this.hour = hour;
	}

	public Integer getReadCount() {
		return readCount;
	}

	public void setReadCount(Integer readCount) {
		this.readCount = readCount;
	}

	public Integer getReadUserCount() {
		return readUserCount;
	}

	public void setReadUserCount(Integer readUserCount) {
		this.readUserCount = readUserCount;
	}

	public Integer getClickCount() {
		return clickCount;
	}

	public void setClickCount(Integer clickCount) {
		this.clickCount = clickCount;
	}

	public Integer getClickUserCount() {
		return clickUserCount;
	}

	public void setClickUserCount(Integer clickUserCount) {
		this.clickUserCount = clickUserCount;
	}

}
