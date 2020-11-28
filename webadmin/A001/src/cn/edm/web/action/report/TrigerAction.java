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
import cn.edm.model.Triger;
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

@Controller
@RequestMapping("/report/triger/**")
public class TrigerAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(TrigerAction.class);
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Result> page = new Page<Result>();
			Pages.page(request, page);

			Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
			String trigerName = StreamToString.getStringByUTF8(Https.getStr(request, "trigerName", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			String cycle = Https.getStr(request, "cycle", R.CLEAN, R.INTEGER);
			
			MapBean mb = new MapBean();
			corpId = UserSession.getCorpId(mb, corpId);
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
                
            List<Triger> trigerList = trigerService.findApi(corpIds, corpId);
            
			page = resultService.search(page, mb, corpIds, corpId,
			        trigerName, beginTime, endTime);
			domainCopy(corpIds, corpId, page.getResult());
			
			mb.put("cycle", cycle);
			
			map.put("page", page);
			map.put("trigerList", trigerList);
			map.put("mb", mb);
            map.put("isTriger", UserSession.isTriger());
            map.put("isJoin", UserSession.isJoin());
			
			return "default/report/triger_page";
		} catch (Exception e) {
			logger.error("(Triger:page) error: ", e);
			return Views._404();
		}
	}
	
	private void domainCopy(Integer[] corpIds, Integer corpId, List<Result> resultList) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : resultList) {
			sbff.append(result.getTaskId()).append(",");
		}
		
		List<Domain> domainList = domainService.trigerGroup(corpIds, corpId, Converts._toIntegers(sbff.toString()));
		Domains.copy("triger", domainList, resultList);
	}
}
