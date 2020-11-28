package cn.edm.util;

import java.util.List;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import com.google.common.collect.Lists;

import cn.edm.constant.ConfigKeys;
import cn.edm.vo.Log;


public class Loggers {
	
	public static List<Log> open(String sid, String time, String mode, String encoding) {
		String logger = PropertiesUtil.get(ConfigKeys.DELIVERY_LOGGER) + "/open?taskid=" + sid + "&begintime=" + time + "&mode=" + mode;
		System.out.println("open url is:"+logger);
		String content = Apis.get(logger, encoding);
		return array(content);
	}
	
	public static List<Log> openCampaign(String[] sids, String[] times, String mode, String encoding) {
		List<String> actions = Lists.newArrayList();
		for(int i=0;i<sids.length;i++) {
			String sid = sids[i];
			String time = times[i];
			String logger = PropertiesUtil.get(ConfigKeys.DELIVERY_LOGGER) + "/open?taskid=" + sid + "&begintime=" + time + "&mode=" + mode;
			System.out.println("logger---"+logger);
			actions.add(logger);
		}
		return getCampaignLog(actions, encoding);
	}
	
	public static List<Log> click(String sid, String time, String mode, String encoding) {
		String logger = PropertiesUtil.get(ConfigKeys.DELIVERY_LOGGER) + "/click?taskid=" + sid + "&begintime=" + time + "&mode=" + mode;
		System.out.println("click url is:"+logger);
		String content = Apis.get(logger, encoding);
		return array(content);
	}
	
	private static List<Log> array(String content) {
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
	
	
	public static List<Log> getCampaignLog(List<String> actions, String encoding) {
		HttpClient client = null;
		String content = null;
		List<Log> logs = Lists.newArrayList();
		try {
			client = new DefaultHttpClient();
			for(int i=0;i<actions.size();i++) {
				String action = actions.get(i);
				HttpGet get = new HttpGet(action);
				HttpResponse response = client.execute(get);
				HttpEntity entity = response.getEntity();
				if (entity != null) {
					content = IOUtils.toString(entity.getContent(), encoding);
				}
				get.abort();
				logs.addAll(array(content));
			}
			
			
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			client.getConnectionManager().shutdown();
		}
		return logs;
	}
}
