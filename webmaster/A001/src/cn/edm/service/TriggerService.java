package cn.edm.service;


import cn.edm.domain.Trigger;

/**
 * API触发业务接口
 * @author Luxh
 *
 */
public interface TriggerService {
	
	/**
	 * 根据任务id查询
	 * @param corpId
	 * @return
	 */
	Trigger getByTaskId(Integer taskId);
	/**
	 * 通过三个ID获得唯一触发任务信息
	 * @param taskId
	 * 			任务ID
	 * @param corpId
	 * 			企业ID
	 * @param templateId
	 * 			模板ID
	 * @return
	 */
	public Trigger getByParams(Integer taskId, Integer corpId, Integer templateId);
	
}
