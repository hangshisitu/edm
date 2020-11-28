package cn.edm.web.api;

import java.util.List;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Config;
import cn.edm.modules.utils.Property;
import cn.edm.web.api.dto.Log;

import com.google.common.collect.Lists;

public class Loggers {

	public static String open(String sid, String time, String mode) {
		return Property.getStr(Config.DELIVERY_LOGGER) + "/open?taskid=" + sid + "&begintime=" + time + "&mode=" + mode;
	}
	
	public static String click(String sid, String time, String mode) {
		return Property.getStr(Config.DELIVERY_LOGGER) + "/click?taskid=" + sid + "&begintime=" + time + "&mode=" + mode;
	}
	
	public static List<Log> convert(String content) {
		if (StringUtils.isBlank(content)) {
			return null;
		}

		List<Log> loggerList = Lists.newArrayList();
		int count = 0;
		for (String line : StringUtils.split(content, "\r\n")) {
			count++;
			if (count == 1) {
				continue;
			}

			String[] records = StringUtils.splitPreserveAllTokens(line, ",");
			if (records.length < 5 || StringUtils.isBlank(records[0])) {
				continue;
			}
			
			Log logger = new Log();
			logger.setEmail(records[0]);
			logger.setRegion(records[3]);
			logger.setTime(records[1] + " " + StringUtils.substringBeforeLast(records[2], ":"));
			loggerList.add(logger);
		}
		
		return loggerList;
	}
}
