package cn.edm.controller.common;


import java.util.Map;

import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import com.google.common.collect.Maps;

import cn.edm.constant.ErrorCode;
import cn.edm.constant.Message;
import cn.edm.exception.Errors;
import cn.edm.util.CSRF;
import cn.edm.util.CookieUtils;


/**
 * 到登陆页面的控制器
 * @author Luxh
 */
@Controller
public class LoginController {
	
	private static Logger log = LoggerFactory.getLogger(LoginController.class);
	
	@RequestMapping(value="/login",method={RequestMethod.POST,RequestMethod.GET,RequestMethod.HEAD})
	public String login(HttpServletRequest request,ModelMap modelMap) {
		try {
			CSRF.generate(request);
			String error = request.getParameter("error");
			//出错处理提示
			if(error!=null&&!"".equals(error)) {
				handleError(modelMap,error);
			}
			cookie(request,modelMap);
		}catch(Errors e) {
			if("illegal request".equals(e.getMessage())) {
				modelMap.put(Message.MSG,"非法的请求来源");
			}
			log.error(e.getMessage(), e);
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}
		
		return "login/login";
	}
	
	/**
	 * 错误代码提示
	 * @param modelMap
	 * @param error
	 */
	private void handleError(ModelMap modelMap,String error) {
		log.error("The error code is: "+error);
		String message = "";
		if(ErrorCode.USERNAME_PASSWORD_NOT_CORRECT.equals(error)) {
			message = "账号或密码输入有误！";
		}else if(ErrorCode.USER_STATUS_INVALID.equals(error)) {
			message = "此用户已失效！";
		}else if(ErrorCode.NOT_ADMIN_ROLE.equals(error)) {
			message = "非系统管理员账号！";
		}else if(ErrorCode.MAX_SESSIONS.equals(error)) {
			message = "此账号已从别处登录！";
		}else if(ErrorCode.PAGE_EXPIRED.equals(error)) {
			message = "页面过期重新登录！";
		}
		modelMap.put(Message.MSG,message);
	}	
	
	private void cookie(HttpServletRequest request, ModelMap map) {
		Cookie[] cookies = request.getCookies();
		if (cookies != null) {
			for (Cookie cookie : cookies) {
				String mid = CookieUtils.get(cookie);
				if (mid != null) {
					map.put("mid", mid);
					map.put("remember_me", true);
					break;
				}
			}
		}
		
	}
	
	@RequestMapping("/error")
	public String error() {
		
		return "common/exception";
	}
	
	
	@RequestMapping("/verifyKaptchaCode")
	public @ResponseBody Map<String,Object> verifyKaptchaCode(HttpServletRequest request) {
		Map<String,Object> resultMap = Maps.newHashMap();
		try {
			String verifyCode = request.getParameter("verifyCode");
            if(StringUtils.isEmpty(verifyCode)) {
                resultMap.put(Message.RESULT, Message.FAILURE);
            }else {
                //获取kaptcha生成存放在session中的验证码
                String kaptchaValue = (String) request.getSession().getAttribute(com.google.code.kaptcha.Constants.KAPTCHA_SESSION_KEY);
                //比较输入的验证码和实际生成的验证码是否相同
                if(kaptchaValue == null || kaptchaValue == "") {
                	resultMap.put(Message.RESULT, Message.EXPIRED);
                }else if(!verifyCode.equalsIgnoreCase(kaptchaValue)){
                	resultMap.put(Message.RESULT, Message.FAILURE);
                }else {
                	resultMap.put(Message.RESULT, Message.SUCCESS);
                }
            }

		}catch(Exception e) {
			resultMap.put(Message.RESULT, Message.FAILURE);
			e.printStackTrace();
		}
		return resultMap;
	}
	
}
