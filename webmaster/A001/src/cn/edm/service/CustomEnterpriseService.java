package cn.edm.service;

import java.util.Map;

import cn.edm.domain.CustomEnterprise;
import cn.edm.util.Pagination;


public interface CustomEnterpriseService {
	
	CustomEnterprise selectByCustomEnterpriseId(String id);
	
	CustomEnterprise selectByCustomEnterpriseStatus(Integer status);
	/**
	 * 分页查询logo
	 * @param paramsMap		查询参数
	 * @param orderBy		排序
	 * @param currentPage	当前页
	 * @param pageSize		每页显示数
	 * @return
	 */
	Pagination<CustomEnterprise> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 保存logo
	 * @param CustomEnterprise
	 */
	void save(CustomEnterprise customEnterprise);
	
	void updateCustomEnterpriseStatus(Map<String,Object> paramMap);
	
	boolean deleteCustomEnterpriseById(Map<String, Object> paramsMap);
	
}
