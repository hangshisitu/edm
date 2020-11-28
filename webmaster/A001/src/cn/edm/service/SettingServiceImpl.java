package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.google.common.collect.Maps;

import cn.edm.domain.Setting;
import cn.edm.persistence.SettingMapper;

@Service
public class SettingServiceImpl implements SettingService{
	
	@Autowired
	private SettingMapper settingMapper;

	@Override
	public Map<Integer, Setting> getSettingByTaskIds(Integer[] taskIds) {
		Map<Integer,Setting> resultMap = Maps.newHashMap();
		Map<String,Object> params = Maps.newHashMap();
		params.put("taskIds", taskIds);
		List<Setting> settings = settingMapper.selectByTaskIds(params);
		if(settings!=null && settings.size()>0) {
			for(Setting s:settings) {
				resultMap.put(s.getTaskId(), s);
			}
		}
		return resultMap;
	}
	
}
