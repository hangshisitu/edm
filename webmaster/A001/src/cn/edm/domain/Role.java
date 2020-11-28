package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;
import java.util.List;

public class Role implements Serializable {
	
	private static final long serialVersionUID = 1922085053903136016L;
	
	private String id;
	
	private String roleName;//角色名称
	
	private String roleDesc;//角色描述		
	
	private int type = 0;//角色类型，如：0-->普通角色；1-->管理员		

	private Date createTime;//创建时间
	
	private Date modifyTime;//操作时间
		
	private String operator;//操作者
	
	private List<Menu> menuList;//角色拥有的菜单列表

	private List<String> menuFuncList;//角色拥有的功能列表
	
	public List<String> getMenuFuncList() {
		return menuFuncList;
	}

	public void setMenuFuncList(List<String> menuFuncList) {
		this.menuFuncList = menuFuncList;
	}

	public List<Menu> getMenuList() {
		return menuList;
	}

	public void setMenuList(List<Menu> menuList) {
		this.menuList = menuList;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}
	
	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getRoleName() {
		return roleName;
	}

	public void setRoleName(String roleName) {
		this.roleName = roleName;
	}

	public String getRoleDesc() {
		return roleDesc;
	}

	public void setRoleDesc(String roleDesc) {
		this.roleDesc = roleDesc;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public String getOperator() {
		return operator;
	}

	public void setOperator(String operator) {
		this.operator = operator;
	}
	
	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}
	
}
