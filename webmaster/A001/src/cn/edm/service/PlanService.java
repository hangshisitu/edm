package cn.edm.service;

import java.util.Map;

import cn.edm.domain.Plan;
import cn.edm.util.Pagination;

/**
 * 任务计划接口
 * 
 * @Date 2014年6月12日 下午6:35:27
 * @author Lich
 *
 */
public interface PlanService {
	/**
	 * 通过计划ID获得获得计划
	 * @param planId
	 * @return
	 */
	public Plan getPlanById(Map<String, Object> paramsMap, Integer planId);
	/**
	 * 通过计划planIds获得获得计划列表
	 * @param planIds
	 * @return
	 */
	public Pagination<Plan> getPlanListByIds(Map<String, Object> paramsMap, Integer[] planIds, int currentPage, int pageSize);
	
}
