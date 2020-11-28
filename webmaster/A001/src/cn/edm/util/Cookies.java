package cn.edm.util;

import javax.servlet.http.Cookie;


public class Cookies {

	public static final int MAX_AGE = 60 * 60 * 24 * 7; // 一周
	public static final String PATH = "/";
	public static final String USERID_COOKIE = "MID";
	public static final String PAGE_SIZE_COOKIE = "PSC";

	public static Cookie add(String key, String name) {
		Cookie cookie = new Cookie(key, EncodeUtils.encodeBase64(name.getBytes()));
		cookie.setPath(PATH);
		cookie.setMaxAge(MAX_AGE);
		return cookie;
	}

	public static Cookie delete(String key) {
		Cookie cookie = new Cookie(key, "");
		cookie.setMaxAge(0);
		cookie.setPath(PATH);
		return cookie;
	}

	public static final String get(Cookie cookie, String key) {
		try {
			String name = cookie.getName();
			if (name != null && name.equals(key)) {
				return new String(EncodeUtils.decodeBase64(cookie.getValue()));
			}
		} catch (Exception e) {
		}
		return null;
	}
}
