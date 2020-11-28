package cn.edm.controller.corp;

import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import cn.edm.constant.Message;
import cn.edm.constants.Status;
import cn.edm.domain.ApiTrigger;
import cn.edm.domain.Corp;
import cn.edm.domain.Cos;
import cn.edm.domain.Resource;
import cn.edm.domain.Robot;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.persistence.CorpMapper;
import cn.edm.security.UserContext;
import cn.edm.service.ApiTriggerService;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.ResourceService;
import cn.edm.service.RobotService;
import cn.edm.service.UserService;
import cn.edm.util.CSRF;
import cn.edm.util.DateUtil;
import cn.edm.util.XssFilter;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;


@Controller
@RequestMapping("/corp")
public class CorpController {
	
	@Autowired
	private CorpService corpService;	
	@Autowired
	private UserService userService;	
	@Autowired
	private CosService cosService;	
	@Autowired
	private ResourceService resourceService;
	@Autowired
	private ApiTriggerService apiTriggerService;	
	@Autowired
	private RobotService robotService;
	@Autowired
	private CorpMapper corpMapper;
		
	/**
	 * 机构列表
	 */
	@RequestMapping(value="/corpList")
	public String corpList(HttpServletRequest request, ModelMap modelMap) {		
		List<Corp> corpList = corpService.getAllCorp();	
		List<Corp> oneCorpList = corpService.getOneCorp();	
		modelMap.put("corpList", corpList);
		modelMap.put("corpTotal",oneCorpList.size());
		return "/corp/corpList";
	}
	
	/**
	 * 查看
	 */
	@RequestMapping(value="/corpView")
	public String corpView(HttpServletRequest request, ModelMap modelMap, @RequestParam(value="id") String corpId) {		
		corpId = XssFilter.filterXss(corpId);
		List<Robot> robotsList = null;
		
		if(corpId!=null && !"".equals(corpId)) {
			Corp corp = corpService.getByCorpId(Integer.valueOf(corpId));
			Cos cos = cosService.getCosByCosId(corp.getCosId());
			modelMap.put("corp", corp);
			modelMap.put("cos", cos);
			ApiTrigger apiTrigger = apiTriggerService.getByCorpId(Integer.valueOf(corpId));
			modelMap.put("apiTrigger", apiTrigger);
							
			Map<String,Object> robotMap = Maps.newHashMap();
			robotMap.put("corpId",corp.getCorpId());
			robotsList = robotService.getRobotList(robotMap);			
		}
		
		Map<String,Object> paramsMap = Maps.newHashMap();
		paramsMap.put("status",Status.ENABLED);
		paramsMap.put("type",Cos.TESTED);
		List<Cos> testCosList = cosService.selectByTypeAndStatus(paramsMap);
		paramsMap.put("type",Cos.FORMAL);
		List<Cos> formalCosList = cosService.selectByTypeAndStatus(paramsMap);
		List<Resource> resourceList = resourceService.getAll();
		// 不显示API通道
		List<Resource> delResouceList = Lists.newArrayList();
		for(Resource r : resourceList) {
			if("api".equals(r.getResourceId())){
				delResouceList.add(r);
			}
		}
		resourceList.removeAll(delResouceList);
		modelMap.put("testCosList", testCosList);
		modelMap.put("formalCosList", formalCosList);
		modelMap.put("resourceList", resourceList);
		modelMap.put("title", "查看");
		modelMap.put("robotsList", robotsList);
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		modelMap.put("triggerNameList", triggerNameList);
				
		modelMap.put("backUrl", "/corp/corpList");
		modelMap.put("successUrl", "/corp/corpList");
		
		modelMap.put("onlyView", 1);//只能查看
				
		return "/corp/corpView";
	}
	
