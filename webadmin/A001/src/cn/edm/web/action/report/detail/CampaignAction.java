package cn.edm.web.action.report.detail;

import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.BrowserMap;
import cn.edm.consts.mapper.LangMap;
import cn.edm.consts.mapper.OsMap;
import cn.edm.consts.mapper.WeekMap;
import cn.edm.model.Browser;
import cn.edm.model.Campaign;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Hour;
import cn.edm.model.Lang;
import cn.edm.model.Os;
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validates;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Counts;
import cn.edm.web.facade.Domains;
import cn.edm.web.facade.Regions;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("ReportDetailCampaign")
@RequestMapping("/report/detail/campaign/**")
public class CampaignAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(CampaignAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String cover(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            Domain domain = domainService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("domain", domain);
            
            return "default/report/detail/campaign_cover";
        } catch (Exception e) {
            logger.error("(Campaign:cover) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String send(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            Domain domain = domainService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("domain", domain);
            
            return "default/report/detail/campaign_send";
        } catch (Exception e) {
            logger.error("(Campaign:cover) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String open(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Hour> hourList = hourService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            
            Map<String, Integer> hourMap = Maps.newLinkedHashMap();
            for (int i = 0; i < 24; i++) {
                hourMap.put(String.valueOf(i), Value.I);
                if (Asserts.empty(hourList)) {
                    continue;
                }
                for (Hour hour : hourList) {
                    if (hour.getHour() == i) {
                        hourMap.put(String.valueOf(i), hour.getReadCount());
                        break;
                    }
                }
            }
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("hourList", hourList);
            map.put("hourMap", hourMap);
            
            return "default/report/detail/campaign_open";
        } catch (Exception e) {
            logger.error("(Campaign:open) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String click(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Hour> hourList = hourService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            
            Map<String, Integer> hourMap = Maps.newLinkedHashMap();
            for (int i = 0; i < 24; i++) {
                hourMap.put(String.valueOf(i), Value.I);
                if (Asserts.empty(hourList)) {
                    continue;
                }
                for (Hour hour : hourList) {
                    if (hour.getHour() == i) {
                        hourMap.put(String.valueOf(i), hour.getClickCount());
                        break;
                    }
                }
            }
            
            List<Url> urlList = urlService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            Url sumUrl = urlService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("hourList", hourList);
            map.put("hourMap", hourMap);
            map.put("urlList", urlList);
            map.put("sumUrl", sumUrl);
            
            return "default/report/detail/campaign_click";
        } catch (Exception e) {
            logger.error("(Campaign:click) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String domain(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Domain> domainList = domainService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            String top5 = Converts.toString(domainService.campaignTop5(corpIds, UserSession.containsCorpId(), campaignId));
            String top6 = top5+"other";
            List<Domain> openList = Lists.newArrayList();
            List<Domain> clickList = Lists.newArrayList();
            
            Domain otherSum = new Domain();
            Domain other = null;
            for (Domain domain : domainList) {
                /*if (StringUtils.contains(top5, domain.getEmailDomain())) {*/
            	if (StringUtils.contains(top6, domain.getEmailDomain())) {
                    if (domain.getReadUserCount() > 0) {
                        openList.add(domain);
                    }
                    if (domain.getClickUserCount() > 0) {
                        clickList.add(domain);
                    }
                } else {
                    Counts.minus(domain, otherSum);
                }
                if (StringUtils.equals(domain.getEmailDomain(), "other")) {
                    other = domain;
                }
            }
            
            if (other != null) {
                domainList.remove(other);
                domainList.add(other);
            }
            
            if (StringUtils.isNotBlank(otherSum.getEmailDomain())) {
                if (otherSum.getReadUserCount() > 0) {
                    openList.add(otherSum);
                }
                if (otherSum.getClickUserCount() > 0) {
                    clickList.add(otherSum);
                }
            }
            
            Collections.sort(openList, new Comparator<Domain>() {
                @Override
                public int compare(Domain d1, Domain d2) {
                    int p1 = d1.getSentCount() == 0 ? 0 : d1.getReadUserCount() * 100 / d1.getSentCount();
                    int p2 = d2.getSentCount() == 0 ? 0 : d2.getReadUserCount() * 100 / d2.getSentCount();
                    return p1 < p2 ? 1 : -1;
                }
            });
            Collections.sort(clickList, new Comparator<Domain>() {
                @Override
                public int compare(Domain d1, Domain d2) {
                    int p1 = d1.getSentCount() == 0 ? 0 : d1.getClickUserCount() * 100 / d1.getSentCount();
                    int p2 = d2.getSentCount() == 0 ? 0 : d2.getClickUserCount() * 100 / d2.getSentCount();
                    return p1 < p2 ? 1 : -1;
                }
            });

            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("domainList", domainList);
            map.put("openList", openList);
            map.put("clickList", clickList);
            
            return "default/report/detail/campaign_domain";
        } catch (Exception e) {
            logger.error("(Campaign:domain) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String region(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Region> provinceList = regionService.campaignGroup(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> cityList = regionService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> regionList = Regions.combine(provinceList, cityList, "-- ");
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("provinceList", provinceList);
            map.put("regionList", regionList);
            
            return "default/report/detail/campaign_region";
        } catch (Exception e) {
            logger.error("(Campaign:region) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String client(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);

            List<Browser> browserList = browserService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Lang> langList = langService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Os> osList = osService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("browserList", browserList);
            map.put("langList", langList);
            map.put("osList", osList);
            map.put("noneBrowserMap", BrowserMap.none(browserList));
            map.put("noneLangMap", LangMap.none(langList));
            map.put("noneOsMap", OsMap.none(osList));
            
            return "default/report/detail/campaign_client";
        } catch (Exception e) {
            logger.error("(Campaign:client) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String history(HttpServletRequest request, ModelMap map) {
        try {
            Page<Result> page = new Page<Result>();
            Pages.page(request, page);
            Integer[] statuses = { Status.CANCELLED, Status.COMPLETED };
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);

            String orderBy = Https.getStr(request, "orderBy", R.CLEAN, R.REGEX, "regex:^deliveryTime|sentCount|reachCount|readUserCount|clickUserCount$");
            orderBy = Values.get(orderBy, "deliveryTime");
            
            MapBean mb = new MapBean();
            page = resultService.search(page, mb, corpIds, UserSession.containsCorpId(),
                    campaignId, statuses, orderBy);
            
            StringBuffer sbff = new StringBuffer();
            for (Result result : page.getResult()) {
                sbff.append(result.getTaskId()).append(",");
            }
            List<Domain> domainList = domainService.group(corpIds, UserSession.containsCorpId(), null, Converts._toIntegers(sbff.toString()));
            Domains.copy("task", domainList, page.getResult());

            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("page", page);
            map.put("mb", mb);
            
            request.setAttribute("location", Webs.location(request));
            
            return "default/report/detail/campaign_history";
        } catch (Exception e) {
            logger.error("(Campaign:history) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String compare(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^week|month|season$");
            m = Values.get(m, "week");

            String recents = Https.getStr(request, "recents", R.CLEAN, R.LENGTH, "{1,1}");
            String checkeds = Https.getStr(request, "checkeds", R.CLEAN, R.LENGTH, R.REGEX, "{1,60}", "regex:^[0-9,]+$");

            DateTime now = new DateTime(); 
            MapBean mb = new MapBean();
            List<Domain> domainList = Lists.newArrayList();
            
            // 历时
            int week = 0;
            int month = 0;
            int season = 0;
            if (recent != null) {
                Date start = (Date) recent.get("minDeliveryTime");
                if (start == null) start = now.toDate();
                DateTime min = Calendars.parse(new DateTime(start).toString(Calendars.DATE), Calendars.DATE);
                DateTime max = Calendars.parse(now.toString(Calendars.DATE), Calendars.DATE);
                week = Calendars.weeks(min, max);
                month = Calendars.months(min, max);
                season = Calendars.seasons(min, max);
            }

            String last = "-";
            if (m.equals("week")) {
                if (week > 1) {
                    Validator.validate(recents, R.REGEX, "regex:^2|3|4$");
                    recents = Values.get(recents, "2");
                    DateTime start = Calendars.start(now.dayOfWeek());
                    DateTime end = Calendars.end(now.dayOfWeek());
                    for (int i = 0; i < Integer.valueOf(recents); i++) {
                        Domain domain = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                                start.plusWeeks(-i).toString(Calendars.DATE),
                                end.plusWeeks(-i).toString(Calendars.DATE));
                        domain.setCompareName(WeekMap.getWeek(i));
                        domainList.add(domain);
                    }
                }
                last = week + "周";
            } else if (m.equals("month")) {
                if (month > 1) {
                    if (StringUtils.isNotBlank(recents)) {
                        Validator.validate(recents, R.REGEX, "regex:^2|3|4$");
                        recents = Values.get(recents, "2");
                        DateTime start = Calendars.start(now.dayOfMonth());
                        DateTime end = Calendars.end(now.dayOfMonth());
                        for (int i = 0; i < Integer.valueOf(recents); i++) {
                            Domain domain = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                                    start.plusMonths(-i).toString(Calendars.DATE),
                                    end.plusMonths(-i).toString(Calendars.DATE));
                            domain.setCompareName(start.plusMonths(-i).toString("yyyy年MM月"));
                            domainList.add(domain);
                        }
                    } else {
                        String[] months = Converts._toStrings(Converts.repeat(checkeds));
                        if (Asserts.empty(months) || months.length < 2 || months.length > 4) {
                            throw new Errors("任务集合范围只能在2至4之间");
                        }
                        String fmt = "yyyyMM";
                        for (String e : months) {
                            if (!Validates.formatter(e, fmt)) {
                                continue;
                            }
                            DateTime datetime = Calendars.parse(e, fmt);
                            Domain domain = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                                    Calendars.start(datetime.dayOfMonth()).toString(Calendars.DATE),
                                    Calendars.end(datetime.dayOfMonth()).toString(Calendars.DATE));
                            domain.setCompareName(datetime.toString("yyyy年MM月"));
                            domainList.add(domain);
                        }
                    }
                }
                last = month + "个月";
            } else if (m.equals("season")) {
                if (season > 1) {
                    if (StringUtils.isNotBlank(recents)) {
                        Validator.validate(recents, R.REGEX, "regex:^2|3|4$");
                        recents = Values.get(recents, "2");
                        int[] pos = Calendars.seasonPos(now);
                        for (int i = 0; i < Integer.valueOf(recents); i++) {
                            int step = (3 * (i + 1));
                            DateTime start = Calendars.start(now.plusMonths(pos[0] - step).dayOfMonth());
                            DateTime end = Calendars.end(now.plusMonths(pos[1] - step).dayOfMonth());
                            Domain domain = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                                    start.toString(Calendars.DATE),
                                    end.toString(Calendars.DATE));
                            domain.setCompareName(start.toString("yyyy年") + Calendars.season(start.getMonthOfYear()) + "季度");
                            domainList.add(domain);
                        }
                    } else {
                        String[] seasons = Converts._toStrings(Converts.repeat(checkeds));
                        if (Asserts.empty(seasons) || seasons.length < 2 || seasons.length > 4) {
                            throw new Errors("任务集合范围只能在2至4之间");
                        }
                        for (String e : seasons) {
                            if (!Validates.formatter(e.substring(0, 4), "yyyy") || !Validates.matches(e.substring(4), "regex:^0|1|2|3|4$")) {
                                continue;
                            }
                            DateTime datetime = new DateTime()
                                .withYear(Integer.valueOf(e.substring(0, 4)))
                                .withMonthOfYear(Integer.valueOf(e.substring(4)) * 3);
                            int[] pos = Calendars.seasonPos(datetime);
                            DateTime start = Calendars.start(datetime.withMonthOfYear(pos[0]).dayOfMonth());
                            DateTime end = Calendars.end(datetime.withMonthOfYear(pos[1]).dayOfMonth());
                            Domain domain = domainService.campaignSum(mb, corpIds, UserSession.containsCorpId(), campaignId,
                                    start.toString(Calendars.DATE),
                                    end.toString(Calendars.DATE));
                            domain.setCompareName(start.toString("yyyy年") + e.substring(4) + "季度");
                            domainList.add(domain);
                        }
                    }
                }
                last = season + "个季度";
            }
            
            map.put("campaign", campaign);
            map.put("recent", recent);
            map.put("domainList", domainList);
            map.put("m", m);
            map.put("recents", recents);
            map.put("checkeds", checkeds);
            map.put("last", last);
            
            return "default/report/detail/campaign_compare";
        } catch (Exception e) {
            logger.error("(Campaign:compare) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String touch(HttpServletRequest request, ModelMap map) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), campaignId, "campaign");
            campaign.setTouchCount(touchCount);
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Task> taskList = taskService.find(campaignId, "campaign");
            String taskIdStr = "";
            for(Task t : taskList) {
            	taskIdStr += t.getTaskId() + ",";
            }
            Integer[] taskIds = Converts._toIntegers(taskIdStr);
        	
        	List<Touch> touchList = touchService.find(Converts._toIntegers(taskIdStr));
        	String touchIdStr = ""; 
        	for(int i=0,len=touchList.size(); i<len; i++) {
        		Touch t = touchList.get(i);
        		touchIdStr += t.getTaskId() + ",";
        	}
        	Integer[] touchIds = Converts._toIntegers(touchIdStr);
        	List<Domain> domainList = null;
        	Map<String, List<Domain>> domainMap = Maps.newHashMap(); // 有活动的触发计划，以任务区分，数据形式：Map<taskName_taskId, domainList>
        	List<Domain> domains = null;
        	if(touchIds!=null && touchIds.length>0){
        		domainList = domainService.touchCampaignGroup(corpIds, UserSession.containsCorpId(), touchIds);
        		for(Domain d : domainList) {
        			String key = d.getTaskName() + "_" + d.getTaskId();
    				domainMap.put(key, null);
        		}
        		for(String k : domainMap.keySet()) {
        			domains = Lists.newArrayList();
        			for(Domain d : domainList) {
            			String key = d.getTaskName() + "_" + d.getTaskId();
            			if(k.equals(key)){
            				Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), d.getTaskId());
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
        	
        	map.put("domainList", domainList);
        	map.put("domainMap", domainMap);
        	map.put("taskIds", taskIds);
            map.put("campaign", campaign);
            map.put("recent", recent);
            
            return "default/report/detail/campaign_touch";
        } catch (Exception e) {
            logger.error("(Campaign:cover) error: ", e);
            return Views._404();
        }
    }
    
}
