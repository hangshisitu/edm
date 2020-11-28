package cn.edm.web.action.account;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.model.Corp;
import cn.edm.model.Cos;
import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.View;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/account/profile/**")
public class ProfileAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(ProfileAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String view(HttpServletRequest request, ModelMap map) {
		try {
			Corp corp = corpService.get(UserSession.getCorpId());
			User user = userService.get(UserSession.getUserId());
			
			Cos cos = cosService.get(corp.getCosId());
			Asserts.isNull(cos, "套餐");
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(corp.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            List<Integer> corpIdList = Lists.newArrayList();
            corpService.cross(corpList, root.getCorpId(), corpIdList);
            corpIdList.add(root.getCorpId()); //子机构的机构ID集合加上父机构的机构ID集合
            Integer[] corpIds = Converts._toIntegers(corpIdList);    
            
			int sentCount = taskService.sentCount(corpIds, null, null);//子父机构所有的发送量
			
			String auditPath = corp.getAuditPath();
			
			View view = (View) request.getAttribute("view");
			
			map.put("cos", cos);
			map.put("sentCount", sentCount);
			String moderate = view.task_moderate(user.getModerate());
			if(user.getRoleType().equals(1)){
				moderate = view.task_moderate(auditPath);
			}
			map.put("moderate", moderate);
			
			return "default/account/profile_view";
		} catch (Exception e) {
			logger.error("(Profile:view) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Corp corp = corpService.get(UserSession.getCorpId());
			User user = userService.get(UserSession.getUserId());
			Cos cos = cosService.get(corp.getCosId());
			
			map.put("corp", corp);
			map.put("user", user);
			map.put("cos", cos);

			return "default/account/profile_add";
		} catch (Exception e) {
			logger.error("(Profile:add) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();

		try {
			CSRF.validate(request);
			
			String company = null;
			String address = null;
			String fax = null;
			String zip = null;

			if (UserSession.getUser().getRole().getType().equals(1) || UserSession.getUser().getRole().getRoleId().equals(0)) {
				company = Https.getStr(request, "company", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "公司名称");
				address = Https.getStr(request, "address", R.CLEAN, R.LENGTH, "{1,100}", "联系地址");
				fax = Https.getStr(request, "fax", R.CLEAN, R.LENGTH, "{1,20}", "传真");
				zip = Https.getStr(request, "zip", R.CLEAN, R.LENGTH, "{1,6}", "邮编");
			}

			String orgEmail = Https.getStr(request, "orgEmail", R.CLEAN, R.LENGTH, R.MAIL, "{1,64}", "邮箱地址");
			String email = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "邮箱地址");
			String trueName = Https.getStr(request, "trueName", R.CLEAN, R.LENGTH, "{1,20}", "姓名");
			String phone = Https.getStr(request, "phone", R.CLEAN, R.LENGTH, "{1,20}", "电话号码");
			
			Asserts.notUnique(userService.unique("email", email, orgEmail), "邮箱地址");
			
			Corp corp = corpService.get(UserSession.getCorpId());
			User user = userService.get(UserSession.getUserId());

			if (UserSession.getUser().getRole().getType().equals(1) || UserSession.getUser().getRole().getRoleId().equals(0)) {
				corp.setCompany(company);
				corp.setContact(trueName);
				corp.setAddress(address);
				corp.setEmail(email);
				corp.setTelephone(phone);
				corp.setFax(fax);
				corp.setZip(zip);
			}

			user.setEmail(email);
			user.setTrueName(trueName);
			user.setPhone(phone);
			
			userService.save(corp, user, UserSession.getUserId());

			Views.ok(mb, "修改资料成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "修改资料失败");
			logger.error("(Profile:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
}
