package cn.edm.controller.system;

import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

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
import cn.edm.constant.Role;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.Status;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.security.UserContext;
import cn.edm.service.UserService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.Md5PasswordEncoder;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.Asserts;
import cn.edm.utils.StreamToString;

@Controller
@RequestMapping("/subUser")
public class SubUserController {
	
private static Logger log = LoggerFactory.getLogger(SubUserController.class);
	
	@Autowired
	private UserService userService;
	
	@RequestMapping(value="/subUserList",method=RequestMethod.GET)
	public String subUserList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		String trueName = StreamToString.getStringByUTF8(request, "trueName");
		String userId  = StreamToString.getStringByUTF8(request, "userId");
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("trueName",trueName);
		initParamsMap.put("userId ",userId );
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		int[] status = {Status.VALID,Status.FREEZE};
		realParamsMap.put("status", status);
		
		int[] roleIds = {Role.SEE.getId()};
		realParamsMap.put("roleIds", roleIds);
		
		Pagination<User> pagination = userService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		
		return "system/subUserList";
	}
	
	/**
	 * 创建子账号
	 */
	@RequestMapping("/add")
	public String add(HttpServletRequest request,ModelMap modelMap) {
		CSRF.generate(request);
		modelMap.put("title", "创建");
		return "system/subUserAdd";
	}
	
	/**
	 * 修改子账号
	 */
	@RequestMapping("/edit")
	public String edit(HttpServletRequest request,ModelMap modelMap,@RequestParam(value="userId") String userId) {
		CSRF.generate(request);
		modelMap.put("title", "修改");
		if(userId != null) {
			User user = userService.getByUserId(userId);
			if(user != null) {
				modelMap.put("user", user);
			}else {
				modelMap.put(Message.MSG,"用户不存在");
			}
		}else {
			modelMap.put(Message.MSG,"用户不存在");
		}
		return "system/subUserEdit";
	}
	
	
	/**
	 * 保存
	 */
	@RequestMapping("/save")
	public @ResponseBody Map<String,Object> save(HttpServletRequest request,User user) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		try {
			//修改
			if(user.getCorpId()!=null) {
				User oldUser = userService.getByUserId(user.getUserId());
				oldUser = updateUser(oldUser,user);
				Map<String,Object> verifyMap = userService.verifyXss(oldUser);
				if((Boolean) verifyMap.get(Message.RESULT)) {
					oldUser.setModerate(0);
					userService.updateUser(oldUser, false);
					resultMap.put(Message.RESULT, Message.SUCCESS);
					resultMap.put(Message.MSG, "修改子账号成功！");
				}else {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, verifyMap.get(Message.MSG));
				}
				
				
			}else {
				//新增
				//验证账号是否已存在
				boolean flag = userService.isUserExist(user.getUserId().trim());
				if(flag) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "子账号已存在！");
				}else {
					Map<String,Object> verifyMap = userService.verifyXss(user);
					if((Boolean) verifyMap.get(Message.RESULT)) {
						if(XssFilter.isValid(user.getUserId())) {
							//验证字段是否符合规则
							Map<String,Object> map2 = userService.verifySubUser(user);
							if((Boolean) map2.get(Message.RESULT)) {
								user.setCorpId(UserContext.getCurrentUser().getCorpId());
								user.setRoleId(Role.SEE.getId());
								user.setCreateTime(DateUtil.getCurrentDate());
								user.setStatus(user.getStatus());
								user.setModerate(0);
								

					            Asserts.password(user.getPassword(), "密码");
								
								Md5PasswordEncoder md5 = new Md5PasswordEncoder();
								user.setPassword(md5.encodePassword(user.getPassword().trim()));
								userService.saveUser(user, false);
								resultMap.put(Message.RESULT, Message.SUCCESS);
								resultMap.put(Message.MSG, "创建子账号成功！");
							}else {
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, map2.get(Message.MSG));
							}
							
							
						}else {
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, verifyMap.get(Message.MSG));
						}
					}else {
						resultMap.put(Message.RESULT, Message.FAILURE);
						resultMap.put(Message.MSG, verifyMap.get(Message.MSG));
					}
				}
			}
		}catch(Errors e) {
            log.error(e.getMessage(), e);
            resultMap.put(Message.RESULT, Message.FAILURE);
            resultMap.put(Message.MSG, e.getMessage());
        }catch(Exception e) {
			e.printStackTrace();
			log.error(e.getMessage(), e);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "保存出错！");
		}
		return resultMap;
	}
	
	@RequestMapping("/delete")
	public String delete(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
		//CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user != null) {
//			Map<String,Object> paramMap = new HashMap<String,Object>();
//			paramMap.put("status", Status.DELETE);
//			paramMap.put("userId", user.getUserId());
			userService.deleteUser(user.getUserId());
			redirectAttributes.addFlashAttribute("message", "删除账号成功!");
		}else {
			redirectAttributes.addFlashAttribute("message", "账号不存在！");
		}
		
		return "redirect:subUserList";
	}
	
	/**
	 * 冻结账号
	 */
	@RequestMapping("/freeze")
	public String freeze(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
		//CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user != null) {
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status", Status.FREEZE);
			paramMap.put("userId", user.getUserId());
			userService.updateUserStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "冻结账号成功!");
		}else {
			redirectAttributes.addFlashAttribute("message", "账号不存在！");
		}
		
		return "redirect:subUserList";
	}
	
	/**
	 * 解冻账号
	 */
	@RequestMapping("/unfreeze")
	public String unfreeze(HttpServletRequest request,RedirectAttributes redirectAttributes,@RequestParam(value="userId") String userId) {
		//CSRF.validate(request);
		User user = userService.getByUserId(userId);
		if(user != null) {
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status", Status.VALID);
			paramMap.put("userId", user.getUserId());
			userService.updateUserStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "解冻账号成功!");
		}else {
			redirectAttributes.addFlashAttribute("message", "账号不存在！");
		}
		
		return "redirect:subUserList";
	}
	
	
	private User updateUser(User oldUser,User newUser) {
		oldUser.setTrueName(newUser.getTrueName());
		oldUser.setPhone(newUser.getPhone());
		oldUser.setStatus(newUser.getStatus());
		if(newUser.getPassword() != null && !"".equals(newUser.getPassword())) {
		    Asserts.password(newUser.getPassword(), "密码");
		    
			Md5PasswordEncoder md5 = new Md5PasswordEncoder();
			
			oldUser.setPassword(md5.encodePassword(newUser.getPassword().trim()));
		}
		return oldUser;
	} 
}
