package cn.edm.controller.report;


import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.Message;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Domain;
import cn.edm.domain.Hour;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Touch;
import cn.edm.domain.TouchSetting;
import cn.edm.domain.Url;
import cn.edm.service.DomainService;
import cn.edm.service.HourService;
import cn.edm.service.RegionService;
import cn.edm.service.ResultService;
import cn.edm.service.TaskService;
import cn.edm.service.TouchService;
import cn.edm.service.TouchSettingService;
import cn.edm.service.UrlService;
import cn.edm.util.ConvertUtils;
import cn.edm.util.DomainStatComparator;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Loggers;
import cn.edm.util.Params;
import cn.edm.util.Senders;
import cn.edm.util.UrlClickComparator;
import cn.edm.util.XssFilter;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;
import cn.edm.vo.DomainStat;
import cn.edm.vo.Log;
import cn.edm.vo.Send;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller
@RequestMapping("/taskDetailStat")
public class TaskDetailStatController {
	
	@Autowired
	private TaskService taskService;
	
	@Autowired
	private ResultService resultService;
	
	@Autowired
	private UrlService urlService;
	
	@Autowired
	private HourService hourService;
	
	@Autowired
	private DomainService domainService;
	
	@Autowired
	private RegionService regionService;
	
	@Autowired
	private TouchSettingService settingService;
	
	@Autowired
	private TouchService touchService;
	
