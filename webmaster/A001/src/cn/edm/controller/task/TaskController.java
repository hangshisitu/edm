package cn.edm.controller.task;

import java.io.File;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import cn.edm.constant.Message;
import cn.edm.constant.PathCons;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.Status;
import cn.edm.constants.Config;
import cn.edm.constants.Value;
import cn.edm.domain.Cnd;
import cn.edm.domain.Resource;
import cn.edm.domain.Setting;
import cn.edm.domain.Task;
import cn.edm.domain.TaskSetting;
import cn.edm.domain.Template;
import cn.edm.domain.TemplateSetting;
import cn.edm.domain.Touch;
import cn.edm.domain.TouchSetting;
import cn.edm.domain.Trigger;
import cn.edm.modules.utils.Encodes;
import cn.edm.service.CndService;
import cn.edm.service.ResourceService;
import cn.edm.service.SettingService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.service.TemplateSettingService;
import cn.edm.service.TouchService;
import cn.edm.service.TouchSettingService;
import cn.edm.service.TriggerService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.FileManagerUtils;
import cn.edm.util.Pagination;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.XssFilter;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Clean;
import cn.edm.utils.web.XSS;
import cn.edm.vo.TaskDetail;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

/**
 * 任务管理
 * @author Luxh
 *
 */
@Controller
@RequestMapping("/task")
public class TaskController {
	
	private static Logger log = LoggerFactory.getLogger(TaskController.class);
	
	@Autowired
	private TaskService taskService;
	
	@Autowired
	private ResourceService resourceService;
	
	@Autowired
	private TemplateService templateService;
	
	@Autowired
	private TriggerService triggerService;
	
	@Autowired
	private CndService cndService;
	
	@Autowired
	private SettingService settingService;
	
	@Autowired
	private TouchService touchService;
	
	@Autowired
	private TemplateSettingService templateSettingService;
	
	@Autowired
	private TouchSettingService touchSettingService;
	
	/**
	 * 待审核任务列表
	 */
	@RequestMapping(value="/unauditList", method=RequestMethod.GET)
	public String unauditList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String taskName =StreamToString.getStringByUTF8(request,"taskName");
			String corpPath =StreamToString.getStringByUTF8(request,"corpPath");
			String userId =StreamToString.getStringByUTF8(request,"userId");		
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("taskName",taskName);
			initParamsMap.put("corpPath",corpPath);
			initParamsMap.put("userId",userId);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);

			//待审核
			int[] status = {Status.REVIEWING};
			realParamsMap.put("status", status);
			
			Pagination<Task> pagination = taskService.getPagination(realParamsMap, "", currentPage, pageSize);
			modelMap.put("pagination", pagination);
			
			List<Resource> resourceList = resourceService.getAll();
			modelMap.put("resourceList", resourceList);
			
			
			//根据任务中的模板id取得模板名称
			if(pagination!=null){
				List<TaskDetail> details = getDetails(pagination);
				modelMap.put("details", details);
			}
		}catch(Exception e) {
			log.error(e.getMessage(),e);
		}
		
		return "task/unauditList";
	}
	
	/**
	 * 查看任务模板
	 */
	@RequestMapping("/view")
	public String view(ModelMap modelMap, HttpServletRequest request) {
		Integer taskId = XssFilter.filterXssInt(request.getParameter("taskId"));
		Integer parentTemplateId = XssFilter.filterXssInt(request.getParameter("parentTemplateId"));
		modelMap.put("taskId", taskId);
		modelMap.put("templateId", parentTemplateId);
		String action = XssFilter.filterXss(request.getParameter("action"));
		String content = "";
		if("touch".equals(action)) {
			Integer corpId = XssFilter.filterXssInt(request.getParameter("corpId"));
			modelMap.put("corpId", corpId);
			Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));
			Template template = null;
			if(templateId != null){
				template = templateService.getByTemplateId(templateId);
				if(template == null){
					template = new Template();
				}else{
					String path = PathCons.ROOT_PATH + template.getFilePath();
					if(!Files.exists(path)){
						modelMap.put(Message.MSG, "模板不存在!");
					}
					content = FileManagerUtils.getContent(PathCons.ROOT_PATH, template.getFilePath());
					template.setContent(content);
				}
			}else{
				template = new Template();
				modelMap.put(Message.MSG, "模板不存在!");
				
			}
			Integer touchId = XssFilter.filterXssInt(request.getParameter("touchId"));
			if(touchId == null){
				// 触发计划子模板
				List<TouchSetting> tSettingList = Lists.newArrayList();
//				List<Touch> touchList = touchService.getByParentId(taskId);
				List<TemplateSetting> tempSetting = templateSettingService.getSubByParentId(parentTemplateId);
				if(!Asserts.empty(tempSetting)){
					for(TemplateSetting t : tempSetting) {
						TouchSetting ts = new TouchSetting();
						ts.setTaskId(t.getTaskId());
						ts.setCorpId(t.getCorpId());
						ts.setTemplateId(t.getTemplateId());
						ts.setTemplateName(t.getTemplateName());
						ts.setSubject(t.getSubject());
						tSettingList.add(ts);
					}
					modelMap.put("action", "touch");
				}
				Task task = taskService.getByTaskId(taskId);
				modelMap.put("task", task);
				modelMap.put("templateId", parentTemplateId);
				Template temp1 = templateService.getByTemplateId(parentTemplateId);
				modelMap.put("template", template);
				modelMap.put("templateName", temp1.getTemplateName());
				modelMap.put("tSettingList", tSettingList);
//				modelMap.put(Message.MSG, "触发任务不存在!");
				return "task/templateDetail";
			}
			Touch touch = touchService.getTouch(corpId, touchId, templateId);
			if(touch == null){
				modelMap.put(Message.MSG, "触发任务不存在!");
			}
			String sid = corpId + "." + touchId + "." + templateId;
			
			String absPath = PathCons.ROOT_PATH + PathCons.TOUCH;
