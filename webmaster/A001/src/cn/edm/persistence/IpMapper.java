package cn.edm.persistence;


import cn.edm.domain.Ip;

public interface IpMapper {
	
	Ip selectByIp(String loginIp);
}
