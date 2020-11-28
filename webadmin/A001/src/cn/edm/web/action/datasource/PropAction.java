package cn.edm.web.action.datasource;

import java.util.Date;
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

import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.TypeMap;
import cn.edm.model.Prop;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
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

@Controller
@RequestMapping("/datasource/prop/**")
public class PropAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(PropAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Prop> page = new Page<Prop>();
			Pages.page(request, page);

			
			String propName = StreamToString.getStringByUTF8(Https.getStr(request, "propName", R.CLEAN, R.LENGTH, "{1,20}"));//解决乱码
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);

			MapBean mb = new MapBean();
			page = propService.search(page, mb, UserSession.getCorpId(), propName, beginTime, endTime);
			prop(page);
			
			map.put("page", page);
			map.put("types", TypeMap.values());
			map.put("mb", mb);

			return "default/datasource/prop_page";
		} catch (Exception e) {
			logger.error("(Prop:page) error: ", e);
			return Views._404();
		}
	}

	private void prop(Page<Prop> page) {
		if (Asserts.empty(page.getResult())) {
			return;
		}
		for (Prop prop : page.getResult()) {
			String propName = PropMap.getName(prop.getPropName());
			prop.setType(TypeMap.getDesc(prop.getType()));
			prop.setPropName(Values.get(propName, prop.getPropName()));
			prop.setKeyword(propName != null);
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer propId = Https.getInt(request, "propId", R.CLEAN, R.INTEGER);
			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^prop|recipient$");
			action = Values.get(action, "prop");

			Prop prop = null;

			if (propId != null) {
				prop = propService.get(UserSession.getCorpId(), UserSession.getUserId(), propId);
			}
			if (prop == null) {
				prop = new Prop();
			}

			prop.setPropName(Values.get(PropMap.getName(prop.getPropName()), prop.getPropName()));
			prop.setType(Values.get(TypeMap.getDesc(prop.getType()), prop.getType()));
			
			map.put("prop", prop);
			map.put("action", action);
			
			return "default/datasource/prop_add";
		} catch (Exception e) {
			logger.error("(Prop:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer propId = null;
		
		try {
			CSRF.validate(request);
			
			propId = Https.getInt(request, "propId", R.CLEAN, R.INTEGER);
			String orgPropName = Https.getStr(request, "orgPropName", R.CLEAN, R.LENGTH, "{1,20}", "属性名称");
			String propName = Https.getStr(request, "propName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "属性名称");
			String propDesc = Https.getStr(request, "propDesc", R.CLEAN, R.LENGTH, "{1,60}", "属性描述");
			String type = Https.getStr(request, "type", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,30}", "数据类型");
			Integer relation = Https.getInt(request, "relation", R.CLEAN, R.REQUIRED, R.INTEGER, "属性类型");
            
			isKeyword(orgPropName, propName);
			
			Asserts.notUnique(propService.unique(UserSession.getCorpId(), propName, orgPropName), "属性");
			
			Prop prop = null;
			if (propId != null) {
				prop = propService.get(UserSession.getCorpId(), UserSession.getUserId(), propId);
				Asserts.isNull(prop, "属性");
				prop.setRelation(relation);
				prop.setPropName(Values.get(PropMap.getId(propName), propName));
				prop.setPropDesc(propDesc);
				prop.setRequired(Value.F);
			} else {
				prop = new Prop();
				prop.setCorpId(UserSession.getCorpId());
				prop.setUserId(UserSession.getUserId());
				prop.setType(type);
				prop.setRelation(relation);
				prop.setPropName(Values.get(PropMap.getId(propName), propName));
				prop.setPropDesc(propDesc);
				prop.setRequired(Value.F);
			}

			
			
			propService.save(prop);

			mb.put("userId", UserSession.getUserId());
			mb.put("propId", prop.getPropId());
			mb.put("propName", propName);
			mb.put("propDesc", prop.getPropDesc());
			mb.put("type", prop.getType());
			mb.put("relation", prop.getRelation());
			Date now = new Date();
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
            
			Views.ok(mb, (propId != null ? "修改" : "新建") + "属性成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (propId != null ? "修改" : "新建") + "属性失败");
			logger.error("(Prop:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			int[] propIds = Converts._toInts(checkedIds);
			
			if (propIds != null && propIds.length == 1) {
				Prop prop = propService.get(UserSession.getCorpId(), UserSession.getUserId(), propIds[0]);
				if (prop != null) {
					map.put("checkedName", prop.getPropName());
				}
			}

			Views.map(map, "del", "", "删除", "所选属性", "删除后收件人、表单及模板中涉及该属性的数据将一并清除。", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Prop:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
			CSRF.validate(request);
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			Integer[] propIds = Converts._toIntegers(checkedIds);

			if (!Asserts.empty(propIds)) {
				StringBuffer sbff = new StringBuffer();
				List<Prop> props = propService.find(null, UserSession.getUserId(), propIds);
				for (Prop prop : props) {
					if (PropMap.getName(prop.getPropName()) != null) {
						continue;
					}
					
					sbff.append(prop.getPropId()).append(",");
				}
				
				checkedIds = sbff.toString();
				propIds = Converts._toIntegers(sbff.toString());
				
				if (!Asserts.empty(propIds)) {
					propService.delete(UserSession.getUserId(), propIds);
				}
			}
			
			Views.ok(mb, "删除属性成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除属性失败");
			logger.error("(Prop:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	private void isKeyword(String orgPropName, String propName) {
		boolean isKeyword = false;
		for (PropMap mapper : PropMap.values()) {
			if (StringUtils.isNotBlank(orgPropName)) {
				if (StringUtils.equals(orgPropName, mapper.getName())) {
					if (!StringUtils.equals(orgPropName, propName)) {
						throw new Errors("不能修改系统定义的属性名称");
					}
				} else {
					if (!StringUtils.equals(orgPropName, propName)) {
						if (StringUtils.equals(propName, mapper.getId()) || StringUtils.equals(propName, mapper.getName())) {
							isKeyword = true;
						}
					}
				}
			} else {
				if (StringUtils.equals(propName, mapper.getId()) || StringUtils.equals(propName, mapper.getName())) {
					isKeyword = true;
				}
			}
			
			if (isKeyword) {
				throw new Errors("属性名称不能含有系统定义的" + mapper.getId() + "或" + mapper.getName() + "关键词");
			}
		}
	}
}

