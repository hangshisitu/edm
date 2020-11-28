package cn.edm.domain;

import java.util.Date;

/**
 * SMTP设置
 * 
 * @Date 2014年6月3日 上午11:10:48
 * @author Lich
 *
 */
public class Smtp {
	
	private Integer id; // ID
	
	private Integer quarter; // 每15分钟投递邮件数
	
	private Integer hour; // 每小时投递邮件数
	
	private Integer day; // 每天投递邮件数
	
	private String ip; // 存储方式：ip1,ip2,……
	
	private Integer size; // 邮件大小
	
	private Date createTime; // 创建时间

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Integer getQuarter() {
		return quarter;
	}

	public void setQuarter(Integer quarter) {
		this.quarter = quarter;
	}

	public Integer getHour() {
		return hour;
	}

	public void setHour(Integer hour) {
		this.hour = hour;
	}

	public Integer getDay() {
		return day;
	}

	public void setDay(Integer day) {
		this.day = day;
	}

	public String getIp() {
		return ip;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public Integer getSize() {
		return size;
	}

	public void setSize(Integer size) {
		this.size = size;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}
	
}
