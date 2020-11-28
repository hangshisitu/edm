package cn.edm.web.action.report.export;

import java.io.OutputStreamWriter;
import java.util.List;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.consts.Value;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Plan;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.View;
import cn.edm.web.action.Action;
import cn.edm.web.facade.CSV;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

@Controller("ReportExportPlan")
@RequestMapping("/report/export/plan/**")
public class PlanAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(PlanAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String total(HttpServletRequest request, HttpServletResponse response) {
        try {
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^[0-9,]+$:不是合法值,checkedIds");
            
            MapBean mb = new MapBean();
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Domain domain = domainService.planSum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(checkedIds));
            
            List<Result> resultList = resultService.referers(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(checkedIds));
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("周期-汇总.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            StringBuffer sbff = new StringBuffer();
            for (Result result : resultList) {
                sbff.append(result.getPlanId()).append(",");
            }
            Integer[] taskIds = Converts._toIntegers(sbff.toString());

            List<Domain> domainList = domainService.group(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), taskIds);

            Result result = new Result();
            result.setTaskId(Value.I);
            result.setTaskName("合计");
            resultList.add(result);
            domain.setTaskId(Value.I);
            domainList.add(domain);
            
            CSV.total(writer, resultList, domainList);
            
            writer.flush();
            writer.close();

            return null;
        } catch (Exception e) {
            logger.error("(Export:total) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value="/touch", method=RequestMethod.GET)
    public String touch(HttpServletRequest request, HttpServletResponse response) {
        try {
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
            
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("周期任务_触发.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            CSV.header(writer, (View) request.getAttribute("view"), plan, task);
            CSV.blank(writer);
        	
            CSV.blank(writer);
            CSV.touch(writer, domainList, null, "detail");
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:task) error: ", e);
            return Views._404();
        }
    }
}
