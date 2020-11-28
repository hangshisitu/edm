package cn.edm.util;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class PropertiesUtil {
	
	public static String get(String key) {
		String fileName = "/app.properties";
		InputStream in = PropertiesUtil.class.getResourceAsStream(fileName);
		Properties p = new Properties();
		try {
			p.load(in);
		} catch (IOException e) {
			e.printStackTrace();
		}finally {
			try {
				if(in != null) 
					in.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return p.getProperty(key);
	}
}
