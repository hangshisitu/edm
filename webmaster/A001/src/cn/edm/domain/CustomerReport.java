package cn.edm.domain;

import java.util.List;


/**
 * 客户发送类统计.
 * 
 * @author like
 */
public class CustomerReport{
	
	private long recordCount;
	private List<Domain> recordList;
			
	public List<Domain> getRecordList() {
		return recordList;
	}
	public void setRecordList(List<Domain> recordList) {
		this.recordList = recordList;
	}
	public long getRecordCount() {
		return recordCount;
	}
	public void setRecordCount(long recordCount) {
		this.recordCount = recordCount;
	}
		

}
