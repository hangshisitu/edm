package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.Url;
import cn.edm.persistence.UrlMapper;


/**
 * 链接统计业务接口实现
 * @author Luxh
 *
 */
@Service
public class UrlServiceImpl implements UrlService{
	
	@Autowired
	private UrlMapper urlMapper;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
//	@Cacheable(cacheName="reportCache")
	public List<Url> getByTaskId(Map<String,Object> paramsMap) {
		return urlMapper.selectByTaskId(paramsMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
//	@Cacheable(cacheName="reportCache")
	public Url getClickCountByTaskId(Map<String,Object> paramsMap) {
		return urlMapper.selectClickCountByTaskId(paramsMap);
	}

}
