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
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Task;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.CSV;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;
import cn.edm.web.facade.Regions;

@Controller("ReportExportTask")
@RequestMapping("/report/export/task/**")
public class TaskAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(TaskAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String total(HttpServletRequest request, HttpServletResponse response) {
        try {
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^[0-9,]+$:不是合法值,checkedIds");
            
            MapBean mb = new MapBean();
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Domain domain = domainService.sum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(checkedIds));
            
            List<Result> resultList = resultService.find(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), Converts._toIntegers(checkedIds));
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-汇总.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            
            StringBuffer sbff = new StringBuffer();
            for (Result result : resultList) {
                sbff.append(result.getTaskId()).append(",");
            }
            Integer[] taskIds = Converts._toIntegers(sbff.toString());

            List<Domain> domainList = domainService.group(corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), taskIds);

            Result result = new Result();
            result.setTaskId(0);
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
    
    @RequestMapping(method = RequestMethod.GET)
    public String cover(HttpServletRequest request, HttpServletResponse response) {
        try {
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");

            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
            Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), taskId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-概览.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
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
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");

            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");
            
            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
            Domain domain = domainService.sum(corpIds, UserSession.containsCorpId(), taskId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-发送.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
            CSV.blank(writer);
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
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");
            
            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-打开.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
            CSV.blank(writer);
            CSV.open(writer, result);

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
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");
            
            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);
            
            List<Url> urlList = urlService.find(corpIds, UserSession.containsCorpId(), taskId);
            Url sumUrl = urlService.sum(corpIds, UserSession.containsCorpId(), taskId);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-点击.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
            CSV.blank(writer);
            CSV.click(writer, result);
            CSV.blank(writer);
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
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");
            
            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);

            List<Domain> domainList = domainService.find(corpIds, UserSession.containsCorpId(), taskId);
            Domains.fetchOther(domainList);
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-域名.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
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
            Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
            Asserts.isNull(taskId, "任务");
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            Task task = taskService.get(corpIds, UserSession.containsCorpId(), taskId);
            Asserts.isNull(task, "任务");
            
            Result result = resultService.get(corpIds, UserSession.containsCorpId(), taskId);

            List<Region> provinceList = regionService.group(corpIds, UserSession.containsCorpId(), taskId);
            List<Region> cityList = regionService.find(corpIds, UserSession.containsCorpId(), taskId);
            List<Region> regionList = Regions.combine(provinceList, cityList, "  ");
            
            ServletOutputStream out = Webs.output(response, Encodes.urlEncode("任务-地域.csv"));
            CSVWriter writer = new CSVWriter(new OutputStreamWriter(out), ',');
            CSV.header(writer, task, result);
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
}
