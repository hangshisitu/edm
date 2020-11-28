package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.TouchSetting;
import cn.edm.persistence.TouchSettingMapper;
import cn.edm.service.TouchSettingService;

@Service
public class TouchSettingServiceImpl implements TouchSettingService {

	@Autowired
	private TouchSettingMapper settingMapper;
	
	@Override
	public List<TouchSetting> getSettings(Map<String, Object> paramsMap, Integer parentId, Integer taskId) {
		paramsMap.put("parentId", parentId);
		paramsMap.put("touchParentId", taskId);
		return settingMapper.selectSettingList(paramsMap);
	}
	
	@Override
	public List<TouchSetting> getSettings(Map<String, Object> paramsMap, Integer parentId, Integer[] taskIds) {
		paramsMap.put("parentId", parentId);
		paramsMap.put("touchParentIds", taskIds);
		return settingMapper.selectSettingList(paramsMap);
	}

	@Override
	public TouchSetting getSetting(Map<String, Object> paramsMap, Integer templateId, Integer taskId) {
		paramsMap.put("templateId", templateId);
		paramsMap.put("taskId", taskId);
		return settingMapper.selectSetting(paramsMap);
	}

}
