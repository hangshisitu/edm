package cn.edm.security;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.Authentication;
import org.springframework.security.web.authentication.AuthenticationSuccessHandler;

import cn.edm.constant.Sessions;
import cn.edm.domain.History;
import cn.edm.domain.Ip;
import cn.edm.service.HistoryService;
import cn.edm.service.IpService;
import cn.edm.util.CookieUtils;
import cn.edm.util.DateUtil;
import cn.edm.util.HttpUtils;
import cn.edm.util.SpringSecurityUtil;

/**
 * 登录验证成功处理器
 * @author Luxh
 */
public class LoginAuthenticationSuccessHandler implements AuthenticationSuccessHandler{
	
	private static Logger log = LoggerFactory.getLogger(LoginAuthenticationSuccessHandler.class);
	
	@Autowired
	private HistoryService historyService;
	
	@Autowired
	private IpService ipService;
	
	//登录验证成功后需要跳转的url
	private String url;
	
	public void onAuthenticationSuccess(HttpServletRequest request,
			HttpServletResponse response, Authentication authentication) throws IOException,
			ServletException {
		log.info("Authentication Success!");
		request.getSession().removeAttribute(Sessions.PASSWORD_ERROR_COUNT);
		String isRememberMe = request.getParameter("remember_me");
		if(StringUtils.isNotEmpty(isRememberMe)) {
			response.addCookie(CookieUtils.add(UserContext.getCurrentUser().getUsername()));
		}else {
			response.addCookie(CookieUtils.delete());
		}
		historyService.saveHistory(getHistory(request));
		response.sendRedirect(request.getContextPath()+url);
		
		
		//request.getRequestDispatcher(url).forward(request, response);
	}
	
	private History getHistory(HttpServletRequest request) {
		
		History his = new History();
		his.setCorpId(UserContext.getCurrentUser().getCorpId());
		his.setUserId(UserContext.getCurrentUser().getUsername());
		his.setResult(0);
		his.setClient(HttpUtils.getClient(request));
		String loginIp = SpringSecurityUtil.getWebAuthenticationDetails().getRemoteAddress();
		his.setLoginIp(loginIp);
		//his.setLoginIp(CommonUtil.getClientIpAddress(request));
		Ip ip = ipService.getByIp(loginIp);
		if(ip != null) {
			his.setRegion(ip.getRegion());
		}
		his.setLoginTime(DateUtil.getCurrentDate());
		return his;
	}
	public void setUrl(String url) {
		this.url = url;
	}

}
