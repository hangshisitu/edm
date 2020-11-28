package cn.edm.controller.system;

import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseBody;

import cn.edm.constant.Message;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.security.UserContext;
import cn.edm.service.UserService;
import cn.edm.util.CSRF;
import cn.edm.util.Md5PasswordEncoder;
import cn.edm.utils.Asserts;

/**
 * 修改密码
 * @author Luxh
 *
 */
@Controller
@RequestMapping("/password")
public class PasswordController {
	
	private static Logger log = LoggerFactory.getLogger(PasswordController.class);
	
	@Autowired
	private UserService userService;
	
	/**
	 * 到修改密码页面
	 */
	@RequestMapping("/edit")
	public String edit(HttpServletRequest request) {
		CSRF.generate(request);
		return "system/passwordEdit";
	}
	
	/**
	 * 保存新密码
	 */
	@RequestMapping("/save")
	public @ResponseBody Map<String,Object> save(HttpServletRequest request) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		try {
			String oldPasswprd = request.getParameter("oldPassword");
			String newPassword = request.getParameter("newPassword");
			//String confirmNewPassword = request.getParameter("confirmNewPassword");
			//System.out.println("newPassword---"+newPassword);
			//密码验证
			
			Asserts.password(newPassword, "新密码");
			
			Map<String,Object> map2 = userService.verifyPassword(newPassword);
			if((Boolean) map2.get(Message.RESULT)) {
				User user = userService.getByUserId(UserContext.getCurrentUser().getUsername());
				Md5PasswordEncoder md5 = new Md5PasswordEncoder();
				if(!md5.encodePassword(oldPasswprd.trim()).equals(user.getPassword())) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "原密码输入有误！");
				}else {
					user.setPassword(md5.encodePassword(newPassword.trim()));
					userService.updateUser(user, false);
					resultMap.put(Message.RESULT, Message.SUCCESS);
					resultMap.put(Message.MSG, "修改密码成功！");
				}
			}else {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, map2.get(Message.MSG));
			}
		}catch(Errors e) {
			log.error(e.getMessage(), e);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, e.getMessage());
		}catch(Exception e) {
            log.error(e.getMessage(), e);
            resultMap.put(Message.RESULT, Message.FAILURE);
            resultMap.put(Message.MSG, "修改密码出错！");
        }
		return resultMap;
	}
}
