package cn.edm.service;

import java.util.List;
import java.util.Map;

import au.com.bytecode.opencsv.CSVWriter;

import cn.edm.vo.DeliveryLogVo;
import cn.edm.vo.DeliveryVo;

public interface DeliveryService {
	
	/**
	 * 获取投递监控数据
	 * @param fromDomain
	 * @param status
	 * @return
	 */
	List<DeliveryVo> getDeliveryVoList(String fromDomain,String status);
	
	/**
	 * 获取投递日志数据
	 * @param ip
	 * @param querytime
	 * @return
	 */
	Map<String,DeliveryLogVo> getDeliveryLogList(String ip,String querytime,int startIndex,int pageSize);
	
	void writeCsv(CSVWriter csvWriter,String date,String[] ip);
	
}
