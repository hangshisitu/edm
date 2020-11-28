package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.CustomEnterprise;

public interface CustomEnterpriseMapper {
	
	/**
	 * 根据id查询
	 * @param userId 
	 * @return
	 */
	CustomEnterprise selectByCustomEnterpriseId(String id);
	
	/**
	 * 分页查询记录数据
	 * @param paramsMap
	 * @return
	 */
	List<CustomEnterprise> selectCustomEnterprises(Map<String, Object> paramsMap);
	
	/**
	 * 查询记录数
	 * @param paramsMap
	 * @return
	 */
	long selectCustomEnterpriseCount(Map<String, Object> paramsMap);
	
	/**
	 * 更新logo状态
	 * @param paramsMap
	 */
	void updateCustomEnterpriseStatus(Map<String, Object> paramsMap);

	/**
	 * 添加账号
	 * @param user
	 */
	void insertCustomEnterprise(CustomEnterprise CustomEnterprise);		
    
	
	CustomEnterprise selectByCustomEnterpriseStatus(Integer status);
	
	void deleteCustomEnterpriseById(Map<String, Object> paramsMap);
	
}
