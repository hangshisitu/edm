package cn.edm.controller.common;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Status;
import cn.edm.domain.User;
import cn.edm.service.UserService;
import cn.edm.util.EncodeUtils;
import cn.edm.util.Https;
import cn.edm.util.Md5PasswordEncoder;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.Valids;


@Controller
public class SettingsAction {
	
	private static final Logger logger = LoggerFactory.getLogger(SettingsAction.class);
	
	@Autowired
	private UserService userService;

	@RequestMapping(value = "/settings", method = RequestMethod.POST)
	public String settings(HttpServletRequest request, ModelMap map) {
		try {
			String SID = Https.getString(request, "SID");
			String R = Https.getString(request, "R");
			String password = Https.getString(request, "password");
			String passwordConfirm = Https.getString(request, "passwordConfirm");
			
			valid(SID, R, password, passwordConfirm);
			
			User user = userService.getByEmail(new String(EncodeUtils.decodeBase64(SID)));
			valid(user, R);
			
			Md5PasswordEncoder encoder = new Md5PasswordEncoder();
			user.setPassword(encoder.encodePassword(password));
			user.setRandCode(null);
			userService.updateUser(user, false);
			success(map);
			return "common/success";
		} catch (Exception e) {
			error(map, e.getMessage());
			logger.error("(Settings: /settings) code: error, message: ", e);
		}
		
		return "common/error";
	}
	
	private void valid(String SID, String R, String password, String passwordConfirm) {
		if (StringUtils.isBlank(SID) || StringUtils.isBlank(R)) {
			throw new RuntimeException("您访问的地址不存在，请确认您输入的URL地址");
		}
		String email = new String(EncodeUtils.decodeBase64(SID));
		Valids.email(email, "注册邮箱无效。");
		Valids.blank(password, "请输入密码");
		Valids.size(password, 6, 18, "密码字符长度在6到16之间");
		Valids.blank(passwordConfirm, "请输入密码确认");
		if (!passwordConfirm.equals(password)) {
			throw new RuntimeException("密码确认与密码要一致");
		}
	}
	
	private void valid(User user, String R) {
		if (user == null) {
			throw new RuntimeException("用户不存在。");
		}
		if (!user.getStatus().equals(Status.VALID)) {
			throw new RuntimeException("用户已失效。");
		}
		if (StringUtils.isBlank(user.getRandCode()) || !user.getRandCode().equals(R)) {
			throw new RuntimeException("重置已失效。");
		}
	}
	
	private void success(ModelMap map) {
		map.put("status", "重置密码");
		map.put("title", "修改帐户密码");
		map.put("message", "您的密码已经重置成功。请重新<a href='" + PropertiesUtil.get(ConfigKeys.WEBADMIN_HTTP) + "/login'><b>登录</b></a>");
	}
	
	private void error(ModelMap map, String message) {
		map.put("status", "修改帐户密码失败");
		map.put("message", message);
	}
}
