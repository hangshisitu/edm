package cn.edm.controller.report;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.util.WebUtils;

import com.google.common.collect.Maps;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.Config;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Domain;
import cn.edm.domain.Result;
import cn.edm.modules.utils.Props;
import cn.edm.service.ApiTriggerService;
import cn.edm.service.DomainService;
import cn.edm.service.ResultService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.ResultDataOption;
import cn.edm.utils.StreamToString;

@Controller
@RequestMapping("/taskReport")
public class TaskReportController {
	
	private static Logger log = LoggerFactory.getLogger(TaskReportController.class);
	
	@Autowired
	private DomainService domainService;
	
	@Autowired
	private ResultService resultService;
	
	@Autowired
	private ApiTriggerService apiTriggerService;
	
	/**
	 * 分页展现任务统计列表
	 */
	@RequestMapping(value="/list", method=RequestMethod.GET)
	public String list(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("corpPath",corpPath);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String type = XssFilter.filterXss(request.getParameter("type"));
		
		String taskId = (String) realParamsMap.get("comTaskId");
		
		if(taskId !=null ) {
			realParamsMap.put("taskId", Integer.valueOf(taskId));
		}
		
		realParamsMap.put("type", Integer.parseInt(type));
//		Pagination<Domain> pagination = domainService.getResultPagination(realParamsMap, "", currentPage, pageSize);
		Pagination<Result> pagination = resultService.getPagination(realParamsMap, "", currentPage, pageSize);
		pagination.setRecordList(resultService.getIsCanImportTask(pagination.getRecordList()));
		domainCopy(pagination.getRecordList());
		pagination.setRecordList(resultService.getIsCanImportTask(pagination.getRecordList()));
		modelMap.put("pagination", pagination);
		modelMap.put("type", type);
		String hrCatalog = Props.getStr(Config.HRCATALOG);
		if(hrCatalog.equals("true")){
			modelMap.put("hrCatalog","true");
		}else{
		    modelMap.put("hrCatalog","false");
		}
		if("0".equals(type)) {
			return "report/taskReportList";
		}else {
			return "report/taskTestReportList";
		}
	}
	
