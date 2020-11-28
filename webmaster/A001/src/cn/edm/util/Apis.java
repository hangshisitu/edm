package cn.edm.util;

import java.io.IOException;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.exception.Errors;

public class Apis {

	private static Logger logger = LoggerFactory.getLogger(Apis.class);
	
	public static String get(String action) {
		HttpClient client = null;
		String status = null;
		try {
			client = new DefaultHttpClient();
			HttpConnectionParams.setConnectionTimeout(client.getParams(), 1000);
			HttpGet get = new HttpGet(action);
			HttpResponse response = client.execute(get);
			status = response.getStatusLine().toString();
			get.abort();
			logger.info("(Api:get) action: " + action + ", status: " + status);
		} catch (Exception e) {
			logger.error("(Api:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}

		return status;
	}
	
	public static String get(String action, String encoding) {
		HttpClient client = null;
		String content = null;
		try {
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet(action);
			HttpResponse response = client.execute(get);
			HttpEntity entity = response.getEntity();
			if (entity != null) {
				content = IOUtils.toString(entity.getContent(), encoding);
			}
			get.abort();
			logger.info("(Api:get) action: " + action);
		} catch (Exception e) {
			e.printStackTrace();
			logger.error("(Api:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}

		return content;
	}
	
	public static String getProperty(String action, String encoding) {
		HttpClient client = null;
		String content = null;
		try {
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet(action);
			HttpResponse response = client.execute(get);
			logger.info("(Api:get) action: " + action);
			int code = response.getStatusLine().getStatusCode();
			if(code==200) {
				HttpEntity entity = response.getEntity();
				if (entity != null) {
					content = IOUtils.toString(entity.getContent(), encoding);
				}
				get.abort();
			}else if(code==201) {
				throw new Errors("标签不存在");
			}else if(code==202) {
				throw new Errors("邮箱格式错误");
			}else if(code==203) {
				throw new Errors("任务ID不存在");
			}else if(code==204) {
				throw new Errors("时间错误");
			}
			
		} catch (Exception e) {
			logger.error("(Api:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}

		return content;
	}
	
	public static HttpResponse getHttpResponse2(String action, String encoding) {
		HttpClient client = null;
		HttpResponse response = null;
		try {
			logger.info("(Api:get) action: " + action);
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet(action);
			response = client.execute(get);
		} catch (Exception e) {
			logger.error("(Api:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}

		return response;
	}
	
	public static HttpResponse getHttpResponse(HttpClient client,String action, String encoding) throws ClientProtocolException, IOException {
		logger.info("(Api:property get) action: " + action);
		HttpGet get = new HttpGet(action);
		HttpResponse response = client.execute(get);
		return response;
	}
	
	
	public static String getDeliveryData(String action, String encoding) {
		HttpClient client = null;
		String content = null;
		try {
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet(action);
			HttpResponse response = client.execute(get);
			logger.info("(Api:get) action: " + action);
			int code = response.getStatusLine().getStatusCode();
			if(code==200) {
				HttpEntity entity = response.getEntity();
				if (entity != null) {
					content = IOUtils.toString(entity.getContent(), encoding);
				}
				get.abort();
			}else {
				
			}
			
		} catch (Exception e) {
			logger.error("(Api:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}

		return content;
	}
	
	
}