	/**
	 * 概览
	 */
	@RequestMapping("/overview")
	public String overview(HttpServletRequest request, ModelMap modelMap) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			Map<String, Object> pMap = Maps.newHashMap();
			pMap.put("taskId", taskId);
			Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
			pMap.put("types", types);
			Domain domain = domainService.getSum(pMap);
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("task", task);
			modelMap.put("result", result);
			modelMap.put("domain", domain);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
		}
		
		return "report/taskOverview";
	}
	
	/**
	 * 发送详情
	 */
	@RequestMapping("/send")
	public String send(HttpServletRequest request, ModelMap modelMap) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("result", result);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			Domain domain = domainService.getSendBackSum(paramsMap);
			modelMap.put("domain", domain);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
		}
		
		return "report/taskSend";
	}
	
	/**
	 * 打开详情
	 */
	@RequestMapping("/open")
	public String open(HttpServletRequest request, ModelMap modelMap) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			if(task != null) {
				paramsMap.put("corpId", task.getCorpId());
			}
			List<Hour> hourList = hourService.getByTaskId(paramsMap);
			Map<String, Integer> hourMap = Maps.newLinkedHashMap();
			for(int i=0;i<24;i++) {
				hourMap.put(String.valueOf(i), 0);
				if(hourList != null && hourList.size()>0) {
					for(Hour h:hourList) {
						if(i==h.getHour()) {
							hourMap.put(String.valueOf(i), h.getReadCount());
							break;
						}
					}
				}else {
					continue;
				}
			}
			modelMap.put("task", task);
			modelMap.put("hourMap", hourMap);
			
			
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("result", result);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
		}
		return "report/taskOpen";
	}
	
	/**
	 * 打开详细列表
	 */
	@RequestMapping("/openLogList")
	public @ResponseBody Map<String,Object> openLogList(HttpServletRequest request) {
		Map<String,Object> resultMap = Maps.newHashMap();
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			//打开的详细列表
			String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
			String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
			List<Log> openList = Loggers.open(sid, time, "head", "utf-8");
			resultMap.put("openList", openList);
		}
		return resultMap;
	}
	
	
	/**
	 * 点击详情
	 */
	@RequestMapping("/click")
	public String click(HttpServletRequest request, ModelMap modelMap) {
		
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			if(task != null) {
				paramsMap.put("corpId", task.getCorpId());
			}
			
			List<Url> urlList = urlService.getByTaskId(paramsMap);
			UrlClickComparator ucc = new UrlClickComparator();
			Collections.sort(urlList, ucc);
			
			List<Url> fiveList = Lists.newArrayList();
			if(urlList!=null&&urlList.size()>5) {
				for(int i=0;i<5;i++) {
					fiveList.add(urlList.get(i));
				}
				modelMap.put("fiveList", fiveList);
			}else {
				modelMap.put("fiveList", urlList);
			}
			
			Url url = urlService.getClickCountByTaskId(paramsMap);
			Integer totalClick = 0;
			if(url != null) {
				totalClick = url.getSumClickCount();
			}
			modelMap.put("task", task);
			modelMap.put("urlList", urlList);
			modelMap.put("totalClick", totalClick);
			
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("result", result);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
		}
		
		return "report/taskClick";
	}
	
	/**
	 * 点击详细列表
	 */
	@RequestMapping("/clickLogList")
	public @ResponseBody Map<String,Object> clickLogList(HttpServletRequest request) {
		Map<String,Object> resultMap = Maps.newHashMap();
		String taskIdTemp = request.getParameter("taskId");
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			//点击的详细列表
			String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
			String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
			List<Log> openList = Loggers.click(sid, time, "head", "utf-8");
			resultMap.put("cliclList", openList);
		}
		return resultMap;
	}
	
	/**
	 * 域名发放详细
	 */
	@RequestMapping("/domain")
	public String domain(HttpServletRequest request,ModelMap modelMap) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			modelMap.put("task", task);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			List<Domain> domainList = domainService.getByTaskIdAndGroupByDomain(paramsMap);
			
			
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("result", result);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
			
			int totalSend = 0;
			if(domainList != null && domainList.size()>0) {
				//计算总发送量
				for(Domain d : domainList) {
					totalSend += d.getSentCount();
				}
				
				List<DomainStat> domainStatList = Lists.newArrayList();
				//计算各个域名发送量占总发送量的百分比
				for(Domain d : domainList) {
					DomainStat ds = new DomainStat();
					ds.setDomainName(d.getEmailDomain());
					ds.setShare((d.getSentCount()*1.0)/totalSend*100.0);
					domainStatList.add(ds);
				}
				
				modelMap.put("domainList", domainList);
				//modelMap.put("domainStatList", domainStatList);
				
				DomainStatComparator dsc = new DomainStatComparator();
				Collections.sort(domainStatList, dsc);
				
				if(domainStatList.size()>7) {
					List<DomainStat> domainStatListTemp = Lists.newArrayList();
					double sum = 0.00;
					for(int i=0;i<7;i++) {
						DomainStat ds = domainStatList.get(i);
						if(!"other".equals(ds.getDomainName())) {
							sum += ds.getShare();
							domainStatListTemp.add(ds);
						}
					}
					double otherShare = 100.00-sum;
					DomainStat other = new DomainStat();
					other.setDomainName("other");
					other.setShare(otherShare);
					domainStatListTemp.add(other);
					modelMap.put("domainStatList", domainStatListTemp);
				}else {
					modelMap.put("domainStatList", domainStatList);
				}
				
				
			}
		}
		
		
		return "report/taskDomain";
	}
	
	/**
	 * 地域发送详细
	 */
	@RequestMapping("/region")
	public String region(HttpServletRequest request,ModelMap modelMap) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			
			List<Region> provinceRegionList = regionService.getByProvince(paramsMap);
			List<Region> cityRegionList = regionService.getByCity(paramsMap);
			
			modelMap.put("task", task);
			modelMap.put("provinceRegionList", provinceRegionList);
			modelMap.put("cityRegionList", cityRegionList);
			
			Result result = resultService.getByTaskId(taskId);
			modelMap.put("result", result);
			
			List<Touch> touchs = touchService.getByParentId(taskId);
			modelMap.put("touchs", touchs);
		}
		return "report/taskRegion";
	}
	
	/**
	 * 对比
	 */
	@RequestMapping("/compare")
	public String compare(HttpServletRequest request,ModelMap modelMap) {
		
		String checkedIds = request.getParameter("checkedIds");
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		List<Result> resultList = resultService.getAll(paramsMap);
		
		modelMap.put("resultList", resultList);
		return "report/taskCompare";
	}
	
	/**
	 * 对比
	 */
	@RequestMapping(value="/compareTask", method=RequestMethod.GET)
	public String compareTask(HttpServletRequest request, ModelMap modelMap) {
		try {
			String[] checkedIds = request.getParameterValues("checkedIds");
			String type = XssFilter.filterXss(request.getParameter("type"));
			String userId = XssFilter.filterXss(request.getParameter("userId"));
			String corpId = XssFilter.filterXss(request.getParameter("corpId"));
			String fromFlag = XssFilter.filterXss(request.getParameter("fromFlag"));
			modelMap.put("type", type);
			modelMap.put("userId", userId);
			modelMap.put("corpId", corpId);
			modelMap.put("fromFlag", fromFlag);
			if(checkedIds==null||checkedIds.length==0) {
				modelMap.put(Message.MSG, "您没有选择任务！");
			}else {
				Integer[] taskIds = null;
				if(checkedIds != null&&!"".equals(checkedIds)) {
					taskIds = ConvertUtils.strArray2Int(checkedIds);
				}
				Map<String,Object> paramsMap = new HashMap<String,Object>();
				paramsMap.put("taskIds", taskIds);
//				List<Result> resultList = resultService.getAll(paramsMap);
				List<Domain> resultList = domainService.getDomainSumByTask(taskIds);
				modelMap.put("resultList", resultList);
			}
		}catch(Exception e) {
			e.printStackTrace();
		}
		
		return "report/taskCompare";
	}
	/**
	 * 对比
	 */
	@RequestMapping("/compareApi")
	public String compareApi(HttpServletRequest request,ModelMap modelMap) {
		
		String checkedIds = request.getParameter("checkedIds");
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		List<Result> resultList = resultService.getAllApi(paramsMap);
		
		modelMap.put("resultList", resultList);
		return "report/taskCompare";
	}
	
	
	/***
	 * 导出总览信息
	 * @param request
	 * @param response
	 */
	@RequestMapping("/exportOverview")
	public void exportOverview(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
//			Result result = resultService.getByTaskId(taskId);
			Map<String, Object> pMap = Maps.newHashMap();
			pMap.put("taskId", taskId);
			Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
			pMap.put("types", types);
			Domain domain = domainService.getSum(pMap);
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计-概览.csv";
				if("4".equals(taskType)){
					title = "数据报告-API发送统计-群发任务-概览.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
//				resultService.writeCsvOverview(csvWriter, result);
				domainService.writeCsvOverview(csvWriter, domain);
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
	@RequestMapping("/exportSend")
	public void exportSend(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Result result = resultService.getByTaskId(taskId);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			Domain domain = domainService.getSendBackSum(paramsMap);
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计_发送.csv";
				if("4".equals(taskType)){
					title = "数据报告_API发送统计_群发任务_发送.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				resultService.writeCsvSend(csvWriter, result, domain);
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
	 * 导出打开信息
	 */
	@RequestMapping("/exportOpen")
	public void exportOpen(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
//			Result result = resultService.getByTaskId(taskId);
			Domain domain = domainService.getByTaskId(taskId);
			Task task = taskService.getByTaskId(taskId);
			//打开的详细列表
			String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
			String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
			List<Log> openList = Loggers.open(sid, time, "head", "utf-8");
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计_打开.csv";
				if("4".equals(taskType)){
					title = "数据报告_API发送统计_群发任务_打开.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writeCsvOpen(csvWriter, domain, openList);
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
	 * 导出点击信息
	 */
	@RequestMapping("/exportClick")
	public void exportClick(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
//			Result result = resultService.getByTaskId(taskId);
			Domain domain = domainService.getByTaskId(taskId);
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			Task task = taskService.getByTaskId(taskId);
			if(task != null) {
				paramsMap.put("corpId", task.getCorpId());
			}
			List<Url> urlList = urlService.getByTaskId(paramsMap);
			UrlClickComparator ucc = new UrlClickComparator();
			Collections.sort(urlList, ucc);
			Url url = urlService.getClickCountByTaskId(paramsMap);
			Integer totalClick = 0;
			if(url != null) {
				totalClick = url.getSumClickCount();
			}
			
			
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计_点击.csv";
				if("4".equals(taskType)){
					title = "数据报告_API发送统计_群发任务_点击.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writeCsvClick(csvWriter, domain, urlList, totalClick);
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
	 * 导出域名信息
	 */
	@RequestMapping("/exportDomain")
	public void exportDomain(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Result result = resultService.getByTaskId(taskId);
			
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			List<Domain> domainList = domainService.getByTaskIdAndGroupByDomain(paramsMap);
			
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计_域名.csv";
				if("4".equals(taskType)){
					title = "数据报告_API发送统计_群发任务_域名.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				resultService.writeCsvDomain(csvWriter,result,domainList);
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
	 * 导出地域信息
	 */
	@RequestMapping("/exportRegion")
	public void exportRegion(HttpServletRequest request,HttpServletResponse response) {
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		if(taskIdTemp != null && !"".equals(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Result result = resultService.getByTaskId(taskId);
			
			Map<String,Object> paramsMap = Maps.newHashMap();
			paramsMap.put("taskId", taskId);
			
			List<Region> provinceRegionList = regionService.getByProvince(paramsMap);
			List<Region> cityRegionList = regionService.getByCity(paramsMap);
			
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_任务发送统计_地域.csv";
				if("4".equals(taskType)){
					title = "数据报告_API发送统计_群发任务_地域.csv";
				}
				out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				resultService.writeCsvRegion(csvWriter, result, provinceRegionList, cityRegionList);
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
	 * 触发详细
	 */
	@RequestMapping(value="/touch", method=RequestMethod.GET)
	public String touch(HttpServletRequest request, ModelMap modelMap) {
		
		Integer taskId = XssFilter.filterXssInt(request.getParameter("taskId"));
		Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType"));
		modelMap.put("taskType", taskType);
		
		Task task = taskService.getByTaskId(taskId);
		
//		List<TemplateSetting> tSettings = settingService.getByParentId(templateId);
		Map<String, Object> pMap = Maps.newHashMap();
		List<TouchSetting> tSettings = settingService.getSettings(pMap, templateId, taskId);
		
		List<Touch> touchs = touchService.getByParentId(taskId);
    	String taskIdStr = ""; 
    	for(int i=0,len=touchs.size(); i<len; i++) {
    		Touch t = touchs.get(i);
    		taskIdStr += t.getTaskId() + ",";
    	}
    	Integer[] taskIds = Converts._toIntegers(taskIdStr);
    	List<Domain> domainList = null;
    	if(taskIds!=null && taskIds.length>0){
    		Map<String, Object> paramsMap = Maps.newHashMap();
    		paramsMap.put("taskIds", taskIds);
    		domainList = domainService.getTouchList(paramsMap);
    		for(Domain d : domainList) {
    			Map<String, Object> dMap = Maps.newHashMap();
    			dMap.put("taskId", d.getTaskId());
    			Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
    			dMap.put("types", types);
    			Domain domain = domainService.getSum(dMap);
    			Integer parentReadUserCount = 0;
            	if(domain != null){
            		parentReadUserCount = domain.getReadUserCount();
            	}
            	d.setParentReadUserCount(parentReadUserCount);
    		}
    		ResultDataOption.copyUrlsToDomain(tSettings, domainList);
    	}
		
		Result result = resultService.getByTaskId(taskId);
		
		modelMap.put("task", task);
		modelMap.put("result", result);
		modelMap.put("domainList", domainList);
		modelMap.put("tSettings", tSettings);
		modelMap.put("touchs", touchs);
		return "report/task_touch";
	}
	/**
	 * 导出触发报告
	 * @param request
	 * @param response
	 */
	@RequestMapping(value="/exportTouch", method=RequestMethod.GET)
	public void exportTouch(HttpServletRequest request, HttpServletResponse response) {
		Integer taskId = XssFilter.filterXssInt(request.getParameter("taskId"));
		Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));
		
		Task task = taskService.getByTaskId(taskId);
		
		Map<String, Object> pMap = Maps.newHashMap();
		List<TouchSetting> tSettings = settingService.getSettings(pMap, templateId, taskId);
		
		List<Touch> touchs = touchService.getByParentId(taskId);
    	String taskIdStr = ""; 
    	for(int i=0,len=touchs.size(); i<len; i++) {
    		Touch t = touchs.get(i);
    		taskIdStr += t.getTaskId() + ",";
    	}
    	Integer[] taskIds = Converts._toIntegers(taskIdStr);
    	List<Domain> domainList = null;
    	if(taskIds!=null && taskIds.length>0){
    		Map<String, Object> paramsMap = Maps.newHashMap();
    		paramsMap.put("taskIds", taskIds);
    		domainList = domainService.getTouchList(paramsMap);
    		for(Domain d : domainList) {
    			Map<String, Object> dMap = Maps.newHashMap();
    			dMap.put("taskId", d.getTaskId());
    			Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
    			dMap.put("types", types);
    			Domain domain = domainService.getSum(dMap);
    			Integer parentReadUserCount = 0;
            	if(domain != null){
            		parentReadUserCount = domain.getReadUserCount();
            	}
            	d.setParentReadUserCount(parentReadUserCount);
    		}
    		ResultDataOption.copyUrlsToDomain(tSettings, domainList);
    	}
		
		Result result = resultService.getByTaskId(taskId);
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			String title = "数据报告_任务发送统计_触发.csv";
			out = IOUitl.getOut(response, EncodeUtils.urlEncode(title));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			
			domainService.writeCsvTouch(csvWriter, domainList, task, null, result, "task");
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
	/**
	 * 导出触发报告
	 * @param request
	 * @param response
	 */
	@RequestMapping(value="/exportRecipients", method=RequestMethod.GET)
	public void exportRecipients(HttpServletRequest request, HttpServletResponse response) {
		Integer taskId = XssFilter.filterXssInt(request.getParameter("taskId"));
		Integer templateId = XssFilter.filterXssInt(request.getParameter("templateId"));
		if(taskId != null) {
			Result result = resultService.getTouchResult(taskId, templateId);
			//打开的详细列表
			String sid = Params.sid(result.getCorpId(), result.getTouchId(), result.getTemplateId());
			String time = new DateTime(result.getDeliveryTime()).toString("yyyyMMdd");
			List<Send> sendList = Senders.send(sid, "success", "utf-8");
			List<Log> openList = Loggers.open(sid, time, "detail", "utf-8");
			List<Log> clickList = Loggers.click(sid, time, "detail", "utf-8");
			ServletOutputStream out = null;
			CSVWriter csvWriter = null;
			try {
				String title = "数据报告_触发_收件人详情.csv";
				out = IOUitl.getOut(response, EncodeUtils.urlEncode(title));
				csvWriter = new CSVWriter(new OutputStreamWriter(out));
				domainService.writeCsvRecipients(csvWriter, sendList, openList, clickList);
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
