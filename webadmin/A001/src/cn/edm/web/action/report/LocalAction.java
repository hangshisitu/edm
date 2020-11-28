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
import cn.edm.consts.mapper.DomainMap;
import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.modules.orm.MapBean;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Counts;

import com.google.common.collect.Lists;

@Controller("ReportLocal")
@RequestMapping("/report/local/**")
public class LocalAction extends Action {

    private static final Logger logger = LoggerFactory.getLogger(LocalAction.class);

    @RequestMapping(method = RequestMethod.GET)
    public String sum(HttpServletRequest request, ModelMap map) {
        try {
            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
            String cycle = Https.getStr(request, "cycle", R.CLEAN, R.INTEGER);
            
            MapBean mb = new MapBean();
            corpId = UserSession.getCorpId(mb, corpId);
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
            Domain total = domainService.sum(mb, corpIds, corpId, UserSession.bindCorpId(),
                    null, beginTime, endTime, new Integer[] {Type.FORMAL, Type.TEST, Type.COLL});
            Domain other = new Domain();
            other.setEmailDomain("其他");
            Counts.plus(total, other);
            
            List<Domain> domains = Lists.newArrayList();
            
            for (DomainMap mapper : DomainMap.values()) {
                mb.clear();
                Domain d = domainService.sum(mb,corpIds, corpId, UserSession.bindCorpId(),
                        DomainMap.getDomains(mapper.getName()), beginTime, endTime, new Integer[] {Type.FORMAL, Type.TEST, Type.COLL});
                d.setEmailDomain(mapper.getName());
                domains.add(d);
                Counts.minus(d, other);
            }
            
            mb.put("cycle", cycle);

            map.put("total", total);
            map.put("other", other);
            map.put("domains", domains);

            map.put("mb", mb);
            map.put("isTriger", UserSession.isTriger());
            map.put("isJoin", UserSession.isJoin());
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            
            return "default/report/local_sum";
        } catch (Exception e) {
            logger.error("(Local:sum) error: ", e);
            return Views._404();
        }
    }
}
