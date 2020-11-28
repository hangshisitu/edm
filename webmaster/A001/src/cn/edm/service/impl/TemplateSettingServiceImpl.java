package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.google.common.collect.Maps;

import cn.edm.domain.TemplateSetting;
import cn.edm.persistence.TemplateSettingMapper;
import cn.edm.service.TemplateSettingService;

@Service
public class TemplateSettingServiceImpl implements TemplateSettingService {

	@Autowired
	private TemplateSettingMapper settingMapper;
	
	@Override
	public List<TemplateSetting> getByParentId(Integer parentId) {
		Map<String,Object> params = Maps.newHashMap();
		params.put("parentId", parentId);
		return settingMapper.selectSetting(params);
	}

	@Override
	public List<TemplateSetting> getByParentIds(Integer[] parentIds) {
		Map<String,Object> params = Maps.newHashMap();
		params.put("parentIds", parentIds);
		return settingMapper.selectSetting(params);
	}

	@Override
	public List<TemplateSetting> getSubByParentId(Integer templateId) {
		Map<String,Object> params = Maps.newHashMap();
		params.put("parentId", templateId);
		return settingMapper.selectSetting(params);
	}

	@Override
	public List<TemplateSetting> getByTemplateIds(Integer[] templateIds) {
		Map<String,Object> params = Maps.newHashMap();
		params.put("templateIds", templateIds);
		return settingMapper.selectSetting(params);
	}

}
