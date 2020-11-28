package cn.edm.interceptor;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.servlet.handler.HandlerInterceptorAdapter;

import cn.edm.constant.Config;
import cn.edm.modules.utils.Props;
import cn.edm.service.SystemSetting;
import cn.edm.utils.web.XSS;

public class ContextInterceptor extends HandlerInterceptorAdapter{

	@Autowired
	private SystemSetting systemSetting;
	
	@Override
	public boolean preHandle(HttpServletRequest request,
			HttpServletResponse response, Object handler) throws Exception {
		request.setAttribute("ctx", request.getContextPath());
		request.setAttribute("xss", new XSS());
		request.setAttribute("nginxUrl",Props.getStr(Config.NGINX_SERVER));
		request.setAttribute("logoUrl",systemSetting.getLogoUrl());
		request.setAttribute("version", System.currentTimeMillis());
		return true;
	}

}
