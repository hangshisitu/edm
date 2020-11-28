package cn.edm.web.action.account;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.encoder.Md5;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;

@Controller
@RequestMapping("/account/password/**")
public class PasswordAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(PasswordAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request) {
		try {
			CSRF.generate(request);
			return "default/account/password_add";
		} catch (Exception e) {
			logger.error("(Password:add) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			String oldPassword = Https.getStr(request, "oldPassword", R.CLEAN, R.REQUIRED, R.LENGTH, "{6,18}", "原密码");
			String password = Https.getStr(request, "password", R.CLEAN, R.REQUIRED, R.LENGTH, "{6,18}", "新密码");
			Https.getStr(request, "passwordConfirm", R.CLEAN, R.REQUIRED, R.LENGTH, R.EQ, "{6,18}", "eq:" + password, "重复密码");

			User user = userService.get(UserSession.getUserId());

			Md5 md5 = new Md5();
			if (!md5.encode(oldPassword).equals(user.getPassword())) {
				throw new Errors("原密码不正确");
			}

			user.setPassword(md5.encode(password));
			userService.save(user, UserSession.getUserId());

			Views.ok(mb, "修改密码成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "修改密码失败");
			logger.error("(Password:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
}
