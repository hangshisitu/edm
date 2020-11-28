package cn.edm.web.action;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.utils.web.Views;

@Controller
public class LogoutAction {

	@RequestMapping(value = "/logout", method = RequestMethod.GET)
	public String logout() {
		return Views.redirect("j_spring_security_logout");
	}
}
