package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Region;




public interface RegionMapper {
	
	/**
	 * 根据任务id按照省份分组查询
	 * @param taskId
	 * @return
	 */
	List<Region> selectByProvince(Map<String,Object> paramsMap); 
	
	/**
	 * 根据任务id按照城市分组查询
	 * @param taskId
	 * @return
	 */
	List<Region> selectByCity(Map<String,Object> paramsMap); 
	
	
	List<Region> selectCampaignProvince(Integer campaignId);
	
	List<Region> selectCampaignCity(Integer campaignId);
	
}
