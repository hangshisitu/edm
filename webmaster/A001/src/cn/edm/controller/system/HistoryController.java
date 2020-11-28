package cn.edm.controller.system;

import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.util.WebUtils;

import cn.edm.constant.Role;
import cn.edm.constant.SearchDefine;
import cn.edm.domain.History;
import cn.edm.security.UserContext;
import cn.edm.service.HistoryService;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;


/**
 * 登录历史
 * @author Luxh
 */
@Controller
@RequestMapping("/history")
public class HistoryController {
	
	@Autowired
	private HistoryService historyService;
	
	@RequestMapping(value="/myHistory", method=RequestMethod.GET)
	public String showMyHistoryList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		realParamsMap.put("userId", UserContext.getCurrentUser().getUsername());
		
		Pagination<History> pagination = historyService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		return "system/historyList";
	}
	
	
	@RequestMapping(value="/subUserHistory", method=RequestMethod.GET)
	public String showSubUserHistoryList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		
		realParamsMap.put("roleId", Role.SEE.getId());
		Pagination<History> pagination = historyService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		return "system/subUserHistoryList";
	}
	
	
}
