package cn.edm.util;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.safety.Whitelist;

import cn.edm.utils.web.Clean;

public class XssFilter {
	
	/**
	 * 过滤非法字符
	 * 如果为非法字符，返回空字符串 ""
	 * 否则返回Object本身
	 * @param obj
	 * @return Object
	 */
	public static Object filter(Object obj) {
		if (obj == null) {
			return "";
		}
		if(obj instanceof String) {
			String text = (String) obj;
			if("".equals(text)) {
				return "";
			}else {
				boolean isClean = Clean.clean(obj.toString());
				if(isClean){
					return obj;
				}else{
					return "";
				}
//					return Jsoup.clean(text, Whitelist.none());
			}
		}
		return obj;
		
	}
	
	/**
	 * 判断字符是否合法
	 * 如果为非法字符，返回空字符串 ""
	 * 否则返回String
	 * @param obj
	 * @return String
	 */
	public static String filterXss(Object obj) {
		if(obj!=null && !"".equals(obj)){
			String text = StringUtils.trim(obj.toString());
			return (String) filter(text);
		}else{
			return null;
		}
		
	}
	/**
	 * 判断字符是否合法
	 * 如果为非法字符，返回空字符串 null
	 * 否则返回Integer
	 * @param obj
	 * @return Integer
	 */
	public static Integer filterXssInt(String param) {
		Integer text = null;
		if (StringUtils.isBlank(param)) {
			return null;
		}
		text = Integer.valueOf(param);
		boolean isClean = Clean.clean(param);
		if(isClean){
			return text;
		}else{
			return null;
		}
	}
	
	/**
	 * 判断字符是否合法
	 * @param content
	 * @return
	 */
	public static boolean isValid(String content) {
		boolean flag = false;
		if(content==null||"".equals(content)) {
			flag = true; 
		}else {
			flag = Jsoup.isValid(content, Whitelist.none());
		}
		return flag;
	}
	
}
