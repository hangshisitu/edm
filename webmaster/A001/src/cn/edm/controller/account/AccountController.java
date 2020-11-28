package cn.edm.controller.account;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
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

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.Status;
import cn.edm.domain.Corp;
import cn.edm.domain.Cos;
import cn.edm.domain.Resource;
import cn.edm.domain.Role;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.security.UserContext;
import cn.edm.service.ApiTriggerService;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.ResourceService;
import cn.edm.service.RobotService;
import cn.edm.service.RoleService;
import cn.edm.service.UserService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.Md5PasswordEncoder;
import cn.edm.util.Pagination;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.XssFilter;
import cn.edm.utils.Asserts;
import cn.edm.utils.StreamToString;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

/**
 * 账号管理
 * @author Luxh
 *
 */

@Controller
@RequestMapping("/account")
public class AccountController {
	
	
	private static Logger log = LoggerFactory.getLogger(AccountController.class);
	
	@Autowired
	private UserService userService;
	
	@Autowired
	private CorpService corpService;
	
	@Autowired
	private CosService cosService;
	
	@Autowired
	private ResourceService resourceService;
	
	@Autowired
	private ApiTriggerService apiTriggerService;
	
	@Autowired
	private RobotService robotService;
	
	@Autowired
	private RoleService roleService;
	
	@RequestMapping("/accountTab")
	public String accountTab() {
		return "account/userTab";
	}
	

	/**
	 * 查看
	 */
	@RequestMapping(value="/view", method=RequestMethod.GET)
	public String view(HttpServletRequest request, ModelMap modelMap, @RequestParam(value="userId") String userId) {
		CSRF.generate(request);
		userId = (String) XssFilter.filter(userId);
		User user = userService.getByUserId(userId);
		if(user == null) {
			throw new RuntimeException("用户不存在");
		}
		Corp corp = corpService.getByCorpId(user.getCorpId());
		if(corp != null) {
			Cos cos = cosService.getCosByCosId(corp.getCosId());
			modelMap.put("cos", cos);
		}
		
		modelMap.put("user", user);
		modelMap.put("corp", corp);
		
		return "/account/accountView";
	}
	

	
	
	/**
	 * 删除账号
	 */
	@RequestMapping("/delete")
	public String delete(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
//		CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user != null) {						
			//删除子账号相关信息
			List<User> subUserList = userService.getSubUserByManagerId(userId);
			for(User subUser : subUserList){
				userService.deleteUser(subUser.getUserId());
			}						
			userService.deleteUser(user.getUserId());
			
			redirectAttributes.addFlashAttribute("message", "删除账号成功");
		}else {
			redirectAttributes.addFlashAttribute("message", "账号不存在");
		}
		
