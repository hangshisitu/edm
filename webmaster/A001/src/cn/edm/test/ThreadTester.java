package cn.edm.test;

import org.junit.Test;

public class ThreadTester {
	
	@Test
	public void test1() {
		MyThread th1 = new MyThread();
		MyThread th2 = new MyThread();
		MyThread th3 = new MyThread();
		MyThread th4 = new MyThread();
		MyThread th5 = new MyThread();
		MyThread th6 = new MyThread();
		MyThread th7 = new MyThread();
		MyThread th8 = new MyThread();
		th1.start();
		th2.start();
		th3.start();
		th4.start();
		th5.start();
		th6.start();
		th7.start();
		th8.start();
		
	}
}
