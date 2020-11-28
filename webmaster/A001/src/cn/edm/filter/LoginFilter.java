package cn.edm.filter;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;

import cn.edm.exception.Errors;
import cn.edm.util.CSRF;


public class LoginFilter implements Filter{

	@Override
	public void destroy() {
		
	}

	@Override
	public void doFilter(ServletRequest req, ServletResponse res,
			FilterChain chain) throws IOException, ServletException {
		HttpServletRequest request = (HttpServletRequest) req;
		HttpServletResponse response = (HttpServletResponse) res;
		String url = request.getServletPath();
		try {
			if (StringUtils.startsWith(url, "/j_spring_security_check")||StringUtils.startsWith(url, "/webmaster/j_spring_security_check")) {
				CSRF.validate(request);
			}
			chain.doFilter(request, response);
		}catch(Errors e) {
			e.printStackTrace();
			request.setAttribute("message", e.getMessage());
			/*if("Page_Expired".equals(e.getMessage())) {
				response.sendRedirect(request.getContextPath()+"/login?error=5");
			}else {
				response.sendRedirect(request.getContextPath()+"/error");
			}*/
			response.sendRedirect(request.getContextPath()+"/login?error=5");
		}
		
	}

	@Override
	public void init(FilterConfig arg0) throws ServletException {
		
	}

}
