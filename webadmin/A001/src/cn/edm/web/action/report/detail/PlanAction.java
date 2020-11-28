package cn.edm.web.action.report.detail;

import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Plan;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

@Controller("ReportDetailPlan")
@RequestMapping("/report/detail/plan/**")
public class PlanAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(PlanAction.class);

    @RequestMapping(method = RequestMethod.GET)
    public String cover(HttpServletRequest request, ModelMap map) {
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
            
            map.put("plan", plan);
            map.put("task", task);
            map.put("domain", domain);
            
            return "default/report/detail/plan_cover";
        } catch (Exception e) {
            logger.error("(Plan:cover) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String history(HttpServletRequest request, ModelMap map) {
        try {
            Page<Result> page = new Page<Result>();
            Pages.page(request, page);
            
            Integer planId = Https.getInt(request, "planId", R.CLEAN, R.INTEGER);
            Asserts.isNull(planId, "计划");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Plan plan = planService.get(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(plan, "计划");
            Task task = taskService.referer(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(task, "任务");
            
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), planId, "plan");
            plan.setTouchCount(touchCount);
            
            MapBean mb = new MapBean();
            page = resultService.search(page, mb, corpIds, UserSession.containsCorpId(), planId);
            
            StringBuffer sbff = new StringBuffer();
            for (Result result : page.getResult()) {
                sbff.append(result.getTaskId()).append(",");
            }
            List<Domain> domainList = domainService.group(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
            Domains.copy("task", domainList, page.getResult());

            map.put("plan", plan);
            map.put("task", task);
            map.put("page", page);
            map.put("mb", mb);
            
            request.setAttribute("location", Webs.location(request));
            
            return "default/report/detail/plan_history";
        } catch (Exception e) {
            logger.error("(Plan:history) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String touch(HttpServletRequest request, ModelMap map) {
        try {
            Page<Result> page = new Page<Result>();
            Pages.page(request, page);
            
            Integer planId = Https.getInt(request, "planId", R.CLEAN, R.INTEGER);
            Asserts.isNull(planId, "计划");
            Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Plan plan = planService.get(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(plan, "计划");
            int taskCount = taskService.refererCount(corpIds, UserSession.containsCorpId(), planId);
            plan.setTaskCount(taskCount);
            int touchCount = taskService.touchCount(corpIds, UserSession.containsCorpId(), planId, "plan");
            plan.setTouchCount(touchCount);
            Task task = taskService.referer(corpIds, UserSession.containsCorpId(), planId);
            Asserts.isNull(task, "任务");
            
            List<Task> taskList = taskService.find(planId, "plan");
            String taskIdStr = "";
            for(Task t : taskList) {
            	taskIdStr += t.getTaskId() + ",";
            }
            
            Integer[] taskIds = Converts._toIntegers(taskIdStr);
        	List<Setting> settingList = settingService.touchQuery(corpIds, UserSession.containsCorpId(), templateId, taskIds);
        	
        	List<Touch> touchList = touchService.find(taskIds);
        	String touchIdStr = ""; 
        	for(int i=0,len=touchList.size(); i<len; i++) {
        		Touch t = touchList.get(i);
        		touchIdStr += t.getTaskId() + ",";
        	}
        	Integer[] touchIds = Converts._toIntegers(touchIdStr);
        	List<Domain> domainList = null;
        	if(touchIds!=null && touchIds.length>0){
        		domainList = domainService.touchGroup(corpIds, UserSession.containsCorpId(), null, touchIds);
        		for(Domain d : domainList) {
        			Domain domain = domainService.planSum(corpIds, UserSession.containsCorpId(), planId);
        			Integer parentReadUserCount = 0;
                	if(domain != null){
                		parentReadUserCount = domain.getReadUserCount();
                	}
                	d.setParentReadUserCount(parentReadUserCount);
        		}
        		Domains.copyUrlsToDomain(settingList, domainList);
        	}
        	
        	map.put("task", task);
        	map.put("plan", plan);
        	map.put("settingList", settingList);
        	map.put("domainList", domainList);
            
            return "default/report/detail/plan_touch";
        } catch (Exception e) {
            logger.error("(Plan:history) error: ", e);
            return Views._404();
        }
    }
    
}
