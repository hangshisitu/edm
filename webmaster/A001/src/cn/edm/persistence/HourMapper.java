package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Hour;



public interface HourMapper {
	
	/**
	 * 根据任务id查询
	 * @return
	 */
	List<Hour> selectByTaskId(Map<String,Object> paramsMap); 
	
	/**
	 * 查询活动统计数据
	 * @param campaignId
	 * @return
	 */
	List<Hour> selectCampaignHour(Integer campaignId);
	
}
