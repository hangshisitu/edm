package cn.edm.util;

import java.io.File;
import java.util.Collection;
import java.util.Map;

import org.apache.commons.lang.StringUtils;

import cn.edm.exception.Errors;

public class Asserts {

	public static void isNull(Object obj, String message) {
		if (obj == null) {
			throw new Errors(message + "不存在");
		}
	}

	public static void notUnique(boolean unique, String message) {
		if (!unique) {
			throw new Errors(message + "已存在");
		}
	}
	
	public static void notExists(String savePath, String filePath, String message) {
		File file = new File(savePath + filePath);
		if (!file.exists()) {
			throw new Errors(message + "文件不存在");
		}
	}
	
	public static void isEmpty(Map<?, ?> map, String message) {
		if (empty(map)) {
			throw new Errors(message + "集合不能为空值");
		}
	}
	
	public static void isEmpty(Collection<?> list, String message) {
		if (empty(list)) {
			throw new Errors(message + "集合不能为空值");
		}
	}
	
	public static void isEmpty(Object[] objects, String message) {
		if (empty(objects)) {
			throw new Errors(message + "集合不能为空值");
		}
	}

	public static boolean empty(Map<?, ?> map) {
		if (map == null || map.isEmpty()) {
			return true;
		}
		return false;
	}

	public static boolean empty(Collection<?> list) {
		if (list == null || list.isEmpty()) {
			return true;
		}
		return false;
	}

	public static boolean empty(Object[] objects) {
		if (objects == null || objects.length == 0) {
			return true;
		}
		return false;
	}
	
	public static boolean hasAny(Object[] objects, Object target) {
		if (objects == null || objects.length == 0) {
			return false;
		}
		if (target == null) {
			return false;
		}

		for (Object object : objects) {
			if (object.equals(target)) {
				return true;
			}
		}
		
		return false;
	}
	
	public static boolean containsAny(String[] objects, String target) {
		if (objects == null || objects.length == 0) {
			return false;
		}
		if (target == null) {
			return false;
		}

		for (String object : objects) {
			if (StringUtils.contains(target, object)) {
				return true;
			}
		}

		return false;
	}
}
