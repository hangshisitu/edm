package cn.edm.web.action.report;

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
import cn.edm.model.Result;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

@Controller("ReportCampaign")
@RequestMapping("/report/campaign/**")
public class CampaignAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(CampaignAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String page(HttpServletRequest request, ModelMap map) {
        try {
            Page<Result> page = new Page<Result>();
            Pages.page(request, page);

            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            String campaignName = StreamToString.getStringByUTF8(Https.getStr(request, "campaignName", R.CLEAN, R.LENGTH, "{1,20}"));
            
            MapBean mb = new MapBean();
            corpId = UserSession.getCorpId(mb, corpId);
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            page = resultService.search(page, mb, corpIds, corpId, UserSession.bindCorpId(), campaignName);
            
            domainCopy(corpIds, corpId, page.getResult());
            
            map.put("page", page);
            map.put("mb", mb);
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            
            return "default/report/campaign_page";
        } catch (Exception e) {
            logger.error("(Campaign:page) error: ", e);
            return Views._404();
        }
    }
    
    private void domainCopy(Integer[] corpIds, Integer corpId, List<Result> resultList) {
        StringBuffer sbff = new StringBuffer();
        for (Result result : resultList) {
            sbff.append(result.getCampaignId()).append(",");
        }
        List<Domain> domainList = domainService.campaignSum(corpIds, corpId, UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
        Domains.copy("campaign", domainList, resultList);
    }
}
