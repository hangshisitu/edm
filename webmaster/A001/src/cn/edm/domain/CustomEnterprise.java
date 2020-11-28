package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

public class CustomEnterprise implements Serializable{

	private static final long serialVersionUID = -7277604472169976786L;
	
	private Integer id;
	private String logoUrl;
	private Date createTime;
	private Integer status;  //0-禁用      1-启用
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getLogoUrl() {
		return logoUrl;
	}
	public void setLogoUrl(String logoUrl) {
		this.logoUrl = logoUrl;
	}
	public Date getCreateTime() {
		return createTime;
	}
	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	public Integer getStatus() {
		return status;
	}
	public void setStatus(Integer status) {
		this.status = status;
	}
	
}
