package cn.edm.web.action.report;

import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Type;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Triger;
import cn.edm.modules.orm.MapBean;
import cn.edm.utils.Values;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Counts;
import cn.edm.web.facade.Tasks;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/report/api/**")
public class ApiAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(ApiAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String sum(HttpServletRequest request, ModelMap map) {
        try {
            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
            String cycle = Https.getStr(request, "cycle", R.CLEAN, R.INTEGER);

            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            List<Triger> trigerList = trigerService.findApi(corpIds, UserSession.containsCorpId());
            
            MapBean mb = new MapBean();
            List<Domain> domainList = Lists.newArrayList();
            Domain total = new Domain();
            
            if (UserSession.isTriger()) {
                Domain triger = domainService.trigerSum(mb, Tasks.corpIds(trigerList), corpId, beginTime, endTime);
                if (triger != null && Values.get(triger.getSentCount()) > 0) {
                    triger.setTaskName("触发任务");
                    Counts.plus(triger, total);
                    domainList.add(triger);    
                }
            }
            if (UserSession.isJoin()) {
                Domain join = domainService.sum(mb, corpIds, corpId, UserSession.bindCorpId(),
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
            
            mb.put("cycle", cycle);

            map.put("domainList", domainList);
            map.put("total", total);
            map.put("mb", mb);
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            
            return "default/report/api_sum";
        } catch (Exception e) {
            logger.error("(Api:sum) error: ", e);
            return Views._404();
        }
    }
}
