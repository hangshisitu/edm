package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Result;

public interface ResultMapper {
	
	/**
	 * 分页查询任务统计记录集合
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectResultList(Map<String, Object> paramsMap);
	
	/**
	 * 查询任务统计记录数量
	 * @param paramsMap
	 * @return
	 */
	long selectResultCount(Map<String, Object> paramsMap);
	
	/**
	 * 查询任务统计
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectAll(Map<String, Object> paramsMap);
	
	/**
	 * 根据发送域名统计
	 * @param paramsMap
	 * @return
	 */
	Result selectByDomainStat(Map<String, Object> paramsMap);
	
	/**
	 * API触发统计
	 * @param paramsMap
	 * @return
	 */
	Result selectApiTriggerSum(Map<String, Object> paramsMap);
	/**
	 * API触发统计,根据客户
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectApiTriggerByUserList(Map<String, Object> paramsMap);
	/**
	 * API触发统计总数,根据客户
	 * @param paramsMap
	 * @return
	 */
	long selectApiTriggerByUserCount(Map<String, Object> paramsMap);
	
	List<Result> selectApiByUserList(Map<String, Object> paramsMap);
	
	List<Result> selectApiByTaskList(Map<String, Object> paramsMap);
	
	long selectApiByTaskCount(Map<String, Object> paramsMap);
	
	
	List<Result> selectAllApi(Map<String, Object> paramsMap);
	/**
	 * 查询所有API触发任务记录 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectAllApiCount(Map<String, Object> paramsMap);
	
	List<Result> selectCampaignList(Map<String, Object> paramsMap);
	
	long selectCampaignCount(Map<String, Object> paramsMap);
	
//	Result selectCampaignSum(Integer campaignId);
	
	
	List<Result> selectCampaignResultList(Map<String, Object> paramsMap);
	
	long selectCampaignResultCount(Map<String, Object> paramsMap);
	
	
	List<Result> selectCampaignResult(Integer campaignId);
	
	/**
	 * 最近两周
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectCampaignRangeTwoWeek(Map<String, Object> paramsMap);
	
	/**
	 * 最近三周
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectCampaignRangeThreeWeek(Map<String, Object> paramsMap);
	
	/**
	 * 最近两周
	 * @param paramsMap
	 * @return
	 */
	List<Result> selectCampaignRangeFourWeek(Map<String, Object> paramsMap);
		
	Result selectCampaignRange(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务统计列表
	 * @param paramsMap
	 * @return
	 */
//	public List<Result> selectPlanTaskList(Map<String, Object> paramsMap);
//	/**
//	 * 查询周期任务投递结果列表
//	 * @param paramsMap
//	 * @return
//	 */
//	public List<Result> selectPlanResultList(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务投递结果SUM列表
	 * @param paramsMap
	 * @return
	 */
	public List<Result> selectPlanResultSumList(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务统计总记录数
	 * @param paramsMap
	 * @return
	 */
	public long selectPlanTaskCount(Map<String, Object> paramsMap);
	/**
	 * 通过任务ID获得任务统计信息(没投递效果)
	 * @param paramsMap
	 * @return
	 */
	public List<Result> selectPlanTaskByPlanId(Map<String, Object> paramsMap);
	/**
	 * 周期任务投递统计汇总
	 * @param paramsMap
	 * @return
	 */
	public Result selectPlanResultSum(Map<String, Object> paramsMap);
	/**
	 * 通过企业ID获取有投递效果的任务
	 * 如果是有投递效果的周期任务，则返回父节点记录
	 * @param paramsMap
	 * @return
	 */
	public List<Result> selectViewDeliveryTasksByCorpId(Map<String, Object> paramsMap);
	/**
	 * 客户发送统计列表 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectViewDeliveryTaskCount(Map<String, Object> paramsMap);
	/**
	 * API群发
	 * @param paramsMap
	 * @return
	 */
	public Result selectApiGroupsSum(Map<String, Object> paramsMap);
	/**
	 * 获取用户API群发任务统计列表
	 * @param paramsMap
	 * @return
	 */
	public List<Result> selectApiGroupsByUserList(Map<String, Object> paramsMap);
	/**
	 * 获取API群发任务统计列表 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectApiGroupsByUserCount(Map<String, Object> paramsMap);
	/**
	 * 客户API群发任务列表
	 * @param paramsMap
	 * @return
	 */
	public List<Result> selectApiGroupsByTaskList(Map<String, Object> paramsMap);
	/**
	 * 客户API群发任务 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectApiGroupsByTaskCount(Map<String, Object> paramsMap);
	
	public Result selectTouchResult(Map<String, Object> paramsMap);
	
}
