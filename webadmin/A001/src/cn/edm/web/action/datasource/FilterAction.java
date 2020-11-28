package cn.edm.web.action.datasource;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.mapper.FilterMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Filter;
import cn.edm.model.Prop;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Filters;
import cn.edm.web.facade.Props;

import com.google.common.collect.Maps;

@Controller
@RequestMapping("/datasource/filter/**")
public class FilterAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(FilterAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Filter> page = new Page<Filter>();
			Pages.page(request, page);

			String filterName = StreamToString.getStringByUTF8(Https.getStr(request, "filterName", R.CLEAN, R.LENGTH, "{1,60}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			
			MapBean mb = new MapBean();
			page = filterService.search(page, mb, UserSession.getCorpId(), filterName, beginTime, endTime);
			Filters.props(page.getResult());
			
			map.put("page", page);
			map.put("mb", mb);
			
			return "default/datasource/filter_page";
		} catch (Exception e) {
			logger.error("(Filter:page) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer filterId = Https.getInt(request, "filterId", R.CLEAN, R.INTEGER);

			Filter filter = null;
			List<Prop> propList = propService.find(UserSession.getCorpId());
			Props.mapper(propList);
			
			if (filterId != null) {
				filter = filterService.get(UserSession.getCorpId(), UserSession.getUserId(), filterId);
			}
			if (filter == null) {
				filter = new Filter();
			}
			
			Map<String, String> propMap = Props.map(filter.getProps());

			map.put("filter", filter);
			map.put("propList", propList);
			map.put("propMap", propMap);
			
			return "default/datasource/filter_add";
		} catch (Exception e) {
			logger.error("(Filter:add) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String setting(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String prop = Https.getStr(request, "prop", R.CLEAN, R.LENGTH, "{1,300}");
			
			String pnm = null;
			String cnd = null;
			String val = null;
			
			if (StringUtils.isNotBlank(prop)) {
				pnm = StringUtils.substringBefore(prop, ".");
				pnm = Values.get(PropMap.getName(pnm), pnm);
				cnd = StringUtils.substringBefore(StringUtils.substringAfter(prop, "."), ",");
				val = StringUtils.substringAfter(StringUtils.substringAfter(prop, "."), ",");
			}

			map.put("pnm", pnm);
			map.put("cnd", cnd);
			map.put("val", val);
			
			return "default/datasource/filter_setting";
		} catch (Exception e) {
			logger.error("(Filter:setting) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void setting(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			String pnm = Https.getStr(request, "pnm", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "属性");
			String cnd = Https.getStr(request, "cnd", R.CLEAN, R.REQUIRED, "过滤条件");
			String val = "";
			
			String pref = StringUtils.substringBefore(cnd, "_");
			String suff = StringUtils.substringAfter(cnd, "_");
			
			if (suff.equals(FilterMap.RANGE.getId())) {
				String min = Https.getStr(request, pref + "_min", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,255}", "过滤内容");
				String max = Https.getStr(request, pref + "_max", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,255}", "过滤内容");
				containsAny(min);
				containsAny(max);
				val = min + "," + max;
			} else {
				val = Https.getStr(request, cnd, R.CLEAN, R.REQUIRED, R.LENGTH, "{1,255}", "过滤内容");
				containsAny(val);
			}
			
			pnm = Values.get(PropMap.getId(pnm), pnm);
			Prop prop = propService.get(UserSession.getCorpId(), null, pnm);
			Asserts.isNull(prop, "属性");
			
			Props.rule(suff, val, prop.getType());

			mb.put("prop", pnm + "." + suff + "," + val);
			mb.put("setting", Props.settings(pnm + ":" + suff + "=" + val));

			Views.ok(mb, "过滤机制设置成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "过滤机制设置失败");
			logger.error("(Filter:setting) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	private void containsAny(String val) {
		if (Asserts.containsAny(val, new String[] { "," })) {
			throw new Errors("过滤内容不能包含“<span style='color: #272727;'>,</span>”字符");
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer filterId = null;
		
		try {
			CSRF.validate(request);
			
			filterId = Https.getInt(request, "filterId");
			String orgFilterName = Https.getStr(request, "orgFilterName", R.CLEAN, R.LENGTH, "{1,60}", "过滤器名称");
			String filterName = Https.getStr(request, "filterName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "过滤器名称");
			String filterDesc = Https.getStr(request, "filterDesc", R.CLEAN, R.LENGTH, "{1,60}", "过滤器描述");
			
			String[] propNames = propNames(request);
			List<Prop> propList = propService.find(UserSession.getCorpId(), propNames);
			String props = props(request, propList);

			Asserts.notUnique(filterService.unique(UserSession.getCorpId(), filterName, orgFilterName), "过滤器");
			
			Filter filter = null;
			if (filterId != null) {
				filter = filterService.get(UserSession.getCorpId(), UserSession.getUserId(), filterId);
				Asserts.isNull(filter, "过滤器");
			} else {
				filter = new Filter();
				filter.setCorpId(UserSession.getCorpId());
				filter.setUserId(UserSession.getUserId());
			}

			filter.setFilterName(filterName);
			filter.setFilterDesc(filterDesc);
			filter.setProps(props);
			
			filterService.save(filter);

			Views.ok(mb, (filterId != null ? "修改" : "新建") + "过滤器成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (filterId != null ? "修改" : "新建") + "过滤器失败");
			logger.error("(Filter:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			int[] filterIds = Converts._toInts(checkedIds);

			if (filterIds != null && filterIds.length == 1) {
				Filter filter = filterService.get(UserSession.getCorpId(), UserSession.getUserId(), filterIds[0]);
				if (filter != null) {
					map.put("checkedName", filter.getFilterName());
				}
			}

			Views.map(map, "del", "", "删除", "所选过滤器", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Filter:del) error: ", e);
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
			Integer[] filterIds = Converts._toIntegers(checkedIds);

			if (!Asserts.empty(filterIds)) {
				filterService.delete(UserSession.getUserId(), filterIds);
			}
			
			Views.ok(mb, "删除过滤器成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除过滤器失败");
			logger.error("(Filter:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
    private String[] propNames(HttpServletRequest request) {
        Map<String, Object> propMap = Servlets.getParametersStartingWith(request, "prop_");

        StringBuffer sbff = new StringBuffer();
        for (Entry<String, Object> entry : propMap.entrySet()) {
            String prop = (String) entry.getValue();
            prop = (String) Validator.validate(prop, R.CLEAN, R.LENGTH, "{1,255}", "属性");
            if (StringUtils.isBlank(prop)) {
                continue;
            }
            
            String pnm = StringUtils.substringBefore(prop, ".");
            pnm = Values.get(PropMap.getId(pnm), pnm);
            sbff.append(pnm).append(",");
        }

        return Converts._toStrings(sbff.toString());
    }
    
    private String props(HttpServletRequest request, List<Prop> propList) {
        Map<String, String> typeMap = Maps.newHashMap();
        for (Prop prop : propList) {
            typeMap.put(prop.getPropName(), prop.getType());
        }
        
        Map<String, Object> propMap = Servlets.getParametersStartingWith(request, "prop_");
        Map<String, String> pnmMap = Maps.newHashMap();
        
        StringBuffer sbff = new StringBuffer();
        for (Entry<String, Object> entry : propMap.entrySet()) {
            String prop = (String) entry.getValue();
            
            if (StringUtils.isBlank(prop)) {
                continue;
            }
            
            String pnm = StringUtils.substringBefore(prop, ".");
            String cnd = StringUtils.substringBefore(StringUtils.substringAfter(prop, "."), ",");
            String val = StringUtils.substringAfter(StringUtils.substringAfter(prop, "."), ",");
            
            Asserts.isNull(typeMap.get(pnm), "属性");
            Props.rule(cnd, val, typeMap.get(pnm));

            if (pnmMap.containsKey(pnm)) {
                throw new Errors("过滤机制" + Values.get(PropMap.getName(pnm), pnm) + "已重复设置");
            }
            
            pnmMap.put(pnm, pnm);
            
            sbff.append(pnm + ":" + cnd + "=" + val).append("&");
        }

        Asserts.isEmpty(pnmMap, "过滤机制");
        if (pnmMap.size() > 8) {
            throw new Errors("过滤机制不能多于" + 8 + "个");
        }
        
        String props = sbff.toString();
        if (StringUtils.endsWith(props, "&")) {
            props = StringUtils.removeEnd(props, "&");
        }
        
        return props;
    }
}

