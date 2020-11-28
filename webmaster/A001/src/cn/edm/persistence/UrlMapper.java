package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Url;



public interface UrlMapper {
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	List<Url> selectByTaskId(Map<String,Object> paramsMap); 
	
	/**
	 * 根据任务id查询点击数
	 * @param taskId
	 * @return
	 */
	Url selectClickCountByTaskId(Map<String,Object> paramsMap);
	
	/**
	 * 获取活动点击统计
	 * @param campaignId
	 * @return
	 */
	List<Url> selectCampaignClick(Integer campaignId);
	
	/**
	 * 根据活动id查询点击数
	 * @param campaignId
	 * @return
	 */
	Integer selectCampaignSum(Integer campaignId);
}
