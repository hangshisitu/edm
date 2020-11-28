package cn.edm.util;

import javax.servlet.http.Cookie;


public class CookieUtils {

	public static final int MAX_AGE = 60 * 60 * 24 * 7; // 一周
	public static final String PATH = "/";
	public static final String USERID_COOKIE = "MID";

	public static Cookie add(String mid) {
		Cookie cookie = new Cookie(USERID_COOKIE, EncodeUtils.encodeBase64(mid.getBytes()));
		cookie.setPath(PATH);
		cookie.setMaxAge(MAX_AGE);
		return cookie;
	}

	public static Cookie delete() {
		Cookie cookie = new Cookie(USERID_COOKIE, "");
		cookie.setMaxAge(0);
		cookie.setPath(PATH);
		return cookie;
	}

	public static final String get(Cookie cookie) {
		try {
			String name = cookie.getName();
			if (name != null && name.equals(USERID_COOKIE)) {
				return new String(EncodeUtils.decodeBase64(cookie.getValue()));
			}
		} catch (Exception e) {
		}
		return null;
	}
}
