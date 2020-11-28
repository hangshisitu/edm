package cn.edm.web.action.report.export;

import java.io.OutputStreamWriter;
import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.consts.mapper.WeekMap;
import cn.edm.model.Browser;
import cn.edm.model.Campaign;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Lang;
import cn.edm.model.Os;
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validates;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.CSV;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;
import cn.edm.web.facade.Regions;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("ReportExportCampaign")
@RequestMapping("/report/export/campaign/**")
public class CampaignAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(CampaignAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String cover(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            Domain domain = domainService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-概览.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.cover(writer, CSV.DEFAULT, domain);

            writer.flush();
            writer.close();

            return null;
        } catch (Exception e) {
            logger.error("(Export:cover) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String send(HttpServletRequest request, HttpServletResponse response) {
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
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-发送.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.send(writer, domain);
            
            CSV.blank(writer);
            CSV.name(writer, "#成功#");
            if (!Asserts.empty(resultList)) {
                for (Result result : resultList) {
                    CSV.send(writer, result, "success");
                }
                CSV.blank(writer);
            }
            CSV.name(writer, "#软弹回#");
            if (!Asserts.empty(resultList)) {
                for (Result result : resultList) {
                    CSV.send(writer, result, "delay");
                }
                CSV.blank(writer);
            }
            CSV.name(writer, "#硬弹回#");
            if (!Asserts.empty(resultList)) {
                for (Result result : resultList) {
                    CSV.send(writer, result, "fail");
                }
            }
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:send) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String open(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), campaignId, "deliveryTime");
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-打开.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            if (!Asserts.empty(resultList)) {
                for (Result result : resultList) {
                    CSV.open(writer, result);
                }
            }
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:open) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String click(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), campaignId, "deliveryTime");
            
            List<Url> urlList = urlService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            Url sumUrl = urlService.campaignSum(corpIds, UserSession.containsCorpId(), campaignId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-点击.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            if (!Asserts.empty(resultList)) {
                for (Result result : resultList) {
                    CSV.click(writer, result);
                }
                CSV.blank(writer);
            }
            CSV.url(writer, urlList, sumUrl);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:click) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String domain(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Domain> domainList = domainService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            Domains.fetchOther(domainList);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-域名.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.domain(writer, domainList);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:domain) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(method = RequestMethod.GET)
    public String region(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Region> provinceList = regionService.campaignGroup(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> cityList = regionService.campaignFind(corpIds, UserSession.containsCorpId(), campaignId);
            List<Region> regionList = Regions.combine(provinceList, cityList, "  ");
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-地域.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.region(writer, regionList);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:region) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String client(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);

            List<Browser> browserList = browserService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Lang> langList = langService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            List<Os> osList = osService.sum(corpIds, UserSession.containsCorpId(), campaignId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-终端.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.client(writer, browserList, langList, osList);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Campaign:client) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String history(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);

            String orderBy = Https.getStr(request, "orderBy", R.CLEAN, R.REGEX, "regex:^deliveryTime|sentCount|reachCount|readUserCount|clickUserCount$");
            orderBy = Values.get(orderBy, "deliveryTime");
            
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), campaignId, orderBy);
            List<Domain> domainList = Lists.newArrayList();
            domain(corpIds, UserSession.containsCorpId(), resultList, domainList);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-历史.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            CSV.history(writer, CSV.DEFAULT, domainList);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Campaign:history) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String compare(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^week|month|season$");
            m = Values.get(m, "week");
            
            String recents = Https.getStr(request, "recents", R.CLEAN, R.LENGTH, "{1,1}");
            String checkeds = Https.getStr(request, "checkeds", R.CLEAN, R.LENGTH, R.REGEX, "{1,60}", "regex:^[0-9,]+$");
            
            DateTime now = new DateTime(); 
            MapBean mb = new MapBean();
            List<Domain> weekList = Lists.newArrayList();
            List<Domain> monthList = Lists.newArrayList();
            List<Domain> seasonList = Lists.newArrayList();
            
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
                        weekList.add(domain);
                    }
                }
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
                            monthList.add(domain);
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
                            monthList.add(domain);
                        }
                    }
                }
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
                            seasonList.add(domain);
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
                            seasonList.add(domain);
                        }
                    }
                }
            }
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-对比.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            
            if (m.equals("week")) {
                CSV.compare(writer, weekList);
            } else if (m.equals("month")) {
                CSV.compare(writer, monthList);
            } else if (m.equals("season")) {
                CSV.compare(writer, seasonList);
            }
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Campaign:history) error: ", e);
            return Views._404();
        }
    }
    
    private void domain(Integer[] corpIds, Integer corpId, List<Result> resultList, List<Domain> domainList) {
        StringBuffer sbff = new StringBuffer();
        for (Result r : resultList) {
            sbff.append(r.getTaskId()).append(",");
        }
        
        List<Domain> list = domainService.group(corpIds, corpId, null, Converts._toIntegers(sbff.toString()));
        Map<Integer, Domain> map = Maps.newHashMap();
        for (Domain d : list) {
            map.put(d.getTaskId(), d);
        }
        
        for (Result r : resultList) {
            Domain d = map.get(r.getTaskId());
            if (d == null) d = new Domain();
            d.setTaskName(r.getTaskName());
            d.setDeliveryTime(r.getDeliveryTime());
            domainList.add(d);
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String touch(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Campaign campaign = campaignService.get(corpIds, UserSession.containsCorpId(), campaignId);
            Asserts.isNull(campaign, "活动");
            MapBean recent = resultService.recent(corpIds, UserSession.containsCorpId(), campaignId);
            
            List<Task> taskList = taskService.find(campaignId, "campaign");
            String taskIdStr = "";
            for(Task t : taskList) {
            	taskIdStr += t.getTaskId() + ",";
            }
            Integer[] taskIds = Converts._toIntegers(taskIdStr);
        	List<Touch> touchList = touchService.find(taskIds);
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
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("活动-触发.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, campaign, recent);
            CSV.blank(writer);
            
            CSV.touch(writer, domainMap, taskIds);

            writer.flush();
            writer.close();

            return null;
        } catch (Exception e) {
            logger.error("(Export:cover) error: ", e);
            return Views._404();
        }
    }
    
}
