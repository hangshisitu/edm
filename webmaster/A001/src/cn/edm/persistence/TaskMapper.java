package cn.edm.persistence;

import java.util.Date;
import java.util.List;
import java.util.Map;

import cn.edm.domain.Task;

/**
 * @author Luxh
 */
public interface TaskMapper {
	
	/**
	 * 根据条件查询任务
	 * @param paramsMap
	 * @return
	 */
	List<Task> selectList(Map<String, Object> paramsMap);
	
	/**
	 * 根据条件查询任务数量
	 * @param paramsMap
	 * @return
	 */
	long selectCount(Map<String, Object> paramsMap);
	
	/**
	 * 根据任务id查询
	 * @param paramsMap
	 */
	void audit(Task task);
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	Task selectByTaskId(Integer taskId);
	
	/**
	 * 根据域名查询
	 * @param paramsMap
	 * @return
	 */
	List<Task> selectBySenderEmail(Map<String, Object> paramsMap);
	
	void updateLabelStatus(Map<String, Object> paramsMap);
	
	List<Task> selectTasks(Map<String, Object> paramsMap);
	
	List<Task> selectTasksByCampaignId(Integer campaignId);
	
	
	Integer selectTaskPeriod(Integer campaignId);
	
	
	Date selectMinTime(Integer campaignId);
	/**
	 * 查询任务优先级列表
	 * @param paramsMap
	 * @return
	 */
	public List<Task> selectTaskPriorityList(Map<String, Object> paramsMap);
	
	long selectPriorityListCount(Map<String, Object> paramsMap);
	/**
	 * 更新任务优先级
	 * @param paramsMap
	 */
	public void updateTaskPriority(Map<String, Object> paramsMap);
	/**
	 * 统计任务投递数
	 * @param paramsMap
	 * @return
	 */
	public int selectTaskCount(Map<String,Object> paramsMap);
	/**
	 * 获取有投递效果的单个周期任务
	 * @param paramsMap
	 * @param planId
	 * @return
	 */
	public Task selectPlanDeliveryTask(Map<String,Object> paramsMap);
	/**
	 * 获取有投递效果的周期任务列表
	 * @param paramsMap
	 * @param planIds
	 * @return
	 */
	public List<Task> selectPlanTasksByPlanId(Map<String,Object> paramsMap);
	/**
	 * 获取周期任务信息
	 * @param paramsMap
	 * @return
	 */
	public Task selectPlanTaskByPlanId(Map<String,Object> paramsMap);
	/**
	 * 触发数
	 * @param paramsMap
	 * @return
	 */
	public int selectTouchCount(Map<String,Object> paramsMap);
	
	public List<Task> selectPlanTouchTask(Map<String,Object> paramsMap);
	
	public List<Task> selectCampaignTouch(Map<String,Object> paramsMap);
	
	public void delTaskByUserId(String userId);
	
}
