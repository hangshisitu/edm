package test.edm.api;

import java.io.BufferedReader;
import java.io.IOException;
import java.net.URI;

import org.apache.http.Header;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpRequestBase;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.junit.Test;

public class SafeTest {

	@Test
	public void safe() {
		HttpClient client = null;
		BufferedReader reader = null;
		try {
			client = new DefaultHttpClient();

			HttpRequestBase request = new HttpRequestBase() {
				@Override
				public String getMethod() {
					return "TRACK";
				}
			};

			request.setURI(new URI("http://edm.cn"));

			HttpResponse response = client.execute(request);
			for (Header header : response.getAllHeaders()) {
				System.out.println(header.getName() + ":" + header.getValue());
			}
			System.out.println(response.toString());
			System.out.println(EntityUtils.toString(response.getEntity()));
			
			request.abort();
		} catch (Exception e) {
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
			client.getConnectionManager().shutdown();
		}
	}
}
