package cn.edm.service;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.constant.CategoryDefine;
import cn.edm.constant.Message;
import cn.edm.constant.PropDefine;
import cn.edm.constant.TypeDefine;
import cn.edm.constant.Value;
import cn.edm.domain.ApiTrigger;
import cn.edm.domain.Corp;
import cn.edm.domain.Prop;
import cn.edm.domain.Resource;
import cn.edm.domain.Robot;
import cn.edm.domain.Tag;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.model.Category;
import cn.edm.persistence.ApiTriggerMapper;
import cn.edm.persistence.CampaignMapper;
import cn.edm.persistence.CategoryMapper;
import cn.edm.persistence.CorpMapper;
import cn.edm.persistence.FilterMapper;
import cn.edm.persistence.FormMapper;
import cn.edm.persistence.PropMapper;
import cn.edm.persistence.ResourceMapper;
import cn.edm.persistence.RobotMapper;
import cn.edm.persistence.SelectionMapper;
import cn.edm.persistence.TagMapper;
import cn.edm.persistence.TaskMapper;
import cn.edm.persistence.TemplateMapper;
import cn.edm.persistence.TemplateSettingMapper;
import cn.edm.persistence.UserMapper;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.util.PropMap;
import cn.edm.util.Regex;
import cn.edm.util.ValidUtil;
import cn.edm.util.XssFilter;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;


/**
 * 用户业务接口实现
 * @author Luxh
 *
 */

@Service
public class UserServiceImpl implements UserService{
	
	@Autowired
	private UserMapper userMapper;
	@Autowired
	private CorpMapper corpMapper;
	
	@Autowired
	private CategoryMapper categoryMapper;
	
	@Autowired
	private PropMapper propMapper;
	
	@Autowired
	private ApiTriggerMapper apiTriggerMapper;
	
	@Autowired
	private TagMapper tagMapper;
	
	@Autowired
	private ResourceMapper resourceMapper;
	
	@Autowired
	private RobotMapper robotMapper;
	
	@Autowired
	private FilterMapper filterMapper;
	
	@Autowired
	private SelectionMapper selectionMapper;
	
	@Autowired
	private FormMapper formMapper;
	
	@Autowired
	private TemplateMapper templateMapper;
	
	@Autowired
	private TaskMapper taskMapper;
	
	@Autowired
	private TemplateSettingMapper templateSettingMapper;
	
