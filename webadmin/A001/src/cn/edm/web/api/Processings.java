package cn.edm.web.api;

import cn.edm.consts.Config;
import cn.edm.modules.utils.Property;
import cn.edm.utils.web.Apis;
import cn.edm.web.facade.Tasks;

public class Processings {
    
	public static String processing(String path) {
		String delivery = Property.getStr(Config.DELIVERY_ENGINE) + "?filePath=" + path;
		return Apis.get(delivery);
	}
	
	public static String pause(Integer corpId, Integer taskId, Integer templateId) {
		String delivery = Property.getStr(Config.DELIVERY_ENGINE) + "?pause=" + Tasks.id(corpId, taskId, templateId);
		return Apis.get(delivery);
	}
	
	public static String restart(Integer corpId, Integer taskId, Integer templateId) {
		String delivery = Property.getStr(Config.DELIVERY_ENGINE) + "?restart=" + Tasks.id(corpId, taskId, templateId);
		return Apis.get(delivery);
	}
	
	public static String stop(Integer corpId, Integer taskId, Integer templateId) {
		String delivery = Property.getStr(Config.DELIVERY_ENGINE) + "?stop=" + Tasks.id(corpId, taskId, templateId);
		return Apis.get(delivery);
	}
	
	public static String touching(String path) {
        String delivery = Property.getStr(Config.TOUCH_ENGINE) + "?filePath=" + path;
        return Apis.get(delivery);
    }
}