	/**
	 * 创建新机构
	 */
	@RequestMapping(value="/createCorp", method=RequestMethod.POST)
	public String createCorp(HttpServletRequest request,ModelMap modelMap) {
		CSRF.generate(request);
		String parentId = request.getParameter("pId");
		List<Robot> robotsList = null;
		if(!"0".equals(parentId)){//非一级机构属性均使用所属一级机构的属性而且不能修改
			if(parentId!=null && !"".equals(parentId)) {
				Corp parentCorp = corpService.getByCorpId(Integer.valueOf(parentId));
				parentCorp.setCompany("");
				Cos cos = cosService.getCosByCosId(parentCorp.getCosId());
				modelMap.put("corp", parentCorp);
				modelMap.put("parentId", parentCorp.getCorpId());
				modelMap.put("cos", cos);
				ApiTrigger apiTrigger = apiTriggerService.getByCorpId(Integer.valueOf(parentId));
				modelMap.put("apiTrigger", apiTrigger);
								
				Map<String,Object> robotMap = Maps.newHashMap();
				robotMap.put("corpId",parentCorp.getCorpId());
				robotsList = robotService.getRobotList(robotMap);
				modelMap.put("robotsList", robotsList);
			}
		}
						
		/*List<Cos> testCosList = cosService.getByType(Cos.TESTED);
		List<Cos> formalCosList = cosService.getByType(Cos.FORMAL);*/
		Map<String,Object> paramsMap = Maps.newHashMap();
		paramsMap.put("status",Status.ENABLED);
		paramsMap.put("type",Cos.TESTED);
		List<Cos> testCosList = cosService.selectByTypeAndStatus(paramsMap);
		paramsMap.put("type",Cos.FORMAL);
		List<Cos> formalCosList = cosService.selectByTypeAndStatus(paramsMap);
		
		List<Resource> resourceList = resourceService.getAll();
		// 通道查询不显示API通道
		List<Resource> delResouceList = Lists.newArrayList();
		for(Resource r : resourceList) {
			if("api".equals(r.getResourceId())){
				delResouceList.add(r);
			}
		}
		resourceList.removeAll(delResouceList);
		modelMap.put("testCosList", testCosList);
		modelMap.put("formalCosList", formalCosList);
		modelMap.put("resourceList", resourceList);
		modelMap.put("parentId", parentId);
		
				
		modelMap.put("backUrl", "/corp/corpList");						
		modelMap.put("successUrl", "/corp/corpList");
		
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		modelMap.put("triggerNameList", triggerNameList);
		
		
		if(!"0".equals(parentId)){
			return "/corp/subCorpEdit";
		}else{
			return "/corp/corpEdit";
		}
						
	}
	
