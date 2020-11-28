package cn.edm.util;

import java.util.Date;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;

import cn.edm.constant.Value;


public class Values {

	public static final Integer get(Integer value) {
		return get(value, Value.I);
	}
	
	public static final Integer get(Integer value, int defaultValue) {
		return value != null ? value : defaultValue;
	}
	
	public static final Long get(Long value) {
		return get(value, Value.L);
	}
	
	public static final Long get(Long value, long defaultValue) {
		return value != null ? value : defaultValue;
	}

	public static final String get(String value) {
		return get(value, Value.S);
	}
	
	public static final String get(String value, String defaultValue) {
		return StringUtils.isNotBlank(value) ? value : defaultValue;
	}
	
	public static final String get(Integer value, String defaultValue) {
		return value != null ? String.valueOf(value) : defaultValue;
	}
	
	public static String string(Date date, String fmt) {
		DateTime now = new DateTime(date);
		return now.toString(fmt);
	}
	
	public static long millis(String date, String fmt) {
		long ms = 0;
		if (StringUtils.isNotBlank(date)) {
			DateTimeFormatter fmter = DateTimeFormat.forPattern(fmt);
			ms = fmter.parseDateTime(date).getMillis();
		}
		return ms;
	}

	public static Date date(String millis) {
		Date d = null;
		if (StringUtils.isNotBlank(millis)) {
			d = new DateTime(Long.valueOf(millis)).toDate();
		}
		return d;
	}
	
	public static Integer integer(String value) {
		Integer i = null;
		if (StringUtils.isNotBlank(value) && Validates.integer(value)) {
			i = Integer.valueOf(value);
		}
		return i;
	}
	
	public static DateTime date(String value, DateTimeFormatter fmt) {
		DateTime d = null;
		if (StringUtils.isNotBlank(value) && Validates.date(value)) {
			d = fmt.parseDateTime(value);
		}
		return d;
	}
}
