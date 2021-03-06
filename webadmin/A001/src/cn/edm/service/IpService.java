package cn.edm.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Ip;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class IpService {

	@Autowired
	private Dao dao;

	public Ip get(String ip) {
		return dao.get("Ip.query", new MapBean("ip", ip));
	}
}
