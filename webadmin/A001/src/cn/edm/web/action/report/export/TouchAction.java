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
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.modules.utils.Encodes;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.action.report.TaskAction;
import cn.edm.web.facade.CSV;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

@Controller("ReportExportTouch")
@RequestMapping("/report/export/touch/**")
public class TouchAction extends Action {
	
	private static final Logger logger = LoggerFactory.getLogger(TaskAction.class);

	@RequestMapping(value = "/cover", method = RequestMethod.GET)
    public String cover(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer touchId = Https.getInt(request, "touchId", R.CLEAN, R.INTEGER);
            Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Setting setting = settingService.get(corpIds, UserSession.containsCorpId(), templateId, touchId);
            Asserts.isNull(setting, "模板设置");
            
            List<Domain> domainList = domainService.touchSum(corpIds, UserSession.containsCorpId(), templateId);
            
            Integer totalParentReadUserCount = 0;
            for(Domain d : domainList) {
            	Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), d.getTaskId());
            	Integer parentReadUserCount = 0;
            	if(domain != null){
            		parentReadUserCount = domain.getReadUserCount();
            	}
            	d.setParentReadUserCount(parentReadUserCount);
            	totalParentReadUserCount += parentReadUserCount;
            }
            Domain totalDomain = domainService.touchTotal(corpIds, UserSession.containsCorpId(), templateId);
            if(totalDomain != null){
            	totalDomain.setParentReadUserCount(totalParentReadUserCount);
            	int touchCount = touchService.touchSum(corpIds, UserSession.containsCorpId(), templateId);
            	totalDomain.setTouchCount(touchCount);
            }
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("触发计划报告.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            CSV.touchSetting(writer, setting);
            CSV.blank(writer);
            
            CSV.name(writer, "各任务触发占比");
            CSV.touch(writer, domainList, null, "touchCount");
            CSV.blank(writer);
            
            CSV.name(writer, "各任务打开占比");
            CSV.touch(writer, domainList, null, "readCount");
            
            CSV.blank(writer);
            CSV.touch(writer, domainList, totalDomain, null);
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:task) error: ", e);
            return Views._404();
        }
    }

	@RequestMapping(value = "/touch", method = RequestMethod.GET)
    public String touch(HttpServletRequest request, HttpServletResponse response) {
        try {
        	
        	Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(taskId, "任务");
        	Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(templateId, "模板");
        	
        	Task task = taskService.get(taskId);
        	Asserts.isNull(task, "任务");
        	
        	List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
                    	
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
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("触发计划报告.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            CSV.header(writer, task, result);
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
	/**
	 * 导出收件人详情
	 */
	@RequestMapping(method = RequestMethod.GET)
	public String recipients(HttpServletRequest request, HttpServletResponse response) {
        try {
        	Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(taskId, "触发计划");
        	
        	List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
        	Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
        	Asserts.isNull(templateId, "模板");
        	
        	Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId, templateId);
        	
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("触发计划报告_收件人详情.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            CSV.touch(writer, result);
            
            writer.flush();
            writer.close();
            
            return null;
        } catch (Exception e) {
            logger.error("(Export:task) error: ", e);
            return Views._404();
        }
	}
	
}
