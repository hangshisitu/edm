package cn.edm.controller.role;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.domain.Menu;
import cn.edm.domain.Role;
import cn.edm.domain.RoleMenu;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.security.UserContext;
import cn.edm.service.MenuService;
import cn.edm.service.RoleService;
import cn.edm.service.RoleServiceImpl;
import cn.edm.service.UserService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;

@Controller
@RequestMapping("/role")
public class RoleController {

	private static Logger log = LoggerFactory.getLogger(RoleController.class);

	@Autowired
	private RoleService roleService;
	
	@Autowired
	private MenuService menuService;
	
	@Autowired
	private UserService userService;


	@RequestMapping(value = "/roleList", method = RequestMethod.GET)
	public String roleList( HttpServletRequest request, ModelMap modelMap,
			@RequestParam(value = "currentPage", defaultValue = "1") int currentPage,
			@RequestParam(value = "pageSize", defaultValue = "20") int pageSize) {
		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
	    Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);

		Pagination<Role> pagination = roleService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);

		return "role/roleList";
	}
	
	@RequestMapping(value="/createRole", method=RequestMethod.GET)
	public String createRole(HttpServletRequest request,ModelMap modelMap){
		CSRF.generate(request);
		modelMap.put("title", "创建角色");
		
		return "role/createRole";
	}
	
	@RequestMapping(value="/editRole", method=RequestMethod.GET)
	public String edit(HttpServletRequest request, ModelMap modelMap, @RequestParam(value="roleId") String roleId){
		CSRF.generate(request);
		roleId = XssFilter.filterXss(roleId);
		if(roleId!=null && !"".equals(roleId)) {
			Role role = roleService.getRoleById(roleId);						
			if(role != null){
				List<Menu> menuList = menuService.selectMenuByRoleId(Integer.valueOf(roleId));
				if(menuList != null && menuList.size() != 0){
					String funcList = RoleServiceImpl.setFuncChecked(menuList);
					modelMap.put("funcList", funcList);
				}
				modelMap.put("role", role);								
			}			
		}
		modelMap.put("title", "编辑角色");
		return "role/createRole";
	}
	
	
	@RequestMapping(value="/saveRole", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> save(HttpServletRequest request, Role role){
		Map<String,Object> resultMap = new HashMap<String,Object>();
//		CSRF.validate(request);
				
		try {
			log.info("开始保存角色，角色名称：" + role.getRoleName() + ",角色描述：" + role.getRoleDesc());	
			String roleName = Https.getStr(request, "roleName", R.CLEAN, R.LENGTH, "{1,15}", "角色名称");
			Https.getStr(request, "roleDesc", R.CLEAN, R.LENGTH, "{1,50}", "角色名称");
			
			role.setOperator(UserContext.getCurrentUser().getUsername());
			
			String[] functionList = request.getParameterValues("func");		
			String funcString = "";
			for(int i=0;i<functionList.length;i++){
				funcString = funcString + functionList[i] + ",";
			}
			if(funcString.indexOf(Menu.MENU_ACCOUNT_USER + ",") != -1){
				role.setType(1);//管理员角色
			}else{
				role.setType(0);//默认普通角色
			}
			
			if(StringUtils.isBlank(role.getId())){//新建角色	
				List<Role> roleList = roleService.getRoleByRoleName(roleName);
				if(roleList != null && roleList.size() != 0){
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "角色名称已存在");
					return resultMap;
				}
				
				roleService.saveRole(role);	
				List<RoleMenu> roleMenuList = RoleServiceImpl.getFunc(role.getId(),funcString);
				roleService.saveRoleMenu(role.getId(),roleMenuList);				
			}else{//修改角色
				List<Role> roleList = roleService.getRoleByRoleName(roleName);
				String oldRoleName = request.getParameter("oldRoleName");
				if(roleList != null && roleList.size() != 0 && !roleName.equals(oldRoleName)){
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "角色名称已存在");
					return resultMap;
				}
				
				roleService.editRole(role);
				roleService.delRoleMenu(role.getId());//删除角色菜单中间表
				List<RoleMenu> roleMenuList = RoleServiceImpl.getFunc(role.getId(),funcString);
				roleService.saveRoleMenu(role.getId(),roleMenuList);				
			}						
			
			resultMap.put(Message.RESULT, Message.SUCCESS);
			resultMap.put(Message.MSG, "成功保存角色");			 
		} catch (Errors e) {
            resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, e.getMessage());
        } catch (Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "保存角色失败");
		}		
		
		return resultMap;
	}
	
	
	@RequestMapping(value="/delRole")
	public String delete(HttpServletRequest request, RedirectAttributes redirectAttributes, @RequestParam(value="roleId") String roleId){
		CSRF.generate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		
		try {
			roleId = XssFilter.filterXss(roleId);
			if(roleId!=null && !"".equals(roleId)) {			
				User user = userService.getByRoleId(Integer.valueOf(roleId));
				if(user != null){
					redirectAttributes.addFlashAttribute("message", "角色被使用中，需清理角色下的账号后方可删除");
				}else{
					log.info("用户" + UserContext.getCurrentUser().getUsername() + "删除角色，角色Id:" + roleId);
					roleService.delRoleById(roleId);
					roleService.delRoleMenu(roleId);//删除角色菜单中间表					
					redirectAttributes.addFlashAttribute("message", "成功删除角色");
				}																
			}
		} catch (Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "删除角色失败");
		}
	
		return "redirect:roleList";
	}
	
	


}
