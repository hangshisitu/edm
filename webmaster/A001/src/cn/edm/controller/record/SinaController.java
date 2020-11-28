package cn.edm.controller.record;

import javax.servlet.http.HttpServletRequest;

import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
@RequestMapping(value="/sina")
public class SinaController {

	@RequestMapping(value="/list")
	public String sinaList(HttpServletRequest request,ModelMap modelMap){
		return "/record/sinaRecord";
	}
	
	
	@RequestMapping(value="/rule")
	public String sinaRule(HttpServletRequest request,ModelMap modelMap){
		return "/record/sinaRecordRule";
	}
	
	
	
	
	
}
