package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.google.common.collect.Maps;

import cn.edm.domain.Cnd;
import cn.edm.persistence.CndMapper;

@Service
public class CndServiceImpl implements CndService{
	
	@Autowired
	private CndMapper cndMapper;

	@Override
	public Map<Integer, Cnd> getCndByTaskIds(Integer[] taskIds) {
		Map<Integer,Cnd> resultMap = Maps.newHashMap();
		Map<String,Object> params = Maps.newHashMap();
		params.put("taskIds", taskIds);
		List<Cnd> cnds = cndMapper.selectByTaskIds(params);
		if(cnds!=null && cnds.size()>0) {
			for(Cnd c:cnds) {
				resultMap.put(c.getTaskId(), c);
			}
		}
		return resultMap;
	}
	
}
