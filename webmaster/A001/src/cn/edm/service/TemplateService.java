package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.TaskSetting;
import cn.edm.domain.Template;

/**
 * 模板业务接口
 * @author Luxh
 */
public interface TemplateService {
	
	/**
	 * 根据模板id查询
	 * @param templateId
	 * @return
	 */
	Template getByTemplateId(Integer templateId);
	
	TaskSetting getByTaskId(Integer taskId);
	
	List<Template> getByIds(Integer[] ids);
	
	Map<Integer,String> getNameById(Integer[] ids);
}