	/**
	 * 导出任务统计信息
	 */
	@RequestMapping("/exportTaskStat")
	public void exportTaskStat(HttpServletRequest request,HttpServletResponse response) {
		
		String checkedIds = XssFilter.filterXss(request.getParameter("checkedIds"));
		
		String corpId = XssFilter.filterXss(request.getParameter("corpId"));
		String type = XssFilter.filterXss(request.getParameter("type"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		if(type != null&&!"".equals(type)) {
			paramsMap.put("type", Integer.parseInt(type));
		}
		if(corpId != null&&!"".equals(corpId)) {
			paramsMap.put("corpId", Integer.parseInt(corpId));
		}
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			String title = "数据报告_任务发送统计.csv";
			if("4".equals(taskType)){
				title = "数据报告_API发送统计_群发任务.csv";
			}
			out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			resultService.writeCsv(csvWriter, paramsMap);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
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
	 * 导出任务统计信息
	 */
	@RequestMapping("/exportTaskStatNew")
	public void exportTaskStatNew(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("corpPath",corpPath);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		
		//String checkedIds = XssFilter.filterXss(request.getParameter("checkedIds"));
		String checkedIds[] = request.getParameterValues("checkedIds");
		String corpId = XssFilter.filterXss(request.getParameter("corpId"));
		String typeStr = XssFilter.filterXss(request.getParameter("typeStr"));
		String type = XssFilter.filterXss(request.getParameter("type"));
		String taskType = XssFilter.filterXss(request.getParameter("taskType")); // taskType = 4为API群发任务
		
		Integer[] taskIds = null;
		if(checkedIds != null&&checkedIds.length>0) {
			taskIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				taskIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		realParamsMap.put("taskIds", taskIds);
		Integer[] types = null;
		if(typeStr!=null && !"".equals(typeStr)) {
			types = ConvertUtils.strArray2Int(StringUtils.split(typeStr, ","));
			realParamsMap.put("types", types);
		}
		if(type!=null && !"".equals(type)) {
			realParamsMap.put("type", type);
		}
		if(corpId != null&&!"".equals(corpId)) {
			realParamsMap.put("corpId", Integer.parseInt(corpId));
		}
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			String title = "数据报告_任务发送统计.csv";
			if("4".equals(taskType)){
				title = "数据报告_API发送统计_群发任务.csv";
			}
			out = IOUitl.getOut(response,EncodeUtils.urlEncode(title));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
//			domainService.writeDomainCsv(csvWriter, realParamsMap);
			resultService.writeCsv(csvWriter, realParamsMap);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
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
	 * 打印统计信息
	 */
	@RequestMapping("/printTaskStat")
	public String printTaskStat(HttpServletRequest request,ModelMap modelMap) {
		String checkedIds = request.getParameter("checkedIds");
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		List<Result> resultList = resultService.getViewDeliveryTasksByCorpId(paramsMap, 0, 0);
		modelMap.put("resultList", resultList);
		return "report/taskPrint";
	}
	/**
	 * 导出任务统计信息（API触发）
	 */
	@RequestMapping("/exportTaskStatApi")
	public void exportTaskStatApi(HttpServletRequest request,HttpServletResponse response) {
		
		String checkedIds = request.getParameter("checkedIds");
		String corpId = request.getParameter("corpId");
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		if(corpId != null&&!"".equals(corpId)) {
			paramsMap.put("corpId", Integer.parseInt(corpId));
		}
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_任务发送统计_API触发.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			resultService.writeCsvApiGroupByTask(csvWriter, paramsMap);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
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
	 * 导出任务统计信息（API触发）
	 */
	@RequestMapping("/exportTaskStatApiNew")
	public void exportTaskStatApiNew(HttpServletRequest request, HttpServletResponse response, ModelMap modelMap) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("corpPath",corpPath);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String[] checkedIds = request.getParameterValues("checkedIds");
		String corpId = request.getParameter("corpId");
		// checkedIds: 任务ID与用户ID拼成，如 taskId_userId
		if(checkedIds!=null && checkedIds.length>0) {
			realParamsMap.put("tidContactUid", checkedIds);
		}
		if(corpId != null&&!"".equals(corpId)) {
			realParamsMap.put("corpId", Integer.parseInt(corpId));
		}
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_API发送统计_触发任务.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			resultService.writeCsvApiGroupByTask(csvWriter, realParamsMap);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
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
	 * 打印统计信息（API触发）
	 */
	@RequestMapping("/printTaskStatApi")
	public String printTaskStatApi(HttpServletRequest request,ModelMap modelMap) {
		String checkedIds = request.getParameter("checkedIds");
		Integer[] taskIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			taskIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("taskIds", taskIds);
		List<Result> resultList = resultService.getAllApi(paramsMap);
		modelMap.put("resultList", resultList);
		return "report/taskPrintApi";
	}
	
	/**
	 * 分页展现任务统计列表(API触发)
	 */
	@RequestMapping("/apiList")
	public String apiList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		Pagination<Result> pagination = resultService.getPaginationGroupByTask(realParamsMap, "", currentPage, pageSize);
		
		modelMap.put("pagination", pagination);
		modelMap.put("triggerNameList", triggerNameList);
		
		return "report/taskApiReportList";
	}
	/**
	 * 导出任务统计信息（API群发）
	 */
	@RequestMapping("/exportTaskStatApiGroups")
	public void exportTaskStatApiGroups(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String[] checkedIds = request.getParameterValues("checkedIds");
		String corpId = request.getParameter("corpId");
		Integer[] taskIds = null;
		if(checkedIds != null&&checkedIds.length>0) {
			taskIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				taskIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		realParamsMap.put("taskIds", taskIds);
		if(corpId != null&&!"".equals(corpId)) {
			realParamsMap.put("corpId", Integer.parseInt(corpId));
		}
		realParamsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_API发送统计_群发任务.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			resultService.writeCsv(csvWriter, realParamsMap);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
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
	
	private void domainCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getTaskId()).append(",");
		}
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("taskIds", ConvertUtils.str2Int(sbff.toString()));
		List<Domain> domainList = domainService.getDomainSum(paramsMap);
		ResultDataOption.copy("task", domainList, sourceResults);
	}
	
}
