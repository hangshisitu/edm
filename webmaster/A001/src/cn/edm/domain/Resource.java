package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

/**
 * 通道.
 * 
 * @author yjli
 */
public class Resource implements Serializable{

	private static final long serialVersionUID = -451411277774586234L;
	private String resourceId;
	private String resourceName;
	private String senderList;
	private String heloList;
	private String skipList;
	private Date modifyTime;

	public String getResourceId() {
		return resourceId;
	}

	public void setResourceId(String resourceId) {
		this.resourceId = resourceId;
	}

	public String getResourceName() {
		return resourceName;
	}

	public void setResourceName(String resourceName) {
		this.resourceName = resourceName;
	}

	public String getSenderList() {
		return senderList;
	}

	public void setSenderList(String senderList) {
		this.senderList = senderList;
	}

	public String getHeloList() {
		return heloList;
	}

	public void setHeloList(String heloList) {
		this.heloList = heloList;
	}

	public String getSkipList() {
		return skipList;
	}

	public void setSkipList(String skipList) {
		this.skipList = skipList;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}

}
