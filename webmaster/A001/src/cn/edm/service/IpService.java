package cn.edm.service;

import cn.edm.domain.Ip;

public interface IpService {
	
	Ip getByIp(String loginIp);
}
