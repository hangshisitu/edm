package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.google.common.collect.Maps;

import cn.edm.domain.Touch;
import cn.edm.persistence.TouchMapper;
import cn.edm.service.TouchService;

@Service
public class TouchServiceImpl implements TouchService {

	@Autowired
	private TouchMapper touchMapper;
	
	@Override
	public List<Touch> getByParentId(Integer parentId) {
		return touchMapper.selectByParentId(parentId);
	}

	@Override
	public Touch getTouch(Integer corpId, Integer touchId, Integer templateId) {
		Map<String, Object> map = Maps.newHashMap();
		map.put("corpId", corpId);
		map.put("taskId", touchId);
		map.put("templateId", templateId);
		return touchMapper.selectTouch(map);
	}

	@Override
	public List<Touch> getByParentIds(Integer[] parentIds) {
		Map<String, Object> map = Maps.newHashMap();
		map.put("parentIds", parentIds);
		return touchMapper.selectByParentIds(map);
	}

}
