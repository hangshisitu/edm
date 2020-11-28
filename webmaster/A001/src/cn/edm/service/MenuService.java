package cn.edm.service;

import java.util.List;

import cn.edm.domain.Menu;

public interface MenuService {
	
	/**
	 * 查询所有的菜单功能列表
	 */
	public List<Menu> getAllMenu();
	
	/**
	 * 根据当前账号所属角色查询出该角色对应的功能菜单列表
	 */
	public List<Menu> selectMenuByRoleId(int roleId);
	
}
