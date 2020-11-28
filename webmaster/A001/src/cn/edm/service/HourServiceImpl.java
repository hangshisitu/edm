package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.Hour;
import cn.edm.persistence.HourMapper;

/**
 * 打开统计业务接口实现
 * @author Luxh
 *
 */

@Service
public class HourServiceImpl implements HourService{
	
	@Autowired
	private HourMapper hourMapper;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
//	@Cacheable(cacheName="reportCache")
	public List<Hour> getByTaskId(Map<String,Object> paramsMap) {
		return hourMapper.selectByTaskId(paramsMap);
	}

}
