package cn.edm.util;

import java.util.Date;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;

import cn.edm.constant.ConfigKeys;

public class DateUtil {
	
	/**
	 * 获取当前时间
	 * @return
	 */
	public static Date getCurrentDate(){
		return new Date();
	}
	
	/**
	 * 根据月份获取日期范围
	 * @param months
	 * @return
	 */
	public static String[] getDateRange(int months) {
		String[] range = new String[2];
		DateTime endTemp = new DateTime();
		DateTime end = new DateTime(endTemp.plusDays(1));
		if(months==0) {
			range[0] = "19000101";
		}else {
			DateTime begin = end.minusMonths(months);
			range[0] = begin.toString(DateTimeFormat.forPattern("yyyyMMdd"));
		}
		range[1] = end.toString(DateTimeFormat.forPattern("yyyyMMdd"));
		return range;
	}
	
	public static String getDateStr(){
		DateTime now = new DateTime();
		return now.toString(DateTimeFormat.forPattern("yyyyMMddHHmmssSSS"));
	}
	
	public static String getDateByForamt(Date date,String format){
		if(date !=null) {
			DateTime now = new DateTime(date);
			return now.toString(DateTimeFormat.forPattern(format));
		}
		return "";
		
	}
	
	public static String getDateStr(String format){
		DateTime now = new DateTime();
		return now.toString(DateTimeFormat.forPattern(format));
	}
	
	
	public static boolean isTimeEnough(Date createTime) {
		boolean flag = false;
		String daysTemp = PropertiesUtil.get(ConfigKeys.PROPERTY_DAYS);
		int days = 15;
		if(StringUtils.isNotEmpty(daysTemp)) {
			days = Integer.parseInt(daysTemp.trim());
		}
		DateTime time = new DateTime(createTime);
		if(time.plusDays(days).isBeforeNow()) {
			flag = true;
		}
		return flag;
	}
	
	
	
	public static DateTime getSeasonFirst(int month){
		DateTime now = new DateTime();
		if(month>=1&&month<=3) {
			now.monthOfYear().withMinimumValue();
		}
		return null;
	}
	
	
	
	public static String[] getLatestWeeks(int n) {
		DateTime now = new DateTime();
		int nowWeek = now.getDayOfWeek();
		int fromDay = nowWeek-1+(7*(n-1));
		//int toDay = 7 - nowWeek;
		DateTime from = now.minusDays(fromDay);
		//DateTime to = now.plusDays(toDay);
		String[] days = new String[n];
		
		if(n==2) {
			DateTime from1 = from;
			DateTime to1 = from.plusDays(6);
			
			DateTime from2 = to1.plusDays(1);
			DateTime to2 = from2.plusDays(6);
			
			String range1 = from1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range2 = from2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			days[0] = range1;
			days[1] = range2;
		}else if(n==3) {
			DateTime from1 = from;
			DateTime to1 = from.plusDays(6);
			
			DateTime from2 = to1.plusDays(1);
			DateTime to2 = from2.plusDays(6);
			
			
			DateTime from3 = to2.plusDays(1);
			DateTime to3 = from3.plusDays(6);
			
			
			
			String range1 = from1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range2 = from2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range3 = from3.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to3.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			
			
			days[0] = range1;
			days[1] = range2;
			days[2] = range3;
		}else if(n==4) {
			DateTime from1 = from;
			DateTime to1 = from.plusDays(6);
			
			DateTime from2 = to1.plusDays(1);
			DateTime to2 = from2.plusDays(6);
			
			
			DateTime from3 = to2.plusDays(1);
			DateTime to3 = from3.plusDays(6);
			
			DateTime from4 = to3.plusDays(1);
			DateTime to4 = from4.plusDays(6);
			
			
			String range1 = from1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to1.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range2 = from2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to2.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range3 = from3.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to3.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			String range4 = from4.toString(DateTimeFormat.forPattern("yyyy-MM-dd 00:00:01"))+"|"+to4.toString(DateTimeFormat.forPattern("yyyy-MM-dd 23:59:59"));
			
			
			days[0] = range1;
			days[1] = range2;
			days[2] = range3;
			days[3] = range4;
		}
		return days;
	}
}
