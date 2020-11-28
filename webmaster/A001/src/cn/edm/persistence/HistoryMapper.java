package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.History;

public interface HistoryMapper {
	
	/**
	 * 查询记录集合
	 * @param paramsMap
	 * @return
	 */
	List<History> selectHistoryList(Map<String, Object> paramsMap);
	
	/**
	 * 查询记录数
	 * @param paramsMap
	 * @return
	 */
	long selectHistoryCount(Map<String, Object> paramsMap);
	
	/**
	 * 保存
	 * @param his
	 */
	void insertHistory(History his);
}
