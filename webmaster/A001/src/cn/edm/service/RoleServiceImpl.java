package cn.edm.service;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.google.common.collect.Maps;

import cn.edm.constant.Message;
import cn.edm.domain.Menu;
import cn.edm.domain.Role;
import cn.edm.domain.RoleMenu;
import cn.edm.persistence.MenuMapper;
import cn.edm.persistence.RoleMapper;
import cn.edm.persistence.RoleMenuMapper;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;

/**
 * 角色管理
 * 
 * @author nico
 * 
 */
@Service
public class RoleServiceImpl implements RoleService {

	@Autowired
	private RoleMapper roleMapper;

	@Autowired
	private RoleMenuMapper roleMenuMapper;

	@Autowired
	private MenuMapper menuMapper;

	@Override
	@Transactional(propagation = Propagation.REQUIRED, readOnly = true)
	public Pagination<Role> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {

		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		int[] types = { 0, 1 };
		paramsMap.put("types", types);

		List<Role> roleList = roleMapper.selectRoleList(paramsMap);
		if (roleList != null && roleList.size() != 0) {// 查询该角色下所有的菜单功能列表
			for (Role role : roleList) {
				List<Menu> menuList = menuMapper.selectMenuByRoleId(Integer.valueOf(role.getId()));
				Collections.sort(menuList);// 根据父节点排序
				List<String> menuFuncList = showFuncList(menuList);//
				role.setMenuFuncList(menuFuncList);
			}

		}
		long recordCount = roleMapper.selectRoleCount(paramsMap);

		Pagination<Role> pagination = new Pagination<Role>(currentPage,
				pageSize, recordCount, roleList);

		return pagination;
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public void saveRole(Role role) {
		role.setCreateTime(new Date());
		role.setModifyTime(new Date());
		roleMapper.insertRole(role);

	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public void editRole(Role role) {
		role.setModifyTime(new Date());
		roleMapper.editRole(role);
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public Role getRoleById(String id) {
		return roleMapper.getRoleById(id);
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public void delRoleById(String id) {
		roleMapper.delRoleById(id);
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public void saveRoleMenu(String roleId, List<RoleMenu> roleMenuList) {
		for (RoleMenu roleMenu : roleMenuList) {
			roleMenuMapper.insertRoleMenu(roleMenu);
		}
		Map<String, Object> params = Maps.newHashMap();
		params.put("roleId", roleId);
		params.put("type", "1");
		roleMenuMapper.insertRoleMenuByType(params);// 插入必选功能菜单
	}

	@Override
	@Transactional(propagation = Propagation.REQUIRED)
	public void delRoleMenu(String roleId) {
		roleMenuMapper.delRoleMenuById(roleId);

	}

	@Override
	public List<Role> getAllRole() {
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		int[] types = { 0, 1 };
		paramsMap.put("types", types);
		return roleMapper.selectRoleList(paramsMap);
	}

	@Override
	public List<Role> getRoleByType(int[] types) {
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		paramsMap.put("types", types);
		return roleMapper.selectRoleList(paramsMap);
	}
	
	@Override
	public List<Role> getRoleByRoleName(String roleName) {
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		paramsMap.put("roleName", roleName);
		return roleMapper.selectRoleList(paramsMap);
	}

	// 获取页面选择的功能列表入库,funcString = M_1,M_2,M_5,M_13,
	public static List<RoleMenu> getFunc(String roleId, String funcString) {
		List<RoleMenu> roleMenuList = new ArrayList<RoleMenu>();
		String allFunc = "0";// 0代表菜单下所有功能模块均可使用
		// 主菜单-数据管理
		if (funcString.indexOf(Menu.MENU_DATASOURCE_TAG + ",") != -1
				|| funcString.indexOf(Menu.MENU_DATASOURCE_PROP + ",") != -1
				|| funcString.indexOf(Menu.MENU_DATASOURCE_FILTER + ",") != -1
				|| funcString.indexOf(Menu.MENU_DATASOURCE_SELECTION + ",") != -1
				|| funcString.indexOf(Menu.MENU_DATASOURCE_SELECTION + ",") != -1) {

			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 数据管理-收件人管理
		if (funcString.indexOf(Menu.MENU_DATASOURCE_TAG + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE_TAG_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}
		// 数据管理-属性管理
		if (funcString.indexOf(Menu.MENU_DATASOURCE_PROP + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE_PROP_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}
		// 数据管理-过滤器管理
		if (funcString.indexOf(Menu.MENU_DATASOURCE_FILTER + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE_FILTER_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}
		// 数据管理-收件人筛选与导出
		if (funcString.indexOf(Menu.MENU_DATASOURCE_SELECTION + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE_SELECTION_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}
		// 数据管理-表单管理
		if (funcString.indexOf(Menu.MENU_DATASOURCE_FORM + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_DATASOURCE_FORM_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 主菜单-邮件管理
		if (funcString.indexOf(Menu.MENU_MAIL_TEMPLATE_ADD + ",") != -1
				|| funcString.indexOf(Menu.MENU_MAIL_TEMPLATE_QUERY + ",") != -1
				|| funcString.indexOf(Menu.MENU_MAIL_TASK + ",") != -1
				|| funcString.indexOf(Menu.MENU_MAIL_TASK_CONTROL + ",") != -1
				|| funcString.indexOf(Menu.MENU_MAIL_TASK + ",") != -1
				|| funcString.indexOf(Menu.MENU_MAIL_CAMPAIGN + ",") != -1) {

			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 邮件管理-模板设计与管理
		if (funcString.indexOf(Menu.MENU_MAIL_TEMPLATE_ADD + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_TEMPLATE_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}// 邮件管理-模板预览
		else if (funcString.indexOf(Menu.MENU_MAIL_TEMPLATE_QUERY + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_TEMPLATE_ID);
			roleMenu.setFuncAuth("1");
			roleMenuList.add(roleMenu);
		}
		// 邮件管理-任务创建和邮件管理-任务投递监控管理
		if (funcString.indexOf(Menu.MENU_MAIL_TASK + ",") != -1
				&& funcString.indexOf(Menu.MENU_MAIL_TASK_CONTROL + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_TASK_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}// 邮件管理-任务创建
		else if (funcString.indexOf(Menu.MENU_MAIL_TASK + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_TASK_ID);
			roleMenu.setFuncAuth("1,2,3");
			roleMenuList.add(roleMenu);
		}// 邮件管理-任务投递监控管理
		else if (funcString.indexOf(Menu.MENU_MAIL_TASK_CONTROL + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_TASK_ID);
			roleMenu.setFuncAuth("4,5");
			roleMenuList.add(roleMenu);
		}
		// 邮件管理-任务审核
		if (funcString.indexOf(Menu.MENU_MAIL_AUDIT + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_AUDIT_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}
		// 邮件管理-活动创建与管理
		if (funcString.indexOf(Menu.MENU_MAIL_CAMPAIGN + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_MAIL_CAMPAIGN_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 主菜单-数据管理
		if (funcString.indexOf(Menu.MENU_REPORT_EXPORT + ",") != -1
				|| funcString.indexOf(Menu.MENU_REPORT_QUERY + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_REPORT);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 数据报告-报告导出
		if (funcString.indexOf(Menu.MENU_REPORT_EXPORT + ",") != -1) {
			// 总览
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setFuncAuth(allFunc);
			roleMenu.setMenuId(Menu.MENU_REPORT_LOCAL_ID);
			roleMenuList.add(roleMenu);
			// 任务
			RoleMenu roleMenu1 = new RoleMenu();
			roleMenu1.setRoleId(Integer.valueOf(roleId));
			roleMenu1.setFuncAuth(allFunc);
			roleMenu1.setMenuId(Menu.MENU_REPORT_TASK_ID);
			roleMenuList.add(roleMenu1);
			// API
			RoleMenu roleMenu2 = new RoleMenu();
			roleMenu2.setRoleId(Integer.valueOf(roleId));
			roleMenu2.setFuncAuth(allFunc);
			roleMenu2.setMenuId(Menu.MENU_REPORT_API_ID);
			roleMenuList.add(roleMenu2);
			// 活动
			RoleMenu roleMenu3 = new RoleMenu();
			roleMenu3.setRoleId(Integer.valueOf(roleId));
			roleMenu3.setFuncAuth(allFunc);
			roleMenu3.setMenuId(Menu.MENU_REPORT_CAMPAIGN_ID);
			roleMenuList.add(roleMenu3);
			// 触发计划
			RoleMenu roleMenu4 = new RoleMenu();
			roleMenu4.setRoleId(Integer.valueOf(roleId));
			roleMenu4.setFuncAuth(allFunc);
			roleMenu4.setMenuId(Menu.MENU_REPORT_TOUCH_ID);
			roleMenuList.add(roleMenu4);

		}// 数据报告-报告查阅
		else if (funcString.indexOf(Menu.MENU_REPORT_QUERY + ",") != -1) {
			// 总览
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setFuncAuth("1");
			roleMenu.setMenuId(Menu.MENU_REPORT_LOCAL_ID);
			roleMenuList.add(roleMenu);
			// 任务
			RoleMenu roleMenu1 = new RoleMenu();
			roleMenu1.setRoleId(Integer.valueOf(roleId));
			roleMenu1.setFuncAuth("1");
			roleMenu1.setMenuId(Menu.MENU_REPORT_TASK_ID);
			roleMenuList.add(roleMenu1);
			// API
			RoleMenu roleMenu2 = new RoleMenu();
			roleMenu2.setRoleId(Integer.valueOf(roleId));
			roleMenu2.setFuncAuth("1");
			roleMenu2.setMenuId(Menu.MENU_REPORT_API_ID);
			roleMenuList.add(roleMenu2);
			// 活动
			RoleMenu roleMenu3 = new RoleMenu();
			roleMenu3.setRoleId(Integer.valueOf(roleId));
			roleMenu3.setFuncAuth("1");
			roleMenu3.setMenuId(Menu.MENU_REPORT_CAMPAIGN_ID);
			roleMenuList.add(roleMenu3);
			// 触发计划
			RoleMenu roleMenu4 = new RoleMenu();
			roleMenu4.setRoleId(Integer.valueOf(roleId));
			roleMenu4.setFuncAuth("1");
			roleMenu4.setMenuId(Menu.MENU_REPORT_TOUCH_ID);
			roleMenuList.add(roleMenu4);
		}

		// 账户管理-子账号
		if (funcString.indexOf(Menu.MENU_ACCOUNT_USER + ",") != -1) {
			RoleMenu roleMenu = new RoleMenu();
			roleMenu.setRoleId(Integer.valueOf(roleId));
			roleMenu.setMenuId(Menu.MENU_ACCOUNT_USER_ID);
			roleMenu.setFuncAuth(allFunc);
			roleMenuList.add(roleMenu);
		}

		// 主菜单-账户管理
		RoleMenu roleMenu = new RoleMenu();
		roleMenu.setRoleId(Integer.valueOf(roleId));
		roleMenu.setMenuId(Menu.MENU_ACCOUNT);
		roleMenu.setFuncAuth(allFunc);
		roleMenuList.add(roleMenu);

		return roleMenuList;
	}

	// 编辑角色模块设置check该角色所有功能按钮
	public static String setFuncChecked(List<Menu> menuList) {
		StringBuffer sb = new StringBuffer();
		for (Menu menu : menuList) {
			// 数据管理-收件人管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_TAG_ID))) {
				sb.append(Menu.MENU_DATASOURCE_TAG + ",");//
			}
			// 数据管理-属性管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_PROP_ID))) {
				sb.append(Menu.MENU_DATASOURCE_PROP + ",");//
			}
			// 数据管理-过滤器管理
			if (menu.getId().equals(
					String.valueOf(Menu.MENU_DATASOURCE_FILTER_ID))) {
				sb.append(Menu.MENU_DATASOURCE_FILTER + ",");//
			}
			// 数据管理-收件人筛选与导出
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_SELECTION_ID))) {
				sb.append(Menu.MENU_DATASOURCE_SELECTION + ",");//
			}
			// 数据管理-表单管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_FORM_ID))) {
				sb.append(Menu.MENU_DATASOURCE_FORM + ",");//
			}

			// 邮件管理-模板设计与管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TEMPLATE_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				sb.append(Menu.MENU_MAIL_TEMPLATE_ADD + ",");//
				sb.append(Menu.MENU_MAIL_TEMPLATE_QUERY + ",");//
			}// 邮件管理-模板预览
			else if (menu.getId().equals(
					String.valueOf(Menu.MENU_MAIL_TEMPLATE_ID))
					&& menu.getFuncAuth().indexOf("1") != -1) {
				sb.append(Menu.MENU_MAIL_TEMPLATE_QUERY + ",");//
			}
			// 邮件管理-任务创建和任务投递监控管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				sb.append(Menu.MENU_MAIL_TASK + ",");//
				sb.append(Menu.MENU_MAIL_TASK_CONTROL + ",");//
			}// 邮件管理-任务投递监控管理
			else if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("5") != -1) {
				sb.append(Menu.MENU_MAIL_TASK_CONTROL + ",");//
			}// 邮件管理-任务创建
			else if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("2") != -1) {
				sb.append(Menu.MENU_MAIL_TASK + ",");//
			}
			// 邮件管理-审核
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_AUDIT_ID))) {
				sb.append(Menu.MENU_MAIL_AUDIT + ",");//
			}
			// 邮件管理-活动创建与管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_CAMPAIGN_ID))) {
				sb.append(Menu.MENU_MAIL_CAMPAIGN + ",");//
			}

			// 数据报告-导出
			if (menu.getId().equals(String.valueOf(Menu.MENU_REPORT_LOCAL_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				sb.append(Menu.MENU_REPORT_QUERY + ",");//
				sb.append(Menu.MENU_REPORT_EXPORT + ",");//
			}// 数据报告-查阅
			else if (menu.getId().equals(
					String.valueOf(Menu.MENU_REPORT_LOCAL_ID))
					&& menu.getFuncAuth().indexOf("1") != -1) {
				sb.append(Menu.MENU_REPORT_QUERY + ",");//
			}

			// 管理员-子账号
			if (menu.getId().equals(String.valueOf(Menu.MENU_ACCOUNT_USER_ID))) {
				sb.append(Menu.MENU_ACCOUNT_USER + ",");//
			}

		}

		return sb.toString();
	}
	
	// 角色列表显示该角色有什么功能
	private static List<String> showFuncList(List<Menu> menuList) {
		List<String> list = new ArrayList<String>();
		for (Menu menu : menuList) {
			// 数据管理-收件人管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_TAG_ID))) {
				list.add("数据管理-收件人管理");
			}
			// 数据管理-属性管理
			if (menu.getId().equals( String.valueOf(Menu.MENU_DATASOURCE_PROP_ID))) {
				list.add("数据管理-属性管理");
			}
			// 数据管理-过滤器管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_FILTER_ID))) {
				list.add("数据管理-过滤器管理");
			}
			// 数据管理-收件人筛选与导出
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_SELECTION_ID))) {
				list.add("数据管理-收件人筛选与导出");
			}
			// 数据管理-表单管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_DATASOURCE_FORM_ID))) {
				list.add("数据管理-表单管理");
			}

			// 邮件管理-模板设计与管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TEMPLATE_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				list.add("邮件管理-模板设计与管理");
				list.add("邮件管理-模板预览");
			}// 邮件管理-模板预览
			else if (menu.getId().equals(
					String.valueOf(Menu.MENU_MAIL_TEMPLATE_ID))
					&& menu.getFuncAuth().indexOf("1") != -1) {
				list.add("邮件管理-模板预览");
			}
			// 邮件管理-任务创建和任务投递监控管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				list.add("邮件管理-任务投递监控管理");
				list.add("邮件管理-任务创建");
			}// 邮件管理-任务投递监控管理
			else if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("5") != -1) {
				list.add("邮件管理-任务投递监控管理");
			}// 邮件管理-任务创建
			else if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_TASK_ID))
					&& menu.getFuncAuth().indexOf("2") != -1) {
				list.add("邮件管理-任务创建");
			}
			// 邮件管理-审核
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_AUDIT_ID))) {
				list.add("邮件管理-审核");
			}
			// 邮件管理-活动创建与管理
			if (menu.getId().equals(String.valueOf(Menu.MENU_MAIL_CAMPAIGN_ID))) {
				list.add("邮件管理-活动创建与管理");
			}

			// 数据报告-导出
			if (menu.getId().equals(String.valueOf(Menu.MENU_REPORT_LOCAL_ID))
					&& menu.getFuncAuth().indexOf("0") != -1) {
				list.add("数据报告-查阅");
				list.add("数据报告-导出");
			}// 数据报告-查阅
			else if (menu.getId().equals(String.valueOf(Menu.MENU_REPORT_LOCAL_ID))
					&& menu.getFuncAuth().indexOf("1") != -1) {
				list.add("数据报告-查阅");
			}

			// 管理员-子账号
			if (menu.getId().equals(String.valueOf(Menu.MENU_ACCOUNT_USER_ID))) {
				list.add("管理员-子账号");
			}

		}

		return list;
	}
	
	@Override
	public Map<String, Object> verifyXss(Role role) {
		Map<String,Object> map = new HashMap<String,Object>();
		String message = "";
		boolean isValid = false;
		if(!XssFilter.isValid(role.getRoleName())) {
			message = "角色名称含有非法字符！";
		}else if(!XssFilter.isValid(role.getRoleDesc())){
			message = "角色描述含有非法字符！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

}
