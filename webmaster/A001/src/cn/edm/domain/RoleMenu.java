package cn.edm.domain;

import java.io.Serializable;

public class RoleMenu implements Serializable {
	
	private static final long serialVersionUID = 1922085053903136016L;
	
	private String id;
	
	private int roleId;//角色表主键
	
	private int menuId;//菜单表主键			
		
	private String funcAuth;//菜单功能增删查改权限（1,2,3,4,5,6）=查询、增加、修改、删除、监管、导出

	
	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public int getRoleId() {
		return roleId;
	}

	public void setRoleId(int roleId) {
		this.roleId = roleId;
	}

	public int getMenuId() {
		return menuId;
	}

	public void setMenuId(int menuId) {
		this.menuId = menuId;
	}

	public String getFuncAuth() {
		return funcAuth;
	}

	public void setFuncAuth(String funcAuth) {
		this.funcAuth = funcAuth;
	}


}
