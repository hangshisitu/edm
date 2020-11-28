package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Touch;

public interface TouchMapper {

	/**
	 * 根据parentId条件得到结果
	 * 		task表的taskId
	 * @return
	 */
	public List<Touch> selectByParentId(Integer parentId);
	
	public Touch selectTouch(Map<String, Object> map);
	
	public List<Touch> selectByParentIds(Map<String, Object> map);
	
}
