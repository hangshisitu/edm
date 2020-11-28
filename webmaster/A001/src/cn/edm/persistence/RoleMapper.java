package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Role;

public interface RoleMapper {
	
	List<Role> selectRoleList(Map<String,Object> paramsMap);
	
	Long selectRoleCount(Map<String,Object> paramsMap);
	
	void insertRole(Role role);
	
	void editRole(Role role);
	
	Role getRoleById(String id);
	
	void delRoleById(String id);
}
