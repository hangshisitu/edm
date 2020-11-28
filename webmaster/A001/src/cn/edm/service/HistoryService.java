package cn.edm.service;

import java.util.Map;

import cn.edm.domain.History;
import cn.edm.util.Pagination;

public interface HistoryService {
	
	/**
	 * 保存历史记录
	 * @param his
	 */
	void saveHistory(History his);
	
	
	/**
	 * 分页查询
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<History> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
}
