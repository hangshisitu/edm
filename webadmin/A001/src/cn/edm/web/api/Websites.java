package cn.edm.web.api;

import cn.edm.utils.web.Apis;

public class Websites {

	public static String index(String domain) {
		return Apis.get(domain + "/index.php", 1500);
	}
}
