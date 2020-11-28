package cn.edm.service;

import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import au.com.bytecode.opencsv.CSVWriter;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

import cn.edm.constant.ConfigKeys;
import cn.edm.persistence.DeliveryQuantityMapper;
import cn.edm.util.Apis;
import cn.edm.util.DateUtil;
import cn.edm.util.LogHashMap;
import cn.edm.util.MyHashMap;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.QuantityVoComparator;
import cn.edm.vo.DeliveryLog;
import cn.edm.vo.DeliveryLogVo;
import cn.edm.vo.DeliveryVo;
import cn.edm.vo.QuantityVo;

@Service
public class DeliveryServiceImpl implements DeliveryService{
	
	@Autowired
	private DeliveryQuantityMapper deliveryQuantityMapper;

	@Override
	public List<DeliveryVo> getDeliveryVoList(String fromDomain, String status) {
		
		List<DeliveryVo> resultList = null;
		
		String url = PropertiesUtil.get(ConfigKeys.FROMDOMAIN_URL)+"?cmd=2003";
		if(fromDomain!=null && !"".equals(fromDomain.trim())&& !"null".equals(fromDomain.trim())) {
			url += "&from="+fromDomain.trim();
		}
		if(status!=null && !"".equals(status.trim())&& !"null".equals(status.trim())) {
			url += "&status="+status.trim();
		}
		String content = Apis.getProperty(url, "utf-8");
		if(content!=null&&!"".equals(content.trim())) {
			resultList = parseDeliveryData(content);
		}
		return resultList;
	}
	
