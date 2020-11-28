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

import org.apache.http.HttpStatus;

import cn.edm.util.Asserts;

public class SafetyFilter implements Filter{

	@Override
	public void destroy() {
		
	}

	@Override
	public void doFilter(ServletRequest req, ServletResponse res,
			FilterChain chain) throws IOException, ServletException {
		HttpServletRequest request = (HttpServletRequest) req;
		HttpServletResponse response = (HttpServletResponse) res;
		if (Asserts.hasAny( new String[] { "GET", "POST" },request.getMethod()))
			chain.doFilter(request, response);
		else {
			response.setStatus(HttpStatus.SC_FORBIDDEN);
		}
	}

	@Override
	public void init(FilterConfig arg0) throws ServletException {
		
	}

}
