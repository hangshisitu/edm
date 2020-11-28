package cn.edm.service;

import java.util.List;

import cn.edm.domain.TemplateSetting;

/**
 * 模板触发连接
 * 
 * @Date 2014年9月10日 下午3:32:15
 * @author Lich
 *
 */
public interface TemplateSettingService {

	/**
	 * 根据parentId条件得到结果
	 * 		parentId是主模板的模板ID
	 * @return
	 */
	public List<TemplateSetting> getByParentId(Integer parentId);
	/**
	 * 根据parentId条件并按模板分组得到结果
	 * 		parentId是主模板的模板ID
	 * @return
	 */
	public List<TemplateSetting> getByParentIds(Integer[] parentIds);
	
	public List<TemplateSetting> getSubByParentId(Integer templateId);
	
	public List<TemplateSetting> getByTemplateIds(Integer[] templateIds);
	
	
}
