package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Url;


/**
 * 链接统计业务接口
 * @author Luxh
 *
 */
public interface UrlService {
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	List<Url> getByTaskId(Map<String,Object> paramsMap);
	
	/**
	 * 根据任务id查询该任务的链接点击总数
	 * @param taskId
	 * @return
	 */
	Url getClickCountByTaskId(Map<String,Object> paramsMap);
}
