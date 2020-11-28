package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Plan;

public interface PlanMapper {
	/**
	 * 获得计划信息Plan
	 * @param paramsMap
	 * @return
	 */
	public Plan selectPlan(Map<String, Object> paramsMap);
	/**
	 * 获得计划信息列表
	 * @param paramsMap
	 * @return
	 */
	public List<Plan> selectPlanList(Map<String, Object> paramsMap);
	/**
	 * 获得计划信息总记录数
	 * @param paramsMap
	 * @return
	 */
	public long selectPlanCount(Map<String, Object> paramsMap);
	
}
