package cn.edm.controller.campaign;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.util.WebUtils;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Browser;
import cn.edm.domain.Campaign;
import cn.edm.domain.Domain;
import cn.edm.domain.Hour;
import cn.edm.domain.Lang;
import cn.edm.domain.Os;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Touch;
import cn.edm.domain.Url;
import cn.edm.exception.Errors;
import cn.edm.service.CampaignService;
import cn.edm.service.DomainService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateSettingService;
import cn.edm.service.TouchService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.DomainStatComparator;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Loggers;
import cn.edm.util.Pagination;
import cn.edm.util.Params;
import cn.edm.util.UrlClickComparator;
import cn.edm.util.XssFilter;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;
import cn.edm.utils.StreamToString;
import cn.edm.vo.DomainStat;
import cn.edm.vo.Log;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller
@RequestMapping("/campaign")
public class CampaignController {
	private static Logger log = LoggerFactory.getLogger(CampaignController.class);

	@Autowired
	private CampaignService campaignService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private TouchService touchService;
	@Autowired
	private TemplateSettingService settingService;
	@Autowired
	private DomainService domainService;
	
	/**
	 * 活动发送统计列表
	 */
	@RequestMapping(value="/list", method=RequestMethod.GET)
	public String list(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String campaignName = StreamToString.getStringByUTF8(request, "campaignName");
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request,SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("campaignName",campaignName);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			//分页查询
			Pagination<Result> pagination = campaignService.getPagination(realParamsMap, "", currentPage, pageSize);
			
			List<Result> resultList = pagination.getRecordList();
			domainCampaignCopy(resultList);
//			Integer campaignId = null;
//			if(resultList!=null && resultList.size()>0){
//				for(Result r : resultList) {
//					int touchCount = 0; // 关联触发数
//					campaignId = r.getCampaignId();
//					List<Task> taskList = taskService.getTouchTask(campaignId, "campaign");
//					if(taskList!=null && taskList.size()>0) touchCount = taskList.size();
//					r.setTouchCount(touchCount);
//				}
//			}
			modelMap.put("pagination", pagination);
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignReportList";
	}
	/**
	 * 活动报告-概览
	 */
	@RequestMapping("/overview")
	public String overview(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				
				Domain domain = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("domain", domain);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignOverview";
	}
	/**
	 * 活动报告-发送
	 */
	@RequestMapping("/send")
	public String send(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				Domain result = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				Domain domain = campaignService.getCampaignSendBackSum(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("result", result);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				modelMap.put("domain", domain);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignSend";
	}
	/**
	 * 活动报告-打开
	 */
	@RequestMapping("/open")
	public String open(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				Domain result = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				List<Hour> hourList = campaignService.getCampaignHour(campaignId);
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
				
				modelMap.put("campaign", campaign);
				modelMap.put("result", result);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				modelMap.put("hourMap", hourMap);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignOpen";
	}
	/**
	 * 打开详细列表
	 */
	@RequestMapping("/openLogList")
	public @ResponseBody Map<String,Object> openLogList(HttpServletRequest request) {
		Map<String,Object> resultMap = Maps.newHashMap();
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				List<Task> tasks = taskService.getTasksByCampaignId(campaignId);
				if(tasks!=null&&tasks.size()>0) {
					String[] sids = new String[tasks.size()];
					String[] times = new String[tasks.size()];
					for(int i=0;i<tasks.size();i++) {
						//打开的详细列表
						Task task = tasks.get(i);
						String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
						String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
						sids[i] = sid;
						times[i] = time;
					}
					List<Log> tempList = Loggers.openCampaign(sids, times,"head", "utf-8");
					resultMap.put("openList", tempList);
				}
				
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		
		return resultMap;
	}
	/**
	 * 活动报告-点击
	 */
	@RequestMapping("/click")
	public String click(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				Domain result = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				List<Url> urlList = campaignService.getCampaignClick(campaignId);
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
				

				Integer totalClick = campaignService.getCampaignClickSum(campaignId);
				modelMap.put("urlList", urlList);
				modelMap.put("totalClick", totalClick);
				
				modelMap.put("campaign", campaign);
				modelMap.put("result", result);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignClick";
	}
	/**
	 * 活动报告-域名
	 */
	@RequestMapping("/domain")
	public String domain(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				
				Domain result = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("result", result);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				
				List<Domain> domainList = campaignService.getCampaignDomainSum(campaignId);
				
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
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignDomain";
	}
	/**
	 * 活动报告-地域
	 */
	@RequestMapping("/region")
	public String region(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				
				Domain result = campaignService.getCampaignSum(campaignId);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("result", result);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				
				List<Region> provinceRegionList = campaignService.getCampaignProvince(campaignId);
				List<Region> cityRegionList = campaignService.getCampaignCity(campaignId);
				
				modelMap.put("provinceRegionList", provinceRegionList);
				modelMap.put("cityRegionList", cityRegionList);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignRegion";
	}
	
	
	/**
	 * 活动报告-终端
	 */
	@RequestMapping("/client")
	public String client(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				
				List<Browser> browserList = campaignService.getBrowsers(campaignId);
				List<Lang> langList = campaignService.getLangs(campaignId);
				List<Os> osList = campaignService.getOss(campaignId);
				Map<String,Browser> browserMap = campaignService.calcBrowserRate(browserList);
				Map<String,Lang> langMap = campaignService.calcLangRate(langList);
				Map<String,Os> osMap = campaignService.calcOsRate(osList);
				
				
				modelMap.put("browser", browserMap);
				modelMap.put("total",browserMap.get("0").getTotal());
				modelMap.put("lang", langMap);
				modelMap.put("os", osMap);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignClient";
	}
	
	/**
	 * 活动报告-投递历史
	 */
	@RequestMapping("/delivery")
	public String delivery(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				String campaignName = StreamToString.getStringByUTF8(request, "campaignName");
				//获取查询参数
				Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request,SearchDefine.SEARCH_PREFIX);
				initParamsMap.put("campaignName",campaignName);
				//过滤参数中的空值
				Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
				realParamsMap.put("campaignId", campaignId);
				
				String orderby = XssFilter.filterXss(request.getParameter("orderBy"));
				if(orderby==null||"".equals(orderby.trim())) {
					orderby = "deliveryTime";
				}
				if("deliveryTime".equals(orderby)||"sentCount".equals(orderby)) {
					realParamsMap.put("orderby", orderby);
				}
				//分页查询
				Pagination<Result> pagination = campaignService.getCampaignResult(realParamsMap, "", currentPage, pageSize);
				
				domainCopy(pagination.getRecordList());
				if("reachCount".equals(orderby)||"readUserCount".equals(orderby)||"clickUserCount".equals(orderby)) {
					//处理排序
					List<Result> resultList = campaignService.sortResultList(pagination.getRecordList(), orderby);
					pagination.setRecordList(resultList);
				}

				modelMap.put("pagination", pagination);
				modelMap.put("orderby", orderby);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				
				
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignDelivery";
	}
	
