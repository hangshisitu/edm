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
import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.domain.Domain;
import cn.edm.service.DomainService;
import cn.edm.util.CommonUtil;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;
import cn.edm.utils.StreamToString;

import com.google.common.collect.Maps;
/**
 * 周期任务报表统计
 * 
 * @Date 2014年6月11日 上午9:59:36
 * @author Lich
 *
 */
@Controller
@RequestMapping("/plan/planReport")
public class PlanReportController {
	
	private static Logger log = LoggerFactory.getLogger(PlanReportController.class);
	
	@Autowired
	private DomainService domainService;
	
	/**
	 * 分页展示周期任务统计列表
	 * @param request
	 * @param modelMap
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	@RequestMapping(value="/reportList", method=RequestMethod.GET)
	public String reportList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String taskName = StreamToString.getStringByUTF8(request,"taskName");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("taskName",taskName);
		initParamsMap.put("corpPath",corpPath);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		
		Pagination<Domain> pagination = domainService.getPaginationPlan(realParamsMap, "", currentPage, pageSize);
		List<Domain> results = pagination.getRecordList();
		copyResultList(results, realParamsMap, currentPage, pageSize);
		pagination.setRecordList(domainService.getIsCanImportTask(pagination.getRecordList()));
		modelMap.put("pagination", pagination);
		
		return "report/plan/plan_report_list";
	}
	/**
	 * 复制Result值
	 * 参数results为没投递效果的任务
	 * 从Result查询数据，复制Result的投递效果的统计数据到results对应属性中
	 * @param results
	 */
	private void copyResultList(List<Domain> results, Map<String,Object> realParamsMap, int currentPage, int pageSize) {
		String pIds = "";
		for(int i=0,len=results.size(); i<len; i++) {
			pIds += results.get(i).getPlanId() + ",";
		}
		if(pIds!=null && !"".equals(pIds)){
			pIds = pIds.substring(0, pIds.length()-1);
		}
		realParamsMap.clear();
		Pagination<Domain> pagination = domainService.getPlanResultSumList(realParamsMap, Converts._toIntegers(pIds), "", 0, 100);
		List<Domain> rs = pagination.getRecordList();
		ResultDataOption.copyResultList(results, rs);
	}
	/**
	 * 周期任务统计汇总
	 * @return
	 */
	@RequestMapping(value="/planCollect", method=RequestMethod.GET)
	public String planCollect(HttpServletRequest request, ModelMap modelMap) {
		
		String checkedIds = XssFilter.filterXss(request.getParameter("checkedIds"));
		Integer[] planIds = Converts._toIntegers(Converts.repeat(checkedIds));
		if (Asserts.empty(planIds) || planIds.length < 2 || planIds.length > 100) {
			modelMap.put(Message.MSG, "任务集合范围只能在2至100之间");
		}
		
		Map<String,Object> paramsMap = Maps.newHashMap(); 
		Domain result = domainService.getPlanResultSum(paramsMap, planIds);
		
		modelMap.put("checkedIds", checkedIds);
		modelMap.put("planTotal", planIds.length);
		modelMap.put("result", result);
        
		return "report/plan/plan_report_collect";
	}
	/**
	 * 导出周期任务统计信息
	 */
	@RequestMapping(value="/exportPlanStat", method=RequestMethod.GET)
	public void exportPlanStat(HttpServletRequest request,HttpServletResponse response) {
		
		String planId = XssFilter.filterXss(request.getParameter("planId"));
		
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		if(planId != null){
			paramsMap.put("planId", Integer.valueOf(planId));
		}
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response, EncodeUtils.urlEncode("周期任务统计.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writePlanCsv(csvWriter, paramsMap);
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
	 * 导出周期任务统计汇总信息
	 */
	@RequestMapping(value="/exportpPlanCollect", method=RequestMethod.GET)
	public void exportpPlanCollect(HttpServletRequest request, HttpServletResponse response, ModelMap modelMap) {
		
		String checkedIds = XssFilter.filterXss(request.getParameter("checkedIds"));
		Integer[] planIds = Converts._toIntegers(Converts.repeat(checkedIds));
		if (Asserts.empty(planIds) || planIds.length < 2 || planIds.length > 100) {
			modelMap.put(Message.MSG, "任务集合范围只能在2至100之间");
		}
		
		Map<String,Object> paramsMap = Maps.newHashMap();
		paramsMap.put("planIds", planIds);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response, EncodeUtils.urlEncode("周期任务统计汇总.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writePlanCollectCsv(csvWriter, paramsMap);
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
	
}
