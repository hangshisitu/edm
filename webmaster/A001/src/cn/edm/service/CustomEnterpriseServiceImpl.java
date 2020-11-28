package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.CustomEnterprise;
import cn.edm.persistence.CustomEnterpriseMapper;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;

@Service("customEnterpriseService")
public class CustomEnterpriseServiceImpl implements CustomEnterpriseService{
	
	private static Logger log = LoggerFactory.getLogger(CustomEnterpriseServiceImpl.class);
	
	@Autowired
	private CustomEnterpriseMapper customEnterpriseMapper;

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public CustomEnterprise selectByCustomEnterpriseId(String id) {
		return customEnterpriseMapper.selectByCustomEnterpriseId(id);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<CustomEnterprise> getPagination(Map<String, Object> paramsMap, String orderBy, int currentPage,
			int pageSize) {
		
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);	
		
		List<CustomEnterprise> recordList = customEnterpriseMapper.selectCustomEnterprises(paramsMap);
		long recordCount = customEnterpriseMapper.selectCustomEnterpriseCount(paramsMap);
		
		Pagination<CustomEnterprise> pagination = new Pagination<CustomEnterprise>(currentPage,pageSize,recordCount,recordList);		
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void save(CustomEnterprise customEnterprise) {
		customEnterpriseMapper.insertCustomEnterprise(customEnterprise);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void updateCustomEnterpriseStatus(Map<String, Object> paramMap) {
		customEnterpriseMapper.updateCustomEnterpriseStatus(paramMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public CustomEnterprise selectByCustomEnterpriseStatus(Integer status) {		
		return customEnterpriseMapper.selectByCustomEnterpriseStatus(status);
	}

	@Override
	public boolean deleteCustomEnterpriseById(Map<String, Object> paramsMap) {
		try{
		   customEnterpriseMapper.deleteCustomEnterpriseById(paramsMap);
		   return true;
		}catch(Exception e){
			log.info("delete logo fail ...");
		   return false;
		}
	}

}
