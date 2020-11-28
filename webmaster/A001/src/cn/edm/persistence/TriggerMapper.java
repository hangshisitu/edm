package cn.edm.persistence;


import java.util.Map;

import cn.edm.domain.Trigger;

/**
 * @author Luxh
 */
public interface TriggerMapper {
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	Trigger selectByTaskId(Integer taskId);

	public Trigger selectByParams(Map<String, Object> paramsMap);
	
}
