package cn.edm.controller.system;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import cn.edm.constant.Config;
import cn.edm.constant.SearchDefine;
import cn.edm.constants.Status;
import cn.edm.domain.CustomEnterprise;
import cn.edm.modules.utils.Props;
import cn.edm.service.CustomEnterpriseService;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;

@Controller
@RequestMapping(value="/custom")
public class CustomEnterpriseController {
	
	private static Logger log = LoggerFactory.getLogger(CustomEnterpriseController.class);
	
	@Autowired
	private CustomEnterpriseService customEnterpriseService;

	@RequestMapping(value="/page")
	public String page(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize){
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		Pagination<CustomEnterprise> pagination = customEnterpriseService.getPagination(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		return "/system/customEnterpriseList";	
	}
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/freeze")
	public String freeze(HttpServletRequest request,ModelMap modelMap,
			RedirectAttributes redirectAttributes,@RequestParam(value="id") String id){
		CustomEnterprise customEnterprise = customEnterpriseService.selectByCustomEnterpriseId(id);
		
		if(customEnterprise != null){
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status",Status.DISABLED );
			paramMap.put("id",id);
			customEnterpriseService.updateCustomEnterpriseStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "logo冻结成功");
		}else{
			redirectAttributes.addFlashAttribute("message", "logo不存在！");
		}
		
		return "redirect:page";	
	}
	
	
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/unfreeze")
	public String unfreeze(HttpServletRequest request,ModelMap modelMap,
			RedirectAttributes redirectAttributes,@RequestParam(value="id") String id){
		//CSRF.validate(request);
		//激活前先判断是否已经禁用其他的logo
		CustomEnterprise  customEnterpriseOther = customEnterpriseService.selectByCustomEnterpriseStatus(Status.ENABLED);
		if(customEnterpriseOther ==null ){
			log.info("activate logo");
			CustomEnterprise customEnterprise = customEnterpriseService.selectByCustomEnterpriseId(id);
			if(customEnterprise == null) {
				redirectAttributes.addFlashAttribute("message", "logo不存在！");
			}else {
				Map<String,Object> paramMap = new HashMap<String,Object>();
				paramMap.put("status", Status.ENABLED);
				paramMap.put("id", id);				
				customEnterpriseService.updateCustomEnterpriseStatus(paramMap);				
			}
		}else{
			redirectAttributes.addFlashAttribute("message", "请先禁用其他的logo");
		}
		return "redirect:page";	
	}
	
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/del")
	public String del(HttpServletRequest request,ModelMap modelMap,
			RedirectAttributes redirectAttributes,@RequestParam(value="id") String id){
		
		Map<String,Object> paramMap = new HashMap<String,Object>();
		paramMap.put("id",id);		
		CustomEnterprise customEnterprise = customEnterpriseService.selectByCustomEnterpriseId(id);
		if(customEnterprise == null){
			redirectAttributes.addFlashAttribute("message", "待删除logo不存在");
			return "redirect:page";
		}
		
		boolean isSuccess = customEnterpriseService.deleteCustomEnterpriseById(paramMap);
        if(!isSuccess){
        	redirectAttributes.addFlashAttribute("message", "删除logo失败");
        }
        redirectAttributes.addFlashAttribute("message", "删除logo成功");
        
        String logoName = Props.getStr(Config.NGINX_PATH)+"/"+customEnterprise.getLogoUrl();
		File deleteLogo = new File(logoName);
		if(deleteLogo.exists()){
			deleteLogo.delete();
			log.info("[ success ]: delete logo url :"+customEnterprise.getLogoUrl());
		}else{
			log.info("[ fail ]: delete logo url :"+customEnterprise.getLogoUrl());
		}
		return "redirect:page";	
	}
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/add")
	public String add(HttpServletRequest request,ModelMap modelMap){
		return "/system/customEnterpriseAdd";	
	}
	
}
