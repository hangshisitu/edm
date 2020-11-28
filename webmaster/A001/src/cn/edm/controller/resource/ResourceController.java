package cn.edm.controller.resource;

import java.util.HashMap;
import java.util.List;
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
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import com.google.common.collect.Lists;

import cn.edm.constant.Message;
import cn.edm.constant.Role;
import cn.edm.constant.SearchDefine;
import cn.edm.constant.Status;
import cn.edm.domain.Cos;
import cn.edm.domain.Resource;
import cn.edm.domain.User;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.ResourceService;
import cn.edm.service.UserService;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.utils.StreamToString;

@Controller
@RequestMapping("/resource")
public class ResourceController {
	
	private static Logger log = LoggerFactory.getLogger(ResourceController.class);
	
	@Autowired
	private UserService userService;
	
	@Autowired
	private CosService cosService;
	
	@Autowired
	private ResourceService resourceService;
	
	@Autowired
	private CorpService corpService;
	
	
	@RequestMapping(value="/resUserList", method=RequestMethod.GET)
	public String resUserList(HttpServletRequest request,ModelMap modelMap,
										@RequestParam(value="currentPage",defaultValue="1") int currentPage,
										@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		//CSRF.generate(request);
		String userId = StreamToString.getStringByUTF8(request,"userId");
		String corpPath = StreamToString.getStringByUTF8(request,"corpPath");		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		initParamsMap.put("userId",userId);
		initParamsMap.put("corpPath",corpPath);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		int[] status = {Status.VALID,Status.FREEZE};
		realParamsMap.put("status", status);
		
		/*int[] roleIds = {Role.MGR.getId()};
		realParamsMap.put("roleIds", roleIds);*/  //暂时屏蔽，不知道作用（通道暂时显示所有的前台机构管理员）
		
		Pagination<User> pagination = userService.getResPagination(realParamsMap, "", currentPage, pageSize);
		
		List<Cos> testCosList = cosService.getByType(Cos.TESTED);
		List<Cos> formalCosList = cosService.getByType(Cos.FORMAL);
		List<Resource> resourceList = resourceService.getAll();
		
		// 通道查询不显示API通道
		List<Resource> delResouceList = Lists.newArrayList();
		for(Resource r : resourceList) {
			if("api".equals(r.getResourceId())){
				delResouceList.add(r);
			}
		}
		resourceList.removeAll(delResouceList);
				
		modelMap.put("pagination", pagination);
		modelMap.put("testCosList", testCosList);
		modelMap.put("formalCosList", formalCosList);
		modelMap.put("resourceList", resourceList);
		
		return "/resource/resUserList";
	}
	
	@RequestMapping("/changeRes")
	public String changeRes(HttpServletRequest request,RedirectAttributes redirectAttributes) {
		//CSRF.validate(request);
		try {
			String resId = request.getParameter("resId");
			String[] corpIds = request.getParameterValues("checkedIds");
			Map<String,Object> paramsMap = new HashMap<String,Object>();
			if(corpIds != null && resId!=null) {
				int len = corpIds.length;
				Integer[] corpIdsArray = new Integer[len];
				for(int i=0;i<len;i++){
					corpIdsArray[i] = Integer.valueOf(corpIds[i]);
				}
				paramsMap.put("corpIdsArray", corpIdsArray);
				paramsMap.put("resId", resId);
				corpService.changeRes(paramsMap);
				redirectAttributes.addFlashAttribute(Message.MSG, "更改成功！");
			}else {
				redirectAttributes.addFlashAttribute(Message.MSG, "没有选择通道！");
			}
		}catch(Exception e) {
			redirectAttributes.addFlashAttribute(Message.MSG, "更改出错！");
			log.error(e.getMessage(), e);
		}
		
		return "redirect:resUserList";
	}
}
