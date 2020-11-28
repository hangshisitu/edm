package cn.edm.test;

import org.junit.Test;

import cn.edm.util.Regex;
import cn.edm.util.ValidUtil;

public class RegexTester {
	
	@Test
	public void test1() {
		
		int a = Integer.MAX_VALUE;
		System.out.println("a --"+a);
	}
	
	
	@Test
	public void test2() {
		String reg = "^[0-9]\\d{3}\\-(([0][1-9])|([1][0-2]))\\-(([0][1-9])|([1][0-9])|([2][0-9])|([3][0-1]))$";
		String pass = "20122-03-31";
		System.out.println(ValidUtil.regex(pass,reg));
	}
}
