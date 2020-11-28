package cn.edm.web.action.account;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Cnds;
import cn.edm.consts.RoleType;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.model.Corp;
import cn.edm.model.Cos;
import cn.edm.model.Role;
import cn.edm.model.Triger;
import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.encoder.Md5;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Audits;
import cn.edm.web.facade.Corps;

@Controller
@RequestMapping("/account/user/**")
public class UserAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(UserAction.class);
	
	/*private static final String REGEX_USER_ID = "regex:^[a-zA-Z][a-zA-Z0-9_]+$:只能由字母、数字或下划线组成";*/
	private static final String REGEX_USER_ID = "regex:^[a-zA-Z0-9_]+$:只能由字母、数字或下划线组成";
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			String userId = StreamToString.getStringByUTF8(Https.getStr(request, "userId", R.CLEAN));
			Integer roleId = Https.getInt(request, "roleId", R.CLEAN, R.INTEGER);
			Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
			Integer status = Https.getInt(request, "status", R.CLEAN);
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			String nameCnd = Values.get(Https.getStr(request, "nameCnd", R.CLEAN, R.REGEX, "regex:^" + Cnds.EQ + "|" + Cnds.LIKE + "$"), Cnds.LIKE);
			
			MapBean mb = new MapBean();
            Integer[] statuses = { Status.ENABLED, Status.FREEZE };
            if (status != null) {
                statuses = new Integer[] { status };
                Pages.search(mb);
            }
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
			Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
			List<User> userList = userService.find(mb, corpIds, UserSession.getCorpId(mb, corpId),
			        userId, roleId, statuses, beginTime, endTime, nameCnd);
			List<Role> roles = roleService.find(new Integer[] { RoleType.CMN, RoleType.MGR, RoleType.ADM });
			for (User user : userList) {
			    for (Role role : roles) {
			        if (role.getRoleId().equals(user.getRoleId())) {
			            user.setRole(role);
			        }
			    }
            }
			List<Role> roleList = roleService.find(new Integer[] { RoleType.CMN, RoleType.MGR });
			map.put("mb", mb);
			map.put("userList", userList);
            map.put("corpChildren", corpChildren);
            map.put("corp", corp);
            map.put("roleList", roleList);
			map.put("status", status);
			
			return "default/account/user_page";
		} catch (Exception e) {
			logger.error("(User:page) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String userId = Https.getStr(request, "userId", R.CLEAN);
			User user = null;
			Role role = null;
			Corp corp = corpService.get(UserSession.getCorpId());
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
			
			if (StringUtils.isNotBlank(userId)) {
	            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
				user = userService.get(corpIds, null, userId);
				role = roleService.get(user.getRoleId());
                if (user != null) {
//                    String auditPath = corp.getAuditPath();
                    boolean moderate = Audits.moderate(user.getModerate());
//                    if (user.getRoleType().equals(RoleType.MGR)) {
//                        moderate = Audits.moderate(auditPath);
//                    }
                    map.put("moderate", moderate);
                }
			}
			if (user == null) {
				user = new User();
			}
			map.put("user", user);
            map.put("role", role);
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            boolean isManager = UserSession.isManager();
            if (isManager) {
                List<Role> roleList = roleService.find(new Integer[] { RoleType.CMN, RoleType.MGR });
                map.put("roleList", roleList);
            }

			return "default/account/user_add";
		} catch (Exception e) {
			logger.error("(User:add) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String orgUserId = null;

		try {
			CSRF.validate(request);
			
			orgUserId = Https.getStr(request, "orgUserId", R.CLEAN, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
			Integer REQUIRED = StringUtils.isBlank(orgUserId) ? R.REQUIRED : R.NONE;
			
			String orgEmail = Https.getStr(request, "orgEmail", R.CLEAN, R.LENGTH, R.MAIL, "{1,64}", "邮箱地址");
			String userId = Https.getStr(request, "userId", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
			String password = Https.getStr(request, "password", R.CLEAN, REQUIRED, R.LENGTH, "{6,18}", "密码");
			String email = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "邮箱地址");
			String trueName = Https.getStr(request, "trueName", R.CLEAN, R.LENGTH, "{1,60}", "姓名");
			String phone = Https.getStr(request, "phone", R.CLEAN, R.LENGTH, "{1,20}", "电话号码");
			Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.REQUIRED, R.INTEGER, "机构");
			Integer roleId = Https.getInt(request, "roleId", R.CLEAN, R.REQUIRED, R.INTEGER, "角色");
            Integer permissionId = Https.getInt(request, "permissionId", R.CLEAN, R.INTEGER, "附加权限");
            Asserts.notUnique(userService.unique("userId", userId, orgUserId), "用户名");
			Asserts.notUnique(userService.unique("email", email, orgEmail), "邮箱地址");

			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
			Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
			if (!Asserts.hasAny(corpId, corpIds)) {
			    throw new Errors("机构不存在");
			}
			Integer required = R.CLEAN;
			Role role = roleService.get(roleId);
			Corp corp = corpService.get(corpId);
			if(corpId.equals(UserSession.getCorpId()) 
					|| corp.getParentId().equals(UserSession.getCorpId()) ){ //同机构下新建管理员时只允许存在一个前台管理员
				if(role.getType() == 1 ){ //新建的是前台管理员
                   long count = userService.getCorpMgrCount(corpId,role.getType());
				   if(count >=1 ){
					   throw new Errors("该机构下已经存在管理员"); 
				   }
				}	
			}

			if (corpId.equals(UserSession.getCorpId())) {
			    if (role.getType().equals(RoleType.MGR)) {
			        throw new Errors("禁止选择下级管理员角色");
			    }
			    required = R.REQUIRED;
			} else {
			    if (role.getType().equals(RoleType.CMN)) {
                    throw new Errors("禁止选择同级别功能角色");
			    }
			}
			
			String moderate = Https.getStr(request, "moderate", R.CLEAN, required, R.REGEX, "审核设置", "regex:^on|off$:只能为on或off");
			
			if (StringUtils.isBlank(orgUserId)) {
				validateChildren(corpId);
			}

			Md5 md5 = new Md5();

			User user = null;
			if (StringUtils.isNotBlank(orgUserId)) {
				user = userService.get(UserSession.getUserId(), orgUserId);
				Asserts.isNull(user, "子账号");
				if (StringUtils.isNotBlank(password)) {
					user.setPassword(md5.encode(password));
				}
			} else {
				user = new User();
				user.setUserId(userId);
				user.setPassword(md5.encode(password));
				user.setOpenSms(Status.DISABLED);
				user.setStatus(Status.ENABLED);
			}

			user.setCorpId(corpId);
			user.setEmail(email);
			user.setTrueName(trueName);
			user.setPhone(phone);
			user.setModerate(moderate.equals("on") ? Value.T : Value.F);
			user.setRoleId(roleId);
			user.setPermissionId(permissionId);
			user.setManagerId(UserSession.getUserId());
			userService.save(user, orgUserId);
			
			boolean isApi = trigerService.isApi(UserSession.getCorpId(), UserSession.getUserId());
			
            Triger triger = new Triger();
            triger.setCorpId(UserSession.getCorpId());
            triger.setUserId(userId);
            triger.setTrigerName(Value.S);
            triger.setStatus(isApi ? Value.T : Value.F);
            trigerService.saveApi(triger);
			
			Views.ok(mb, (StringUtils.isNotBlank(orgUserId) ? "修改" : "新建") + "子账号成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (StringUtils.isNotBlank(orgUserId) ? "修改" : "新建") + "子账号失败");
			logger.error("(User:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			view(request, map, "del", "删除");
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(User:del) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		update(request, mb, "del", "删除", Status.DELETED);
		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String freeze(HttpServletRequest request, ModelMap map) {
		try {
			view(request, map, "freeze", "冻结");
			return "default/account/user_status";
		} catch (Exception e) {
			logger.error("(User:freeze) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void freeze(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		update(request, mb, "freeze", "冻结", Status.FREEZE);
		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String enabled(HttpServletRequest request, ModelMap map) {
		try {
			view(request, map, "enabled", "激活");
			return "default/account/user_status";
		} catch (Exception e) {
			logger.error("(User:enabled) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void enabled(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		update(request, mb, "enabled", "激活", Status.ENABLED);
		ObjectMappers.renderJson(response, mb);
	}
	
	private void view(HttpServletRequest request, ModelMap map, String m, String opt) {
		CSRF.generate(request);
		
		String checkedIds = Https.getStr(request, "userId", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
		String[] userIds = Converts._toStrings(checkedIds);
		
		if (!Asserts.empty(userIds) && userIds.length == 1) {
			map.put("checkedName", userIds[0]);
		}
		
		Views.map(map, m, "", opt, "子账号", "checkedIds", checkedIds);
	}

	private void update(HttpServletRequest request, MapBean mb, String m, String operate, int status) {
		String checkedIds = null;
		try {
			CSRF.validate(request);
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
			String[] userIds = Converts._toStrings(checkedIds);
			if (!Asserts.empty(userIds) && userIds.length == 1) {
				String userId = userIds[0];
				if (userId.equals(UserSession.getUserId())) {
					throw new Errors("禁止" + operate + "自己");
				}

				User user = userService.get(UserSession.getUserId(), userId);
				Asserts.isNull(user, "子账号");
				user.setStatus(status);

				userService.save(user, userId);
			}

			Views.ok(mb, operate + "子账号成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, operate + "子账号失败");
			logger.error("(User:" + m + ") error: ", e);
		}
		
		mb.put("checkedIds", checkedIds);
	}

	private void validateChildren(Integer corpId) {
		Corp corp = corpService.get(corpId);
		int count = (int) userService.children(null, corpId);
		Cos cos = cosService.get(corp.getCosId());
		if (cos.getUserCount() == -1) {
		    return;
		}
		Asserts.isNull(cos, "套餐");
		if (count >= cos.getUserCount()) {
			throw new Errors("子账号不能多于" + cos.getUserCount() + "个");
		}
	}
}
