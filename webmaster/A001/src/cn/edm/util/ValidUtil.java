package cn.edm.util;

import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class ValidUtil {
	
	/**
	 * 正则校验
	 * @param text
	 * @param regex
	 * @return
	 */
	public static boolean regex(String text, String regex) {
		if (text != null && !"".equals(text)) {
			return text.matches(regex);
		}
		return true;
	}
	
	public static boolean find(String text,String regex) {
		Pattern pattern = Pattern.compile(regex);
		Matcher matcher = pattern.matcher(text);
		if (text != null && !"".equals(text)) {
			return matcher.find();
		}
		return false;
		
	}
}
