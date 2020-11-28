package cn.edm.test;

import java.io.IOException;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;


public class MyThread extends Thread {
	
	public MyThread() {
		System.out.println("dddddddddd");
	}

	@Override
	public void run() {
		long t = System.currentTimeMillis();
		for(int i=0;i<200;i++) {
			HttpClient client = null;
			System.out.println("执行测试===");
			String url = "http://localhost:8080/ssh1/test?flag="+i;
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet(url);
			try {
				HttpResponse response = client.execute(get);
				System.out.println("response---"+response.getStatusLine().getStatusCode());
			} catch (ClientProtocolException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}finally {
				client.getConnectionManager().shutdown();
			}
		}
		System.out.println("结束---"+(System.currentTimeMillis()-t));
	}
	
	public static void main(String[] args) {
		new MyThread().start();
	}

}
