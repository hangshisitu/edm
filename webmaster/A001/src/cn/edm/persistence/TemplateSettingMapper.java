package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.TemplateSetting;

public interface TemplateSettingMapper {
	/**
	 * 根据parentId条件得到结果
	 * 		parentId是主模板的模板ID
	 * @return
	 */
	public List<TemplateSetting> selectSetting(Map<String, Object> paramsMap);
	
	public void delTemplateSettingByUserId(String userId);
	
}
