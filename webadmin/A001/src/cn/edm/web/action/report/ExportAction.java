package cn.edm.web.action.report;

import java.io.OutputStreamWriter;
import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.consts.Type;
import cn.edm.consts.mapper.DomainMap;
import cn.edm.consts.mapper.WeekMap;
import cn.edm.model.Browser;
import cn.edm.model.Campaign;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Lang;
import cn.edm.model.Os;
import cn.edm.model.Plan;
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.model.Triger;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.View;
import cn.edm.web.action.Action;
import cn.edm.web.facade.CSV;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Counts;
import cn.edm.web.facade.Domains;
import cn.edm.web.facade.Regions;
import cn.edm.web.facade.Tasks;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("ReportExport")
public class ExportAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(ExportAction.class);
	
	@RequestMapping(value = "/report/export/local", method = RequestMethod.GET)
    public String local(HttpServletRequest request, HttpServletResponse response) {
        try {
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
            
            MapBean mb = new MapBean();
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Domain total = domainService.sum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(),
                    null, beginTime, endTime, new Integer[] {Type.FORMAL, Type.TEST, Type.COLL});
            Domain other = new Domain();
            
            total.setEmailDomain("合计");
            other.setEmailDomain("其他");
            Counts.plus(total, other);
            
            List<Domain> domainList = Lists.newArrayList();
            
            for (DomainMap mapper : DomainMap.values()) {
                Domain d = domainService.sum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(),
                        DomainMap.getDomains(mapper.getName()), beginTime, endTime, new Integer[] {Type.FORMAL, Type.TEST, Type.COLL});
                d.setEmailDomain(mapper.getName());
                domainList.add(d);
                Counts.minus(d, other);
            }

            domainList.add(other);
            domainList.add(total);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("平台投递任务.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.sum(writer, CSV.LOCAL, domainList);

            writer.flush();
            writer.close();

            return null;
        } catch (Exception e) {
            logger.error("(Export:local) error: ", e);
            return Views._404();
        }
    }
	
	@RequestMapping(value = "/report/export/api", method = RequestMethod.GET)
    public String api(HttpServletRequest request, HttpServletResponse response) {
        try {
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            List<Triger> trigerList = trigerService.findApi(corpIds, UserSession.containsCorpId());
            
            MapBean mb = new MapBean();
            List<Domain> domainList = Lists.newArrayList();
            Domain total = new Domain();
            
            if (UserSession.isTriger()) {
                Domain triger = domainService.trigerSum(mb, Tasks.corpIds(trigerList), UserSession.containsCorpId(), beginTime, endTime);
                if (triger != null && Values.get(triger.getSentCount()) > 0) {
                    triger.setTaskName("触发任务");
                    Counts.plus(triger, total);
                    domainList.add(triger);    
                }
            }
            if (UserSession.isJoin()) {
                Domain join = domainService.sum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(),
                        null, beginTime, endTime, new Integer[] {Type.JOIN});
                if (join != null && Values.get(join.getSentCount()) > 0) {
                    join.setTaskName("群发任务");
                    Counts.plus(join, total);
                    domainList.add(join);
                }
            }

            if (domainList.size() > 1) {
                total.setTaskName("合计");
                domainList.add(total);
            }
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("API投递任务.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.sum(writer, CSV.API, domainList);

            writer.flush();
            writer.close();

            return null;
        } catch (Exception e) {
            logger.error("(Export:api) error: ", e);
            return Views._404();
        }
    }
	
	@RequestMapping(value = "/report/export/task", method = RequestMethod.GET)
	public String task(HttpServletRequest request, HttpServletResponse response) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");

			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");

			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
			
			Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), taskId);
			
			List<Domain> domainList = domainService.find(corpIds, UserSession.containsCorpId(), taskId);
			Domains.fetchOther(domainList);
			
			List<Region> provinceList = regionService.group(corpIds, UserSession.containsCorpId(), taskId);
			List<Region> cityList = regionService.find(corpIds, UserSession.containsCorpId(), taskId);
			List<Region> regionList = Regions.combine(provinceList, cityList, "  ");
			
        	// 触发
        	int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), taskId, "task");
        	task.setTouchCount(touchCount);
            List<Domain> touchDomainList = null;
            if (touchCount > 0) {
                List<Touch> touchList = touchService.find(taskId);
                String taskIdStr = "";
                for (int i = 0, len = touchList.size(); i < len; i++) {
                    Touch t = touchList.get(i);
                    taskIdStr += t.getTaskId() + ",";
                }
                Integer[] taskIds = Converts._toIntegers(taskIdStr);
                if (taskIds != null && taskIds.length > 0) {
                    List<Setting> settingList = settingService.touchQuery(corpIds, UserSession.containsCorpId(), task.getTemplateId(), taskId);
                    touchDomainList = domainService.touchGroup(corpIds, UserSession.containsCorpId(), null, taskIds);
                    for (Domain d : touchDomainList) {
                        Domain d2 = domainService.sum(corpIds, UserSession.containsCorpId(), d.getTaskId());
                        Integer parentReadUserCount = 0;
                        if (d2 != null) {
                            parentReadUserCount = d2.getReadUserCount();
                        }
                        d.setParentReadUserCount(parentReadUserCount);
                    }
                    Domains.copyUrlsToDomain(settingList, touchDomainList);
                }
            }
			
			ServletOutputStream out = Webs.output(response, Encodes.urlEncode((task.getType().equals(Type.JOIN) ? "群发" : "单项") + "任务.csv"));
			CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
			CSV.header(writer, task, result);
			CSV.blank(writer);
			
			CSV.name(writer, "#概览#");
			CSV.cover(writer, CSV.DEFAULT, domain);
			CSV.blank(writer);
			
			CSV.name(writer, "#成功#");
			CSV.send(writer, result, "success");
			CSV.blank(writer);
			CSV.name(writer, "#软弹回#");
			CSV.send(writer, result, "delay");
			CSV.blank(writer);
			CSV.name(writer, "#硬弹回#");
			CSV.send(writer, result, "fail");
			CSV.blank(writer);
			
			CSV.name(writer, "#打开#");
			CSV.open(writer, result);
			CSV.blank(writer);
			
			CSV.name(writer, "#点击#");
			CSV.click(writer, result);
			CSV.blank(writer);

			CSV.name(writer, "#域名#");
			CSV.domain(writer, domainList);
			CSV.blank(writer);
			
			CSV.name(writer, "#地域#");
			CSV.region(writer, regionList);
			
			if(touchCount > 0){
				CSV.blank(writer);
	            CSV.name(writer, "#触发#");
	            CSV.touch(writer, touchDomainList, null, "detail");
            }
			
			writer.flush();
			writer.close();
			
			return null;
		} catch (Exception e) {
			logger.error("(Export:task) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "/report/export/plan", method = RequestMethod.GET)
    public String plan(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer planId = Https.getInt(request, "planId", R.CLEAN, R.INTEGER);
            Asserts.isNull(planId, "计划");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Plan plan = planService.get(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(plan, "计划");
            Task task = taskService.referer(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(task, "任务");

            int taskCount = taskService.refererCount(corpIds, UserSession.containsCorpId(), planId);
            plan.setTaskCount(taskCount);
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), planId, "plan");
            plan.setTouchCount(touchCount);
            
            Domain domain = domainService.planSum(corpIds, UserSession.containsCorpId(), planId);
            
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), planId);
            
            List<Domain> historyList = Lists.newArrayList();
            domain(corpIds, resultList, historyList);
            
            List<Domain> touchDomainList = null;
            if(touchCount > 0){
            	// 触发
            	List<Task> taskList = taskService.find(planId, "plan");
            	String taskIdStr = "";
            	for(Task t : taskList) {
            		taskIdStr += t.getTaskId() + ",";
            	}
            	
            	Integer[] taskIds = Converts._toIntegers(taskIdStr);
            	List<Setting> settingList = settingService.touchQuery(corpIds, UserSession.containsCorpId(), task.getTemplateId(), taskIds);
            	
            	List<Touch> touchList = touchService.find(taskIds);
            	String touchIdStr = ""; 
            	for(int i=0,len=touchList.size(); i<len; i++) {
            		Touch t = touchList.get(i);
            		touchIdStr += t.getTaskId() + ",";
            	}
                Integer[] touchIds = Converts._toIntegers(touchIdStr);
                if (touchIds != null && touchIds.length > 0) {
                    touchDomainList = domainService.touchGroup(corpIds, UserSession.containsCorpId(), null, touchIds);
                    for (Domain d : touchDomainList) {
                        Domain d2 = domainService.planSum(corpIds,
                                UserSession.containsCorpId(), planId);
                        Integer parentReadUserCount = 0;
                        if (d2 != null) {
                            parentReadUserCount = d2.getReadUserCount();
                        }
                        d.setParentReadUserCount(parentReadUserCount);
                    }
                    Domains.copyUrlsToDomain(settingList, touchDomainList);
                }
            }
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("周期任务.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, (View) request.getAttribute("view"), plan, task);
            CSV.blank(writer);
            
            CSV.name(writer, "#概览#");
            CSV.cover(writer, CSV.PLAN, domain);
            CSV.blank(writer);
            
            CSV.name(writer, "#历史#");
            CSV.history(writer, CSV.PLAN, historyList);
            CSV.blank(writer);
            
            if(touchCount > 0){
             	CSV.name(writer, "#触发#");
            	CSV.touch(writer, touchDomainList, null, "detail");
            	CSV.blank(writer);
            }
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:task) error: ", e);
            return Views._404();
        }
    }
	
	@RequestMapping(value = "/report/export/triger", method = RequestMethod.GET)
	public String triger(HttpServletRequest request, HttpServletResponse response) {
		try {
		    Integer trigerCorpId =  Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
		    Asserts.isNull(trigerCorpId, "触发");
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
			
			Triger triger = trigerService.getApi(corpIds, UserSession.getCorpId(), trigerCorpId);
			Asserts.isNull(triger, "触发");
			Integer[] trigerCorpIds = { triger.getTrigerCorpId() };
			
			Triger task = trigerService.get(trigerCorpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(trigerCorpIds, UserSession.containsCorpId(), taskId);
			Domain domain = domainService.trigerSum(trigerCorpIds, UserSession.containsCorpId(), taskId);
			
			List<Url> urlList = urlService.find(corpIds, UserSession.containsCorpId(), taskId);
			Url sumUrl = urlService.sum(corpIds, UserSession.containsCorpId(), taskId);
			
			List<Domain> domainList = domainService.trigerFind(trigerCorpIds, UserSession.containsCorpId(), taskId);
			Domains.fetchOther(domainList);
			
			List<Region> provinceList = regionService.trigerGroup(trigerCorpIds, UserSession.containsCorpId(), taskId);
			List<Region> cityList = regionService.find(trigerCorpIds, UserSession.containsCorpId(), taskId);
			List<Region> regionList = Regions.combine(provinceList, cityList, "  ");
			
			ServletOutputStream out = Webs.output(response, Encodes.urlEncode("触发任务.csv"));
			CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
			CSV.header(writer, task, result);
			CSV.blank(writer);
			
			CSV.name(writer, "#概览#");
			CSV.cover(writer, CSV.DEFAULT, domain);
			CSV.blank(writer);

			CSV.name(writer, "#发送 #");
			CSV.send(writer, domain);
			CSV.blank(writer);
			CSV.name(writer, "#成功#");
			CSV.send(writer, result, "success");
			CSV.blank(writer);
			CSV.name(writer, "#软弹回#");
			CSV.send(writer, result, "delay");
			CSV.blank(writer);
			CSV.name(writer, "#硬弹回#");
			CSV.send(writer, result, "fail");
			CSV.blank(writer);
			
			CSV.name(writer, "#打开#");
			CSV.open(writer, result);
			CSV.blank(writer);
			
			CSV.name(writer, "#点击#");
			CSV.click(writer, result);
			CSV.blank(writer);
			CSV.name(writer, "#链接#");
			CSV.url(writer, urlList, sumUrl);
			CSV.blank(writer);

			CSV.name(writer, "#域名#");
			CSV.domain(writer, domainList);
			CSV.blank(writer);
			
			CSV.name(writer, "#地域#");
			CSV.region(writer, regionList);
			
			writer.flush();
			writer.close();
			
			return null;
		} catch (Exception e) {
			logger.error("(Export:triger) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "/report/export/campaign", method = RequestMethod.GET)
    public String campaign(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            Domain domain = domainService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), campaignId, "deliveryTime");
            List<Url> urlList = urlService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            Url sumUrl = urlService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Domain> domainList = domainService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            Domains.fetchOther(domainList);
            
            List<Region> provinceList = regionService.campaignGroup(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> cityList = regionService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> regionList = Regions.combine(provinceList, cityList, "  ");
            
            List<Browser> browserList = browserService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Lang> langList = langService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Os> osList = osService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Domain> historyList = Lists.newArrayList();
            domain(corpIds, resultList, historyList);
            
            DateTime now = new DateTime(); 
            MapBean mb = new MapBean();
            List<Domain> domainListByWeek = Lists.newArrayList();
            List<Domain> domainListByMonth = Lists.newArrayList();
            List<Domain> domainListBySeason = Lists.newArrayList();
            
            // 历时
            int week = 0;
            int month = 0;
            int season = 0;
            if (recent != null) {
                Date start = (Date) recent.get("MINDELIVERYTIME");
                if (start == null) start = now.toDate();
                DateTime min = Calendars.parse(new DateTime(start).toString(Calendars.DATE), Calendars.DATE);
                DateTime max = Calendars.parse(now.toString(Calendars.DATE), Calendars.DATE);
                week = Calendars.weeks(min, max);
                month = Calendars.months(min, max);
                season = Calendars.seasons(min, max);
            }
            
            if (week > 1) {
                DateTime start = Calendars.start(now.dayOfWeek());
                DateTime end = Calendars.end(now.dayOfWeek());
                for (int i = 0; i < 2; i++) {
                    Domain d = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                            start.plusWeeks(-i).toString(Calendars.DATE),
                            end.plusWeeks(-i).toString(Calendars.DATE));
                    d.setCompareName(WeekMap.getWeek(i));
                    domainListByWeek.add(d);
                }
            }
            if (month > 1) {
                DateTime start = Calendars.start(now.dayOfMonth());
                DateTime end = Calendars.end(now.dayOfMonth());
                for (int i = 0; i < 2 ; i++) {
                    Domain d = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                            start.plusMonths(-i).toString(Calendars.DATE),
                            end.plusMonths(-i).toString(Calendars.DATE));
                    d.setCompareName(start.plusMonths(-i).toString("yyyy年MM月"));
                    domainListByMonth.add(d);
                }
            }
            if (season > 1) {
                int[] pos = Calendars.seasonPos(now);
                for (int i = 0; i < 2; i++) {
                    int step = (3 * (i + 1));
                    DateTime start = Calendars.start(now.plusMonths(pos[0] - step).dayOfMonth());
                    DateTime end = Calendars.end(now.plusMonths(pos[1] - step).dayOfMonth());
                    Domain d = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                            start.toString(Calendars.DATE),
                            end.toString(Calendars.DATE));
                    d.setCompareName(start.toString("yyyy年") + Calendars.season(start.getMonthOfYear()) + "季度");
                    domainListBySeason.add(d);
                }
            }
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            // 触发
            Map<String, List<Domain>> domainMap = Maps.newHashMap(); // 有活动的触发计划，以任务区分，数据形式：Map<taskName_taskId, domainList>
            Integer[] taskIds = null;
            if(touchCount > 0){
            	List<Task> taskList = taskService.find(campaignId, "campaign");
            	String taskIdStr = "";
                for(Task t : taskList) {
                	taskIdStr += t.getTaskId() + ",";
                }
                
            	taskIds = Converts._toIntegers(taskIdStr);
            	List<Touch> touchList = touchService.find(Converts._toIntegers(taskIdStr));
            	String touchIdStr = ""; 
            	for(int i=0,len=touchList.size(); i<len; i++) {
            		Touch t = touchList.get(i);
            		touchIdStr += t.getTaskId();
            		if(i < len-1){
            			touchIdStr += ",";
            		}
            	}
            	Integer[] touchIds = Converts._toIntegers(touchIdStr);
            	List<Domain> domains = null;
            	if(touchIds!=null && touchIds.length>0){
            		List<Domain> touchDomainList = domainService.touchCampaignGroup(corpIds, UserSession.containsCorpId(), touchIds);
            		for(Domain d : touchDomainList) {
            			String key = d.getTaskName() + "_" + d.getTaskId();
        				domainMap.put(key, null);
            		}
            		for(String k : domainMap.keySet()) {
            			domains = Lists.newArrayList();
            			for(Domain d : touchDomainList) {
                			String key = d.getTaskName() + "_" + d.getTaskId();
                			if(k.equals(key)){
                				Domain d2 = domainService.sum(corpIds, UserSession.containsCorpId(), d.getTaskId());
                	        	Integer parentReadUserCount = 0;
                	        	if(d2 != null){
                	        		parentReadUserCount = d2.getReadUserCount();
                	        	}
                	        	d.setParentReadUserCount(parentReadUserCount);
                				domains.add(d);
                			}
                		}
            			domainMap.put(k, domains);
            		}
            	}
            }
        	
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动任务.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            
            CSV.name(writer, "#概览#");
            CSV.cover(writer, CSV.DEFAULT, domain);
            CSV.blank(writer);
            
            CSV.name(writer, "#成功#");
            if (!Asserts.empty(resultList)) for (Result result : resultList) CSV.send(writer, result, "success");
            CSV.blank(writer);
            
            CSV.name(writer, "#软弹回#");
            if (!Asserts.empty(resultList)) for (Result result : resultList) CSV.send(writer, result, "delay");
            CSV.blank(writer);
            
            CSV.name(writer, "#硬弹回#");
            if (!Asserts.empty(resultList)) for (Result result : resultList) CSV.send(writer, result, "fail");
            CSV.blank(writer);
            
            CSV.name(writer, "#打开#");
            if (!Asserts.empty(resultList)) for (Result result : resultList) CSV.open(writer, result);
            CSV.blank(writer);
            
            CSV.name(writer, "#点击#");
            if (!Asserts.empty(resultList)) for (Result result : resultList) CSV.click(writer, result);
            CSV.blank(writer);
            
            CSV.name(writer, "#链接#");
            CSV.url(writer, urlList, sumUrl);
            CSV.blank(writer);

            CSV.name(writer, "#域名#");
            CSV.domain(writer, domainList);
            CSV.blank(writer);

            CSV.name(writer, "#地域#");
            CSV.region(writer, regionList);
            CSV.blank(writer);

            CSV.name(writer, "#终端#");
            CSV.client(writer, browserList, langList, osList);
            CSV.blank(writer);

            CSV.name(writer, "#历史#");
            CSV.history(writer, CSV.DEFAULT, historyList);
            CSV.blank(writer);

            CSV.name(writer, "#最近2周#");
            CSV.compare(writer, domainListByWeek);
            CSV.blank(writer);
            CSV.name(writer, "#最近2个月#");
            CSV.compare(writer, domainListByMonth);
            CSV.blank(writer);
            CSV.name(writer, "#最近2个季度#");
            CSV.compare(writer, domainListBySeason);
            
            if(touchCount > 0){
            	CSV.blank(writer);
	            CSV.name(writer, "#触发#");
	            CSV.touch(writer, domainMap, taskIds);
            }
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:campaign) error: ", e);
            return Views._404();
        }
    }
	
	private void domain(Integer[] corpIds, List<Result> resultList, List<Domain> domainList) {
        StringBuffer sbff = new StringBuffer();
        for (Result r : resultList) {
            sbff.append(r.getTaskId()).append(",");
        }
        
        List<Domain> list = domainService.group(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
        Map<Integer, Domain> map = Maps.newHashMap();
        for (Domain d : list) {
            map.put(d.getTaskId(), d);
        }
        
        for (Result r : resultList) {
            Domain d = map.get(r.getTaskId());
            if (d == null) d = new Domain();
            d.setTaskName(r.getTaskName());
            d.setStatus(r.getStatus());
            d.setDeliveryTime(r.getDeliveryTime());
            domainList.add(d);
        }
    }
}
