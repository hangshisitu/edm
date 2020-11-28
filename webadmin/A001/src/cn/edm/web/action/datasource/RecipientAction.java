package cn.edm.web.action.datasource;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Cnds;
import cn.edm.consts.Status;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Category;
import cn.edm.model.Form;
import cn.edm.model.Prop;
import cn.edm.model.Recipient;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.MailBox;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Concurrencys;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Recipients;

import com.google.common.collect.Maps;

@Controller
@RequestMapping("/datasource/recipient/**")
public class RecipientAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(RecipientAction.class);
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/datasource/recipient/prop", token);
			CSRF.generate(request, "/datasource/export/recipient", token);
			
			Page<Recipient> page = new Page<Recipient>();
			Pages.page(request, page);
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^tag|form|form.5|category$");
            action = Values.get(action, "tag");
            
			String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^active|inactive|5$");
			m = Values.get(m, "all");
			
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			
			String email = Https.getStr(request, "email", R.CLEAN, R.LENGTH, "{1,64}");
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			String _id = Https.getStr(request, "_id", R.CLEAN, R.LENGTH, R.REGEX, "{24,24}", "regex:^[a-f0-9]+");

            if (action.equals("form.5")) {
                Integer formId = Https.getInt(request, "formId", R.CLEAN, R.INTEGER);
                map.put("formId", formId);
            }
			
			Map<Integer, String> mapper = mapper(page.getPageNo(), _id);

			MapBean mb = new MapBean();
			Tag tag = tagService.get(UserSession.getCorpId(), null, tagId);
			Asserts.isNull(tag, "标签");
			page = emailService.search(page, mb, mapper, tag.getDbId(), tag.getCorpId(), m, tag.getTagId(), email, beginTime, endTime);
			Pages.put(mb, "tagId", tag.getTagId());
			
			map.put("page", page);
			map.put("mb", mb);
            map.put("m", m);
            map.put("action", action);
            map.put("tag", tag);
			map.put("_id", _id(mapper));
			
			return "default/datasource/recipient_page";
		} catch (Exception e) {
			logger.error("(Recipient:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void prop(HttpServletRequest request, HttpServletResponse response) {
		Map<String, String> parameterMap = Maps.newHashMap();
		try {
			CSRF.validate(request);
			
			String email = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}");
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			Asserts.isNull(tag, "标签");
			Recipient recipient = emailService.get(tag, email, true);
			Asserts.isNull(recipient, "收件人");
			
			Recipients.parameterMap(parameterMap, recipient);
            
			if (tag.getCategoryName().equals(CategoryMap.FORM.getName())) {
	            Map<String, String> formMap = Maps.newHashMap();
                List<Form> formList = formService.find(UserSession.getCorpId(), tagId, Status.ENABLED);
                for (Form form : formList) {
                    String content = Files.get(Webs.rootPath(), form.getInputPath());
                    Document doc = Jsoup.parse(content);
                    List<Element> inputs = doc.select("#tempForm input");
                    for (Element e : inputs) {
                        String name = e.attr("name");
                        if (StringUtils.isBlank(name)) {
                            continue;
                        }
                        formMap.put(Values.get(PropMap.getName(name), name), name);
                    }
                }
                Iterator<String> iter = parameterMap.keySet().iterator();
                while (iter.hasNext()) {
                    if (!formMap.containsKey(iter.next())) {
                        iter.remove();
                    }
                }
            }
		} catch (Exception e) {
			logger.error("(Recipient:prop) error: ", e);
		}
		
		ObjectMappers.renderJson(response, parameterMap);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^tag|all|active|inactive$");
			m = Values.get(m, "all");
			
			String email = Https.getStr(request, "email", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}");
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			
			Tag tag = null;
			Recipient recipient = null;
			if (tagId != null) {
				tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			}
			
			boolean isForm = false;
			if (tag != null && StringUtils.isNotBlank(email)) {
				recipient = emailService.get(tag, email, true);
                if (tag.getCategoryName().equals(CategoryMap.FORM.getName())) {
                    isForm = true;
                }
			}
			if (recipient == null) {
				recipient = new Recipient();
			}
			
            List<Tag> tagList = null;
            Map<String, List<Tag>> groupMap = Maps.newHashMap();
            List<Category> categoryList = new ArrayList<Category>();
           if (isForm) {
                tagList = tagService.find(UserSession.getCorpId(), CategoryMap.FORM.getName(), Cnds.EQ);
			} else {
                tagList = tagService.find(UserSession.getCorpId(), CategoryMap.FORM.getName(), Cnds.NOT_EQ);
            }
            categoryList = categoryService.find(null,UserSession.getCorpId(), Category.TAG, null, null);//获取不到其他的标签类别
            groupMap = Categorys.groupMap(categoryList, tagList); 
           
			List<Prop> propList = propService.find(UserSession.getCorpId());

            Map<String, String> parameterMap = Maps.newHashMap();
            Map<String, String> formMap = Maps.newHashMap();
            Recipients.parameterMap(parameterMap, recipient);
            
            if (isForm) {
                List<Form> formList = formService.find(UserSession.getCorpId(), tagId, Status.ENABLED);
                for (Form form : formList) {
                    String content = Files.get(Webs.rootPath(), form.getInputPath());
                    if (StringUtils.isBlank(content))
                        continue;
                    Document doc = Jsoup.parse(content);
                    List<Element> inputs = doc.select("#tempForm input");
                    for (Element e : inputs) {
                        String name = e.attr("name");
                        if (StringUtils.isBlank(name)) {
                            continue;
                        }
                        formMap.put(Values.get(PropMap.getName(name), name), name);
                    }
                }
            }
            
			Iterator<Prop> iter = propList.iterator();
			while (iter.hasNext()) {
				Prop prop = iter.next();
				String propName = Values.get(PropMap.getName(prop.getPropName()), prop.getPropName());
				if (parameterMap.containsKey(propName) || StringUtils.equals(propName, PropMap.EMAIL.getName())) {
					iter.remove();
					continue;
				}
				prop.setPropName(propName);
			}

			map.put("tagId", tagId);
			map.put("recipient", recipient);
			map.put("tagList", tagList);
			map.put("propList", propList);
            map.put("parameterMap", parameterMap);
            map.put("formMap", formMap);
            map.put("isForm", isForm);
			map.put("m", m);
			
			map.put("groupMap", groupMap);
			map.put("categoryList", categoryList);
			
			return "default/datasource/recipient_add";
		} catch (Exception e) {
			logger.error("(Recipient:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String orgEmail = null;
		
		Counter counter = null;
		
		try {
			CSRF.validate(request);
			
			orgEmail = Https.getStr(request, "orgEmail", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}", "邮箱地址");
			String email = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}", "邮箱地址");
			
			String[] propNames = propNames(request);
			List<Prop> propList = propService.find(UserSession.getCorpId(), propNames);
			String parameters = parameters(request, propList);
			
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(),null, tagId);
			Asserts.isNull(tag, "标签");
			
			Integer orgTagId = null;
			Tag orgTag = null;
			if (StringUtils.isNotBlank(orgEmail)) {
				orgTagId = Https.getInt(request, "orgTagId", R.CLEAN, R.INTEGER);
				Asserts.isNull(orgTagId, "标签");
				if (orgTagId.equals(tagId)) {
					orgTag = tag;
				} else {
					orgTag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), orgTagId);
				}
				Asserts.isNull(orgTag, "标签");
                if (!orgTag.getTagId().equals(tag.getTagId())) {
                    if (StringUtils.equals(orgTag.getCategoryName(), CategoryMap.FORM.getName())) {
                        throw new Errors("禁止修改所属标签");
                    }
                    if (StringUtils.equals(tag.getCategoryName(), CategoryMap.FORM.getName())) {
                        throw new Errors("所属标签类别不能为表单收集类别");
                    }
                }
			} else {
			    if (StringUtils.equals(tag.getCategoryName(), CategoryMap.FORM.getName())) {
                    throw new Errors("所属标签类别不能为表单收集类别");
                }
			}

			Asserts.notUnique(emailService.unique(tag, email, orgEmail), "邮箱地址");
			
			Recipient recipient = null;
			if (StringUtils.isNotBlank(orgEmail)) {
				recipient = emailService.get(orgTag, orgEmail, false);
				Asserts.isNull(recipient, "收件人");
			} else {
				recipient = new Recipient();
			}

			recipient.setCorpId(UserSession.getCorpId());
			recipient.setEmail(email);
			recipient.setParameterList(parameters);
			
			counter = Concurrencys.getCounter(cache, false, UserSession.getUserId());
			emailService.save(tag, recipient, orgTag, orgEmail);
			counter.setCode(Counter.COMPLETED);
			
			Views.ok(mb, (StringUtils.isNotBlank(orgEmail) ? "修改" : "新建") + "收件人成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
            Concurrencys.clearCounter(cache, true, UserSession.getUserId());
		} catch (Exception e) {
			Views.error(mb, (StringUtils.isNotBlank(orgEmail) ? "修改" : "新建") + "收件人失败");
			logger.error("(Recipient:add) error: ", e);
			Concurrencys.clearCounter(cache, true, UserSession.getUserId());
		} finally {
			Concurrencys.clearCounter(cache, false, UserSession.getUserId());
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer tagId = Https.getInt(request, "action", R.CLEAN, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			Asserts.isNull(tag, "标签");
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN);
			validate(checkedIds);
			
			String[] emails = Converts._toStrings(checkedIds);
			
			if (emails != null && emails.length == 1) {
				Recipient recipient = emailService.get(tag, emails[0], false);
				if (recipient != null) {
					map.put("checkedName", recipient.getEmail());
				}
			}
			
			Views.map(map, "del", String.valueOf(tagId), "删除", "所选收件人", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Recipient:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		
		Counter counter = null;
		
		try {
			CSRF.validate(request);
			
			Integer tagId = Https.getInt(request, "action", R.CLEAN, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			Asserts.isNull(tag, "标签");
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN);
			validate(checkedIds);
			
			String[] emails = Converts._toStrings(checkedIds);

			if (!Asserts.empty(emails)) {
				counter = Concurrencys.getCounter(cache, false, UserSession.getUserId());
				emailService.delete(tag, emails);
				counter.setCode(Counter.COMPLETED);
			}
			
			Views.ok(mb, "删除收件人成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
            Concurrencys.clearCounter(cache, true, UserSession.getUserId());
		} catch (Exception e) {
			Views.error(mb, "删除收件人失败");
			logger.error("(Recipient:del) error: ", e);
            Concurrencys.clearCounter(cache, true, UserSession.getUserId());
		} finally {
            Concurrencys.clearCounter(cache, false, UserSession.getUserId());
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	private void validate(String checkedIds) {
		for (String email : StringUtils.split(checkedIds, ",")) {
			if (StringUtils.isNotBlank(email) && !MailBox.validate(email)) {
				throw new Errors("checkedIds不是合法值");
			}
		}
	}
	
    private Map<Integer, String> mapper(Integer pageNo, String _id) {
        Map<Integer, String> mapper = Maps.newHashMap();
        mapper.put(1, "ffffffffffffffffffffffff");
        if (pageNo != null && StringUtils.isNotBlank(_id)) {
            mapper.put(pageNo, _id);
        }

        return mapper;
    }
    
    private Map<String, String> _id(Map<Integer, String> mapper) {
        Map<String, String> _id = Maps.newHashMap();
        for (Integer pageNo : mapper.keySet()) {
            _id.put(String.valueOf(pageNo), mapper.get(pageNo));
        }

        return _id;
    }
    
    private String[] propNames(HttpServletRequest request) {
        Map<String, Object> props = Servlets.getParametersStartingWith(request, "prop_");
        
        StringBuffer sbff = new StringBuffer();
        for (Entry<String, Object> entry : props.entrySet()) {
            String propName = (String) entry.getValue();
            propName = (String) Validator.validate(propName, R.CLEAN, R.LENGTH, "{1,255}", "属性");
            if (StringUtils.isBlank(propName)) {
                continue;
            }
            propName = Values.get(PropMap.getId(propName), propName);
            sbff.append(propName).append(",");
        }
        
        return Converts._toStrings(sbff.toString());
    }
    
    private String parameters(HttpServletRequest request, List<Prop> propList) {
        Map<String, Prop> propMap = Maps.newHashMap();
        for (Prop prop : propList) {
            propMap.put(prop.getPropName(), prop);
        }
        
        Map<String, Object> props = Servlets.getParametersStartingWith(request, "prop_");
        Map<String, Object> parameterMap = Servlets.getParametersStartingWith(request, "parameter_");
        
        StringBuffer sbff = new StringBuffer();
        
        for (Entry<String, Object> entry : props.entrySet()) {
            String index = entry.getKey();
            String propName = (String) entry.getValue();
            if (StringUtils.isBlank(propName)) {
                continue;
            }
            
            propName = Values.get(PropMap.getId(propName), propName);
            if (!propMap.containsKey(propName)) {
                continue;
            }
            
            Prop prop = propMap.get(propName);
            
            String val = Values.get(PropMap.getName(prop.getPropName()), prop.getPropName());
            
            String parameter = (String) parameterMap.get(index);
            parameter = (String) Validator.validate(parameter, R.CLEAN, val);
            
            Imports.isParameter(prop, parameter, val);
            
            sbff.append(prop.getPropName()).append("=").append(parameter).append(",");
        }
        
        return sbff.toString();
    }
}
