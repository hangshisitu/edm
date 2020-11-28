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

import com.google.common.collect.Maps;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Domain;
import cn.edm.service.ApiTriggerService;
import cn.edm.service.DomainService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.utils.ResultDataOption;

/**
 * 历史发送总览
 * @author Luxh
 *
 */
@Controller
@RequestMapping("/historyReport")
public class HistoryReportController {
	
	private static Logger log = LoggerFactory.getLogger(HistoryReportController.class);
	
	@Autowired
	private DomainService domainService;
	
	@Autowired
	private ApiTriggerService apiTriggerService;
	
	/**
	 * 历史发送总览列表
	 */
	@RequestMapping(value="/list", method=RequestMethod.GET)
	public String list(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN};
		realParamsMap.put("types", types);
		Domain domain = domainService.getSum(realParamsMap);
		if(domain==null) {
			domain = new Domain();
		}
		double success1 = 0.00d;
		if((domain.getSentCount()-domain.getAfterNoneCount())!=0) {
			success1 = (domain.getReachCount()*100.00)/(domain.getSentCount()-domain.getAfterNoneCount());
		}
		domain.setSuccessRate(CommonUtil.getFormatNumber(success1));
		
		double open1 = 0.00d;
//		if(domain.getSentCount()!=0) {
//			open1 = (domain.getReadUserCount()*100.00)/domain.getSentCount();
//		}
		if(domain.getReachCount()!=0) {
            open1 = (domain.getReadUserCount()*100.00)/domain.getReachCount();
        }
		domain.setOpenRate(CommonUtil.getFormatNumber(open1));
		
		double click1 = 0.00d;
		if(domain.getReadUserCount()!=0) {
			click1 = (domain.getClickUserCount()*100.00)/domain.getReadUserCount();
		}
		domain.setClickRate(CommonUtil.getFormatNumber(click1));
		modelMap.put("domain", domain);
		List<Domain> domailList = domainService.getSumList(realParamsMap);
		
		if(domailList==null||domailList.size()==0) {
			modelMap.put("hasdomain","N");
		}else {
			modelMap.put("hasdomain","Y");
			Domain ohterDomain = new Domain();
			for(Domain d:domailList) {
				if("139.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domain139 = CommonUtil.getDomain(d);
					
					modelMap.put("domain139", domain139);
				}else if("163.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domain163 = CommonUtil.getDomain(d);
					
					modelMap.put("domain163", domain163);
					
				}else if("gmail.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainGmail = CommonUtil.getDomain(d);
					modelMap.put("domainGmail", domainGmail);
				}else if("qq.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainQQ =CommonUtil.getDomain(d);
					modelMap.put("domainQQ", domainQQ);
				}else if("sina.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainSina =CommonUtil.getDomain(d);
					modelMap.put("domainSina", domainSina);
				}else if("sohu.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainSohu = CommonUtil.getDomain(d);
					modelMap.put("domainSohu", domainSohu);
				}else {
					ohterDomain.setSentCount(ohterDomain.getSentCount()+d.getSentCount());
					ohterDomain.setReachCount(ohterDomain.getReachCount()+d.getReachCount());
					ohterDomain.setSoftBounceCount(ohterDomain.getSoftBounceCount()+d.getSoftBounceCount());
					ohterDomain.setHardBounceCount(ohterDomain.getHardBounceCount()+d.getHardBounceCount());
					ohterDomain.setReadUserCount(ohterDomain.getReadUserCount()+d.getReadUserCount());
					ohterDomain.setReadCount(ohterDomain.getReadCount()+d.getReadCount());
					ohterDomain.setClickUserCount(ohterDomain.getClickUserCount()+d.getClickUserCount());
					ohterDomain.setClickCount(ohterDomain.getClickCount()+d.getClickCount());
					ohterDomain.setUnsubscribeCount(ohterDomain.getUnsubscribeCount()+d.getUnsubscribeCount());
					ohterDomain.setForwardCount(ohterDomain.getForwardCount()+d.getForwardCount());
					//ohterDomain.setBeforeNoneCount(ohterDomain.getBeforeNoneCount()+d.getBeforeNoneCount());
					ohterDomain.setAfterNoneCount(ohterDomain.getAfterNoneCount()+d.getAfterNoneCount());
				}
			}
			double success = 0.00d;
			if((ohterDomain.getSentCount()-ohterDomain.getAfterNoneCount())!=0) {
				success = (ohterDomain.getReachCount()*100.00)/(ohterDomain.getSentCount()-ohterDomain.getAfterNoneCount());
			}
			ohterDomain.setSuccessRate(CommonUtil.getFormatNumber(success));
			
			double open = 0.00d;
//			if(ohterDomain.getSentCount()!=0) {
//                open = (ohterDomain.getReadUserCount()*100.00)/ohterDomain.getSentCount();
//			}
			if(ohterDomain.getReachCount()!=0) {
              open = (ohterDomain.getReadUserCount()*100.00)/ohterDomain.getReachCount();
			}
			ohterDomain.setOpenRate(CommonUtil.getFormatNumber(open));
			
			double click = 0.00d;
			if(ohterDomain.getReadUserCount()!=0) {
				click = (ohterDomain.getClickUserCount()*100.00)/ohterDomain.getReadUserCount();
			}
			ohterDomain.setClickRate(CommonUtil.getFormatNumber(click));
			modelMap.put("ohterDomain",ohterDomain);
		}
		
