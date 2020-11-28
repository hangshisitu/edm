package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Role;
import cn.edm.domain.RoleMenu;
import cn.edm.util.Pagination;

/**
 * 角色管理
 * @author nico
 *
 */
public interface RoleService {
	
	/**
	 * 分页查询用户
	 * @param paramsMap		查询参数
	 * @param orderBy		排序
	 * @param currentPage	当前页
	 * @param pageSize		每页显示数
	 * @return
	 */
	Pagination<Role> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 保存角色
	 * @param role
	 */
	public void saveRole(Role role);
	/**
	 * 编辑角色
	 * @param role
	 */
	public void editRole(Role role);
	
	public Role getRoleById(String id);
	
	public void delRoleById(String id);
	
	/**
	 * 
	 * @param roleMenu
	 */
	public void saveRoleMenu(String roleId,List<RoleMenu> roleMenuList);
	
	/**
	 * 
	 * @param roleMenu
	 */
	public void delRoleMenu(String roleId);
	
	/**
	 * 查询所有角色列表
	 */
	public List<Role> getAllRole();
	
	public List<Role> getRoleByType(int[] types);
    /**
     * 根据角色名称查询角色信息
     */
	List<Role> getRoleByRoleName(String roleName);
	/**
	 * 校验输入内容合法性
	 * @param role
	 * @return
	 */
	public Map<String, Object> verifyXss(Role role);
}
