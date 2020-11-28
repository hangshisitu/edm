package cn.edm.util;

import java.util.List;

import org.apache.commons.lang.StringUtils;


import com.google.common.collect.Lists;

public class ConvertUtils {

	public static List<Integer> toInts(String str) {
		List<Integer> list = Lists.newArrayList();
		if (str != null && !str.equals("")) {
			for (String id : StringUtils.split(str, ",")) {
				list.add(Integer.valueOf(id));
			}
		}
		return list;
	}
	
	public static List<String> convertStringToStringList(String str) {
		List<String> list = Lists.newArrayList();
		if (str != null && !str.equals("")) {
			for (String name : StringUtils.split(str, ",")) {
				list.add(name);
			}
		}

		return list;
	}
	
	public static Integer[] str2Int(String str) {
		Integer[] num = null;
		if (str != null && !str.equals("")) {
			String[] strArray = StringUtils.split(str, ",");
			num = new Integer[strArray.length];
			for (int i=0;i<strArray.length;i++) {
				num[i] = Integer.parseInt(strArray[i]);
			}
		}
		return num;
	}
	
	public static Integer[] strArray2Int(String[] str) {
		Integer[] num = null;
		if (str != null && !str.equals("")) {
			num = new Integer[str.length];
			for (int i=0;i<str.length;i++) {
				num[i] = Integer.parseInt(str[i]);
			}
		}
		return num;
	}
}
