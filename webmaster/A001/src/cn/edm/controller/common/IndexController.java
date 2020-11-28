package cn.edm.controller.common;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.constant.ConfigKeys;
import cn.edm.security.UserContext;
import cn.edm.util.PropertiesUtil;

@Controller
public class IndexController {
	
	private static Logger log = LoggerFactory.getLogger(IndexController.class);
	
	@RequestMapping(value = { "/", "/index" },method={RequestMethod.POST,RequestMethod.GET,RequestMethod.HEAD})
	public String index(HttpServletRequest request){
		request.getSession().setAttribute("currentUser", UserContext.getCurrentUser());
		String isShow = PropertiesUtil.get(ConfigKeys.SMTP_CATALOG_SHOW);
		String hrCatalogShow = PropertiesUtil.get(ConfigKeys.HR_CATALOG_SHOW);
		String roleCatalogShow = PropertiesUtil.get(ConfigKeys.ROLE_CATALOG_SHOW);
		request.getSession().setAttribute("isShowSmtpCatalog", isShow);
		if(StringUtils.isBlank(hrCatalogShow)){//不加配置默认true，显示菜单
			hrCatalogShow = "true";
		}
		request.getSession().setAttribute("hrCatalog", hrCatalogShow);
		if(StringUtils.isBlank(roleCatalogShow)){//角色管理不加配置默认true，显示菜单
			roleCatalogShow = "true";
		}
		request.getSession().setAttribute("roleCatalog", roleCatalogShow);
		log.info("to index page.......");
		return "/layout/layout";
	}
	
}