		//Pagination<Domain> pagination = domainService.getPagination(realParamsMap, "", currentPage, pageSize);
		//modelMap.put("pagination", pagination);
		
		return "report/historyReportList";
	}
	
	/**
	 * 导出用户统计信息
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
	 * 打印统计信息
	 */
	@RequestMapping("/printUserStat")
	public String printStat(HttpServletRequest request,ModelMap modelMap) {
		String checkedIds = request.getParameter("checkedIds");
		Integer[] corpIds = null;
		if(checkedIds != null&&!"".equals(checkedIds)) {
			corpIds = ConvertUtils.str2Int(checkedIds);
		}
		Map<String,Object> paramsMap = new HashMap<String,Object>();
		paramsMap.put("corpIds", corpIds);
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN};
		paramsMap.put("types", types);
		List<Domain> domainList = domainService.getDomainList(paramsMap);
		modelMap.put("domainList", domainList);
		return "report/reportPrint";
	}
	/**
	 * API触发类
	 */
	@RequestMapping("/apiTrigger")
	public String apiTrigger(HttpServletRequest request,ModelMap modelMap) {
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		Domain domain = domainService.getApiTriggerSum(realParamsMap);
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		modelMap.put("domain", domain);
		modelMap.put("triggerNameList", triggerNameList);
		
		return "report/apiTriggerSum";
	}
	
	/**
	 * 导出统计信息
	 */
	@RequestMapping("/exportHis")
	public void exportHis(HttpServletRequest request, HttpServletResponse response, ModelMap modelMap) {
	
		Map<String,Domain> allMap = Maps.newHashMap();	
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN};
		realParamsMap.put("types", types);
		Domain domain = domainService.getSum(realParamsMap);
		if(domain==null) {
			domain = new Domain();
		}
		double success1 = 0.00d;
		if((domain.getSentCount()-domain.getAfterNoneCount())!=0) {
			success1 = (domain.getReachCount()*100.00)/(domain.getSentCount()-domain.getAfterNoneCount());
		}
		domain.setSuccessRate(CommonUtil.getFormatNumber(success1));
		
		double open1 = 0.00d;
		if(domain.getReachCount()!=0) {
			open1 = (domain.getReadUserCount()*100.00)/domain.getReachCount();
		}
		domain.setOpenRate(CommonUtil.getFormatNumber(open1));
		
		double click1 = 0.00d;
		if(domain.getReadUserCount()!=0) {
			click1 = (domain.getClickUserCount()*100.00)/domain.getReadUserCount();
		}
		domain.setClickRate(CommonUtil.getFormatNumber(click1));
		allMap.put("domain", domain);
		
		List<Domain> domailList = domainService.getSumList(realParamsMap);
		
		if(domailList==null||domailList.size()==0) {
			modelMap.put("hasdomain","N");
			Domain nullDomain = new Domain();
			allMap.put("domain139", nullDomain);
			allMap.put("domain163", nullDomain);
			allMap.put("domainGmail", nullDomain);
			allMap.put("domainQQ", nullDomain);
			allMap.put("domainSina", nullDomain);
			allMap.put("domainSohu", nullDomain);
			allMap.put("ohterDomain", nullDomain);
		}else {
			modelMap.put("hasdomain","Y");
			Domain ohterDomain = new Domain();
			for(Domain d:domailList) {
				if("139.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domain139 = CommonUtil.getDomain(d);
					allMap.put("domain139", domain139);
				}else if("163.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domain163 = CommonUtil.getDomain(d);
					
					allMap.put("domain163", domain163);
					
				}else if("gmail.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainGmail = CommonUtil.getDomain(d);
					allMap.put("domainGmail", domainGmail);
				}else if("qq.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainQQ = CommonUtil.getDomain(d);
					allMap.put("domainQQ", domainQQ);
				}else if("sina.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainSina = CommonUtil.getDomain(d);
					allMap.put("domainSina", domainSina);
				}else if("sohu.com".equals(d.getEmailDomain().toLowerCase())){
					Domain domainSohu = CommonUtil.getDomain(d);
					allMap.put("domainSohu", domainSohu);
				}else {
					ohterDomain.setSentCount(ohterDomain.getSentCount()+d.getSentCount());
					ohterDomain.setReachCount(ohterDomain.getReachCount()+d.getReachCount());
					ohterDomain.setSoftBounceCount(ohterDomain.getSoftBounceCount()+d.getSoftBounceCount());
					ohterDomain.setHardBounceCount(ohterDomain.getHardBounceCount()+d.getHardBounceCount());
					ohterDomain.setReadUserCount(ohterDomain.getReadUserCount()+d.getReadUserCount());
					ohterDomain.setReadCount(ohterDomain.getReadCount()+d.getReadCount());
					ohterDomain.setClickUserCount(ohterDomain.getClickUserCount()+d.getClickUserCount());
					ohterDomain.setClickCount(ohterDomain.getClickCount()+d.getClickCount());
					ohterDomain.setUnsubscribeCount(ohterDomain.getUnsubscribeCount()+d.getUnsubscribeCount());
					ohterDomain.setForwardCount(ohterDomain.getForwardCount()+d.getForwardCount());
					//ohterDomain.setBeforeNoneCount(ohterDomain.getBeforeNoneCount()+d.getBeforeNoneCount());
					ohterDomain.setAfterNoneCount(ohterDomain.getAfterNoneCount()+d.getAfterNoneCount());
				}
			}
			double success = 0.00d;
			if((ohterDomain.getSentCount()-ohterDomain.getAfterNoneCount())!=0) {
				success = (ohterDomain.getReachCount()*100.00)/(ohterDomain.getSentCount()-ohterDomain.getAfterNoneCount());
			}
			ohterDomain.setSuccessRate(CommonUtil.getFormatNumber(success));
			
			double open = 0.00d;
			if(ohterDomain.getReachCount()!=0) {
				open = (ohterDomain.getReadUserCount()*100.00)/ohterDomain.getReachCount();
			}
			ohterDomain.setOpenRate(CommonUtil.getFormatNumber(open));
			
			double click = 0.00d;
			if(ohterDomain.getReadUserCount()!=0) {
				click = (ohterDomain.getClickUserCount()*100.00)/ohterDomain.getReadUserCount();
			}
			ohterDomain.setClickRate(CommonUtil.getFormatNumber(click));
			allMap.put("ohterDomain", ohterDomain);
		}
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_历史发送统计_投递发送类.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeHisCsv(csvWriter, allMap);
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
	 * API投递类 </br>
	 * 1. API触发类 </br>
	 * 2. API群发类</br>
	 */
	@RequestMapping(value="/apiDelivery", method=RequestMethod.GET)
	public String apiDelivery(HttpServletRequest request,ModelMap modelMap) {
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		// API触发类
		Domain resultTrigger = domainService.getApiTriggerSum(realParamsMap);
		// API群发类
		Domain resultGroups = domainService.getApiGroupsSum(realParamsMap);
		if(resultTrigger == null){
			resultTrigger = new Domain();
		}
		if(resultGroups == null){
			resultGroups = new Domain();
		}
		// 触发任务与群发任务合计
		Domain resultTotal = ResultDataOption.statResultTotal(resultTrigger, resultGroups);
		
		modelMap.put("resultTrigger", resultTrigger);
		modelMap.put("resultGroups", resultGroups);
		modelMap.put("resultTotal", resultTotal);
		
		return "report/history/api_delivery_stat";
	}
	/**
	 * 导出用户统计信息
	 */
	@RequestMapping(value="/exportApiDelivery", method=RequestMethod.GET)
	public void exportApiDelivery(HttpServletRequest request, HttpServletResponse response, ModelMap modelMap) {
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("数据报告_历史发送统计_Api投递.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			domainService.writeCsvApiHis(csvWriter, realParamsMap);
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
