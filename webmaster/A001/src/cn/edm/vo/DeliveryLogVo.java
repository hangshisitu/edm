package cn.edm.vo;

import java.util.List;

public class DeliveryLogVo {
	private String ip;
	private List<DeliveryLog> logList;
	
	private Integer total;
	
	public DeliveryLogVo() {
		
	}
	
	public DeliveryLogVo(Integer total) {
		this.total = total;
	}
	public Integer getTotal() {
		return total;
	}
	public void setTotal(Integer total) {
		this.total = total;
	}
	public String getIp() {
		return ip;
	}
	public void setIp(String ip) {
		this.ip = ip;
	}
	public List<DeliveryLog> getLogList() {
		return logList;
	}
	public void setLogList(List<DeliveryLog> logList) {
		this.logList = logList;
	}
}
