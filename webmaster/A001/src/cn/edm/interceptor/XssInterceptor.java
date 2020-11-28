package cn.edm.interceptor;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.springframework.web.servlet.handler.HandlerInterceptorAdapter;

import cn.edm.modules.utils.Encodes;
import cn.edm.utils.web.Clean;

public class XssInterceptor extends HandlerInterceptorAdapter {
	
	@Override
	public boolean preHandle(HttpServletRequest request, HttpServletResponse response, Object handler) throws Exception {
		String action = request.getRequestURI();
		String query = request.getQueryString();
		
        if (action.startsWith("/resources")) {
            return true;
        }
		
		if (!r(StringUtils.substringBefore(action, ";jsessionid=")) || !r(StringUtils.substringAfter(action, ";jsessionid="))) {
			response.sendRedirect("/404");
			return false;
		}
		if (StringUtils.isNotBlank(query)) {
			for (String each : StringUtils.splitPreserveAllTokens(query, "&")) {
				each = Encodes.urlDecode(each);
				if (!r(StringUtils.substringBefore(each, "=")) || !Clean.clean(StringUtils.substringAfter(each, "="))) {
					response.sendRedirect(action);
					return false;
				}
			}
		}
		
		return true;
	}
	
	private static boolean r(String str) {
		char[] ch = str.toCharArray();
		for (int i = 0; i < ch.length; i++) {
			char c = ch[i];
			if (!(c >= 'A' && c <= 'Z') && 
				!(c >= 'a' && c <= 'z') && 
				!(c >= '0' && c <= '9') &&
				!(c == '/' || c == '_' || c == '.')) {
				return false;
			}
		}
		return true;
	}
}
