package cn.edm.service;

import java.util.HashMap;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Trigger;
import cn.edm.persistence.TriggerMapper;

@Service
public class TriggerServiceImpl implements TriggerService{
	
	@Autowired
	private TriggerMapper triggerMapper;
	
	@Override
	public Trigger getByTaskId(Integer taskId) {
		// TODO Auto-generated method stub
		return triggerMapper.selectByTaskId(taskId);
	}

	@Override
	public Trigger getByParams(Integer taskId, Integer corpId, Integer templateId) {
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		paramsMap.put("taskId", taskId);
		paramsMap.put("corpId", corpId);
		paramsMap.put("templateId", templateId);
		
		return triggerMapper.selectByParams(paramsMap);
	}

}
