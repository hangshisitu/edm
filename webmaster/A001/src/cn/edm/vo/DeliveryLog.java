package cn.edm.vo;

public class DeliveryLog {
	private String ip;
	private String fromDomain;
	private String rcptDomain;
	private String success;
	private String failure;
	private String queryTime;
	
	
	public String getIp() {
		return ip;
	}
	public void setIp(String ip) {
		this.ip = ip;
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
	public String getSuccess() {
		return success;
	}
	public void setSuccess(String success) {
		this.success = success;
	}
	public String getFailure() {
		return failure;
	}
	public void setFailure(String failure) {
		this.failure = failure;
	}
	public String getQueryTime() {
		return queryTime;
	}
	public void setQueryTime(String queryTime) {
		this.queryTime = queryTime;
	}
}
