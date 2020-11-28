package cn.edm.util;

import java.util.List;

import org.apache.commons.lang.StringUtils;

import cn.edm.constants.Config;
import cn.edm.vo.Send;

import com.google.common.collect.Lists;

public class Senders {

	public static String get(String sid, String action) {
		return PropertiesUtil.get(Config.DELIVERY_SENDER) + "/" + action + "?taskid=" + sid;
	}
	
	public static List<Send> send(String sid, String action, String encoding) {
		String content = Apis.get(get(sid, action), encoding);
		return convert(content);
	}
	
	public static List<Send> convert(String content) {
		if (StringUtils.isBlank(content)) {
			return null;
		}

		List<Send> sendList = Lists.newArrayList();
		int count = 0;
		for (String line : StringUtils.split(content, "\r\n")) {
			count++;
			if (count == 1) {
				continue;
			}

			String[] records = StringUtils.splitPreserveAllTokens(line, ",");
			if (records.length < 4 || StringUtils.isBlank(records[0])) {
				continue;
			}
			
			Send send = new Send();
			send.setEmail(records[0]);
			send.setTime(records[1] + " " + StringUtils.substringBeforeLast(records[2], ":"));
			sendList.add(send);
		}
		
		return sendList;
	}
}
