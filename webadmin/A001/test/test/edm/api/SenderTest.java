package test.edm.api;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.junit.Test;

public class SenderTest {

	@Test
	public void send() {
		HttpClient client = null;
		BufferedReader reader = null;
		try {
			client = new DefaultHttpClient();
			HttpGet get = new HttpGet("http://139edm.com:55669/success?taskid=259.10933.6996&begintime=20130415&mode=detail");
			HttpResponse response = client.execute(get);
			HttpEntity entity = response.getEntity();
			if (entity != null) {
				reader = new BufferedReader(new InputStreamReader(entity.getContent(), "gbk"));
				String line = null;
				while ((line = reader.readLine()) != null) {
					System.out.println(line);
				}
			}
			get.abort();
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
