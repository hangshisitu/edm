package cn.edm.util;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;

public class HttpUtils {

	public static Integer getInteger(HttpServletRequest request, String paramName, Integer defaultValue) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return Integer.parseInt(value);
		}
		return defaultValue;
	}
	
	public static Integer getInteger(HttpServletRequest request, String paramName) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return Integer.parseInt(value);
		}
		return null;
	}
	
	public static String getString(HttpServletRequest request, String paramName, String defaultValue) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return value;
		}
		return defaultValue;
	}
	
	public static String getString(HttpServletRequest request, String paramName) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return value;
		}
		return null;
	}
	
	public static Boolean getBoolean(HttpServletRequest request, String paramName, Boolean defaultValue) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return Boolean.parseBoolean(value);
		}
		return defaultValue;
	}
	
	public static Boolean getBoolean(HttpServletRequest request, String paramName) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			return Boolean.parseBoolean(value);
		}
		return null;
	}
	
	public static String getIp(HttpServletRequest request) {
		String ip = request.getHeader("X-Forwarded-For");
		if (ip == null || ip.length() == 0 || "unknown".equalsIgnoreCase(ip))
			ip = request.getHeader("Proxy-Client-IP");
		if (ip == null || ip.length() == 0 || "unknown".equalsIgnoreCase(ip))
			ip = request.getHeader("WL-Proxy-Client-IP");
		if (ip == null || ip.length() == 0 || "unknown".equalsIgnoreCase(ip))
			ip = request.getHeader("HTTP_CLIENT_IP");
		if (ip == null || ip.length() == 0 || "unknown".equalsIgnoreCase(ip))
			ip = request.getHeader("HTTP_X_FORWARDED_FOR");
		if (ip == null || ip.length() == 0 || "unknown".equalsIgnoreCase(ip))
			ip = request.getRemoteAddr();

		return ip;
	}

	public static String getClient(HttpServletRequest request) {
		String browser = request.getHeader("USER-AGENT");
		if (StringUtils.contains(browser, "MSIE")) {
			browser = "IE" + StringUtils.substringAfter(browser, "MSIE ");
			browser = StringUtils.substringBefore(browser, ";");
		} else if(StringUtils.contains(browser, "Firefox")) {
			browser = "Firefox" + StringUtils.substringAfter(browser, "Firefox");
		} else if(StringUtils.contains(browser, "Chrome")) {
			browser = "Chrome" + StringUtils.substringAfter(browser, "Chrome");
			browser = StringUtils.substringBefore(browser, " ");
		} else if(StringUtils.contains(browser, "Opera")) {
			browser = StringUtils.replace(browser, "Opera ", "Opera/");
			browser = "Opera" + StringUtils.substringAfter(browser, "Opera");
			browser = StringUtils.substringBefore(browser, " ");
		} else if(StringUtils.contains(browser, "Safari")) {
			browser = "Safari" + StringUtils.substringAfter(browser, "Safari");
		}else if(StringUtils.contains(browser, "Navigator")) {
			browser = "Navigator" + StringUtils.substringAfter(browser, "Navigator");
		} else {
			browser = "Other";
		}
		
		browser = StringUtils.replace(browser, "/", " ");
		return browser;
	}
	
	public static String htmlEncode(String text) {
		if (text == null || "".equals(text)) {
			return "";
		}
		text = StringUtils.replace(text, "<", "&lt;");
		text = StringUtils.replace(text, ">", "&gt;");
		text = StringUtils.replace(text, " ", "&nbsp;");
		text = StringUtils.replace(text, "\"", "&quot;");
		text = StringUtils.replace(text, "\'", "&apos;");
		text = StringUtils.replace(text, "\n", "<br/>");
		return text;
	}
}
