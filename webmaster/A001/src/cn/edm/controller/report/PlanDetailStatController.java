package cn.edm.controller.report;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Domain;
import cn.edm.domain.Plan;
import cn.edm.domain.Task;
import cn.edm.domain.Touch;
import cn.edm.domain.TouchSetting;
import cn.edm.service.DomainService;
import cn.edm.service.PlanService;
import cn.edm.service.TaskService;
import cn.edm.service.TouchService;
import cn.edm.service.TouchSettingService;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;

import com.google.common.collect.Maps;
/**
 * 周期任务报表详细
 * 
 * @Date 2014年6月11日 下午3:29:59
 * @author Lich
 *
 */
@Controller
@RequestMapping("/plan/planDetailStat")
public class PlanDetailStatController {
	
	@Autowired
	private DomainService domainService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private PlanService planService;
	@Autowired
	private TouchSettingService settingService;
	@Autowired
	private TouchService touchService;
	
	/**
	 * 周期任务统计概览
	 * @param request
	 * @param modelMap
	 * @return
	 */
	@RequestMapping(value="/planReportView", method=RequestMethod.GET)
	public String planReportView(HttpServletRequest request, ModelMap modelMap) {
		Integer planId = XssFilter.filterXssInt(request.getParameter("planId"));
		//过滤参数中的空值
		Map<String,Object> realParamsMap = new HashMap<String, Object>();
		if(planId != null) {
			Plan plan = planService.getPlanById(realParamsMap, planId);
			int taskCount = taskService.getTaskCount(realParamsMap, planId);
			plan.setTaskCount(taskCount);
			plan.setTranslateCron(plan.getCron());
			Task task = taskService.getPlanTaskByPlanId(realParamsMap, planId, TaskTypeConstant.TASK_PARENT_PLAN);
			Domain domain = domainService.getPlanResultByPlanId(planId);
			
			int touchCount = taskService.getTouchCount(planId, "plan");
			plan.setTouchCount(touchCount);
			
			modelMap.put("plan", plan);
			modelMap.put("task", task);
			modelMap.put("domain", domain);
		}
		
		return "report/plan/plan_report_view";
	}
	/**
	 * 投递历史
	 */
	@RequestMapping(value="/deliveryHistory", method=RequestMethod.GET)
	public String deliveryHistory(HttpServletRequest request, ModelMap modelMap, 
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		Integer planId = XssFilter.filterXssInt(request.getParameter("planId"));
		if(planId != null) {
			Map<String,Object> paramsMap = Maps.newHashMap();
			
			Task task = taskService.getPlanTaskByPlanId(paramsMap, planId, TaskTypeConstant.TASK_PARENT_PLAN);
			Pagination<Domain> pagination = domainService.getResultList(paramsMap, new Integer[]{ planId }, "", currentPage, pageSize);
			
			int taskCount = taskService.getTaskCount(paramsMap, planId);
			
			Map<String,Object> map = Maps.newHashMap();
			Plan plan = planService.getPlanById(map, planId);
			int touchCount = taskService.getTouchCount(planId, "plan");
			plan.setTouchCount(touchCount);
			
			modelMap.put("taskCount", taskCount);
			modelMap.put("task", task);
			modelMap.put("plan", plan);
			modelMap.put("pagination", pagination);
		}
		
		return "report/plan/plan_delivery_history";
	}
	/***
	 * 导出总览信息
	 * @param request
	 * @param response
	 */
	@RequestMapping("/exportReportView")
	public void exportReportView(HttpServletRequest request,HttpServletResponse response) {
		String planIdTemp = XssFilter.filterXss(request.getParameter("planId"));
		if(planIdTemp != null && !"".equals(planIdTemp)) {
			Integer planId = Integer.parseInt(planIdTemp);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("planId", planId);
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				out = IOUitl.getOut(response,EncodeUtils.urlEncode("周期任务投递统计_概览.csv"));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writePlanCsv(csvWriter, paramsMap);
				csvWriter.flush();
			}catch(Exception e) {
				e.printStackTrace();
			}finally {
				try {
					if(out != null) {
						out.close();
					}
					if(csvWriter != null) {
						csvWriter.close();
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	/***
	 * 导出发送信息
	 */
	@RequestMapping(value="/exportDeliveryHistory", method=RequestMethod.GET)
	public void exportDeliveryHistory(HttpServletRequest request,HttpServletResponse response) {
		String planIdTemp = XssFilter.filterXss(request.getParameter("planId"));
		if(planIdTemp != null && !"".equals(planIdTemp)) {
			Integer planId = Integer.parseInt(planIdTemp);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("planId", planId);
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				out = IOUitl.getOut(response,EncodeUtils.urlEncode("周期任务投递统计_历史.csv"));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writePlanCsv(csvWriter, paramsMap);
				csvWriter.flush();
			}catch(Exception e) {
				e.printStackTrace();
			}finally {
				try {
					if(out != null) {
						out.close();
					}
					if(csvWriter != null) {
						csvWriter.close();
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	/**
	 * 周期任务：触发计划
	 * @param request
	 * @param modelMap
	 * @return
	 */
	@RequestMapping(value="/planReportTouch", method=RequestMethod.GET)
	public String planReportTouch(HttpServletRequest request, ModelMap modelMap) {
		
		Integer planId = XssFilter.filterXssInt(request.getParameter("planId"));
		Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));

		Map<String, Object> paramsMap = Maps.newHashMap();
        Plan plan = planService.getPlanById(paramsMap, planId);
        int touchCount = taskService.getTouchCount(planId, "plan");
        int taskCount = taskService.getTaskCount(paramsMap, planId);
		plan.setTouchCount(touchCount);
		plan.setTaskCount(taskCount);
		plan.setTranslateCron(plan.getCron());
        Task task = taskService.getPlanTaskByPlanId(paramsMap, planId, TaskTypeConstant.TASK_PARENT_PLAN);
        
        List<Task> taskList = taskService.getTouchTask(planId, "plan");
        String taskIdStr = "";
        for(Task t : taskList) {
        	taskIdStr += t.getTaskId() + ",";
        }
        Integer[] taskIds = Converts._toIntegers(taskIdStr);
        Map<String, Object> pMap = Maps.newHashMap();
    	List<TouchSetting> settingList = settingService.getSettings(pMap, templateId, taskIds);
    	
    	List<Touch> touchList = touchService.getByParentIds(taskIds);
    	String touchIdStr = ""; 
    	for(int i=0,len=touchList.size(); i<len; i++) {
    		Touch t = touchList.get(i);
    		touchIdStr += t.getTaskId() + ",";
    	}
    	Integer[] touchIds = Converts._toIntegers(touchIdStr);
    	List<Domain> domainList = null;
    	if(touchIds!=null && touchIds.length>0){
	    	paramsMap.clear();
	    	paramsMap.put("taskIds", touchIds);
	    	domainList = domainService.getTouchList(paramsMap);
	    	for(Domain d : domainList) {
				Domain domain = domainService.getPlanResultSum(paramsMap, new Integer[]{ planId });
				Integer parentReadUserCount = 0;
	        	if(domain != null){
	        		parentReadUserCount = domain.getReadUserCount();
	        	}
	        	d.setParentReadUserCount(parentReadUserCount);
			}
	    	ResultDataOption.copyUrlsToDomain(settingList, domainList);
    	}
    	
    	modelMap.put("task", task);
    	modelMap.put("plan", plan);
    	modelMap.put("settingList", settingList);
    	modelMap.put("domainList", domainList);
		
		return "report/plan/plan_report_touch";
	}
	/**
	 * 周期任务：触发计划
	 */
	@RequestMapping(value="/exportPlanTouch", method=RequestMethod.GET)
	public void exportPlanTouch(HttpServletRequest request,HttpServletResponse response) {
		Integer planId = XssFilter.filterXssInt(request.getParameter("planId"));
		if(planId != null) {
			Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));

			Map<String,Object> paramsMap = Maps.newHashMap();
	        Plan plan = planService.getPlanById(paramsMap, planId);
	        int touchCount = taskService.getTouchCount(planId, "plan");
			plan.setTouchCount(touchCount);
			int taskCount = taskService.getTaskCount(paramsMap, planId);
			plan.setTaskCount(taskCount);
	        Task task = taskService.getPlanTaskByPlanId(paramsMap, planId, TaskTypeConstant.TASK_PARENT_PLAN);
	        
	        List<Task> taskList = taskService.getTouchTask(planId, "plan");
	        String taskIdStr = "";
	        for(Task t : taskList) {
	        	taskIdStr += t.getTaskId() + ",";
	        }
	        Integer[] taskIds = Converts._toIntegers(taskIdStr);
	        Map<String, Object> pMap = Maps.newHashMap();
	    	List<TouchSetting> settingList = settingService.getSettings(pMap, templateId, taskIds);
	    	
	    	List<Touch> touchList = touchService.getByParentIds(taskIds);
	    	String touchIdStr = ""; 
	    	for(int i=0,len=touchList.size(); i<len; i++) {
	    		Touch t = touchList.get(i);
	    		touchIdStr += t.getTaskId() + ",";
	    	}
	    	Integer[] touchIds = Converts._toIntegers(touchIdStr);
	    	List<Domain> domainList = null;
	    	if(touchIds!=null && touchIds.length>0){
		    	paramsMap.clear();
		    	paramsMap.put("taskIds", touchIds);
	    		domainList = domainService.getTouchList(paramsMap);
	    		for(Domain d : domainList) {
	    			Domain domain = domainService.getPlanResultSum(paramsMap, new Integer[]{ planId });
	    			Integer parentReadUserCount = 0;
	            	if(domain != null){
	            		parentReadUserCount = domain.getReadUserCount();
	            	}
	            	d.setParentReadUserCount(parentReadUserCount);
	    		}
	    		ResultDataOption.copyUrlsToDomain(settingList, domainList);
	    	}
	    	
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				out = IOUitl.getOut(response,EncodeUtils.urlEncode("周期任务投递统计_触发.csv"));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writeCsvTouch(csvWriter, domainList, task, plan, null, "plan");
				csvWriter.flush();
			}catch(Exception e) {
				e.printStackTrace();
			}finally {
				try {
					if(out != null) {
						out.close();
					}
					if(csvWriter != null) {
						csvWriter.close();
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	
}
