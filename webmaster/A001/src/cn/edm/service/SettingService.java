package cn.edm.service;

import java.util.Map;

import cn.edm.domain.Setting;

public interface SettingService {
	
	Map<Integer,Setting> getSettingByTaskIds(Integer[] taskIds);
}
