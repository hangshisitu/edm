package cn.edm.domain;

import java.util.Date;

public class DeliveryQuantity {
	
	private String fromDomain;
	private String rcptDomain;
	private Integer quantity;
	private Integer seconds;
	private Integer status;
	private Date createTime;
	public String getFromDomain() {
		return fromDomain;
	}
	public void setFromDomain(String fromDomain) {
		this.fromDomain = fromDomain;
	}
	public String getRcptDomain() {
		return rcptDomain;
	}
	public void setRcptDomain(String rcptDomain) {
		this.rcptDomain = rcptDomain;
	}
	public Integer getQuantity() {
		return quantity;
	}
	public void setQuantity(Integer quantity) {
		this.quantity = quantity;
	}
	public Integer getSeconds() {
		return seconds;
	}
	public void setSeconds(Integer seconds) {
		this.seconds = seconds;
	}
	public Integer getStatus() {
		return status;
	}
	public void setStatus(Integer status) {
		this.status = status;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Date getUpdate_time() {
		return update_time;
	}
	public void setUpdate_time(Date update_time) {
		this.update_time = update_time;
	}
	private Date update_time;
}
