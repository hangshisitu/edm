package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Region;

public interface RegionService {
	/**
	 * 根据任务id按照省份分组查询
	 * @param taskId
	 * @return
	 */
	List<Region> getByProvince(Map<String,Object> paramsMap); 
	
	/**
	 * 根据任务id按照城市分组查询
	 * @param taskId
	 * @return
	 */
	List<Region> getByCity(Map<String,Object> paramsMap); 
}