	@RequestMapping(value="/corpEdit")
	public String corpEdit(HttpServletRequest request, ModelMap modelMap) {
		CSRF.generate(request);
		String corpId = XssFilter.filterXss(request.getParameter("id"));
		String parentId = XssFilter.filterXss(request.getParameter("pId"));
		List<Robot> robotsList = null;
		
		if(corpId!=null && !"".equals(corpId)) {
			Corp corp = corpService.getByCorpId(Integer.valueOf(corpId));
			Cos cos = cosService.getCosByCosId(corp.getCosId());
			modelMap.put("corp", corp);
			modelMap.put("parentId", corp.getParentId());
			modelMap.put("cos", cos);
			ApiTrigger apiTrigger = apiTriggerService.getByCorpId(Integer.valueOf(corpId));
			modelMap.put("apiTrigger", apiTrigger);
							
			Map<String,Object> robotMap = Maps.newHashMap();
			robotMap.put("corpId",corp.getCorpId());
			robotsList = robotService.getRobotList(robotMap);			
		}
		
		/*List<Cos> testCosList = cosService.getByType(Cos.TESTED);
		List<Cos> formalCosList = cosService.getByType(Cos.FORMAL);*/
		Map<String,Object> paramsMap = Maps.newHashMap();
		paramsMap.put("status",Status.ENABLED);
		paramsMap.put("type",Cos.TESTED);
		List<Cos> testCosList = cosService.selectByTypeAndStatus(paramsMap);
		paramsMap.put("type",Cos.FORMAL);
		List<Cos> formalCosList = cosService.selectByTypeAndStatus(paramsMap);
		
		List<Resource> resourceList = resourceService.getAll();
		// 不显示API通道
		List<Resource> delResouceList = Lists.newArrayList();
		for(Resource r : resourceList) {
			if("api".equals(r.getResourceId())){
				delResouceList.add(r);
			}
		}
		resourceList.removeAll(delResouceList);
		modelMap.put("testCosList", testCosList);
		modelMap.put("formalCosList", formalCosList);
		modelMap.put("resourceList", resourceList);

		modelMap.put("robotsList", robotsList);
		List<String> triggerNameList = apiTriggerService.getTriggerName();
		modelMap.put("triggerNameList", triggerNameList);		
				
		modelMap.put("backUrl", "/corp/corpList");
		modelMap.put("successUrl", "/corp/corpList");
				
		if(!"0".equals(parentId)){
			modelMap.put("exist", corpId);
			return "/corp/subCorpEdit";
		}else{
			return "/corp/corpEdit";
		}
	}
	//保存对一级机构的新增或修改
	@RequestMapping(value="/save", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> save(HttpServletRequest request, Corp corp) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		corp.setModifyTime(new Date());
		try {
			String isNeedAudit = corp.getIsNeedAudit();						
			//新增一级机构
			if(corp.getExist()==null || "".equals(corp.getExist())){	
				//判断机构是否存在
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("company", corp.getCompany().trim());
				paramsMap.put("corpId", null);
				boolean corpFalg = corpService.isCorpExist(paramsMap);				
				
				if(corpFalg){
					resultMap = getResultMap(0, 0, 0, "","add", "机构名称已存在");
				}else {
					corp = getCorp(corp,isNeedAudit);
					//xss检查
					Map<String,Object> map = userService.verifyXss(corp);						
					if((Boolean) map.get(Message.RESULT)) {						
						//网络发件人验证
						String[] robots = request.getParameterValues("robots");
						userService.checkRobots(robots);
						userService.isRobotsExist(corp.getFormalId(), robots);
						corp.setRobots(robots);
						corp.setCompany(corp.getCompany().trim());
						corp.setCorpPath(corp.getCompany().trim());
						userService.save(corp);
						
						resultMap = getResultMap(1, corp.getCorpId(), corp.getParentId(), corp.getCompany(),"add", "创建机构成功");				
					}else {
						resultMap = getResultMap(0, 0, 0, "", "add", "创建机构失败");
					}
				}
				
			}else {
				Corp oldCorp = corpService.getByCorpId(Integer.valueOf(corp.getExist()));				
				//判断机构是否存在
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("company", corp.getCompany().trim());
				paramsMap.put("corpId", oldCorp.getCorpId());
				boolean corpFalg = corpService.isCorpExist(paramsMap);

				if(corpFalg){
					resultMap.put(Message.MSG, "机构名称已存在");
					resultMap = getResultMap(0, 0, 0, "","update", "机构名称已存在");
				}else {
					oldCorp = updateCorp(oldCorp, corp, isNeedAudit);
					int returnCorpId = oldCorp.getCorpId(); 
					int returnParentId = oldCorp.getParentId();
					String returnCompany = oldCorp.getCompany();
					
					if(resultMap.get(Message.MSG) == null){
						//xss检查
						Map<String,Object> map = userService.verifyXss(oldCorp);
						if((Boolean) map.get(Message.RESULT)) {
							//网络发件人验证
							String[] robots = request.getParameterValues("robots");
							userService.checkRobots(robots);
							userService.isRobotsExist(corp.getFormalId(), robots);
							oldCorp.setRobots(robots);
							oldCorp.setModifyTime(new Date());
							userService.update(oldCorp);
							
							//随着一级机构的属性修改，所属所有的子机构属性跟着修改
							Map<String,Object> paramsMap1 = Maps.newHashMap();
							paramsMap1.put("corpId", oldCorp.getCorpId());
							List<Corp> subCorpList = corpService.getCorpByTopLevel(paramsMap1);
							List<Integer> corpIds = subCorpList.get(0).getBranchCorp();
							for(int oldCorpId : corpIds){//需要修改的机构							
								Corp oldSubCorp = corpService.getByCorpId(oldCorpId);
								if(oldSubCorp.getParentId() != 0){//修改一级机构的子机构
									//除了corpId、parentId、company、corpPath之外其余属性均用一级机构属性
									Corp oldSubParentCorp = corpService.getByCorpId(oldSubCorp.getParentId());																		
									oldCorp.setCorpId(oldSubCorp.getCorpId());
									oldCorp.setParentId(oldSubCorp.getParentId());
									oldCorp.setCompany(oldSubCorp.getCompany());
									oldCorp.setCorpPath(oldSubParentCorp.getCorpPath() + "-" + oldSubCorp.getCompany());
									oldCorp.setModifyTime(new Date());
									userService.update(oldCorp);
								}																							
							}
						
							resultMap = getResultMap(1, returnCorpId, returnParentId, returnCompany,"update", "修改机构成功");
						}else {
							resultMap = getResultMap(0, 0, 0, "", "update", "修改机构失败");
						}
					}
				}
			}
		}catch(Errors e) {
			e.printStackTrace();
			resultMap = getResultMap(0, 0, 0, "","add", e.getMessage());
		}catch(Exception e) {
			e.printStackTrace();
			resultMap = getResultMap(0, 0, 0, "","add", "保存机构失败");
		}
		return resultMap;
	}
	
