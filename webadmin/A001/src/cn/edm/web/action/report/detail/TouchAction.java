package cn.edm.web.action.report.detail;

import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.model.Corp;
import cn.edm.model.Domain;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.modules.orm.Page;
import cn.edm.utils.Asserts;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;

@Controller("ReportDetailTouch")
@RequestMapping(value="/report/detail/touch/**")
public class TouchAction extends Action {
	
	@RequestMapping(method = RequestMethod.GET)
	public String cover(HttpServletRequest request, ModelMap map) {
		Page<Result> page = new Page<Result>();
		Pages.page(request, page);

		Integer touchId = Https.getInt(request, "touchId", R.CLEAN, R.INTEGER);
		Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
		Asserts.isNull(templateId, "模板");
		
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
        if(totalDomain != null) {
        	totalDomain.setParentReadUserCount(totalParentReadUserCount);
        	int touchCount = touchService.touchSum(corpIds, UserSession.containsCorpId(), templateId);
        	totalDomain.setTouchCount(touchCount);
        }
        map.put("touchId", touchId);
        map.put("templateId", templateId);
        map.put("setting", setting);
        map.put("domainList", domainList);
        map.put("totalDomain", totalDomain);
		return "default/report/detail/touch_cover";
	}

}
