package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Hour;

/**
 * 打开统计业务接口
 * @author Luxh
 *
 */
public interface HourService {
	
	/**
	 * 根据任务id查询
	 * @param taskId
	 * @return
	 */
	List<Hour> getByTaskId(Map<String,Object> paramsMap);
}
