package cn.edm.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Ip;
import cn.edm.persistence.IpMapper;

@Service
public class IpServiceImpl implements IpService{
	
	@Autowired
	private IpMapper ipMapper;
	
	@Override
//	@Cacheable(cacheName="ipCache")
	public Ip getByIp(String loginIp) {
		return ipMapper.selectByIp(loginIp);
	}

}
