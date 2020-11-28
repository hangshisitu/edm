package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.Region;
import cn.edm.persistence.RegionMapper;

@Service
public class RegionServiceImpl implements RegionService{
	
	@Autowired
	private RegionMapper regionMapper;

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
//	@Cacheable(cacheName="reportCache")
	public List<Region> getByProvince(Map<String, Object> paramsMap) {
		return regionMapper.selectByProvince(paramsMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
//	@Cacheable(cacheName="reportCache")
	public List<Region> getByCity(Map<String, Object> paramsMap) {
		return regionMapper.selectByCity(paramsMap);
	}
	
}
