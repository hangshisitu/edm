package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.TouchSetting;

public interface TouchSettingMapper {

	public List<TouchSetting> selectSettingList(Map<String, Object> paramsMap);
	
	public TouchSetting selectSetting(Map<String, Object> paramsMap);
	
}
