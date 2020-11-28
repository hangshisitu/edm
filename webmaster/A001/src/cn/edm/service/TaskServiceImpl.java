package cn.edm.service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.google.common.collect.Maps;

import cn.edm.constant.Role;
import cn.edm.constant.Status;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Task;
import cn.edm.domain.User;
import cn.edm.persistence.TaskMapper;
import cn.edm.persistence.UserMapper;
import cn.edm.security.UserContext;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.Pagination;
import cn.edm.utils.file.Files;

/**
 * 任务业务接口实现
 * @author Luxh
 *
 */
@Service
public class TaskServiceImpl implements TaskService{
	
	@Autowired
	private TaskMapper taskMapper;
	
	@Autowired
	private UserMapper userMapper;
	
	@Override
	public Pagination<Task> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		// 单项任务与周期任务的审核条件
		// 还有API类的群发任务审核
		Integer[] types = {TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_PARENT_PLAN, TaskTypeConstant.TASK_API_GROUPS};
		paramsMap.put("types", types);
		
		List<Task> recordList = taskMapper.selectList(paramsMap);
		long recordCount = taskMapper.selectCount(paramsMap);
		
		Pagination<Task> pagination = new Pagination<Task>(currentPage,pageSize,recordCount,recordList);
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public String audit(Integer taskId,String flag,String suggestion) {
		String message = "";
		if(taskId == null) {
			message = "任务不存在";
		}else {
			Task task = taskMapper.selectByTaskId(taskId);
			if(task == null) {
				message = "任务不存在";
			}else {
				if(task.getEmailCount()<1) {
					message = "该任务发送人数为空";
				}else if(Status.REVIEWING!=task.getStatus()){
					message = "该任务已不在待审核状态";
				}else {
					if("yes".equals(flag)) {
						if(task.getJobTime()!=null) {
							task.setStatus(Status.WAITED);//待发送
						}else {
							if(task.getPlanId()!=null && "None".equals(task.getPlanReferer())){
								task.setStatus(Status.WAITED);//待发送
							}else{
								task.setStatus(Status.QUEUEING);//队列中
							}
						}
					}else {
						task.setStatus(Status.RETURN);//审核不通过
					}
					if(task.getUserId()==null||"".equals(task.getUserId())) {
						Map<String,Object> paramsMap = new HashMap<String,Object>();
						paramsMap.put("corpId", task.getCorpId());
						paramsMap.put("roleId", Role.MGR);
						User user = userMapper.selectByCorpIdAndRoleId(paramsMap);
						if(user != null) {
							task.setUserId(user.getUserId());
						}
					}
					task.setModeratePath("/"+UserContext.getCurrentUser().getCorpId()+"/");
					task.setModifyTime(DateUtil.getCurrentDate());
					task.setSuggestion(suggestion);
					taskMapper.audit(task);
					if("yes".equals(flag)) {
						if(task.getType() == TaskTypeConstant.TASK_PARENT_PLAN) {
							Files.createPlan(task.getPlanId());
						}else{
							Files.createTask(task.getTaskId());
						}
					}
					message = "审核成功！";
				}
				
			}
		}
		return message;
		
	}

	@Override
//	@Cacheable(cacheName="reportCache")
	public Task getByTaskId(Integer taskId) {
		return taskMapper.selectByTaskId(taskId);
	}

	@Override
//	@Cacheable(cacheName="reportCache")
	public List<Task> getBySenderEmail(Map<String, Object> paramsMap) {
		return taskMapper.selectBySenderEmail(paramsMap);
	}

	@Override
	public List<Task> getTasks(Map<String, Object> paramsMap) {
		return taskMapper.selectTasks(paramsMap);
	}

	@Override
	public List<Task> getTasksByCampaignId(Integer campaignId) {
		return taskMapper.selectTasksByCampaignId(campaignId);
	}

	@Override
	public void updateTaskPriority(Map<String, Object> paramsMap) {
		taskMapper.updateTaskPriority(paramsMap);
	}

	@Override
	public Pagination<Task> getTaskPriorityPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		// 可供调控的在投、待投任务，状态 status in ( 21,22,23,24,25 )
		Integer[] status = {Status.TEST, Status.WAITED, Status.QUEUEING, Status.PROCESSING, Status.PAUSED};
		paramsMap.put("status", status);
		paramsMap.put("emailCount", ">0");
		
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_API_GROUPS};
		paramsMap.put("types", types);
		
		List<Task> recordList = taskMapper.selectTaskPriorityList(paramsMap);
		long recordCount = taskMapper.selectPriorityListCount(paramsMap);
		
		Pagination<Task> pagination = new Pagination<Task>(currentPage, pageSize, recordCount, recordList);
		return pagination;
	}

	@Override
	public int getTaskCount(Map<String, Object> paramsMap, Integer planId) {
		paramsMap.put("planId", planId);
//		paramsMap.put("planReferer", "None");
//		paramsMap.put("planRefererCnd", "!eq");
		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
		paramsMap.put("status", Status.task("processing|completed"));
		int count = taskMapper.selectTaskCount(paramsMap);
		return count;
	}

	@Override
	public Task getPlanDeliveryTask(Map<String, Object> paramsMap, Integer planId) {
		paramsMap.put("planId", planId);
		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
		paramsMap.put("status", Status.task("processing|completed"));
		Task task = taskMapper.selectPlanDeliveryTask(paramsMap);
		return task;
	}

	@Override
	public List<Task> getPlanTasksByPlanId(Map<String, Object> paramsMap, Integer planId) {
		paramsMap.put("planId", planId);
		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
		paramsMap.put("status", Status.task("processing|completed"));
		List<Task> tasks = taskMapper.selectPlanTasksByPlanId(paramsMap);
		return tasks;
	}

	@Override
	public Task getPlanTaskByPlanId(Map<String, Object> paramsMap, Integer planId, Integer type) {
		paramsMap.clear();
		paramsMap.put("planId", planId);
		paramsMap.put("type", type);
		Task task = taskMapper.selectPlanTaskByPlanId(paramsMap);
		return task;
	}

	@Override
	public int getTouchCount(Integer otherId, String type) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		if("plan".equals(type)){
			paramsMap.put("planId", otherId);
    		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
    		paramsMap.put("statuses", Status.task("processing|completed"));
		}else if("campaign".equals(type)){
			paramsMap.put("campaignId", otherId);
//    		paramsMap.put("type", TaskTypeConstant.TASK_SINGLE);
    		paramsMap.put("statuses", Status.task("completed"));
		}
		return taskMapper.selectTouchCount(paramsMap);
	}

	@Override
	public List<Task> getTouchTask(Integer otherId, String type) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		if("plan".equals(type)) {
    		paramsMap.put("planId", otherId);
    		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
    		paramsMap.put("statuses", Status.task("processing|completed"));
    		return taskMapper.selectPlanTouchTask(paramsMap);
    	}else if("campaign".equals(type)){
    		paramsMap.put("campaignId", otherId);
//    		paramsMap.put("type", TaskTypeConstant.TASK_SINGLE);
    		paramsMap.put("statuses", Status.task("completed"));
    		return taskMapper.selectCampaignTouch(paramsMap);
    	}
		return null;
	}

}
