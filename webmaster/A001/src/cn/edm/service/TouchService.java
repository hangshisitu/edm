package cn.edm.service;

import java.util.List;

import cn.edm.domain.Touch;

/**
 * 触发任务接口
 * 
 * @Date 2014年9月10日 下午3:11:26
 * @author Lich
 *
 */
public interface TouchService {

	/**
	 * 
	 * @param parentId
	 * 			task表的taskId
	 * @return
	 */
	public List<Touch> getByParentId(Integer parentId);
	
	public Touch getTouch(Integer corpId, Integer touchId, Integer templateId);
	
	public List<Touch> getByParentIds(Integer[] parentIds);
	
}
