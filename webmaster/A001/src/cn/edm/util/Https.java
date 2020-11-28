package cn.edm.util;

import java.io.UnsupportedEncodingException;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;

public class Https {

	public static String getStr(HttpServletRequest request, String name, Object... validators) {
		String value = StringUtils.trim(request.getParameter(name));
		return (String) Validator.validate(value, validators);
	}
	
	public static Integer getInt(HttpServletRequest request, String name, Object... validators) {
		String value = StringUtils.trim(request.getParameter(name));
		value = (String) Validator.validate(value, validators);
		return StringUtils.isBlank(value) ? null : Integer.valueOf(value);
	}
	
	public static String getString(HttpServletRequest request, String paramName) {
		String value = request.getParameter(paramName);
		if (value != null && !value.equals("")) {
			try {
				value = new String(value.getBytes("ISO-8859-1"),"UTF-8");
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}
			return value;
		}
		return null;
	}
	
}