	/**
	 * 活动报告-对比分析
	 */
	@RequestMapping("/compare")
	public String compare(HttpServletRequest request, ModelMap modelMap) {
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			String mode = XssFilter.filterXss(request.getParameter("m"));
			String recents = XssFilter.filterXss(request.getParameter("recents"));
			
			String checkeds = XssFilter.filterXss(request.getParameter("checkeds"));
			String extra = XssFilter.filterXss(request.getParameter("extra"));
			
			if(mode==null||"".equals(mode.trim())) {
				mode = "week";
			}
			if(recents==null||"".equals(recents.trim())) {
				recents = "2";
			}
			modelMap.put("mode", mode);
			modelMap.put("checkeds", checkeds);
			String last = "-";
			
			if(campaignIdTemp != null && !"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				
				Campaign campaign = campaignService.getById(campaignId);
				int touchCount = taskService.getTouchCount(campaignId, "campaign");
				campaign.setTouchCount(touchCount);
				long taskNum = campaignService.getTaskNumById(campaignId);
				String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
				
				modelMap.put("campaign", campaign);
				modelMap.put("taskNum", taskNum);
				modelMap.put("latestTime", latestTime);
				
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("campaignId", campaignId);
				
				
				Map<String,Integer> lastMap = campaignService.getLastTime(campaignId);
				
				if("week".equals(mode)) {
					int week = lastMap.get("week");
					last = week + "周";
					modelMap.put("weekId", recents);
					if(week>1) {
						List<Domain> resultList = campaignService.getResultListByRange(paramsMap, mode, recents,extra,checkeds);
						modelMap.put("resultList", resultList);
					}
				}else if("month".equals(mode)){
					int month = lastMap.get("month");
					last = month + "个月";
					if("other".equals(extra)) {
						modelMap.put("monthId", "otherMonth");
					}else {
						modelMap.put("monthId", recents);
					}
					if(month>1) {
						List<Domain>  resultList = campaignService.getResultListByRange(paramsMap, mode, recents, extra, checkeds);
						modelMap.put("resultList", resultList);
					}
				}else if("season".equals(mode)){
					int season = lastMap.get("season");
					last = season + "个季度";
					if("other".equals(extra)) {
						modelMap.put("seasonId", "otherSeason");
					}else {
						modelMap.put("seasonId", recents);
					}
					if(season>1) {
						List<Domain> resultList = campaignService.getResultListByRange(paramsMap, mode, recents,extra,checkeds);
						modelMap.put("resultList", resultList);
					}
					
				}
			}
			modelMap.put("last", last);
		}catch(Errors e) {
			log.error(e.getMessage(), e);
			modelMap.put(Message.MSG, e.getMessage());
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaignCompare";
	}
	/**
	 * 导出csv
	 */
	@RequestMapping("/exportCsv")
	public void exportCsv(HttpServletRequest request,HttpServletResponse response) {
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			String campaignIdTemp = XssFilter.filterXss(request.getParameter("campaignId"));
			String flag = XssFilter.filterXss(request.getParameter("flag"));
			if(campaignIdTemp!=null&&!"".equals(campaignIdTemp.trim())) {
				Integer campaignId = Integer.parseInt(campaignIdTemp);
				if("overview".equals(flag)) {
					//导出总览
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_概览.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("send".equals(flag)){
					//导出发送
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_发送.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("open".equals(flag)){
					//导出打开
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_打开.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("click".equals(flag)){
					//导出点击
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_点击.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("domain".equals(flag)){
					//导出域名
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_域名.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("region".equals(flag)){
					//导出地域
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_地域.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("delivery".equals(flag)){
					//导出投递历史
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_投递历史.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					String orderby = XssFilter.filterXss(request.getParameter("orderBy"));
					campaignService.writeDeliveryCsv(csvWriter, campaignId, flag, orderby);
					csvWriter.flush();
				}else if("compare".equals(flag)){
					//导出对比
					String mode = XssFilter.filterXss(request.getParameter("m"));
					String recents = XssFilter.filterXss(request.getParameter("recents"));
					
					String checkeds = XssFilter.filterXss(request.getParameter("checkeds"));
					String extra = XssFilter.filterXss(request.getParameter("extra"));
					
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_对比.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					
					campaignService.writeCompareCsv(csvWriter, campaignId, flag,mode,recents,checkeds,extra);
					csvWriter.flush();
				}else if("client".equals(flag)){
					//导出终端
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_终端.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("touch".equals(flag)) {
					// 导出触发
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_触发.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}else if("all".equals(flag)) {
					//导出所有
					out = IOUitl.getOut(response,EncodeUtils.urlEncode("活动统计_全局.csv"));
					csvWriter = new CSVWriter(new OutputStreamWriter(out));
					campaignService.writeCsv(csvWriter, campaignId, flag);
					csvWriter.flush();
				}
			}
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
	 * 活动报告-触发
	 */
	@RequestMapping("/touch")
	public String touch(HttpServletRequest request, ModelMap modelMap) {
		try {
			Integer campaignId = XssFilter.filterXssInt(request.getParameter("campaignId"));
			if(campaignId != null) {
				
	            Campaign campaign = campaignService.getById(campaignId);
	            int touchCount = taskService.getTouchCount(campaignId, "campaign");
	            campaign.setTouchCount(touchCount);
	            long taskNum = campaignService.getTaskNumById(campaignId);
	            campaign.setTaskCount( (int) taskNum);
	            String latestTime = campaignService.getLatestDeliveryTimeById(campaignId);
	            
	            Map<String, List<Domain>> domainMap = Maps.newHashMap(); // 有活动的触发计划，以任务区分，数据形式：Map<taskName_taskId, domainList>
	            List<Domain> domainList = null;
	            Integer[] taskIds = null;
	            List<Task> taskList = taskService.getTouchTask(campaignId, "campaign");
	            if(taskList!=null && taskList.size()>0){
	            	String taskIdStr = "";
	            	for(Task t : taskList) {
	            		taskIdStr += t.getTaskId() + ",";
	            	}
	            	taskIds = Converts._toIntegers(taskIdStr);
	            	List<Touch> touchList = touchService.getByParentIds(taskIds);
	            	String touchIdStr = ""; 
	            	for(int i=0,len=touchList.size(); i<len; i++) {
	            		Touch t = touchList.get(i);
	            		touchIdStr += t.getTaskId() + ",";
	            	}
	            	Integer[] touchIds = Converts._toIntegers(touchIdStr);
	            	List<Domain> domains = null;
	            	if(touchIds!=null && touchIds.length>0){
	            		Map<String, Object> paramsMap = Maps.newHashMap();
	            		paramsMap.put("taskIds", touchIds);
	            		domainList = domainService.getCampaignTouchList(paramsMap);
	            		for(Domain d : domainList) {
	            			String key = d.getTaskName() + "_" + d.getTaskId();
	            			domainMap.put(key, null);
	            		}
	            		for(String k : domainMap.keySet()) {
	            			domains = Lists.newArrayList();
	            			for(Domain d : domainList) {
	            				String key = d.getTaskName() + "_" + d.getTaskId();
	            				if(k.equals(key)){
	            					Map<String, Object> pMap = Maps.newHashMap();
	            					pMap.put("taskId", d.getTaskId());
	            					Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
	            					pMap.put("types", types);
	            					Domain domain = domainService.getSum(pMap);
	            					Integer parentReadUserCount = 0;
	            					if(domain != null){
	            						parentReadUserCount = domain.getReadUserCount();
	            					}
	            					d.setParentReadUserCount(parentReadUserCount);
	            					domains.add(d);
	            				}
	            			}
	            			domainMap.put(k, domains);
	            		}
	            	}
	            }
	        	
	        	modelMap.put("domainList", domainList);
	        	modelMap.put("domainMap", domainMap);
	        	modelMap.put("taskIds", taskIds);
	            
	            modelMap.put("campaign", campaign);
	            modelMap.put("taskNum", taskNum);
	            modelMap.put("latestTime", latestTime);
			}
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		return "campaign/campaign_touch";
	}
	
	private void domainCampaignCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
        for (Result result : sourceResults) {
            sbff.append(result.getCampaignId()).append(",");
        }
		List<Domain> domainList = campaignService.getCampaignSumList(ConvertUtils.str2Int(sbff.toString()), "campaign");
		ResultDataOption.copy("campaign", domainList, sourceResults);
	}
	
	private void domainCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getCampaignId()).append(",");
		}
		List<Domain> domainList = campaignService.getCampaignSumList(ConvertUtils.str2Int(sbff.toString()), "task");
		ResultDataOption.copy("task", domainList, sourceResults);
	}
	
}
