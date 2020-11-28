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
 * API投递类 Controller </br>
 * 1. API触发类 </br>
 * 2. API群发类 </br>
 * 
 * @Date 2014年7月15日 下午1:53:15
 * @author Lich
 *
 */
@Controller
@RequestMapping(value="/apiReport")
public class ApiReportController {
	
	private static Logger log = LoggerFactory.getLogger(ApiReportController.class);
	
	@Autowired
	private ApiTriggerService apiTriggerService;
	
	@Autowired
	private DomainService domainService;
	
	@Autowired
	private ResultService resultService;
	
	/**
	 * api触发
	 */
	@RequestMapping(value="/apiTriggerList", method=RequestMethod.GET)
	public String apiTriggerList(HttpServletRequest request, ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("corpPath",corpPath);
		initParamsMap.put("taskName",taskName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		currentPage = XssFilter.filterXssInt(currentPage + "");
		pageSize = XssFilter.filterXssInt(pageSize + "");
		Pagination<Result> pagination = resultService.getAllApiTriggerPagination(realParamsMap, "", currentPage, pageSize);
		domainApiCopy(pagination.getRecordList());
		modelMap.put("pagination", pagination);
		
		return "report/api/api_trigger_list";
	}
	/**
	 * 导出api触发任务统计
	 */
	@RequestMapping(value="/exportApiTrigger", method=RequestMethod.GET)
	public void exportApiTrigger(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("corpPath",corpPath);
		initParamsMap.put("taskName",taskName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		String[] checkedIds = request.getParameterValues("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds != null&&checkedIds.length>0) {
			corpIds = new Integer[checkedIds.length];
			for(int i=0;i<checkedIds.length;i++) {
				corpIds[i] = Integer.parseInt(checkedIds[i]);
			}
		}
		realParamsMap.put("corpIds", corpIds);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_客户发送统计_API触发.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeCsvApi(csvWriter, realParamsMap);
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
				log.error(e.getMessage(), e);
			}
		}
	}
	/**
	 * 打印统计信息api触发任务统计
	 */
	@RequestMapping(value="/printApiTrigger", method=RequestMethod.GET)
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
		return "report/api/api_print";
	}
	/**
	 * 所有API群发任务统计列表
	 */
	@RequestMapping(value="/apiGroupsList", method=RequestMethod.GET)
	public String apiGroupsList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("corpPath",corpPath);
		initParamsMap.put("taskName",taskName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		Pagination<Result> pagination = resultService.getApiGroupsByTaskPagination(realParamsMap, "", currentPage, pageSize);
		pagination.setRecordList(resultService.getIsCanImportTask(pagination.getRecordList()));
		domainGroupsCopy(pagination.getRecordList());
		modelMap.put("pagination", pagination);
		modelMap.put("taskType", "4");
		
		return "report/api/api_groups_list";
	}
	/**
	 * API群发任务统计详细信息列表
	 */
	@RequestMapping(value="/apiGroupsStatList", method=RequestMethod.GET)
	public String apiGroupsStatList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("corpPath",corpPath);
		initParamsMap.put("taskName",taskName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
//		Domain domain = domainService.getApiGroupsSum(realParamsMap);
		
		Pagination<Result> pagination = resultService.getApiGroupsByTaskPagination(realParamsMap, "", currentPage, pageSize);
		domainGroupsCopy(pagination.getRecordList());
//		modelMap.put("domain", domain);
		modelMap.put("pagination", pagination);
		
		return "report/api/api_groups_list";
	}
	
	private void domainApiCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getCorpId()).append(",");
		}
		List<Domain> domainList = domainService.getApiTriggerSumList(ConvertUtils.str2Int(sbff.toString()));
		ResultDataOption.copy("triger", domainList, sourceResults);
	}
	
	private void domainGroupsCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getTaskId()).append(",");
		}
		List<Domain> domainList = domainService.getDomainSumByTask(ConvertUtils.str2Int(sbff.toString()));
		ResultDataOption.copy("task", domainList, sourceResults);
	}
	
}
