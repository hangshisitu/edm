package cn.edm.service.impl;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;




import cn.edm.domain.ApiTrigger;
import cn.edm.persistence.ApiTriggerMapper;
import cn.edm.service.ApiTriggerService;

/**
 * API触发业务接口实现
 * @author Luxh
 *
 */
@Service
public class ApiTriggerServiceImpl implements ApiTriggerService{
	
	@Autowired
	private ApiTriggerMapper apiTriggerMapper;
	
	@Override
	public ApiTrigger getByCorpId(Integer corpId) {
		return apiTriggerMapper.selectByCorpId(corpId);
	}

	@Override
	public List<String> getTriggerName() {
		return apiTriggerMapper.selectTriggerName();
	}

	
}
