package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Plan;
import cn.edm.persistence.PlanMapper;
import cn.edm.service.PlanService;
import cn.edm.util.Pagination;
@Service
public class PlanServiceImpl implements PlanService {
	
	@Autowired
	private PlanMapper planMapper;

	@Override
	public Plan getPlanById(Map<String, Object> paramsMap, Integer planId) {
		paramsMap.put("planId", planId);
		Plan plan = planMapper.selectPlan(paramsMap);
		return plan;
	}

	@Override
	public Pagination<Plan> getPlanListByIds(Map<String, Object> paramsMap, Integer[] planIds, int currentPage, int pageSize) {
		paramsMap.put("planIds", planIds);
		List<Plan> plans = planMapper.selectPlanList(paramsMap);
		long count = planMapper.selectPlanCount(paramsMap);
		Pagination<Plan> pages = new Pagination<Plan>(currentPage, pageSize, count, plans);
		return pages;
	}

}
