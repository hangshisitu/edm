package cn.edm.controller.common;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
public class SecurityController {
	
	@RequestMapping({"/manager","/bin","/logs","/config","/temp","/conf","/work","/lib","/webapps","/404"})
	public String handleError() {
		System.out.println("the url is not exists-----");
		return "common/404";
	}
}

