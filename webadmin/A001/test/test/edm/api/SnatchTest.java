package test.edm.api;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.util.EntityUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.utils.Asserts;

public class SnatchTest {

	private static Logger logger = LoggerFactory.getLogger(SnatchTest.class);

	public static String get(String url, String charset) {
		HttpClient client = null;
		String content = null;
		try {
            client = new DefaultHttpClient();
            HttpConnectionParams.setConnectionTimeout(client.getParams(), 10 * 1000);
            HttpConnectionParams.setSoTimeout(client.getParams(), 10 * 1000);
			HttpGet get = new HttpGet(url);
			get.setHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.1.2)");
			get.setHeader("Accept-Language", "zh-cn,zh;q=0.5");
			get.setHeader("Accept-Charset", "GB2312,utf-8;q=0.7,*;q=0.7");
			
			HttpResponse response = client.execute(get);

			Integer[] codes = { 
			        HttpStatus.SC_MOVED_PERMANENTLY,
			        HttpStatus.SC_MOVED_TEMPORARILY,
					HttpStatus.SC_SEE_OTHER,
					HttpStatus.SC_TEMPORARY_REDIRECT
			};
			
			Integer status = response.getStatusLine().getStatusCode();
			if (Asserts.hasAny(status, codes)) {
				String location = response.getLastHeader("Location").getValue();
				client = new DefaultHttpClient();
				get = new HttpGet(location);
				response = client.execute(get);
			}

			HttpEntity entity = response.getEntity();
			if (entity != null) {
				byte[] bytes = EntityUtils.toByteArray(entity);
				content = new String(bytes, charset);
			}
		} catch (Exception e) {
			logger.error("(Snatchs:get) error: ", e);
		} finally {
			client.getConnectionManager().shutdown();
		}
		
		return content;
	}
	
	public static void main(String[] args) {
        System.out.println(get("http://edm.cn/snatch/sum", "UTF-8"));
    }
}
