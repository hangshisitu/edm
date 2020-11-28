package cn.edm.web.action.report.detail;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Value;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Hour;
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.model.Url;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Counts;
import cn.edm.web.facade.Domains;
import cn.edm.web.facade.Regions;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("ReportDetailTask")
@RequestMapping("/report/detail/task/**")
public class TaskAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(TaskAction.class);

    @RequestMapping(method = RequestMethod.GET)
    public String cover(HttpServletRequest request, ModelMap map) {
        try {
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");

            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
            Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), taskId);

            map.put("task", task);
            map.put("result", result);
            map.put("domain", domain);
            
            return "default/report/detail/task_cover";
        } catch (Exception e) {
            logger.error("(Task:cover) error: ", e);
            return Views._404();
        }
    }
    
	@RequestMapping(method = RequestMethod.GET)
	public String send(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);

			Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), taskId);
			
			map.put("task", task);
			map.put("result", result);
			map.put("domain", domain);
			
			return "default/report/detail/task_send";
		} catch (Exception e) {
			logger.error("(Task:send) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String open(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
			
			List<Hour> hourList = hourService.find(corpIds, UserSession.containsCorpId(), taskId);
			
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
			
			map.put("task", task);
			map.put("result", result);
			map.put("hourList", hourList);
			map.put("hourMap", hourMap);
			
			return "default/report/detail/task_open";
		} catch (Exception e) {
			logger.error("(Task:open) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String click(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
			
			List<Hour> hourList = hourService.find(corpIds, UserSession.containsCorpId(), taskId);
			
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
			
			List<Url> urlList = urlService.find(corpIds, UserSession.containsCorpId(), taskId);
			Url sumUrl = urlService.sum(corpIds, UserSession.containsCorpId(), taskId);
			
			map.put("urlList", urlList);
			map.put("sumUrl", sumUrl);
			
			map.put("task", task);
			map.put("result", result);
			map.put("hourList", hourList);
			map.put("hourMap", hourMap);
			
			return "default/report/detail/task_click";
		} catch (Exception e) {
			logger.error("(Task:click) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String domain(HttpServletRequest request, ModelMap map) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
			
			List<Domain> domainList = domainService.find(corpIds, UserSession.containsCorpId(), taskId);
			String top5 = Converts.toString(domainService.top5(corpIds, UserSession.containsCorpId(), taskId));
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
			
			map.put("task", task);
			map.put("result", result);
			map.put("domainList", domainList);
			map.put("openList", openList);
			map.put("clickList", clickList);
			
			return "default/report/detail/task_domain";
		} catch (Exception e) {
			logger.error("(Task:domain) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String region(HttpServletRequest request, ModelMap map) {
		try {
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
			Asserts.isNull(task, "任务");
			Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);

			List<Region> provinceList = regionService.group(corpIds, UserSession.containsCorpId(), taskId);
			List<Region> cityList = regionService.find(corpIds, UserSession.containsCorpId(), taskId);
			List<Region> regionList = Regions.combine(provinceList, cityList, "-- ");
			
			map.put("task", task);
			map.put("result", result);
			map.put("provinceList", provinceList);
			map.put("regionList", regionList);
			
			return "default/report/detail/task_region";
		} catch (Exception e) {
			logger.error("(Task:region) error: ", e);
			return Views._404();
		}
	}
	
    @RequestMapping(method = RequestMethod.GET)
    public String touch(HttpServletRequest request, ModelMap map) {
        try {
        	Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(taskId, "任务");
        	Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(templateId, "模板");
        	
        	Task task = taskService.get(taskId);
        	Asserts.isNull(task, "任务");
        	
        	List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
        	
        	int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), taskId, "task");
        	task.setTouchCount(touchCount);
        	
        	Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
        	
        	List<Setting> settingList = settingService.touchQuery(corpIds, UserSession.containsCorpId(), templateId, taskId);
        	
        	List<Touch> touchList = touchService.find(taskId);
        	
        	String taskIdStr = ""; 
        	for(int i=0,len=touchList.size(); i<len; i++) {
        		Touch t = touchList.get(i);
        		taskIdStr += t.getTaskId() + ",";
        	}
        	Integer[] taskIds = Converts._toIntegers(taskIdStr);
        	List<Domain> domainList = null;
        	if(taskIds!=null && taskIds.length>0){
        		domainList = domainService.touchGroup(corpIds, UserSession.containsCorpId(), null, taskIds);
        		for(Domain d : domainList) {
        			Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), d.getTaskId());
        			Integer parentReadUserCount = 0;
                	if(domain != null){
                		parentReadUserCount = domain.getReadUserCount();
                	}
                	d.setParentReadUserCount(parentReadUserCount);
        		}
    			Domains.copyUrlsToDomain(settingList, domainList);
        	}
        	
        	map.put("task", task);
        	map.put("result", result);
        	map.put("settingList", settingList);
        	map.put("domainList", domainList);
            return "default/report/detail/task_touch";
        } catch (Exception e) {
            logger.error("(Task:touch) error: ", e);
            return Views._404();
        }
    }
}