		return "redirect:accountList";
	}
	
	/**
	 * 激活账号
	 */
	@RequestMapping("/activate")
	public String activate(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
		log.info("activate account");
		//CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user == null) {
			redirectAttributes.addFlashAttribute("message", "账号不存在！");
		}else {
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status", Status.VALID);
			paramMap.put("userId", user.getUserId());
			
			userService.updateUserStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "激活账号成功！");
		}
		return "redirect:accountList";
	}

	/**
	 * 冻结
	 */
	@RequestMapping("/freeze")
	public String freeze(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
//		CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user != null) {
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status", Status.FREEZE);
			paramMap.put("userId", user.getUserId());
			userService.updateUserStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "冻结账号成功");
		}else {
			redirectAttributes.addFlashAttribute("message", "账号不存在！");
		}
		
		return "redirect:accountList";
	}
	
	
	/**
	 * 创建新账号
	 */
	@RequestMapping(value="/createAccount", method=RequestMethod.GET)
	public String createAccount(HttpServletRequest request,ModelMap modelMap) {
		CSRF.generate(request);
		
		int[] types = {1};//前台管理员
		List<Role> roleList = roleService.getRoleByType(types);
		modelMap.put("roleList", roleList);
		
		List<Corp> corpList = corpService.getAllCorp();
		modelMap.put("corpList", corpList);
		
		modelMap.put("title", "新建");
		modelMap.put("backUrl", "/account/accountList");
		modelMap.put("accountType", "1");						
		modelMap.put("successUrl", "/account/accountList");
		
		return "/account/accountEdit";				
	}
	
	/**
	 * 保存账号
	 */
	@RequestMapping(value="/save", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> save(HttpServletRequest request, User user) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		
		try {	 
			//同机构下新建管理员时只允许存在一个前台管理员
			Role role = roleService.getRoleById(user.getRoleId()+"");
			if(role != null){
			   if(role.getType() == 1 ){ //新建的是前台管理员
				   Map<String,Object> paramMap = new HashMap<String,Object>();
				      paramMap.put("corpId",user.getCorpId());
				      paramMap.put("type",role.getType());
                      long count = userService.selectUsersByCorpIdAndRoleId(paramMap);
				      if(count >=1 ){
					     resultMap.put(Message.RESULT, Message.FAILURE);
					     resultMap.put(Message.MSG, "该机构下已经存在管理员");
					     return resultMap;
				      }
			   }	
		    }
				
			//新增
			if(user.getExist()==null||"".equals(user.getExist())){
				//验证账号是否已存在
				boolean flag = userService.isUserExist(user.getUserId().trim());							
				
				if(flag) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "用户名已存在");
				}else {
					//验证邮箱是否已存在
					boolean isEmailExist = false;
					if(user.getEmail()!=null&&!"".equals(user.getEmail().trim())) {
						isEmailExist = userService.isEmailExist(user.getEmail().trim());
					}
					if(isEmailExist) {
						resultMap.put(Message.RESULT, Message.FAILURE);
						resultMap.put(Message.MSG, "邮箱已存在");
					}else {
						user = getUser(user);
						//xss检查
						Map<String,Object> map = userService.verifyXss(user);						
						if((Boolean) map.get(Message.RESULT)) {							
							//验证字段是否符合规则
							Map<String,Object> map2 = userService.verifyRegex(user);
							if((Boolean) map2.get(Message.RESULT)) {
								user.setModerate(0);
//								user.setRobots(robots);
								userService.save(user);
								resultMap.put(Message.RESULT, Message.SUCCESS);
								resultMap.put(Message.MSG, "创建账号成功!</br>初始密码 123456 + 登录账号前两位，如123456ad");
							}else {
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, map2.get(Message.MSG));
							}
							
						}else {
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, map.get(Message.MSG));
						}
					}
				}
				
			}else {
				//修改
				User oldUser = userService.getByUserId(user.getUserId());				
				//验证邮箱是否已存在
				boolean isEmailExist = false;
				if(user.getEmail()!=null&&!"".equals(user.getEmail().trim())) {
					Map<String,Object> paramMap = Maps.newHashMap();
					paramMap.put("email", user.getEmail().trim());
					paramMap.put("userId", user.getUserId());
					isEmailExist = userService.isEmailExistByUserId(paramMap);
				}
				if(isEmailExist) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "邮箱已存在");
				}else {
					oldUser = updateUser(oldUser, user, resultMap);
					if(resultMap.get(Message.MSG) == null){
						//xss检查
						Map<String,Object> map = userService.verifyXss(oldUser);
						if((Boolean) map.get(Message.RESULT)) {							
							//验证字段是否符合规则
							Map<String,Object> map2 = userService.verifyRegex(user);
							if((Boolean) map2.get(Message.RESULT)) {
								
								//网络发件人验证
//								String[] robots = request.getParameterValues("robots");
//								userService.checkRobots(robots);
//								userService.isRobotsExist(corp.getFormalId(), robots);
//								oldUser.setRobots(robots);
								oldUser.setModerate(0);
								userService.update(oldUser);
								resultMap.put(Message.RESULT, Message.SUCCESS);
								resultMap.put(Message.MSG, "修改账号成功!");
							}else {
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, map2.get(Message.MSG));
							}
						}else {
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, map.get(Message.MSG));
						}
					}
				}
			}
		}catch(Errors e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, e.getMessage());
		}catch(Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "保存账号失败");
		}
		return resultMap;
		
	}
	
	
	private User updateUser(User oldUser,User newUser, Map<String, Object> resultMap) {
		if(newUser.getPassword()!=null&&!"".equals(newUser.getPassword().trim())) {
		    String validateMsg = Asserts.validatePwd(newUser.getPassword(), "账号密码");
		    if(validateMsg == null){
		    	Md5PasswordEncoder md5 = new Md5PasswordEncoder();
		    	oldUser.setPassword(md5.encodePassword(newUser.getPassword().trim()));
		    }else{
		    	resultMap.put(Message.RESULT, Message.FAILURE);
		    	resultMap.put(Message.MSG, validateMsg);
		    }
		}
		oldUser.setEmail(newUser.getEmail());
		oldUser.setJob(newUser.getJob());
		oldUser.setIsNeedApi(newUser.getIsNeedApi());
		oldUser.setTriggerType(newUser.getTriggerType());
		oldUser.setTrueName(newUser.getTrueName());
		oldUser.setOtherTriggerType(newUser.getOtherTriggerType());
		oldUser.setModerate(newUser.getModerate());
		oldUser.setUpdateTime(DateUtil.getCurrentDate());
		oldUser.setPhone(newUser.getPhone());

		oldUser.setOpenSms(newUser.getOpenSms() == null ? 0 : newUser.getOpenSms());
		
		DateTime dt = new DateTime();
//		if(0==type) {
//			//试用套餐,有效期2个月
//			oldUser.setExpireTime(dt.plusMonths(2).toDate());
//		}else {
//			//正式套餐,有效期3年
//			oldUser.setExpireTime(dt.plusYears(3).toDate());
//		}
		oldUser.setExpireTime(dt.plusYears(3).toDate());
		oldUser.setIsGroupsApi(newUser.getIsGroupsApi());
		oldUser.setIsTriggerApi(newUser.getIsTriggerApi());
		oldUser.setCustomerType(newUser.getCustomerType());
		return oldUser;
	}
	
	
	private User getUser(User user) {
		user.setStatus(Status.VALID);
		user.setCreateTime(DateUtil.getCurrentDate());
		user.setUpdateTime(DateUtil.getCurrentDate());
		String password = PropertiesUtil.get(ConfigKeys.LOGIN_PASSWORD) + StringUtils.substring(user.getUserId(), 0, 2);
		Md5PasswordEncoder md5 = new Md5PasswordEncoder();
		user.setPassword(md5.encodePassword(password));
		DateTime dt = new DateTime();
		//正式套餐,有效期3年
		user.setExpireTime(dt.plusYears(3).toDate());
		return user;
	}
		
	/**
	 * 编辑账号
	 */
	@RequestMapping(value="/edit", method=RequestMethod.GET)
	public String edit(HttpServletRequest request, ModelMap modelMap, @RequestParam(value="userId") String userId) {
		CSRF.generate(request);
		if(userId!=null && !"".equals(userId)) {
			User user = userService.getByUserId(userId);
			if(user != null) {
				modelMap.put("user", user);	
			}			
		}		
		List<Corp> corpList = corpService.getAllCorp();
		modelMap.put("corpList", corpList);
		
		List<Role> roleList = roleService.getAllRole();
		modelMap.put("roleList", roleList);
		
		modelMap.put("backUrl", "/account/accountList");
		modelMap.put("successUrl", "/account/accountList");
		
		return "/account/accountEdit";
		
	}
	
	/**
	 * 查询子账号
	 */
	@RequestMapping("/getSubUser")
	public @ResponseBody Map<String,Object> getSubUser(HttpServletRequest request) {
		Map<String,Object> resultMap = new HashMap<String,Object>();
		String corpIdTemp = request.getParameter("corpId");
		// 过滤危险字符
		corpIdTemp = XssFilter.filterXss(corpIdTemp);
		if(corpIdTemp != null && !"".equals(corpIdTemp)) {
			Integer corpId = Integer.parseInt(corpIdTemp);
			List<User> userList = userService.getByCorpId(corpId);
			resultMap.put(Message.RESULT, Message.SUCCESS);
			resultMap.put("userList", userList);
		}
		return resultMap;
	}
	
