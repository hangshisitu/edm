package cn.edm.util;

import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;

import cn.edm.exception.Errors;


public class Valids {

	public static void blank(Integer text, String message) {
		if (text == null) {
			throw new Errors(message);
		}
	}

	public static void blank(Long text, String message) {
		if (text == null) {
			throw new Errors(message);
		}
	}

	public static void blank(String text, String message) {
		if (text == null || text.trim().equals("")) {
			throw new Errors(message);
		}
	}

	public static void size(String text, int max, String message) {
		if (text != null && text.trim().length() > max) {
			throw new Errors(message);
		}
	}

	public static void size(String text, int min, int max, String message) {
		if (text != null && (text.trim().length() < min || text.trim().length() > max)) {
			throw new Errors(message);
		}
	}

	public static void regex(String text, String regex, String message) {
		if (text != null && !text.trim().equals("")) {
			if (!text.matches(regex)) {
				throw new Errors(message);
			}
		}
	}

	public static void email(String text, String message) {
		if (text != null && !text.trim().equals("")) {
			if (!EmailValid.valid(text)) {
				throw new Errors(message);
			}
		}
	}

	public static String date(String text) {
		DateTimeFormatter fmt = DateTimeFormat.forPattern("yyyy-MM-dd");
		try {
			fmt.parseDateTime(text);
			return text;
		} catch (Exception e) {
			return null;
		}
	}
}
