package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Task;
import cn.edm.util.Pagination;

/**
 * 任务业务接口
 * @author Luxh
 *
 */
public interface TaskService {
	
	/**
	 * 分页查询任务
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Task> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 审核任务
	 * @param taskId  任务id
	 * @param flag	  通过还是不通过标识
	 * @param suggestion	审核意见
	 * @return
	 */
	String audit(Integer taskId,String flag,String suggestion);
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	Task getByTaskId(Integer taskId);
	
	/**
	 * 根据域名查询
	 * @param paramsMap
	 * @return
	 */
	List<Task> getBySenderEmail(Map<String,Object> paramsMap);
	
	List<Task> getTasks(Map<String,Object> paramsMap);
	
	List<Task> getTasksByCampaignId(Integer campaignId);
	/**
	 * 分页查询任务优先级列表
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Task> getTaskPriorityPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	/**
	 * 更新任务优先级
	 * @param paramsMap
	 */
	public void updateTaskPriority(Map<String,Object> paramsMap);
	/**
	 * 统计任务投递数
	 * @param paramsMap
	 * @return
	 */
	public int getTaskCount(Map<String,Object> paramsMap, Integer planId);
	/**
	 * 获取有投递效果的单个周期任务
	 * @param paramsMap
	 * @param planId
	 * @return
	 */
	public Task getPlanDeliveryTask(Map<String,Object> paramsMap, Integer planId);
	/**
	 * 获取有投递效果的周期任务列表
	 * @param paramsMap
	 * @param planIds
	 * @return
	 */
	public List<Task> getPlanTasksByPlanId(Map<String,Object> paramsMap, Integer planId);
	/**
	 * 获取周期任务信息
	 * @param paramsMap
	 * @param planId
	 * @param type
	 * @return
	 */
	public Task getPlanTaskByPlanId(Map<String,Object> paramsMap, Integer planId, Integer type);
	/**
	 * 获取是否有触发任务
	 * @param otherId
	 * @param type 
	 * 			1. 周期任务：plan
	 * 			2. 活动： campaign
	 * @return
	 */
	public int getTouchCount(Integer otherId, String type);
	
	public List<Task> getTouchTask(Integer otherId, String type);
	
}
