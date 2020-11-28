package cn.edm.web.action.account;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.model.History;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;

@Controller
@RequestMapping("/account/history/**")
public class HistoryAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(HistoryAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<History> page = new Page<History>();
			page.setPageSize(100);

			MapBean mb = new MapBean();
			page = historyService.search(page, mb, UserSession.getCorpId(), UserSession.getUserId());

			map.put("page", page);
			map.put("mb", mb);

			return "default/account/history_page";
		} catch (Exception e) {
			logger.error("(History:page) error: ", e);
			return Views._404();
		}
	}
}
