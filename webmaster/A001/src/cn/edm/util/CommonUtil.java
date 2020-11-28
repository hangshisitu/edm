package cn.edm.util;

import java.math.BigDecimal;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Map;
import java.util.TreeMap;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.springframework.ui.ModelMap;

import cn.edm.constant.SearchDefine;
import cn.edm.domain.Domain;

public class CommonUtil {
	
	/**
	 * 过滤查询参数中的空值与不法字符
	 * @param initParams
	 * @return
	 */
	public static Map<String,Object> getRealSearchParams(Map<String,Object> initParamsMap, ModelMap modelMap){
		Map<String, Object> realSearchParams = new TreeMap<String, Object>();
		for(Map.Entry<String, Object> entry : initParamsMap.entrySet()){
			 if(entry.getValue() == null){
				 continue;
			 }
			 if( entry.getValue() instanceof String && StringUtils.isBlank((String)entry.getValue())){
				 continue;
			 }
			 realSearchParams.put(entry.getKey(), XssFilter.filter(entry.getValue()));
			 modelMap.put(SearchDefine.SEARCH_PREFIX + entry.getKey(), XssFilter.filter(entry.getValue()));
		}
		return realSearchParams;
	}
	
	/**
	 * 根据当前页和每页记录数获取起点
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public static int getStartIndex(int currentPage,int pageSize){
		if(pageSize<1||pageSize>1000000) {
			throw new RuntimeException("显示数量超出范围");
		}
		if(currentPage<0||currentPage>53687091) {
			throw new RuntimeException("页码超出范围");
		}
		return ((currentPage - 1)<0?0:(currentPage - 1))*pageSize;
	}
	
	public static String getClientIpAddress(HttpServletRequest req) {
		String ipAddress = null;
		// ipAddress = this.getRequest().getRemoteAddr();
		ipAddress = req.getHeader("x-forwarded-for");
		if (ipAddress == null || ipAddress.length() == 0
				|| "unknown".equalsIgnoreCase(ipAddress)) {
			ipAddress = req.getHeader("Proxy-Client-IP");
		}
		if (ipAddress == null || ipAddress.length() == 0
				|| "unknown".equalsIgnoreCase(ipAddress)) {
			ipAddress = req.getHeader("WL-Proxy-Client-IP");
		}
		if (ipAddress == null || ipAddress.length() == 0
				|| "unknown".equalsIgnoreCase(ipAddress)) {
			ipAddress = req.getRemoteAddr();
			if (ipAddress.equals("127.0.0.1")) {
				// 根据网卡取本机配置的IP
				InetAddress inet = null;
				try {
					inet = InetAddress.getLocalHost();
				} catch (UnknownHostException e) {
					e.printStackTrace();
				}
				ipAddress = inet.getHostAddress();
			}
		}
		// 对于通过多个代理的情况，第一个IP为客户端真实IP,多个IP按照','分割
		if (ipAddress != null && ipAddress.length() > 15) { // "***.***.***.***".length()
			if (ipAddress.indexOf(",") > 0) {
				ipAddress = ipAddress.substring(0, ipAddress.indexOf(","));
			}
		}
		return ipAddress;
	}
	
	
	public static String format(double num) {
		DecimalFormat df = new DecimalFormat("#.00");
		return df.format(num);
	}
	
	
	public static String[] getScore(String scores) {
		
		String[] score = new String[2];
		if(StringUtils.isNotEmpty(scores)) {
			score = StringUtils.split(scores, "-");
		}else {
			score[0] = "0";
			score[1] = "99999999";
		}
		return score;
	}
	
	public static String getFormatNumber(double num){
		NumberFormat nf = NumberFormat.getInstance();
		nf.setMaximumFractionDigits(2);
		nf.setMinimumFractionDigits(2);
		return nf.format(num)+"%";
	}
	
	public static String getFormatNumber2(double num){
		NumberFormat nf = NumberFormat.getInstance();
		nf.setMaximumFractionDigits(2);
		nf.setMinimumFractionDigits(2);
		return nf.format(num);
	}
	
	public static double getFormatNumber4(double num,int bit){
		BigDecimal bg = new BigDecimal(num);
		return bg.setScale(bit, BigDecimal.ROUND_HALF_DOWN).doubleValue();
	}
	
	public static float getFormatNumber3(double num){
		NumberFormat nf = NumberFormat.getInstance();
		nf.setMaximumFractionDigits(2);
		nf.setMinimumFractionDigits(2);
		return Float.valueOf(nf.format(num));
	}
	
	
	public static Domain getDomain(Domain d) {
		double success = 0.00d;
		if((d.getSentCount()-d.getBeforeNoneCount()-d.getAfterNoneCount())!=0) {
			success = (d.getReachCount()*100.00)/(d.getSentCount()-d.getBeforeNoneCount()-d.getAfterNoneCount());
		}
		d.setSuccessRate(CommonUtil.getFormatNumber(success));
		
		double open = 0.00d;
		if(d.getReachCount()!=0) {
			open = (d.getReadUserCount()*100.00)/d.getReachCount();
		}
		d.setOpenRate(CommonUtil.getFormatNumber(open));
		
		double click = 0.00d;
		if(d.getReadUserCount()!=0) {
			click = (d.getClickUserCount()*100.00)/d.getReadUserCount();
		}
		d.setClickRate(CommonUtil.getFormatNumber(click));
		return d;
	}
}
