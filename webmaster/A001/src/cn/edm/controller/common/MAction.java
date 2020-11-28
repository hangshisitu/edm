package cn.edm.controller.common;

import java.util.regex.Pattern;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Message;
import cn.edm.constant.Status;
import cn.edm.domain.Corp;
import cn.edm.domain.User;
import cn.edm.service.CorpService;
import cn.edm.service.UserService;
import cn.edm.util.Cookies;
import cn.edm.util.EncodeUtils;
import cn.edm.util.Https;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.Regex;


@Controller
public class MAction {

	private static final Logger logger = LoggerFactory.getLogger(MAction.class);

	@Autowired
	private CorpService corpService;
	@Autowired
	private UserService userService;
	
	@RequestMapping("/m")
	public String m(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		System.out.println("开始重置密码");
		try {
			String M = Https.getString(request, "M");
			String SID = Https.getString(request, "SID");
			String R = Https.getString(request, "R");
			
			if (StringUtils.isBlank(M)
					|| (!M.equals("active") && !M.equals("settings"))
					|| StringUtils.isBlank(SID) || StringUtils.isBlank(R)) {
				return redirect(request, "login");
			}

			if (M.equals("active")) {
				active(response, SID, R,map);
				return redirect(request, "login");
			}
			
			if (M.equals("settings")) {
				settings(SID, R, map);
				return "common/settings";
			}
		}catch (Exception e) {
			logger.error("(M: /m) code: error, message: ", e);
			error(map, e.getMessage());
		}
		
		return "action/error";
	}
	
	private void error(ModelMap map, String message) {
		map.put("code", "重置密码");
		map.put("title", "修改帐户密码");
		map.put("message", message);
	}
	
	private void active(HttpServletResponse response,String SID, String R, ModelMap map) {
		String email = new String(EncodeUtils.decodeBase64(SID));
		User user = userService.getByEmail(email);
		if (user == null) {
			map.put(Message.RESULT, Message.FAILURE);
			map.put(Message.MSG, "用户不存在");
		}
		
		if (user.getStatus().equals(Status.INVALID)
				&& StringUtils.isNotBlank(user.getRandCode())
				&& user.getRandCode().equals(R)) {
			Corp corp = corpService.getByCorpId(user.getCorpId());
			corp.setStatus(Status.VALID);
			user.setStatus(Status.VALID);
			user.setRandCode(null);
			userService.update(corp, user);
			
			response.addCookie(Cookies.add(Cookies.USERID_COOKIE, user.getUserId()));
		} else {
			map.put(Message.RESULT, Message.FAILURE);
			map.put(Message.MSG, "此用户已失效");
		}
	}
	
	private void settings(String SID, String R, ModelMap map) {
		String email = new String(EncodeUtils.decodeBase64(SID));
		if (!Pattern.matches(Regex.EMAIL, email)) {
			map.put(Message.RESULT, Message.FAILURE);
			map.put(Message.MSG, "注册邮箱无效");
		}
		User user = userService.getByEmail(email);
		if (user == null) {
			map.put(Message.RESULT, Message.FAILURE);
			map.put(Message.MSG, "用户不存在");
		}
		if (!user.getStatus().equals(Status.INVALID)) {
			map.put(Message.RESULT, Message.FAILURE);
			map.put(Message.MSG, "此用户已失效");
		}
		map.put("userId", user.getUserId());
		map.put("email", email);
		map.put("SID", SID);
		map.put("R", R);
	}
	
	public static String redirect(HttpServletRequest request, String result) {
		result = PropertiesUtil.get(ConfigKeys.APPLICATION_LOCATION) + "/" + result;
		return "redirect:" + result;
	}
}
