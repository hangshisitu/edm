package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Setting;





public interface SettingMapper {
	
	List<Setting> selectByTaskIds(Map<String,Object> params);
}
