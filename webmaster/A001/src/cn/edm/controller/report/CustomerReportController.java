package cn.edm.controller.report;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.util.WebUtils;




import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.CustomerReport;
import cn.edm.domain.Domain;
import cn.edm.domain.Result;
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

/**
 * @author Luxh
 *
 */
@Controller
@RequestMapping("/customerReport")
public class CustomerReportController {
	
	private static Logger log = LoggerFactory.getLogger(CustomerReportController.class);
	
	@Autowired
	private DomainService domainService;
	
	@Autowired
	private ApiTriggerService apiTriggerService;
	
	@Autowired
	private ResultService resultService;
	
	/**
	 * 客户任务列表
	 */
	@RequestMapping(value="/list", method=RequestMethod.GET)
	public String list(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		String company =StreamToString.getStringByUTF8(request, "company");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("company",company);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		Pagination<Domain> pagination = domainService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
				
		return "report/customerStatList";
	}
	/**
	 * 导出用户发送统计
	 */
	@RequestMapping("/exportUserStat")
	public void exportUserStat(HttpServletRequest request,HttpServletResponse response) {
		
		String checkedIds = request.getParameter("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			corpIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("corpIds", corpIds);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_客户发送统计.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeCsv(csvWriter, paramsMap);
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
	 * 导出用户发送统计
	 */
	@RequestMapping(value="/exportUserStatNew", method=RequestMethod.GET)
	public void exportUserStatNew(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		String company =StreamToString.getStringByUTF8(request, "company");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("company",company);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		String[] checkedIds = request.getParameterValues("checkedIds");
//		String typeStr = XssFilter.filterXss(request.getParameter("typeStr"));
		
		Integer[] corpIds = null;
		if(checkedIds!=null && checkedIds.length>0) {
			corpIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				corpIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		String exportFileName = "数据报告_客户发送统计_投递发送类.csv";
		realParamsMap.put("corpIds", corpIds);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode(exportFileName));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeCsv(csvWriter, realParamsMap);
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
	@RequestMapping(value="/printUserStat", method=RequestMethod.GET)
	public String printStat(HttpServletRequest request,ModelMap modelMap) {
		String company =StreamToString.getStringByUTF8(request, "company");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("company",company);
		//过滤参数中的空值
		Map<String,Object> paramsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		String checkedIds = XssFilter.filterXss(request.getParameter("checkedIds"));
		Integer[] corpIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			corpIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> topCorpMap = new HashMap<String, Object>(); 
		topCorpMap.put("corpIds", corpIds);		
		CustomerReport report = domainService.getCustomerReportList(topCorpMap, paramsMap);
		
		modelMap.put("recordList", report.getRecordList());
		return "report/reportPrint";
	}
	
	/**
	 * 客户任务列表
	 */
	@RequestMapping(value="/taskList", method=RequestMethod.GET)
	public String tasklist(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("corpPath",corpPath);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		
		Integer corpId =Integer.parseInt(XssFilter.filterXss(request.getParameter("corpId")));
		String userId = XssFilter.filterXss(request.getParameter("userId"));
		
		realParamsMap.put("corpId", corpId);
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN};
		realParamsMap.put("types", types);
		Domain domain = domainService.getSum(realParamsMap);
		modelMap.put("domain", domain);
		
		Pagination<Domain> pagination = domainService.getPaginationByCustomer(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		
		modelMap.put("userId", userId);
		modelMap.put("corpId", corpId);
		
		return "report/customerTaskStatList";
	}
	/**
	 * api触发
	 */
	@RequestMapping(value="/apiTriggerList", method=RequestMethod.GET)
	public String apiTriggerList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		Pagination<Domain> pagination = domainService.getApiTriggerPagination(realParamsMap, "", currentPage, pageSize);
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		
		modelMap.put("pagination", pagination);
		modelMap.put("triggerNameList", triggerNameList);
		
		return "report/customerStatApiTriggerList";
	}
//	/**
//	 * 导出用户发送api统计
//	 */
//	@RequestMapping("/exportUserStatApi")
//	public void exportUserStatApi(HttpServletRequest request,HttpServletResponse response) {
//		
//		String checkedIds = request.getParameter("checkedIds");
//		Integer[] corpIds = null;
//		if(checkedIds != null&&!"".equals(checkedIds)) {
//			corpIds = ConvertUtils.str2Int(checkedIds);
//		}
//		Map<String,Object> paramsMap = new HashMap<String,Object>();
//		paramsMap.put("corpIds", corpIds);
//		
//		ServletOutputStream out = null;
//		CSVWriter csvWriter = null;
//		try {
//			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_客户发送统计_API触发类.csv"));
//			csvWriter = new CSVWriter(new OutputStreamWriter(out));
//			domainService.writeCsvApi(csvWriter, paramsMap);
//			csvWriter.flush();
//		}catch(Exception e) {
//			log.error(e.getMessage(), e);
//		}finally {
//			try {
//				if(out != null) {
//					out.close();
//				}
//				if(csvWriter != null) {
//					csvWriter.close();
//				}
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
//		}
//	}
	/**
	 * 导出用户发送api统计
	 */
	@RequestMapping("/exportUserStatApiNew")
	public void exportUserStatApiNew(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> paramsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String[] checkedIds = request.getParameterValues("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds != null&&checkedIds.length>0) {
			corpIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				corpIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		paramsMap.put("corpIds", corpIds);		
		
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_客户发送统计_API触发.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeCsvApi(csvWriter, paramsMap);
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
	 * 打印统计信息api统计
	 */
	@RequestMapping("/printStatApi")
	public String printStatApi(HttpServletRequest request,ModelMap modelMap) {
		String checkedIds = request.getParameter("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			corpIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("corpIds", corpIds);
		List<Domain> domainList = domainService.getApiByUserList(paramsMap);
		modelMap.put("domainList", domainList);
		return "report/reportPrintApi";
	}
	
	/**
	 * 客户任务列表API
	 */
	@RequestMapping("/taskApiList")
	public String taskApiList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String corpIdTemp = XssFilter.filterXss(request.getParameter("corpId"));
		String userId =  XssFilter.filterXss(request.getParameter("userId"));
		
		Integer corpId =Integer.parseInt(corpIdTemp);
		
		realParamsMap.put("corpId", corpId);
		
		Domain domain = domainService.getApiSum(realParamsMap);
		modelMap.put("domain", domain);
		
		Pagination<Result> pagination = resultService.getPaginationGroupByTask(realParamsMap, "", currentPage, pageSize);
		domainCopy(pagination.getRecordList());
		modelMap.put("pagination", pagination);

		modelMap.put("userId", userId);
		modelMap.put("corpId", corpId);
		
		return "report/customerTaskStatApiTriggerList";
	}
	/**
	 * 客户API群发任务统计列表
	 */
	@RequestMapping(value="/apiGroupsStatList", method=RequestMethod.GET)
	public String apiGroupsStatList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		Pagination<Domain> pagination = domainService.getApiGroupsByUserPagination(realParamsMap, "", currentPage, pageSize);
		
		modelMap.put("pagination", pagination);
		
		return "report/customer/api_groups_stat_list";
	}
	/**
	 * 客户API群发任务统计详细信息列表
	 */
	@RequestMapping(value="/apiGroupsTaskStatList", method=RequestMethod.GET)
	public String apiGroupsTaskStatList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String corpIdTemp = XssFilter.filterXss(request.getParameter("corpId"));
		String userId =  XssFilter.filterXss(request.getParameter("userId"));
		
		Integer corpId =Integer.parseInt(corpIdTemp);
		realParamsMap.put("corpId", corpId);
		
		Domain domain = domainService.getApiGroupsSum(realParamsMap);
		
		Pagination<Result> pagination = resultService.getApiGroupsByTaskPagination(realParamsMap, "", currentPage, pageSize);
		domainCopy(pagination.getRecordList());
		
		modelMap.put("domain", domain);
		modelMap.put("pagination", pagination);
		modelMap.put("userId", userId);
		modelMap.put("corpId", corpId);
		
		return "report/customer/api_groups_task_stat_list";
	}
	
	/**
	 * 导出API群发类报表
	 */
	@RequestMapping(value="/exportUserApiGroups", method=RequestMethod.GET)
	public void exportUserApiGroups(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		String userIdLike = StreamToString.getStringByUTF8(request,"userIdLike");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userIdLike",userIdLike);
		//过滤参数中的空值
		Map<String,Object> paramsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		String[] checkedIds = request.getParameterValues("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds!=null && checkedIds.length>0) {
			corpIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				corpIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		String exportFileName = "数据报告_客户发送统计_API群发类.csv";
		
		paramsMap.put("corpIds", corpIds);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode(exportFileName));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeApiGroupCsv(csvWriter, paramsMap);
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
		List<Domain> domainList = domainService.getByDomainSumByCorp(ConvertUtils.str2Int(sbff.toString()));
		ResultDataOption.copy("customer", domainList, sourceResults);
	}
	

	
}