	@Autowired
	private CampaignMapper campaignMapper;

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public User getByUserId(String userId) {
		return userMapper.selectByUserId(userId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<User> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		
		List<User> recordList = userMapper.selectUsers(paramsMap);
		long recordCount = userMapper.selectUsersCount(paramsMap);
		
		Pagination<User> pagination = new Pagination<User>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}


	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void save(Corp corp) {
		corp.setJoinApi(corp.getIsGroupsApi()); // 默认不开通API群发
		corpMapper.insertCorp(corp);
		
		// API投递任务
		// 触发类型
		// 只有企业客户才有触发类型的任务: 1为企业客户账号，6为只读的企业客户账号，4为运营人员账号
		ApiTrigger apiTrigger = new ApiTrigger();
		apiTrigger.setCorpId(corp.getCorpId());
		apiTrigger.setTriggerName("");
		apiTrigger.setStatus(corp.getIsTriggerApi());
		apiTriggerMapper.insert(apiTrigger);

		String[] robots = corp.getRobots();
		if(robots!=null&&robots.length>0) {
			List<Robot> robotList = Lists.newArrayList();
			for(String robot:robots) {
				if(StringUtils.isNotEmpty(robot)) {
					Robot r = new Robot();
					r.setCorpId(corp.getCorpId());
					r.setEmail(robot.trim());
					robotList.add(r);
				}
			}
			if(robotList.size()>0) {
				robotMapper.insertBatch(robotList);
			}
		}
		
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void save(User user) {
		userMapper.insertUser(user);
		/*//每个账号在trigger_api表有单独配置，但状态由机构统一管理		
		ApiTrigger corpApiTrigger = apiTriggerMapper.selectByCorpId(user.getCorpId());
						
		ApiTrigger apiTrigger = new ApiTrigger();
		apiTrigger.setCorpId(user.getCorpId());
		apiTrigger.setUserId(user.getUserId());
		apiTrigger.setTriggerName("");
		if(corpApiTrigger != null){
			apiTrigger.setStatus(corpApiTrigger.getStatus());
		}else{
			apiTrigger.setStatus(0);
		}		
		apiTriggerMapper.insert(apiTrigger);*/
		//保存类别
		Corp corp = corpMapper.selectByCorpId(user.getCorpId());
		categoryMapper.insertBatch(getCategory(user.getCorpId(),corp.getParentId(),user.getUserId()));
		//保存属性
		propMapper.insertBatch(getProp(user.getCorpId(), user.getUserId()));
		
		tagMapper.insertTag(getTag(user.getCorpId(), user.getUserId()));		
	}
	
	private List<Category> getCategory(Integer corpId,Integer parentId,String userId) {
		List<Category> categoryList = Lists.newArrayList();
		//保存类别
		Category c1 = new Category();
		c1.setCorpId(corpId);
		c1.setUserId(userId);
		c1.setCategoryName(CategoryDefine.DELIVERY.getKey());
		c1.setCategoryDesc(CategoryDefine.DELIVERY.getVal());
		c1.setType(Category.TAG);
		categoryList.add(c1);
		
		Category c2 = new Category();
		c2.setCorpId(corpId);
		c2.setUserId(userId);
		c2.setCategoryName(CategoryDefine.TEST.getKey());
		c2.setCategoryDesc(CategoryDefine.TEST.getVal());
		c2.setType(Category.TAG);
		categoryList.add(c2);
		
		Category c3 = new Category();
		c3.setCorpId(corpId);
		c3.setUserId(userId);
		c3.setCategoryName(CategoryDefine.UNSUBSCRIBE.getKey());
		c3.setCategoryDesc(CategoryDefine.UNSUBSCRIBE.getVal());
		c3.setType(Category.TAG);
		categoryList.add(c3);
		
		Category c4 = new Category();
		c4.setCorpId(corpId);
		c4.setUserId(userId);
		c4.setCategoryName(CategoryDefine.FORM.getKey());
		c4.setCategoryDesc(CategoryDefine.FORM.getVal());
		c4.setType(Category.TAG);
		categoryList.add(c4);
		
		Category c5 = new Category();
		c5.setCorpId(corpId);
		c5.setUserId(userId);
		c5.setCategoryName(CategoryDefine.TOUCH.getKey());
		c5.setCategoryDesc(CategoryDefine.TOUCH.getVal());
		c5.setType(Category.TEMPLATE);
		categoryList.add(c5);
		
		Category c6 = new Category();
		c6.setCorpId(corpId);
		c6.setUserId(userId);
		c6.setCategoryName(CategoryDefine.TEMP.getKey());
		c6.setCategoryDesc(CategoryDefine.TEMP.getVal());
		c6.setType(Category.TAG);
		categoryList.add(c6);
		
		Category c7 = new Category();
		c7.setCorpId(corpId);
		c7.setUserId(userId);
		c7.setCategoryName(CategoryDefine.BILL.getKey());
		c7.setCategoryDesc(CategoryDefine.BILL.getVal());
		c7.setType(Category.TEMPLATE);
		categoryList.add(c7);
				
		Category c8 = new Category();
		c8.setCorpId(corpId);
		c8.setUserId(userId);
		c8.setCategoryName(CategoryDefine.DEFAULT.getKey());
		c8.setCategoryDesc(CategoryDefine.DEFAULT.getVal());
		c8.setType(Category.TEMPLATE);
		categoryList.add(c8);
		
		if(parentId == 0){//一级机构添加“自定义推荐”模板
			Category c9 = new Category();
			c9.setCorpId(corpId);
			c9.setUserId(userId);
			c9.setCategoryName(CategoryDefine.USER_DEFINED.getKey());
			c9.setCategoryDesc(CategoryDefine.USER_DEFINED.getVal());
			c9.setType(Category.TEMPLATE);
			categoryList.add(c9);
		}
								
		return categoryList;		
	}
	
	/*private List<Category> getCategory(Integer corpId,String userId) {
		List<Category> categoryList = Lists.newArrayList();
		//保存类别
		Category c1 = new Category();
		c1.setCorpId(corpId);
		c1.setUserId(userId);
		c1.setCategoryName(CategoryDefine.DELIVERY.getKey());
		c1.setCategoryDesc(CategoryDefine.DELIVERY.getVal());
		c1.setType(Category.TAG);
		categoryList.add(c1);
		
		Category c2 = new Category();
		c2.setCorpId(corpId);
		c2.setUserId(userId);
		c2.setCategoryName(CategoryDefine.TEST.getKey());
		c2.setCategoryDesc(CategoryDefine.TEST.getVal());
		c2.setType(Category.TAG);
		categoryList.add(c2);
		
		Category c3 = new Category();
		c3.setCorpId(corpId);
		c3.setUserId(userId);
		c3.setCategoryName(CategoryDefine.UNSUBSCRIBE.getKey());
		c3.setCategoryDesc(CategoryDefine.UNSUBSCRIBE.getVal());
		c3.setType(Category.TAG);
		categoryList.add(c3);
		
		Category c4 = new Category();
		c4.setCorpId(corpId);
		c4.setUserId(userId);
		c4.setCategoryName(CategoryDefine.FORM.getKey());
		c4.setCategoryDesc(CategoryDefine.FORM.getVal());
		c4.setType(Category.TAG);
		categoryList.add(c4);
		
		Category c5 = new Category();
		c5.setCorpId(corpId);
		c5.setUserId(userId);
		c5.setCategoryName(CategoryDefine.TOUCH.getKey());
		c5.setCategoryDesc(CategoryDefine.TOUCH.getVal());
		c5.setType(Category.TEMPLATE);
		categoryList.add(c5);
		
		Category c6 = new Category();
		c6.setCorpId(corpId);
		c6.setUserId(userId);
		c6.setCategoryName(CategoryDefine.TEMP.getKey());
		c6.setCategoryDesc(CategoryDefine.TEMP.getVal());
		c6.setType(Category.TAG);
		categoryList.add(c6);
		
		return categoryList;
		
	}*/
	
	public List<Prop> getProp(Integer corpId,String userId) {
		List<Prop> propList = Lists.newArrayList();
		Prop p1 = new Prop();
		p1.setCorpId(corpId);
		p1.setUserId(userId);
		p1.setPropName(PropDefine.EMAIL.getKey());
		p1.setPropDesc(Value.DEFAULT+PropDefine.EMAIL.getVal());
		p1.setType(TypeDefine.STRING.getKey());
		p1.setRequired(1);
		propList.add(p1);
		
		Prop p2 = new Prop();
		p2.setCorpId(corpId);
		p2.setUserId(userId);
		p2.setPropName(PropDefine.TRUE_NAME.getKey());
		p2.setPropDesc(Value.DEFAULT+PropDefine.TRUE_NAME.getVal());
		p2.setType(TypeDefine.STRING.getKey());
		p2.setRequired(1);
		propList.add(p2);
		
		Prop p3 = new Prop();
		p3.setCorpId(corpId);
		p3.setUserId(userId);
		p3.setPropName(PropDefine.NICK_NAME.getKey());
		p3.setPropDesc(Value.DEFAULT+PropDefine.NICK_NAME.getVal());
		p3.setType(TypeDefine.STRING.getKey());
		p3.setRequired(1);
		propList.add(p3);
		
		Prop p4 = new Prop();
		p4.setCorpId(corpId);
		p4.setUserId(userId);
		p4.setPropName(PropDefine.GENDER.getKey());
		p4.setPropDesc(Value.DEFAULT+PropDefine.GENDER.getVal());
		p4.setType(TypeDefine.STRING.getKey());
		p4.setRequired(1);
		propList.add(p4);
		
		Prop p5 = new Prop();
		p5.setCorpId(corpId);
		p5.setUserId(userId);
		p5.setPropName(PropDefine.JOB.getKey());
		p5.setPropDesc(Value.DEFAULT+PropDefine.JOB.getVal());
		p5.setType(TypeDefine.STRING.getKey());
		p5.setRequired(1);
		propList.add(p5);
		
		Prop p6 = new Prop();
		p6.setCorpId(corpId);
		p6.setUserId(userId);
		p6.setPropName(PropDefine.AGE.getKey());
		p6.setPropDesc(Value.DEFAULT+PropDefine.AGE.getVal());
		p6.setType(TypeDefine.INTEGER.getKey());
		p6.setRequired(1);
		propList.add(p6);
		
		Prop p7 = new Prop();
		p7.setCorpId(corpId);
		p7.setUserId(userId);
		p7.setPropName(PropDefine.BIRTHDAY.getKey());
		p7.setPropDesc(Value.DEFAULT+PropDefine.BIRTHDAY.getVal());
		p7.setType(TypeDefine.DATE.getKey());
		p7.setRequired(1);
		propList.add(p7);
		
		return propList;
	}
	
	
	private Tag getTag(Integer corpId,String userId) {
		
		Map<String,Object> paramsMap = Maps.newHashMap();
		paramsMap.put("userId", userId);
		paramsMap.put("categoryName", "退订");
		
		Category c = categoryMapper.selectByUserIdAndCatalogName(paramsMap);
		
		Tag tag = new Tag();
		tag.setCorpId(corpId);
		tag.setUserId(userId);
		tag.setCategoryId(c.getCategoryId());
		tag.setDbId(0);
		tag.setTagName("默认退订");
		tag.setTagDesc("系统默认退订标签");
		tag.setEmailCount(0);
		//tag.setFilterCnds("");
		return tag;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public boolean isUserExist(String userId) {
		boolean flag = false;
		User user = userMapper.selectByUserId(userId);
		if(user != null) {
			flag = true;
		}
		return flag;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void update(Corp corp, User user) {
		
//		// 是否群发，1为启用  0为禁用
//		corp.setJoinApi(user.getIsGroupsApi());
		corpMapper.updateCorp(corp);
		userMapper.updateUser(user);
		
//		// API投递任务
//		// 触发类型
//		// 只有企业客户才有触发类型的任务: 1为企业客户账号，6为只读的企业客户账号，4为运营人员账号 
//		if("1".equals(user.getCustomerType())){
//			// API 触发：1为启用  0为禁用
//			List<ApiTrigger> apiTriggers = apiTriggerMapper.selectApiTriggersByCorpId(corp.getCorpId());
//			if(apiTriggers != null){
//				for(ApiTrigger apiTrigger : apiTriggers) {
//					// 更新触发类型
//					apiTrigger.setStatus(user.getIsTriggerApi());
//					apiTriggerMapper.update(apiTrigger);
//				}
//			}else{
//				ApiTrigger apiTrigger = new ApiTrigger();
//				apiTrigger.setCorpId(corp.getCorpId());
//				apiTrigger.setUserId(user.getUserId());
//				apiTrigger.setTriggerName("");
//				apiTrigger.setStatus(user.getIsTriggerApi());
//				apiTriggerMapper.insert(apiTrigger);
//			}
//		}
//		
//		//先删除
//		Map<String,Object> params = Maps.newHashMap();
//		params.put("corpId", corp.getCorpId());
//		params.put("userId", user.getUserId());
//		robotMapper.deleteByCorpId(params);
//		String[] robots = user.getRobots();
//		if(robots!=null&&robots.length>0) {
//			
//			List<Robot> robotList = Lists.newArrayList();
//			for(String robot:robots) {
//				if(StringUtils.isNotEmpty(robot)) {
//					Robot r = new Robot();
//					r.setCorpId(corp.getCorpId());
//					//r.setUserId(user.getUserId());
//					r.setEmail(robot.trim());
//					robotList.add(r);
//				}
//			}
//			
//			if(robotList.size()>0) {
//				robotMapper.insertBatch(robotList);
//			}
//		}
		
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void update(Corp corp) {		
		// 是否群发，1为启用  0为禁用
		corp.setJoinApi(corp.getIsGroupsApi());
		corpMapper.updateCorp(corp);
		
		// API投递任务
		// 触发类型
		// 只有企业客户才有触发类型的任务: 1为企业客户账号，6为只读的企业客户账号，4为运营人员账号 
		// API 触发：1为启用  0为禁用
		List<ApiTrigger> apiTriggers = apiTriggerMapper.selectApiTriggersByCorpId(corp.getCorpId());
		if (apiTriggers != null) {
			for (ApiTrigger apiTrigger : apiTriggers) {
				// 更新触发类型
				apiTrigger.setStatus(corp.getIsTriggerApi());
				apiTriggerMapper.update(apiTrigger);
			}
		} else {
			ApiTrigger apiTrigger = new ApiTrigger();
			apiTrigger.setCorpId(corp.getCorpId());
			apiTrigger.setTriggerName("");
			apiTrigger.setStatus(corp.getIsTriggerApi());
			apiTriggerMapper.insert(apiTrigger);
		}
		
		//先删除
		Map<String,Object> params = Maps.newHashMap();
		params.put("corpId", corp.getCorpId());
		robotMapper.deleteByCorpId(params);
		String[] robots = corp.getRobots();
		if(robots!=null&&robots.length>0) {
			
			List<Robot> robotList = Lists.newArrayList();
			for(String robot:robots) {
				if(StringUtils.isNotEmpty(robot)) {
					Robot r = new Robot();
					r.setCorpId(corp.getCorpId());
					//r.setUserId(user.getUserId());
					r.setEmail(robot.trim());
					robotList.add(r);
				}
			}
			
			if(robotList.size()>0) {
				robotMapper.insertBatch(robotList);
			}
		}
		
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void update(User user) {		
		userMapper.updateUser(user);		
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<User> getResPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		int[] types = {1};//前台管理员
		paramsMap.put("types", types);
		
		List<User> recordList = userMapper.selectResourceList(paramsMap);
		long recordCount = userMapper.selectResourceCount(paramsMap);
		
		Pagination<User> pagination = new Pagination<User>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public User getByRoleId(Integer roleId) {
		return userMapper.selectByRoleId(roleId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void saveUser(User user, boolean withRobots) {
		userMapper.insertUser(user);
		
		//保存类别
		Corp corp = corpMapper.selectByCorpId(user.getCorpId());
		categoryMapper.insertBatch(getCategory(user.getCorpId(),corp.getParentId(),user.getUserId()));
		//保存属性+
		propMapper.insertBatch(getProp(user.getCorpId(), user.getUserId()));
		
		tagMapper.insertTag(getTag(user.getCorpId(), user.getUserId()));
		
		if(withRobots) {
		
    		//先删除
    		Map<String,Object> params = Maps.newHashMap();
    		params.put("corpId", user.getCorpId());
    		robotMapper.deleteByCorpId(params);
    		String[] robots = user.getRobots();
    		if(robots!=null&&robots.length>0) {
    			List<Robot> robotList = Lists.newArrayList();
    			for(String robot:robots) {
    				if(StringUtils.isNotEmpty(robot)) {
    					Robot r = new Robot();
    					r.setCorpId(user.getCorpId());
    					//r.setUserId(user.getUserId());
    					r.setEmail(robot.trim());
    					robotList.add(r);
    				}
    			}
    			if(robotList.size()>0) {
    				robotMapper.insertBatch(robotList);
    			}
    		}
		
		}
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void updateUser(User user, boolean withRobots) {
		userMapper.updateUser(user);
		
		if(withRobots) {
			//先删除
	        Map<String,Object> params = Maps.newHashMap();
	        params.put("corpId", user.getCorpId());
	        robotMapper.deleteByCorpId(params);
	        String[] robots = user.getRobots();
	        if(robots!=null&&robots.length>0) {
	            
	            List<Robot> robotList = Lists.newArrayList();
	            for(String robot:robots) {
	                if(StringUtils.isNotEmpty(robot)) {
	                    Robot r = new Robot();
	                    r.setCorpId(user.getCorpId());
	                    //r.setUserId(user.getUserId());
	                    r.setEmail(robot.trim());
	                    robotList.add(r);
	                }
	            }
	            
	            if(robotList.size()>0) {
	                robotMapper.insertBatch(robotList);
	            }
	        }
		}
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void deleteUser(String userId) {		
		userMapper.deleteByUserId(userId);
		userMapper.deleteCampaignByUserId(userId);
		userMapper.deleteCategoryByUserId(userId);
		/*userMapper.deleteCopyRecipientTriggerByUserId(userId);*/  //这些数据和机构相关，删除账号时候不能删除，删除机构的时候要删除
		/*userMapper.deleteCopyTagTriggerByUserId(userId);*/
		userMapper.deleteFileByUserId(userId);
		userMapper.deleteFilterByUserId(userId);
		userMapper.deleteFormByUserId(userId);
		userMapper.deleteLoginHistoryByUserId(userId);
		userMapper.deletePlanByUserId(userId);
		userMapper.deletePropByUserId(userId);
		/*userMapper.deleteRecipientByUserId(userId);*/
		userMapper.deleteSelectionByUserId(userId);
		userMapper.deleteSenderByUserId(userId);
		userMapper.deleteTagByUserId(userId);
		userMapper.deleteTaskByUserId(userId);
		userMapper.deleteTemplateByUserId(userId);
		userMapper.deleteTemplateSettingByUserId(userId);
		userMapper.deleteTouchByUserId(userId);
		userMapper.deleteTrigerApiByUserId(userId);
		
		
	}

	
	@Override
	public Map<String, Object> verifyXss(Corp corp) {
		Map<String,Object> map = new HashMap<String,Object>();
		String message = "";
		boolean isValid = false;
		if(!XssFilter.isValid(corp.getCompany())){
			message = "机构名称含有非法字符！";
		}else if(!XssFilter.isValid(corp.getAddress())){
			message = "机构地址含有非法字符！";
		}else if(!XssFilter.isValid(corp.getWebsite())){
			message = "机构网址含有非法字符！";
		}else if(!XssFilter.isValid(corp.getContact())){
			message = "联系人姓名含有非法字符！";
		}else if(!XssFilter.isValid(corp.getTelephone())){
			message = "联系电话含有非法字符！";
		}else if(!XssFilter.isValid(corp.getZip())){
			message = "邮编含有非法字符！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public boolean isEmailExist(String email) {
		boolean flag = false;
		User user = userMapper.selectByEmail(email);
		if(user != null) {
			flag = true;
		}
		return flag;
	}

	@Override
	public Map<String, Object> verifyRegex(User user) {
		Map<String,Object> map = new HashMap<String,Object>();
		boolean isValid = false;
		String message = "";
		if(!ValidUtil.regex(user.getUserId(), Regex.ACCOUNT)) {
			message = "用户名由字母数字下划线组成";
		}else if(ValidUtil.find(user.getPassword(), Regex.PASSWORD)){
			message = "密码不能含有汉字！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	public Map<String, Object> verifyXss(User user) {
		Map<String,Object> map = new HashMap<String,Object>();
		String message = "";
		boolean isValid = false;
		if(!XssFilter.isValid(user.getUserId())) {
			message = "用户名含有非法字符！";
		}else if(!XssFilter.isValid(user.getEmail())){
			message = "邮箱含有非法字符！";
		}else if(!XssFilter.isValid(user.getTrueName())){
			message = "姓名含有非法字符！";
		}else if(!XssFilter.isValid(user.getPhone())){
			message = "联系电话含有非法字符！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public User getByCorpIdAndRoleId(Map<String, Object> paramMap) {
		return userMapper.selectByCorpIdAndRoleId(paramMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Long selectUsersByCorpIdAndRoleId (Map<String, Object> paramMap) {
		return userMapper.selectUsersByCorpIdAndRoleId(paramMap);
	}

	@Override
	public Map<String, Object> verifySubUser(User user) {
		Map<String,Object> map = new HashMap<String,Object>();
		boolean isValid = false;
		String message = "";
		if(!ValidUtil.regex(user.getUserId(), Regex.USER_NAME)) {
			message = "用户名以字母开头，由字母数字下划线组成!";
		}else if(ValidUtil.find(user.getPassword(), Regex.PASSWORD)){
			message = "密码不能含有汉字！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	public Map<String, Object> verifyPassword(String password) {
		Map<String,Object> map = new HashMap<String,Object>();
		boolean isValid = false;
		String message = "";
		if(ValidUtil.find(password, Regex.PASSWORD)){
			message = "密码不能含有汉字！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	public List<User> getByCorpId(Integer corpId) {
		// TODO Auto-generated method stub
		return userMapper.selectByCorpId(corpId);
	}

	@Override
	public User getByEmail(String email) {
		// TODO Auto-generated method stub
		return userMapper.selectByEmail(email);
	}

	@Override
	public boolean isEmailExistByUserId(Map<String, Object> paramMap) {
		boolean flag = false;
		User user = userMapper.selectByEmailAndUserId(paramMap);
		if(user != null) {
			flag = true;
		}
		return flag;
				
	}

	@Override
	public User getByUserIdAndRoleId(Map<String, Object> paramMap) {
		// TODO Auto-generated method stub
		return userMapper.selectByUserIdAndRoleId(paramMap);
	}

	@Override
	public void updateUserStatus(Map<String, Object> paramMap) {
		userMapper.updateStatusByUserId(paramMap);		
	}

	@SuppressWarnings("unchecked")
	@Override
	public void checkRobots(String[] robots) {
		if(robots==null||robots.length<1) {
			throw new Errors("请输入网络发件人！");
		}
		
		int counter = 0;
		for(String r:robots) {
			if(r!=null&&!"".equals(r.trim())) {
				counter++;
			}
		}
		if(counter == 0) {
			throw new Errors("请输入网络发件人！");
		}
		
		
		Map<String,Integer> robotMap = new PropMap();
		for(String robot : robots) {
			if(robot!=null&&!"".equals(robot.trim())) {
				robotMap.put(robot.trim(), 1);
				if(!ValidUtil.find(robot, Regex.UPPERCASE)){
					if(!ValidUtil.regex(robot, Regex.EMAIL)) {
						throw new Errors("网络发件人 " + robot + " 格式不正确！");
					}
				}else{
					throw new Errors("网络发件人不能包含大写字母！");
				}
				if(!XssFilter.isValid(robot)) {
					throw new Errors("网络发件人含有非法字符！");
				}
			}
		}
		
		//判断是否重复
		Iterator<Entry<String,Integer>>  iterator =  robotMap.entrySet().iterator();
		while(iterator.hasNext()) {
		    Entry<String,Integer> entry = iterator.next();
		    String key = entry.getKey();
		    Integer value = entry.getValue();
		    if(value>1) {
		    	throw new Errors("网络发件人 "+key+" 重复！");
		    }
		}
		
		
	}

	@Override
	public void isRobotsExist(String resId, String[] robots) {
		if(robots==null||robots.length<1) {
			throw new Errors("请输入网络发件人！");
		}
		String helo = resourceMapper.selectHeloList(resId);
		if(StringUtils.isEmpty(helo)) {
			throw new Errors("所选正式通道无发件人域！");
		}
		String[] heloArray = StringUtils.split(helo, ",");
		for(String robot : robots) {
			if(StringUtils.isNotEmpty(robot)) {
				boolean flag = false;
				String domain = robot.split("@")[1];
				for(String h:heloArray) {
					if(StringUtils.equalsIgnoreCase(h, domain.trim())){
						flag = true;
						break;
					}
				}
				if(!flag) {
					throw new Errors(robot+"发件人域需与所选正式通道配置域对应！");
				}
			}
		}
	}

	@Override
	public void isOperateRobotsExist(String[] robots) {
		if(robots==null||robots.length<1) {
			throw new Errors("请输入网络发件人！");
		}
		List<Resource> resList = resourceMapper.selectAll();
		StringBuilder sb = new StringBuilder();
		if(resList!=null && resList.size()>0) {
			for(int i=0;i<resList.size();i++) {
				sb.append(resList.get(i).getHeloList());
				if(i!=resList.size()-1){
					sb.append(",");
				}
			}
		}
		if(StringUtils.isEmpty(sb.toString())) {
			throw new Errors("通道表无发件人域！");
		}
		
		String[] heloArray = StringUtils.split(sb.toString(), ",");
		for(String robot : robots) {
			if(StringUtils.isNotEmpty(robot)) {
				boolean flag = false;
				String domain = robot.split("@")[1];
				for(String h:heloArray) {
					if(StringUtils.equalsIgnoreCase(h, domain.trim())){
						flag = true;
						break;
					}
				}
				if(!flag) {
					throw new Errors(robot+"发件人域需与通道表配置域对应！");
				}
			}
		}
		
	}
	/**
	 * 查找子账号
	 */
	public List<User> getSubUserByManagerId(String UserId){		
		return userMapper.getSubUserByManagerId(UserId);
	}
	
	public List<User> getAllUserByCorpId(String corpId){
		return userMapper.selectAllUserByCorpId(corpId);
	}
	
}
