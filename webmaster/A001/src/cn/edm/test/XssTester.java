package cn.edm.test;


import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.text.ParseException;
import java.util.Calendar;
import java.util.Date;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.jsoup.Jsoup;
import org.jsoup.safety.Whitelist;
import org.junit.Test;

import cn.edm.util.DateUtil;
import cn.edm.util.PropQueue;
import cn.edm.vo.PropImportVo;

public class XssTester {
	
	@Test
	public void testXss() throws ParseException {
		System.out.println(new Integer(8).equals(new Integer(8)));
	}
	
	@Test
	public void testQueue() {
		PropQueue pq = new PropQueue();
		PropImportVo pv1 = new PropImportVo();
		pv1.setTaskId(1);
		pv1.setLabelIds("1,2");
		pq.offer(pv1);
		
		PropImportVo pv2 = new PropImportVo();
		pv2.setTaskId(2);
		pv2.setLabelIds("1,2");
		pq.offer(pv2);
		
		PropImportVo pv3 = new PropImportVo();
		pv3.setTaskId(5);
		pv3.setLabelIds("1,2");
		pq.offer(pv3);
		
		PropImportVo pv4 = new PropImportVo();
		pv4.setTaskId(4);
		pv4.setLabelIds("1,2");
		pq.offer(pv4);
		while(pq.size()>0) {
			System.out.println(pq.poll().getTaskId());
		}
	}
	
	@Test
	public void testd() {
		//String[] range = new String[2];
		DateTime end = new DateTime();
		DateTime begin = end.minusMonths(12);
		System.out.println(end.toString(DateTimeFormat.forPattern("yyyyMMdd")));
		System.out.println(begin.toString(DateTimeFormat.forPattern("yyyyMMdd")));
		System.out.println(System.getProperty("file.separator"));
		
		String b = "20100101";
		DateTime dt2 = DateTimeFormat.forPattern("yyyyMMdd").parseDateTime(b); 
		System.out.println(dt2.getYear());
	}
	
	@Test
	public void test2() throws IOException {
		/*FileOutputStream fos = new FileOutputStream(new File("d:/aaaa.txt"));
		OutputStreamWriter osw = new  OutputStreamWriter(fos,"UTF-8");
		BufferedWriter br = new BufferedWriter(osw);
		for(int i=1;i<200;i++) {
			br.write("aaa"+i+"@sina.com");
			br.write("\r\n");
		}
		br.flush();
		br.close();
		osw.close();
		fos.close();*/
		
		Calendar c = Calendar.getInstance();
		long nowTime = c.getTimeInMillis();
		
		Calendar c2 = Calendar.getInstance();
		c2.set(1987, 10, 23, 22, 12, 22);
		//1987-10-23 22:12:22
		long oldTime = c2.getTimeInMillis();
		
		new Date().getTime();
	}
	
	@Test
	public void testReplace() {
		String t = "2010-02-10";
		System.out.println(StringUtils.replace(t, "-", ""));
	}
	
	
}
