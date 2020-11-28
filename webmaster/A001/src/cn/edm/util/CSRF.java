package cn.edm.util;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Value;
import cn.edm.exception.Errors;
import cn.edm.modules.utils.web.Randoms;

public class CSRF {

	private static final String TOKEN = "token";

	public static String generate(HttpServletRequest request) {
		filterReferer(request);
		
		String link = request.getServletPath();
		String mapper = mapper(link);

		String token = Randoms.getRandomString(32);
		request.getSession().setAttribute(TOKEN + "_" + mapper, token);
		request.setAttribute(TOKEN, token);
		return token;
	}

	public static void validate(HttpServletRequest request) {
		filterReferer(request);
		
		String link = request.getServletPath();
		String mapper = mapper(link);
		
		String client = Https.getStr(request, TOKEN, R.CLEAN, R.REQUIRED, R.LENGTH, "{1,32}");
		String server = (String) request.getSession().getAttribute(TOKEN + "_" + mapper);
		if (StringUtils.isBlank(client) || !StringUtils.equals(client, server)) {
			throw new Errors("Page_Expired");
		}
	}
	
	private static String mapper(String link) {
		String[] split = StringUtils.split(link, "/");

		String mapper = Value.S;

		if (split.length > 2) {
			String module = split[0];
			String action = split[1];
			String m = split[2];
			mapper = module + "_" + action + "_" + m;
		}
		if (split.length == 1) {
			String m = split[0];
			if (m.equals("j_spring_security_check")) {
				m = "login";
			}
			
			mapper = m;
		}

		return mapper;
	}
	
	private static void filterReferer(HttpServletRequest request) {
		/*String referer = request.getHeader("Referer");
		if (referer != null) {
			referer = (String) Validator.validate(referer, R.CLEAN, R.REQUIRED, "请求来源");
			//String[] appUrls = StringUtils.split(PropertiesUtil.get(ConfigKeys.APP_URLS), ",");
			String appUrl = PropertiesUtil.get(ConfigKeys.APP_URL);
			boolean ok = false;
			if (StringUtils.startsWith(referer, appUrl)) {
				ok = true;
			}
			if (!ok) {
				throw new Errors("请求来源不是本网站");
			}
		}*/
		
		String referer = request.getHeader("Referer");
		if (referer != null) {
			referer = (String) Validator.validate(referer, R.CLEAN, R.REQUIRED, "请求来源");
			String[] appUrls = StringUtils.split(PropertiesUtil.get(ConfigKeys.APP_URLS), ",");

			boolean ok = false;
			for (String appUrl : appUrls) {
				if (StringUtils.startsWith(referer, appUrl)) {
					ok = true;
					break;
				}
			}
			if (!ok) {
				String appUrl = PropertiesUtil.get(ConfigKeys.APP_URL);
				if (StringUtils.startsWith(referer, appUrl)) {
					ok = true;
				}
			}
			
			if (!ok) {
				throw new Errors("illegal request");
			}
		}
		
		
	}
}