//			String dirPath = PropertiesUtil.get(ConfigKeys.WEBADMIN_PATH) + "/WEB-INF/static/touch/" + sid + "/"; // 模板名称，由时间最早的来查找
			String dirPath = absPath + sid + "/"; // 模板名称，由时间最早的来查找
			File f = Files.getNewestFile(dirPath);
			Map<String, Object> paramsMap = Maps.newHashMap();
			TouchSetting touchSetting = touchSettingService.getSetting(paramsMap, touch.getTemplateId(), touch.getTaskId());
			
			if(f != null){
				String filePath = dirPath + f.getName();
				if (Files.exists(filePath)) {
					if(touchSetting == null) {
						touchSetting = new TouchSetting();
					}
					String senderName = FileManagerUtils.node(filePath, "NICKNAME");
					String senderEmail = FileManagerUtils.node(filePath, "SENDER");
					String robot = FileManagerUtils.node(filePath, "ROBOT");
					String rand = FileManagerUtils.node(filePath, "RAND");
					String md = FileManagerUtils.node(filePath, "MODULA");
					
					Task task = taskService.getByTaskId(touch.getParentId());
					Resource resource = resourceService.getById(task.getResourceId());
					String modula =  this.filter(resource, md);
					
					content = FileManagerUtils.parseContent(modula);
					modelMap.put("senderName", senderName);
					modelMap.put("senderEmail", senderEmail);
					modelMap.put("robot", robot);
					modelMap.put("rand", rand);
					
				}else{
					modelMap.put(Message.MSG, "模板文件不存在!");
				}
			}else{
				String path = PathCons.ROOT_PATH + template.getFilePath();
				if(!Files.exists(path)){
					modelMap.put(Message.MSG, "模板不存在!");
				}
				content = FileManagerUtils.getContent(PathCons.ROOT_PATH, template.getFilePath());
				if(taskId != null){
					TaskSetting set = templateService.getByTaskId(taskId);
					if(set != null) {
						modelMap.put("rand", set.getRand());
						String robot = "";
						if(null==set.getRobot() || "".equals(set.getRobot().trim()) || "-1".equals(set.getRobot())) {
							robot = "随机";
						}else {
							robot = set.getRobot();
						}
						modelMap.put("robot", robot);
					}
				}
			}
			String templateName = null;
			if(touchSetting != null){
				templateName = touchSetting.getParentName();
				template.setSubject(touchSetting.getSubject());
				template.setTemplateId(touchSetting.getTemplateId());
			}
			template.setContent(content);
			modelMap.put("template", template);
			// 触发计划子模板
			List<TouchSetting> tSettingList = Lists.newArrayList();
			List<Touch> touchList = null;
			if(taskId != null){
				touchList = touchService.getByParentId(taskId);
			}
			if(!Asserts.empty(touchList)){
				for(Touch t : touchList) {
					TouchSetting ts = new TouchSetting();
					ts.setTaskId(t.getTaskId());
					ts.setCorpId(t.getCorpId());
					ts.setTemplateId(t.getTemplateId());
					ts.setTemplateName(t.getTemplateName());
					ts.setSubject(t.getSubject());
					tSettingList.add(ts);
				}
				modelMap.put("action", "touch");
			}
			modelMap.put("templateName", templateName);
			modelMap.put("tSettingList", tSettingList);
		}else{
			if(taskId != null) {
				Task task = taskService.getByTaskId(taskId);
				if(task != null) {
					Template template = templateService.getByTemplateId(task.getTemplateId());
					if(template == null) {
						boolean isDelivery = task.getDeliveryTime() != null;
						String time = isDelivery ? new DateTime(task.getDeliveryTime()).toString("yyyyMM") : Value.S;
						String sid = task.getCorpId() + "." + taskId + "." + task.getTemplateId();
//						String path = PropertiesUtil.get(ConfigKeys.WEBADMIN_PATH) + "/WEB-INF/static/task/" + time + "/" + sid + ".txt";
						String path = PathCons.ROOT_PATH + PathCons.TASK + time + "/" + sid + ".txt";
						if(isDelivery && Files.exists(path)){
							String subject = FileManagerUtils.node(path, "SUBJECT");
							String md = FileManagerUtils.node(path, "MODULA");
							
							Resource resource = resourceService.getById(task.getResourceId());
							String modula =  this.filter(resource, md);
							
							String ct = FileManagerUtils.parseContent(modula);
							template = new Template();
							template.setSubject(subject);
							template.setContent(ct);
							modelMap.put("template", template);
						}else{
							template = new Template();
							modelMap.put(Message.MSG, "模板不存在!");
						}
					}else {
						template.setContent(FileManagerUtils.getContent(PathCons.ROOT_PATH, template.getFilePath()));
					}
					modelMap.put("template", template);
					modelMap.put("task", task);
					
					TaskSetting set = templateService.getByTaskId(task.getTaskId());
					if(set != null) {
						modelMap.put("templateName", set.getTemplate());
						modelMap.put("rand", set.getRand());
						String robot = "";
						if(null==set.getRobot() || "".equals(set.getRobot().trim()) || "-1".equals(set.getRobot())) {
							robot = "随机";
						}else {
							robot = set.getRobot();
						}
						modelMap.put("robot", robot);
					}
					
					// 触发计划子模板
					List<TouchSetting> tSettingList = Lists.newArrayList();
					List<Touch> touchList = touchService.getByParentId(taskId);					
					if(!Asserts.empty(touchList)){
						for(Touch t : touchList) {
							TouchSetting ts = new TouchSetting();
							ts.setTaskId(t.getTaskId());
							ts.setCorpId(t.getCorpId());
							ts.setTemplateId(t.getTemplateId());
							ts.setTemplateName(t.getTemplateName());
							ts.setSubject(t.getSubject());
							tSettingList.add(ts);
						}
						modelMap.put("action", "touch");
					}else{
						List<TemplateSetting> tempSetting = templateSettingService.getSubByParentId(task.getTemplateId());
						for(TemplateSetting t : tempSetting) {
							TouchSetting ts = new TouchSetting();
							ts.setTaskId(t.getTaskId());
							ts.setCorpId(t.getCorpId());
							ts.setTemplateId(t.getTemplateId());
							ts.setTemplateName(t.getTemplateName());
							ts.setSubject(t.getSubject());
							tSettingList.add(ts);
						}
						modelMap.put("action", "touch");
						
					}
					modelMap.put("templateId", task.getTemplateId());
					modelMap.put("tSettingList", tSettingList);
				}
			} else {
				modelMap.put(Message.MSG, "任务不存在!");
			}
			
		}
		return "task/templateDetail";
	}
	
	/**
	 * 查看任务模板
	 */
	@RequestMapping("/viewApi")
	public String viewApi(ModelMap modelMap, HttpServletRequest request) {
		String tkId = XssFilter.filterXss(request.getParameter("taskId")); 
		String cpId = XssFilter.filterXss(request.getParameter("corpId")); 
		String tpId = XssFilter.filterXss(request.getParameter("templateId")); 
		if(StringUtils.isNotBlank(tkId) && StringUtils.isNotBlank(cpId) && StringUtils.isNotBlank(tpId)) {
			Integer taskId = Integer.valueOf(tkId);
			Integer corpId = Integer.valueOf(cpId);
			Integer templateId = Integer.valueOf(tpId);
			Trigger trigger = triggerService.getByParams(taskId, corpId, templateId);
			if(trigger != null) {
				Template template = templateService.getByTemplateId(trigger.getTemplateId());
				if(template == null) {
					modelMap.put(Message.MSG, "模板不存在!");
				}else {
					template.setContent(FileManagerUtils.getContent(PathCons.ROOT_PATH, template.getFilePath()));
				}
				modelMap.put("template", template);
				modelMap.put("trigger", trigger);
				//String rand = StringUtils.substringBefore(StringUtils.substringAfter(task.getSendSettings(), "rand="), ",robot=");
				//String robot = StringUtils.substringBefore(StringUtils.substringAfter(task.getSendSettings(), "robot=["), "]");
				//modelMap.put("rand", rand);
				//modelMap.put("robot", robot);
			}
		}else {
			modelMap.put(Message.MSG, "任务不存在!");
		}
		return "task/templateApiDetail";
	}
	
	/**
	 * 审核页面
	 */
	@RequestMapping(value="/auditPage", method=RequestMethod.GET)
	public String auditPage(ModelMap modelMap,HttpServletRequest request) {
		CSRF.generate(request);
		Integer taskId = Integer.parseInt(XssFilter.filterXss(request.getParameter("taskId")));
		String flag = XssFilter.filterXss(request.getParameter("flag"));
		
		Task task = taskService.getByTaskId(taskId);
		modelMap.put("taskId", taskId);
		modelMap.put("flag", flag);
		modelMap.put("task", task);
		return "task/auditPage";
	}
	
	/**
	 * 审核
	 */
	/*@RequestMapping("/audit")
	public String audit(HttpServletRequest request,RedirectAttributes redirectAttributes) {
		try {
			Integer taskId = Integer.parseInt(request.getParameter("taskId"));
			String flag = request.getParameter("flag");
			System.out.println("taskId----"+taskId);
			System.out.println("flag----"+flag);
			String suggestion = request.getParameter("suggestion");
			String message = taskService.audit(taskId, flag, suggestion);
			redirectAttributes.addFlashAttribute(Message.MSG, message);
		}catch(Exception e) {
			redirectAttributes.addFlashAttribute(Message.MSG, "审核出错！");
			log.error(e.getMessage(), e);
		}
		return "redirect:unauditList";
	}*/
	
	/**
	 * 审核
	 */
	@RequestMapping("/audit")
	public @ResponseBody Map<String,Object> audit(HttpServletRequest request) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		try {
			Integer taskId = Integer.parseInt(request.getParameter("taskId"));
			String flag = request.getParameter("flag");
			String suggestion = request.getParameter("suggestion");
			boolean isClean = Clean.clean(suggestion.toString()); // true 不包含非法字符
			if(isClean){
				if(suggestion.length() > 60){
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "不能超过60个字符");
				}else{
					String message = taskService.audit(taskId, flag, suggestion);
					resultMap.put(Message.RESULT, Message.SUCCESS);
					resultMap.put(Message.MSG, message);
				}
			}else{
				String message = XSS.clean(suggestion);
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "审核意见包含非法字符： " + message);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "审核出错！");
		}
		return resultMap;
	}
	/**
	 * 已审核任务列表
	 */
	@RequestMapping(value="/auditedList", method=RequestMethod.GET)
	public String auditedList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String taskName =StreamToString.getStringByUTF8(request,"taskName");
			String corpPath =StreamToString.getStringByUTF8(request,"corpPath");
			String userId =StreamToString.getStringByUTF8(request,"userId");		
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("taskName",taskName);
			initParamsMap.put("corpPath",corpPath);
			initParamsMap.put("userId",userId);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			
			//待审核
			realParamsMap.put("auditFlag", "audited");
			
			Pagination<Task> pagination = taskService.getPagination(realParamsMap, "", currentPage, pageSize);
			modelMap.put("pagination", pagination);
			
			List<Resource> resourceList = resourceService.getAll();
			modelMap.put("resourceList", resourceList);
			
			//根据任务中的模板id取得模板名称
			if(pagination!=null){
				List<TaskDetail> details = getDetails(pagination);
				modelMap.put("details", details);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		
		
		return "task/auditedList";
	}
	/**
	 * 不需要审核任务列表
	 */
	@RequestMapping(value="/needlessAuditList", method=RequestMethod.GET)
	public String needlessAuditList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		try {
			String taskName =StreamToString.getStringByUTF8(request,"taskName");
			String corpPath =StreamToString.getStringByUTF8(request,"corpPath");
			String userId =StreamToString.getStringByUTF8(request,"userId");		
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("taskName",taskName);
			initParamsMap.put("corpPath",corpPath);
			initParamsMap.put("userId",userId);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			
			//待审核
			realParamsMap.put("auditFlag", "needless");
			
			Pagination<Task> pagination = taskService.getPagination(realParamsMap, "", currentPage, pageSize);
			modelMap.put("pagination", pagination);
			
			List<Resource> resourceList = resourceService.getAll();
			modelMap.put("resourceList", resourceList);
			
			//根据任务中的模板id取得模板名称
			if(pagination!=null){
				List<TaskDetail> details = getDetails(pagination);
				modelMap.put("details", details);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		
		
		return "task/needlessAuditList";
	}
	
	private List<TaskDetail> getDetails(Pagination<Task> pagination) {
		List<Task> tasks = pagination.getRecordList();
		Integer[] templateIds = new Integer[tasks.size()];
		Integer[] taskIds = new Integer[tasks.size()];
		for(int i=0;i<tasks.size();i++) {
			templateIds[i] = tasks.get(i).getTemplateId();
			taskIds[i] = tasks.get(i).getTaskId();
		}
		List<TaskDetail> details = Lists.newArrayList();
		if(templateIds != null && templateIds.length>0) {
			Map<Integer, String> templates = templateService.getNameById(templateIds);
			Map<Integer, Cnd> cnds = cndService.getCndByTaskIds(taskIds);
			Map<Integer, Setting> settings = settingService.getSettingByTaskIds(taskIds);
			
			TaskDetail td = null;
			for(int i=0;i<tasks.size();i++) {
				Task t = tasks.get(i);
				td = new TaskDetail();
				td.setTaskId(t.getTaskId());
				td.setTaskName(t.getTaskName());
				td.setSubject(t.getSubject());
				td.setTemplateName(templates.get(t.getTemplateId()));
				td.setSenderEmail(t.getSenderEmail());
				td.setSenderName(t.getSenderName());
				
				if(cnds.get(t.getTaskId())!=null) {
					td.setTag(cnds.get(t.getTaskId()).getIncludes());
					td.setExculde(cnds.get(t.getTaskId()).getExcludes());
				}
				
				Setting taskSetting = settings.get(t.getTaskId());
				if(taskSetting != null) {
					td.setSetting(taskSetting.getUnsubscribe());
					String sendCode = taskSetting.getSendCode();
					if("current".equals(sendCode)) {
						td.setWay("实时发送");
					}else if("schedule".equals(sendCode)) {
						td.setWay("定时发送 "+DateUtil.getDateByForamt(t.getJobTime(), "yyyy-MM-dd HH:mm:ss"));
					}else if("plan".equals(sendCode)){
						td.setWay("周期发送");
					}else{
						td.setWay("无");
					}
					String robot = taskSetting.getRobot();
					if(robot==null || "".equals(robot) || "-1".equals(robot)) {
						robot = "随机";
					}
					String rand = taskSetting.getRand();
					if("true".equals(rand)) {
						robot+="（加入随机数）";
					}
					td.setRobot(robot);
					
					td.setReplier(taskSetting.getReplier());
					
					String ad = taskSetting.getAd();
					if("true".equals(ad)) {
						td.setSubject("（AD）"+td.getSubject());
					}
					
					String campaignName = taskSetting.getCampaign();
					if(campaignName==null || "".equals(campaignName.trim())) {
						td.setCampaignName("无");
					}else {
						td.setCampaignName(taskSetting.getCampaign());
					}
					
					String templateName = taskSetting.getTemplate();
					if(StringUtils.isNotBlank(templateName)) {
					    td.setTemplateName(templateName);
					}
				}
				
				td.setReceiverName(t.getReceiver());
				if(td.getCampaignName() == null) {
					td.setCampaignName("无");
				}
				details.add(td);
			}
		}
		
		return details;
	}
	/**
	 * 任务优先级列表
	 * @param request
	 * @param modelMap
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	@RequestMapping(value="/priority/taskPriorityList", method=RequestMethod.GET)
	public String taskPriorityList(HttpServletRequest request, ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String taskName =StreamToString.getStringByUTF8(request,"taskName");
			String corpPath =StreamToString.getStringByUTF8(request,"corpPath");
			String userId =StreamToString.getStringByUTF8(request,"userId");		
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("taskName",taskName);
			initParamsMap.put("corpPath",corpPath);
			initParamsMap.put("userId",userId);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
			
			Pagination<Task> pagination = taskService.getTaskPriorityPagination(realParamsMap, "", currentPage, pageSize);
			modelMap.put("pagination", pagination);
			
			List<Resource> resourceList = resourceService.getAll();
			modelMap.put("resourceList", resourceList);
			
			Map<Integer, String> priorityMap = new HashMap<Integer, String>();
			priorityMap.put(Status.COMMON, "一般");
			priorityMap.put(Status.PRIOR, "优先");
			
			modelMap.put("priorityMap", priorityMap);
			//根据任务中的模板id取得模板名称
			if(pagination!=null){
				List<TaskDetail> details = getDetails(pagination);
				modelMap.put("details", details);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		
		return "task/priority/task_priority_list";
	}
	@RequestMapping(value="/priority/switchPriority", method=RequestMethod.POST)
	public String switchPriority(HttpServletRequest request, RedirectAttributes redirectAttributes) {
		Integer taskId = XssFilter.filterXssInt(request.getParameter("taskId"));
		Integer priority = XssFilter.filterXssInt(request.getParameter("priority"));
		Task task = taskService.getByTaskId(taskId);
		if(task != null) {
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("taskId", taskId);
			paramMap.put("priority", priority);
			paramMap.put("modifyTime", new Date());
			taskService.updateTaskPriority(paramMap);
			redirectAttributes.addFlashAttribute(Message.MSG, "优先级转换成功");
		}else {
			redirectAttributes.addFlashAttribute(Message.MSG, "任务不存在！");
		}
		return "redirect:taskPriorityList";
	}
	
	private String filter(Resource resource, String modula) {
		Document doc = Jsoup.parse(modula);

		// open
		if(!StringUtils.isBlank(doc.body().html()) && !StringUtils.isBlank(doc.body().children().html())){
			doc.body().children().first().remove();
		}

		// click
		StringBuffer sbff = new StringBuffer();
		for (String skip : Converts.toStrings(resource.getSkipList())) {
			sbff.append("http://").append(skip).append("/test3.php").append(",");
		}
		String[] skips = Converts._toStrings(sbff.toString());

		// online
		String online = PropertiesUtil.get(Config.APP_URL) + "/online";

		Elements links = doc.select("[href]");
		Elements images = doc.select("[src]");

		for (Element link : links) {
			String href = link.attr("abs:href");
			if (Asserts.containsAny(href, skips)) {
				String u = null;
				for (String strs : StringUtils.split(StringUtils.substringAfter(href, "?"), "&")) {
					String k = StringUtils.substringBefore(strs, "=");
					String v = StringUtils.substringAfter(strs, "=");
					if (k.equals("u")) {
						u = new String(Encodes.decodeBase64(v));
						break;
					}
				}
				link.attr("href", u);
			}

			if (StringUtils.startsWith(href, online)) {
				link.attr("href", online);
			}
		}
		for (Element link : images) {
			link.removeAttr("href");
		}

		return doc.toString();
	}
	
}
