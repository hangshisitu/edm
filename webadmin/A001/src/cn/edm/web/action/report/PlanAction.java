package cn.edm.web.action.report;

import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Cnds;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Result;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

@Controller("ReportPlan")
@RequestMapping("/report/plan/**")
public class PlanAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(PlanAction.class);

    @RequestMapping(method = RequestMethod.GET)
    public String page(HttpServletRequest request, ModelMap map) {
        try {
            Page<Result> page = new Page<Result>();
            Pages.page(request, page);

            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            String taskName = StreamToString.getStringByUTF8(Https.getStr(request, "taskName", R.CLEAN, R.LENGTH, "{1,20}"));
            String nameCnd = Https.getStr(request, "nameCnd", R.CLEAN, R.REGEX, "regex:^" + Cnds.EQ + "|" + Cnds.LIKE + "$");
            nameCnd = Values.get(nameCnd, Cnds.LIKE);
            
            MapBean mb = new MapBean();
            corpId = UserSession.getCorpId(mb, corpId);
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            page = resultService.search(page, mb, corpIds, corpId, UserSession.bindCorpId(), taskName, nameCnd);
           
            domainCopy(corpIds, corpId, page.getResult());
            
            map.put("page", page);
            map.put("mb", mb);
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            
            return "default/report/plan_page";
        } catch (Exception e) {
            logger.error("(Plan:page) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String total(HttpServletRequest request, ModelMap map) {
        try {
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            Integer[] planIds = Converts._toIntegers(Converts.repeat(checkedIds));
            if (Asserts.empty(planIds) || planIds.length < 2 || planIds.length > 100) {
                throw new Errors("任务集合范围只能在2至100之间");
            }
            
            MapBean mb = new MapBean();
            
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Domain domain = domainService.planSum(mb, corpIds, UserSession.containsCorpId(), UserSession.bindCorpId(), planIds);
            
            map.put("checkedIds", checkedIds);
            map.put("totalItems", planIds.length);
            map.put("domain", domain);
            map.put("mb", mb);
            
            return "default/report/plan_total";
        } catch (Exception e) {
            logger.error("(Plan:total) error: ", e);
            return Views._404();
        }
    }
    
    private void domainCopy(Integer[] corpIds, Integer corpId, List<Result> resultList) {
        StringBuffer sbff = new StringBuffer();
        for (Result result : resultList) {
            sbff.append(result.getPlanId()).append(",");
        }
        
        List<Domain> domainList = domainService.planGroup(corpIds, corpId, UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
        Domains.copy("plan", domainList, resultList);
    }
}
