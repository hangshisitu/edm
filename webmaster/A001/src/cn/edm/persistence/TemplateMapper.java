package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Template;



/**
 * @author Luxh
 */
public interface TemplateMapper {
	
	/**
	 * 根据模板id查询
	 * @param templateId
	 * @return
	 */
	Template selectByTemplateId(Integer templateId);
	
	List<Template> selectByIds(Map<String,Object> params);
	
	void delTemplateByUserId(String userId);
}
