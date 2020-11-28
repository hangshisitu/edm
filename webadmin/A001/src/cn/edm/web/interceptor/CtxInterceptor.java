package cn.edm.web.interceptor;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.servlet.handler.HandlerInterceptorAdapter;

import cn.edm.consts.Config;
import cn.edm.modules.utils.Property;
import cn.edm.service.SystemSetting;
import cn.edm.utils.web.XSS;
import cn.edm.web.UserSession;
import cn.edm.web.View;

public class CtxInterceptor extends HandlerInterceptorAdapter {

	@Autowired
	private SystemSetting systemSetting;
	
	@Override
	public boolean preHandle(HttpServletRequest request, HttpServletResponse response, Object handler) throws Exception {
		request.setAttribute("ctx", request.getContextPath());
		request.setAttribute("currentUser", UserSession.getUser());
		request.setAttribute("xss", new XSS());
        request.setAttribute("view", new View());
        request.setAttribute("appUrl", Property.getStr(Config.APP_URL));
        request.setAttribute("website", Property.getStr(Config.WEBSITE_URL));
        String hrCatalogShow = StringUtils.isBlank(Property.getStr(Config.HR_CATALOG_SHOW)) ? "true" : Property.getStr(Config.HR_CATALOG_SHOW);
        request.setAttribute("hrCatalogShow", hrCatalogShow);
        request.setAttribute("nginxUrl",Property.getStr(Config.NGINX_SERVER));
		request.setAttribute("logoUrl",systemSetting.getLogoUrl());
		return true;
	}
}