	@RequestMapping(value="/saveSubCorp", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> saveSubCorp(HttpServletRequest request, Corp corp) {
		CSRF.validate(request);
		Map<String,Object> resultMap = new HashMap<String,Object>();				
		try {
			if(corp.getExist()==null || "".equals(corp.getExist())){//新增
				//判断机构是否存在
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("company", corp.getCompany().trim());
				paramsMap.put("corpId", null);
				boolean corpFalg = corpService.isCorpExist(paramsMap);
				
				if(corpFalg){
					resultMap = getResultMap(0, 0, 0, "","add", "机构名称已存在");
				}else {
					if(!XssFilter.isValid(corp.getCompany())){
						resultMap = getResultMap(0, 0, 0, "","add", "机构名称含有非法字符");
					}else{
						//网络发件人验证
						String[] robots = request.getParameterValues("robots");
						
						Corp parentCorp = corpService.getByCorpId(corp.getParentId());
						//子机构使用父机构属性
						Corp subCorp = parentCorp;
						subCorp.setParentId(corp.getParentId());
						subCorp.setRobots(robots);
						subCorp.setCompany(corp.getCompany().trim());
						subCorp.setCreateTime(DateUtil.getCurrentDate());
						subCorp.setIsGroupsApi(corp.getIsGroupsApi());
						subCorp.setIsTriggerApi(corp.getIsTriggerApi());
						//设置有层级关系的机构全称
						subCorp.setCorpPath(parentCorp.getCorpPath() + "-" + corp.getCompany().trim());
						subCorp.setModifyTime(new Date());
                        userService.save(subCorp);
						
						resultMap = getResultMap(1, subCorp.getCorpId(), corp.getParentId(), corp.getCompany(),"add", "创建机构成功");	
					}					
				}								
				
			}else{//修改
				Corp oldCorp = corpService.getByCorpId(Integer.valueOf(corp.getExist()));
				String oldCompany = oldCorp.getCompany();
				//判断机构是否存在
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("company", corp.getCompany().trim());
				paramsMap.put("corpId", oldCorp.getCorpId());
				boolean corpFalg = corpService.isCorpExist(paramsMap);
				if(corpFalg){
					resultMap = getResultMap(0, 0, 0, "","update", "机构名称已存在");
				}else {
					if(!XssFilter.isValid(corp.getCompany())){
						resultMap = getResultMap(0, 0, 0, "","add", "机构名称含有非法字符");
					}else{
						oldCorp.setCompany(corp.getCompany().trim());
						oldCorp.setModifyTime(DateUtil.getCurrentDate());
						corpMapper.updateCorp(oldCorp);
						//修改下属子机构的机构全称
						Map<String,Object> paramsMap1 = Maps.newHashMap();
						paramsMap1.put("corpPath", "-" + oldCompany);
						List<Corp> oldSubCorpList = corpMapper.getCorpList(paramsMap1);
						for(Corp oldSubCorp : oldSubCorpList){
							String newCorpPath = oldSubCorp.getCorpPath().replace("-" + oldCompany, "-" + corp.getCompany().trim());
							oldSubCorp.setCorpPath(newCorpPath);
							oldSubCorp.setModifyTime(new Date());
							corpMapper.updateCorp(oldSubCorp);
						}
						
						resultMap = getResultMap(1, oldCorp.getCorpId(), oldCorp.getParentId(), oldCorp.getCompany(),"update", "修改机构成功");
					}				
				}			
				
			}
						
		}catch(Errors e) {
			e.printStackTrace();
			resultMap = getResultMap(0, 0, 0, "","add", e.getMessage());
		}catch(Exception e) {
			e.printStackTrace();
			resultMap = getResultMap(0, 0, 0, "","add", "保存子机构失败");
		}		
		
		return resultMap;
	}
	
	/**
	 * 删除机构
	 */
	@RequestMapping(value="/delete", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> delete(HttpServletRequest request,@RequestParam(value="id") String corpId) {
		corpId = XssFilter.filterXss(corpId);
		Map<String,Object> resultMap = new HashMap<String,Object>();
		
		try {
			List<Corp> corpList = corpService.getSubCorp(corpId);
			if(corpList != null && corpList.size() >0){
				resultMap.put("code", 0);
				resultMap.put("message", "当前机构下有子机构,不能删除！");
			}else{
				List<User> userList = userService.getAllUserByCorpId(corpId);
				for(User user : userList){
					//删除子账号相关信息
					List<User> subUserList = userService.getSubUserByManagerId(user.getUserId());
					for(User subUser : subUserList){
						userService.deleteUser(subUser.getUserId());
					}						
					userService.deleteUser(user.getUserId());			
				}		
				corpService.deleteByCorpId(corpId);
				
				resultMap.put("code", 1);
				resultMap.put("message", "成功删除机构");
			}								
		} catch (Exception e) {
			e.printStackTrace();
			resultMap.put("code", 0);
			
		}	
		resultMap.put("id", corpId);
		resultMap.put("type", "delete");
		
		return resultMap;
	}
	
	
	private Corp getCorp(Corp corp,String isNeedAudit) {
		if("1".equals(isNeedAudit)) {
			corp.setAuditPath("/"+UserContext.getCurrentUser().getCorpId()+"/");
		}else {
			corp.setAuditPath("");
		}
		corp.setStatus(Status.ENABLED);
		corp.setCreateTime(DateUtil.getCurrentDate());
		corp.setWay("system");
		return corp;
	}
	
	private Corp updateCorp(Corp oldCorp,Corp newCorp,String isNeedAudit) {
		if("1".equals(isNeedAudit)) {
			oldCorp.setAuditPath("/"+UserContext.getCurrentUser().getCorpId()+"/");
		}else {
			oldCorp.setAuditPath("");
		}
		oldCorp.setCompany(newCorp.getCompany().trim());
		oldCorp.setAddress(newCorp.getAddress());
		oldCorp.setWebsite(newCorp.getWebsite());
		oldCorp.setContact(newCorp.getContact());
		oldCorp.setTelephone(newCorp.getTelephone());
		oldCorp.setIndustry(newCorp.getIndustry());
		oldCorp.setZip(newCorp.getZip());
		oldCorp.setCosId(newCorp.getCosId());
		oldCorp.setFormalId(newCorp.getFormalId());
		oldCorp.setTestedId(newCorp.getTestedId());
		oldCorp.setSenderValidate(newCorp.getSenderValidate());
		oldCorp.setOpenSms(newCorp.getOpenSms());
		oldCorp.setIsGroupsApi(newCorp.getIsGroupsApi());
		oldCorp.setIsTriggerApi(newCorp.getIsTriggerApi());
		oldCorp.setIsNeedAudit(newCorp.getIsNeedAudit());
		oldCorp.setCorpPath(newCorp.getCompany());
		return oldCorp;
	}
	
	
	private Map<String,Object> getResultMap(int code,int corpId,int parentId,String corpName,String type,String message){
		Map<String,Object> resultMap = new HashMap<String, Object>();
		if(code == 1){//成功
			resultMap.put("code", code);
			resultMap.put("id", corpId);
			resultMap.put("pid", parentId);
			resultMap.put("name", corpName);
			resultMap.put("type", type);
			resultMap.put("message", message);
		}else{
			resultMap.put("code", code);
			resultMap.put("message", message);
		}	
		return resultMap;
	}

}
