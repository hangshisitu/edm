package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

/**
 * 套餐.
 * type(类型. 0:测试, 1:正式)
 * 
 * @author yjli
 */
public class Cos implements Serializable{

	private static final long serialVersionUID = 2457750830725482752L;
	public static final int TESTED = 0;
	public static final int FORMAL = 1;
	
	private Integer cosId;
	private String cosName;
	private Integer type;
	private Integer userCount;
	private Integer daySend;
	private Integer weekSend;
	private Integer monthSend;
	private Integer totalSend;
    private Date    startTime;
    private Date    endTime;
	private String remind;
	private Date   createTime;
	private String status;
	
	public Integer getCosId() {
		return cosId;
	}

	public void setCosId(Integer cosId) {
		this.cosId = cosId;
	}

	public String getCosName() {
		return cosName;
	}

	public void setCosName(String cosName) {
		this.cosName = cosName;
	}

	public Integer getType() {
		return type;
	}

	public void setType(Integer type) {
		this.type = type;
	}

	public Integer getUserCount() {
		return userCount;
	}

	public void setUserCount(Integer userCount) {
		this.userCount = userCount;
	}

	public Integer getDaySend() {
		return daySend;
	}

	public void setDaySend(Integer daySend) {
		this.daySend = daySend;
	}

	public Integer getWeekSend() {
		return weekSend;
	}

	public void setWeekSend(Integer weekSend) {
		this.weekSend = weekSend;
	}

	public Integer getMonthSend() {
		return monthSend;
	}

	public void setMonthSend(Integer monthSend) {
		this.monthSend = monthSend;
	}

	public Integer getTotalSend() {
		return totalSend;
	}

	public void setTotalSend(Integer totalSend) {
		this.totalSend = totalSend;
	}

	public Date getStartTime() {
		return startTime;
	}

	public void setStartTime(Date startTime) {
		this.startTime = startTime;
	}

	public Date getEndTime() {
		return endTime;
	}

	public void setEndTime(Date endTime) {
		this.endTime = endTime;
	}

	public String getRemind() {
		return remind;
	}

	public void setRemind(String remind) {
		this.remind = remind;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}
 
}
