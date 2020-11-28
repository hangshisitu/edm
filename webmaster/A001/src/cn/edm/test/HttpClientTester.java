package cn.edm.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.params.HttpConnectionParams;
import org.junit.Test;

public class HttpClientTester {
	
	@Test
	public void test1() throws ClientProtocolException, IOException{
		HttpClient httpClient = new DefaultHttpClient();
		HttpConnectionParams.setConnectionTimeout(httpClient.getParams(), 2000);
		HttpPost post = new HttpPost("http://localhost:8080/ssh1/t");
		List<NameValuePair> formParams = new ArrayList<NameValuePair>();   
		NameValuePair nv1 = new BasicNameValuePair("userName","eeeeee");
		NameValuePair nv2 = new BasicNameValuePair("password","22222");
		formParams.add(nv1);
		formParams.add(nv2);
		HttpEntity entity = new UrlEncodedFormEntity(formParams,"UTF-8");
		post.setEntity(entity);
		HttpResponse response = httpClient.execute(post);
		System.out.println(response.getStatusLine().getStatusCode());
		String content = IOUtils.toString(response.getEntity().getContent(), "UTF-8");
		System.out.println(content);
	}
}
