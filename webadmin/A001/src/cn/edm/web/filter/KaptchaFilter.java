package cn.edm.web.filter;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Fqn;
import cn.edm.consts.Sessions;
import cn.edm.consts.Value;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Kaptchas;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;

/**
 * 集成Kaptcha验证码的Filter.
 * 
 * @author yjli
 */
public class KaptchaFilter implements Filter {

	/**
	 * Filter回调请求处理函数.
	 */
	@Override
	public void doFilter(ServletRequest req, ServletResponse res, FilterChain chain) throws IOException, ServletException {
		HttpServletRequest request = (HttpServletRequest) req;
		HttpServletResponse response = (HttpServletResponse) res;
		
		String url = request.getServletPath();
		String k = request.getSession().getId();
		
		try {
			// 符合j_spring_security_check为验证处理请求, 其余生成验证图片请求.
			if (StringUtils.startsWith(url, "/j_spring_security_check")) {
				
				CSRF.validate(request);
				
				if (validateCaptchaChallenge(request)) {
					chain.doFilter(request, response);
				} else {
					throw new Errors("验证码输入有误");
				}
			} else {
				String code = Kaptchas.generate(request, response);
				Ehcache cache = CtxHolder.getBean("ehcache");
				cache.put(Fqn.KAPTCHA, k, code);
			}
		} catch (Errors e) {
			redirectFailureUrl(request, response, e.getMessage());
		}
	}
	
	@Override
	public void init(FilterConfig config) throws ServletException {
	}

	@Override
	public void destroy() {
	}
	
	/**
	 * 验证验证码.
	 */
	private boolean validateCaptchaChallenge(HttpServletRequest request) {
		HttpSession session = request.getSession();
		int count = session.getAttribute(Sessions.KAPTCHA_COUNT) == null ? 0 : (Integer) session.getAttribute(Sessions.KAPTCHA_COUNT);
		
		if (count < 5) {
			count += 1;
			session.setAttribute(Sessions.KAPTCHA_COUNT, count);
			return true;
		}
		
		Ehcache cache = CtxHolder.getBean("ehcache");
		String key = (String) cache.get(Fqn.KAPTCHA, session.getId());
		String kaptcha = Https.getStr(request, "kaptcha", R.CLEAN);
		if (StringUtils.isBlank(key)) {
			throw new Errors("验证码超时");
		}

		return Kaptchas.validate(key, kaptcha);
	}

	/**
	 * 跳转到失败页面.
	 */
	private void redirectFailureUrl(final HttpServletRequest request, final HttpServletResponse response, String message) throws IOException {
		Object count = request.getSession().getAttribute(Sessions.KAPTCHA_COUNT);
		MapBean mb = new MapBean();
		Views.error(mb, message);
		mb.put("count", count == null ? Value.I : (Integer) count);
		
		ObjectMappers.renderJson(response, mb);
		return;
	}
}
