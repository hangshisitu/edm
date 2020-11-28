package cn.edm.persistence;

import java.util.Map;

import cn.edm.domain.RoleMenu;


public interface RoleMenuMapper {
	
	/**
	 * 创建角色功能权限
	 * @param roleId
	 * @param menuId
	 * @param funcAuth
	 */
	void insertRoleMenu(RoleMenu roleMenu);
	
	/**
	 * 删除角色菜单关系
	 * @param roleId
	 */
	void delRoleMenuById(String roleId);
	
	/**
	 * 插入每个角色必选菜单功能，例如账户管理
	 */
	void insertRoleMenuByType(Map<String,Object> paramsMap);
}