//	/**
//	 * 根据账号类型选择页面
//	 */
//	@RequestMapping("/selectAccountTypePage")
//	public String selectAccountTypePage(HttpServletRequest request, ModelMap modelMap) {
//		CSRF.generate(request);
//		
//		List<Cos> testCosList = cosService.getByType(Cos.TESTED);
//		List<Cos> formalCosList = cosService.getByType(Cos.FORMAL);
//		List<Resource> resourceList = resourceService.getAll();
//		modelMap.put("testCosList", testCosList);
//		modelMap.put("formalCosList", formalCosList);
//		modelMap.put("resourceList", resourceList);
//		List<String> triggerNameList = apiTriggerService.getTriggerName();
//		modelMap.put("triggerNameList", triggerNameList);
//		String accountType = XssFilter.filterXss(request.getParameter("accountType"));
//		String result = "";
//		if("6".equals(accountType)) {
//			result = "/account/accountReadonlyEdit";
//		}else if("4".equals(accountType)) {
//			Map<String,Object> paramMap = Maps.newHashMap();
//			paramMap.put("userId", "admin");
//			paramMap.put("roleId", 0);
//			User adminUser = userService.getByUserIdAndRoleId(paramMap);
//			Map<String,Object> robotMap = Maps.newHashMap();
//			robotMap.put("corpId",adminUser.getCorpId());
//			List<Robot> robotsList = robotService.getRobotList(robotMap);
//			modelMap.put("robotsList", robotsList);
//			result = "/account/accountOperateEdit";
//		}else {
//			modelMap.put("backUrl", "/account/accountList");
//			modelMap.put("successUrl", "/account/accountList");
//			result = "/account/accountEdit";
//		}
//		modelMap.put("title", "新建");
//		modelMap.put("accountType", accountType);
//		
//		int[] types = {1};//前台管理员
//		List<Role> roleList = roleService.getRoleByType(types);
//		modelMap.put("roleList", roleList);
//		return result;
//	}
	
	/**
	 * 账号列表
	 */
	@RequestMapping(value="/accountList", method=RequestMethod.GET)
	public String accountList(HttpServletRequest request,ModelMap modelMap,
										@RequestParam(value="currentPage",defaultValue="1") int currentPage,
										@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		
		String userId =StreamToString.getStringByUTF8(request,"userId");
		String companyName =StreamToString.getStringByUTF8(request,"companyName");
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userId",userId);
		initParamsMap.put("companyName",companyName);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		int[] status = {Status.VALID,Status.FREEZE,Status.INVALID};
		realParamsMap.put("status", status);
		
		int[] types = {1};//前台管理员
		realParamsMap.put("types", types);
		
		List<Role> roleList = roleService.getRoleByType(types);
		modelMap.put("roleList", roleList);		
		
		List<Resource> resourceList = resourceService.getAll();
		// 不显示API通道
		List<Resource> delResouceList = Lists.newArrayList();
		for(Resource r : resourceList) {
			if("api".equals(r.getResourceId())){
				delResouceList.add(r);
			}
		}
		resourceList.removeAll(delResouceList);
		modelMap.put("resourceList", resourceList);
	
		Pagination<User> pagination = userService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		
		return "account/accountList";
	}
	
}
