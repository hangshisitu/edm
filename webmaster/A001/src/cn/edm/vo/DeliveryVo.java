package cn.edm.vo;

import java.util.Date;
import java.util.List;

public class DeliveryVo {
	
	private String fromDomain;
	private String rcptDomain;
	private String quarter;
	private String quarterStatus;
	private String hour;
	private String hourStatus;
	private String day;
	private String dayStatus;
	
	private List<QuantityVo> qvList;
	
	public List<QuantityVo> getQvList() {
		return qvList;
	}

	public void setQvList(List<QuantityVo> qvList) {
		this.qvList = qvList;
	}

	private Date queryTime;
	
	private String flag;

	public String getFlag() {
		return flag;
	}

	public void setFlag(String flag) {
		this.flag = flag;
	}

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

	public String getQuarter() {
		return quarter;
	}

	public void setQuarter(String quarter) {
		this.quarter = quarter;
	}

	public String getQuarterStatus() {
		return quarterStatus;
	}

	public void setQuarterStatus(String quarterStatus) {
		this.quarterStatus = quarterStatus;
	}

	public String getHour() {
		return hour;
	}

	public void setHour(String hour) {
		this.hour = hour;
	}

	public String getHourStatus() {
		return hourStatus;
	}

	public void setHourStatus(String hourStatus) {
		this.hourStatus = hourStatus;
	}

	public String getDay() {
		return day;
	}

	public void setDay(String day) {
		this.day = day;
	}

	public String getDayStatus() {
		return dayStatus;
	}

	public void setDayStatus(String dayStatus) {
		this.dayStatus = dayStatus;
	}

	public Date getQueryTime() {
		return queryTime;
	}

	public void setQueryTime(Date queryTime) {
		this.queryTime = queryTime;
	}
}
