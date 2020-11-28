package cn.edm.service;

import java.util.List;

import cn.edm.domain.ApiTrigger;

/**
 * API触发业务接口
 * @author Luxh
 *
 */
public interface ApiTriggerService {
	
	/**
	 * 根据企业id查询
	 * @param corpId
	 * @return
	 */
	ApiTrigger getByCorpId(Integer corpId);
	
	/**
	 * 查询触发类型
	 * @return
	 */
	List<String> getTriggerName();
}