	/**
	 * 解析监控数据
	 * @param content
	 * @return
	 */
	private List<DeliveryVo> parseDeliveryData(String content) {
		List<DeliveryVo> resultList = Lists.newArrayList();
		String[] results = StringUtils.split(content, "\r\n");
		if(results!=null&&results.length>0) {
			Map<String,String> dataMap = new MyHashMap();
			for(String line:results) {
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 4 || StringUtils.isBlank(records[0])) {
					continue;
				}
				String key = records[0].trim()+"_"+records[1].trim();
				String value = records[2].trim()+"/"+records[3].trim();
				dataMap.put(key, value);
			}
			
			Iterator<Entry<String,String>>  iterator =  dataMap.entrySet().iterator();
			while(iterator.hasNext()) {
			    Entry<String,String> entry = iterator.next();
			    String key = entry.getKey();
			    String value = entry.getValue();
			    
			    DeliveryVo vo = new DeliveryVo();
			    String[] domain = StringUtils.splitPreserveAllTokens(key, "_");
			    vo.setFromDomain(domain[0]);
				vo.setRcptDomain(domain[1]);
				
				String[] values = StringUtils.splitPreserveAllTokens(value, ",");
				List<QuantityVo> qvList = Lists.newArrayList();
				for(String v:values) {
					QuantityVo qv = new QuantityVo();
					String[] rule = StringUtils.splitPreserveAllTokens(v, "/");
					if("900".equals(rule[1].trim())) {
						vo.setQuarter(rule[0]+"/15分钟");
						if("normal".equals(rule[2].trim())) {
							vo.setQuarterStatus("正常");
						}else if("stop".equals(rule[2].trim())) {
							vo.setQuarterStatus("超限停止");
						}
						qv.setRule(vo.getQuarter());
						qv.setStatus(vo.getQuarterStatus());
						qv.setOrder(1);
						qvList.add(qv);
					}else if("3600".equals(rule[1].trim())){
						vo.setHour(rule[0]+"/1小时");
						if("normal".equals(rule[2].trim())) {
							vo.setHourStatus("正常");
						}else if("stop".equals(rule[2].trim())) {
							vo.setHourStatus("超限停止");
						}
						qv.setRule(vo.getHour());
						qv.setStatus(vo.getHourStatus());
						qv.setOrder(2);
						qvList.add(qv);
					}else if("86400".equals(rule[1].trim())){
						vo.setDay(rule[0]+"/1天");
						if("normal".equals(rule[2].trim())) {
							vo.setDayStatus("正常");
						}else if("stop".equals(rule[2].trim())) {
							vo.setDayStatus("超限停止");
						}
						qv.setRule(vo.getDay());
						qv.setStatus(vo.getDayStatus());
						qv.setOrder(3);
						qvList.add(qv);
					}
				}
				vo.setQueryTime(DateUtil.getCurrentDate());
				Collections.sort(qvList,new QuantityVoComparator());
				vo.setQvList(qvList);
				resultList.add(vo);
				
			}
		
		}
		return resultList;
	}

	@Override
	public Map<String, DeliveryLogVo> getDeliveryLogList(String ip,
			String querytime,int startIndex,int pageSize) {
		String url = PropertiesUtil.get(ConfigKeys.FROMDOMAIN_URL)+"?cmd=2002";
		if(ip!=null && !"".equals(ip.trim())&& !"null".equals(ip.trim())) {
			url += "&ip="+ip.trim();
		}
		if(querytime!=null && !"".equals(querytime.trim())&& !"null".equals(querytime.trim())) {
			querytime = StringUtils.replace(querytime, "-", "");
			url += "&date="+querytime.trim();
		}
		String content = Apis.getProperty(url, "utf-8");
		if(content!=null&&!"".equals(content.trim())){
			return parseLogData(content,startIndex,pageSize);
		}else {
			Map<String, DeliveryLogVo> map = Maps.newHashMap();
			map.put("total", new DeliveryLogVo(0));
			return map;
		}
	}
	
	private Map<String, DeliveryLogVo> parseLogData(String content,int startIndex,int pageSize) {
		Map<String, DeliveryLogVo> logMap = new LogHashMap();
		String[] results = StringUtils.split(content, "\r\n");
		if(results!=null&&results.length>0) {
			int len = results.length;
			System.out.println("count-results---"+results.length);
			logMap.put("total", new DeliveryLogVo(len));
			
			int end = startIndex + pageSize -1;
			if(end > len) {
				end = len-1;
			}
			
			
			for(int i=startIndex;i<=end;i++) {
				String line = results[i];
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 6 || StringUtils.isBlank(records[0])) {
					continue;
				}
				
				DeliveryLogVo logVo = new DeliveryLogVo();
				logVo.setIp(records[0].trim());
				DeliveryLog log = new DeliveryLog();
				log.setIp(records[0]);
				log.setFromDomain(records[1]);
				log.setRcptDomain(records[2]);
				log.setQueryTime(records[3]);
				log.setSuccess(records[4]);
				log.setFailure(records[5]);
				List<DeliveryLog> logList = Lists.newArrayList();
				logList.add(log);
				logVo.setLogList(logList);
				logMap.put(records[0].trim(), logVo);
			}
			
		}else {
			logMap.put("total", new DeliveryLogVo(0));
		}
		return logMap;
	}

	@Override
	public void writeCsv(CSVWriter csvWriter, String date, String[] ip) {
		String[] title = new String[] {"IP","发件域", "收件域", "成功数", "失败数", "　　时间"};
		csvWriter.writeNext(title);
		for(int j=0;j<ip.length;j++) {
			Map<String, DeliveryLogVo> logMap = getLogData(date,ip[j]);
			if(logMap != null&&logMap.size()>0) {
				
				Iterator<Entry<String,DeliveryLogVo>> it = logMap.entrySet().iterator();
				String[] content = new String[title.length];
				while(it.hasNext()) {
					Entry<String,DeliveryLogVo> entry = it.next();
					String key = entry.getKey();
					DeliveryLogVo value = entry.getValue();
					int len = value.getLogList().size();
					for(int i=0;i<len;i++) {
						DeliveryLog log = value.getLogList().get(i);
						content[0] = key;
						content[1] = log.getFromDomain();
						content[2] = log.getRcptDomain();
						content[3] = log.getSuccess();
						content[4] = log.getFailure();
						content[5] = "　"+date;
						csvWriter.writeNext(content);
					}
				}
			}
		}
		
		
		
	}
	
	private Map<String, DeliveryLogVo> getLogData(String date,String ip){
		Map<String, DeliveryLogVo> logMap = new LogHashMap();
		String url = PropertiesUtil.get(ConfigKeys.FROMDOMAIN_URL)+"?cmd=2002";
		if(ip!=null && !"".equals(ip.trim())&& !"null".equals(ip.trim())) {
			url += "&ip="+ip.trim();
		}
		if(date != null && !"".equals(date.trim())&& !"null".equals(date.trim())) {
			date = StringUtils.replace(date, "-", "");
			url += "&date="+date.trim();
		}
		String content = Apis.getProperty(url, "utf-8");
		String[] results = StringUtils.split(content, "\r\n");
		if(results!=null && results.length>0) {
			int len = results.length;
			System.out.println("count-results---"+results.length);
			for(int i=0;i<len;i++) {
				String line = results[i];
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 6 || StringUtils.isBlank(records[0])) {
					continue;
				}
				
				DeliveryLogVo logVo = new DeliveryLogVo();
				logVo.setIp(records[0].trim());
				DeliveryLog log = new DeliveryLog();
				log.setIp(records[0]);
				log.setFromDomain(records[1]);
				log.setRcptDomain(records[2]);
				log.setQueryTime(records[3]);
				log.setSuccess(records[4]);
				log.setFailure(records[5]);
				List<DeliveryLog> logList = Lists.newArrayList();
				logList.add(log);
				logVo.setLogList(logList);
				logMap.put(records[0].trim(), logVo);
			}
		}
		return logMap;
	}
	
	
	

}
