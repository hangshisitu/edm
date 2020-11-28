package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Menu;

public interface MenuMapper {
	
	/**
	 * 查询全部功能菜单
	 * @return
	 */
	public List<Menu> selectMenuList();
	
	/**
	 * 根据当前账号所属角色查询出该角色对应的功能菜单列表
	 */
	public List<Menu> selectMenuByRoleId(int roleId);

}
