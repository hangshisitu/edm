package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Domain;

public interface DomainMapper {
	
	/**
	 * 发送总览
	 * @param paramsMap
	 * @return
	 */
	Domain selectSum(Map<String,Object> paramsMap);
	
	/**
	 * 发送总览
	 * @param paramsMap
	 * @return
	 */
	List<Domain> selectSumList(Map<String,Object> paramsMap);
	
	/**
	 * 查询一级机构下属所有机构的发送详情列表
	 * @param paramsMap
	 * @return
	 */
	List<Domain> selectDomainResultList(Map<String,Object> paramsMap);
	
	/**
	 * 客户任务结果统计总数
	 * @param paramsMap
	 * @return
	 */
	long selectDomainResultCount(Map<String,Object> paramsMap);
	
	/**
	 * 查询客户发送统计信息
	 * @param paramsMap
	 * @return
	 */
	List<Domain> selectUserStatList(Map<String,Object> paramsMap);
	
	/**
	 * 根据任务id按域名分组查询
	 * @param paramsMap
	 * @return
	 */
	List<Domain> selectByTaskIdAndGroupByDomain(Map<String,Object> paramsMap);
	
	/**
	 * 根据任务id按域名分组查询总数
	 * @param paramsMap
	 * @return
	 */
	long selectByTaskIdAndGroupByDomainCount(Map<String,Object> paramsMap);
	
	/**
	 * 发送总览(api触发)
	 * @param paramsMap
	 * @return
	 */
	Domain selectApiSum(Map<String,Object> paramsMap);
	
	/**
	 * 发送弹回统计
	 * @param paramsMap
	 * @return
	 */
	Domain selectSendBackSum(Map<String,Object> paramsMap);
	
	/**
	 * 发送
	 * @param campaignId
	 * @return
	 */
	Domain selectCampaignSendBackSum(Integer campaignId);
	
	/**
	 * 活动域名发送统计
	 * @param campaignId
	 * @return
	 */
	List<Domain> selectCampaignDomainSum(Integer campaignId);
	/**
	 * API群发
	 * @param paramsMap
	 * @return
	 */
	public Domain selectApiGroupsSum(Map<String, Object> paramsMap);
	////////////////////////////////////////////////
	////////////////////////////////////////////////
	/**
	 * 查询所有API触发任务记录列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectAllApi(Map<String,Object> paramsMap);
	/**
	 * 查询所有API触发任务记录 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectAllApiCount(Map<String,Object> paramsMap);
	
	public List<Domain> selectApiByUserList(Map<String,Object> paramsMap);
	/**
	 * 客户发送统计列表 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectViewDeliveryTaskCount(Map<String, Object> paramsMap);
	/**
	 * API触发统计,根据客户
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectApiTriggerByUserList(Map<String,Object> paramsMap);
	/**
	 * API触发统计总数,根据客户
	 * @param paramsMap
	 * @return
	 */
	public long selectApiTriggerByUserCount(Map<String,Object> paramsMap);
	
	public List<Domain> selectApiByTaskList(Map<String,Object> paramsMap);
	
	public long selectApiByTaskCount(Map<String,Object> paramsMap);
	/**
	 * 获取用户API群发任务统计列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectApiGroupsByUserList(Map<String, Object> paramsMap);
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
	public List<Domain> selectApiGroupsByTaskList(Map<String, Object> paramsMap);
	/**
	 * 客户API群发任务 总数
	 * @param paramsMap
	 * @return
	 */
	public long selectApiGroupsByTaskCount(Map<String, Object> paramsMap);
	/**
	 * 通过企业ID获取有投递效果的任务
	 * 如果是有投递效果的周期任务，则返回父节点记录
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectViewDeliveryTasksByCorpId(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务投递结果SUM列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectPlanResultSumList(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务统计总记录数
	 * @param paramsMap
	 * @return
	 */
	public long selectPlanTaskCount(Map<String, Object> paramsMap);
	/**
	 * 周期任务投递统计汇总
	 * @param paramsMap
	 * @return
	 */
	public Domain selectPlanResultSum(Map<String, Object> paramsMap);
	/**
	 * 查询任务统计
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectDomainSumByTask(Map<String,Object> paramsMap);
	/**
	 * API触发统计
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectApiTriggerSum(Map<String,Object> paramsMap);
	/**
	 * 查询周期任务投递结果列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectPlanResultList(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务历史投递结果列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectResultHisList(Map<String, Object> paramsMap);
	/**
	 * 查询周期任务统计列表
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectPlanTaskList(Map<String, Object> paramsMap);
	/**
	 * 分页查询任务统计记录集合
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> selectResultList(Map<String,Object> paramsMap);
	/**
	 * 查询任务统计记录数量
	 * @param paramsMap
	 * @return
	 */
	public long selectResultCount(Map<String,Object> paramsMap);
	/**
	 * 触发任务发送合计统计
	 * @param paramsMap
	 * @return
	 */
	public Domain selectTouchSum(Map<String,Object> paramsMap);
	
	public List<Domain> selectTouchList(Map<String,Object> paramsMap);
	
	public List<Domain> selectCampaignTouchList(Map<String,Object> paramsMap);

	public List<Domain> selectDomainSum(Map<String,Object> paramsMap);
	
	public Domain selectByDomainStat(Map<String,Object> paramsMap);
	
	public Domain selectCampaignSum(Map<String,Object> paramsMap);
	
	public List<Domain> selectByDomainSumByCorp(Map<String,Object> paramsMap);
	
	public List<Domain> selectAll(Map<String,Object> paramsMap);
	
	public List<Domain> selectApiTriggerSumList(Map<String,Object> paramsMap);
	
	public List<Domain> selectCampaignSumList(Map<String,Object> paramsMap);
	/**
	 * 对比
	 * @param paramsMap
	 * @return
	 */
	public Domain selectCampaignRange(Map<String, Object> paramsMap);
	
}
