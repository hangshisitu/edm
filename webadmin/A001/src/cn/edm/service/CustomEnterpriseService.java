package cn.edm.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.CustomEnterprise;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class CustomEnterpriseService {
	
	@Autowired
    private Dao dao;
	
	public CustomEnterprise selectByCustomEnterpriseStatus(Integer status) {		
		return dao.get("CustomEnterprise.query", new MapBean("status",status));
	}
	
}
