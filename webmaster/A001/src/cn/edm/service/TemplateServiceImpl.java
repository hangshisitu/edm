package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.google.common.collect.Maps;

import cn.edm.domain.TaskSetting;
import cn.edm.domain.Template;
import cn.edm.persistence.TaskSettingMapper;
import cn.edm.persistence.TemplateMapper;

/**
 * 模板业务接口实现
 * @author Luxh
 *
 */
@Service
public class TemplateServiceImpl implements TemplateService{
	
	@Autowired
	private TemplateMapper templateMapper;
	
	@Autowired
	private TaskSettingMapper taskSettingMapper;
	
	@Override
//	@Cacheable(cacheName="templateCache")
	public Template getByTemplateId(Integer templateId) {
		return templateMapper.selectByTemplateId(templateId);
	}

	@Override
	public TaskSetting getByTaskId(Integer taskId) {
		return taskSettingMapper.selectByTaskId(taskId);
	}

	@Override
	public List<Template> getByIds(Integer[] ids) {
		Map<String,Object> params = Maps.newHashMap();
		params.put("templateIds", ids);
		return templateMapper.selectByIds(params);
	}

	@Override
	public Map<Integer,String> getNameById(Integer[] ids) {
		List<Template> tems = getByIds(ids);
		Map<Integer,String> map = Maps.newHashMap();
		if(tems != null && tems.size()>0) {
			for(Template t:tems) {
				map.put(t.getTemplateId(), t.getTemplateName());
			}
		}
		return map;
	}

}
