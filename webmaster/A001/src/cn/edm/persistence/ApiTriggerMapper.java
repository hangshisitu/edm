package cn.edm.persistence;


import java.util.List;

import cn.edm.domain.ApiTrigger;

public interface ApiTriggerMapper {
	
	void insert(ApiTrigger apiTrigger);
	
	ApiTrigger selectByCorpId(Integer corpId);
	
	List<String> selectTriggerName();

	void update(ApiTrigger apiTrigger);
	
	void deleteById(Integer triggerId);
	/**
	 * 通过企业ID，查询所有的api触发
	 * @param corpId
	 * @return
	 */
	public List<ApiTrigger> selectApiTriggersByCorpId(Integer corpId);
	/**
	 * 通过用户ID，查询所有的api触发
	 * @param userId
	 * @return
	 */
	public ApiTrigger selectByUserId(String userId);
	
}
