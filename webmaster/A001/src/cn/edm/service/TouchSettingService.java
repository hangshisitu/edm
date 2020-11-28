package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.TouchSetting;

public interface TouchSettingService {

	public List<TouchSetting> getSettings(Map<String, Object> paramsMap, Integer parentId, Integer taskId);
	
	public List<TouchSetting> getSettings(Map<String, Object> paramsMap, Integer parentId, Integer[] taskIds);
	
	public TouchSetting getSetting(Map<String, Object> paramsMap, Integer templateId, Integer taskId);
	
}
