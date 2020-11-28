package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.googlecode.ehcache.annotations.TriggersRemove;

import cn.edm.domain.History;
import cn.edm.persistence.HistoryMapper;
import cn.edm.service.HistoryService;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
@Service
public class HistoryServiceImpl implements HistoryService{
	
	@Autowired
	private HistoryMapper historyMapper;
	
	@Override
//	@Cacheable(cacheName="historyCache")
	public Pagination<History> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<History> recordList = historyMapper.selectHistoryList(paramsMap);
		long recordCount = historyMapper.selectHistoryCount(paramsMap);
		Pagination<History> pagination = new Pagination<History>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	@TriggersRemove(cacheName="historyCache",removeAll=true)
	public void saveHistory(History his) {
		historyMapper.insertHistory(his);
	}

}
