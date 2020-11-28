package cn.edm.web.action.mailing;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Device;
import cn.edm.consts.Id;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.RelationMap;
import cn.edm.model.Adapter;
import cn.edm.model.Category;
import cn.edm.model.Corp;
import cn.edm.model.Form;
import cn.edm.model.Prop;
import cn.edm.model.Resource;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Readers;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Safety;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.api.Snatchs;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Tasks;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/mailing/template/**")
public class TemplateAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(TemplateAction.class);
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Template> page = new Page<Template>();
			Pages.page(request, page);

			Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
			String templateName = StreamToString.getStringByUTF8(Https.getStr(request, "templateName", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			
            MapBean mb = new MapBean();
			Corp corp = corpService.get(UserSession.getCorpId());
            //List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());  //农信银只能看本机构的模板，不能看到下级机构的模板
			List<Corp> corps = new ArrayList<>();
			corps.add(corp);
			Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corps);
			page = templateService.search(page, mb, corpIds, UserSession.getCorpId(mb, corpId), templateName, beginTime, endTime);
			
			map.put("page", page);
			map.put("corp", corp);
			map.put("corpChildren",corps);
			map.put("mb", mb);

			return "default/mailing/template_page";
		} catch (Exception e) {
			logger.error("(Template:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/template/preview", token);
			CSRF.generate(request, "/mailing/template/recommend", token);
			CSRF.generate(request, "/mailing/template/history", token);
            CSRF.generate(request, "/mailing/template/charset", token);
            CSRF.generate(request, "/mailing/template/setting", token);

            String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^copy|add$");
            t = Values.get(t, "add");
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
			Template template = null;
			Adapter adapter = null;
			MapBean attrs = new MapBean();
			MapBean phoneAttrs = new MapBean();
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			if (templateId != null) {
				template = templateService.get(corpIds, null, null, templateId);
				Asserts.isNull(template, "模板");
				Templates.page(request, map, template);
                Templates.attrs(template, attrs);
                
                adapter = adapterService.get(templateId);
                if (adapter != null) {
                    Templates.page(request, map, adapter);
                    Templates.attrs(adapter, phoneAttrs);
                }
			}
			if (template == null) {
				template = new Template();
			}
			if (adapter == null) {
			    adapter = new Adapter();
			}
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
			
            List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), null, Category.TEMPLATE, null, null);
			Category customCategory = null;
			Category billCategory = null;
            for (Category category : categoryList) {
                if (category.getCategoryName().equals(CategoryMap.CUSTOM.getName())) {
                    if (!(root.getCorpId().equals(UserSession.getCorpId()) && UserSession.isManager())) {
                        customCategory = category;
                    }
                }
                if (category.getCategoryName().equals(CategoryMap.BILL.getName())) {
                    if (Property.getStr(Config.CATEGORY_BILL).equals("off")
                            || (!UserSession.getUserId().equals(template.getUserId())
                                    && templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 0)) {
                        billCategory = category;
                    }
                }
            }
            categoryList.remove(customCategory);
            categoryList.remove(billCategory);
			
			List<Prop> propList = propService.find(UserSession.getCorpId());

            List<Prop> propListOfOne = Lists.newArrayList();
            List<Prop> propListOfMany = Lists.newArrayList();
			
            for (Prop prop : propList) {
                if (prop.getRelation().equals(RelationMap.ONE.getId())) {
                    propListOfOne.add(prop);
                } else {
                    propListOfMany.add(prop);
                }
            }
			
            Integer[] rootCorpIds = { Id.CORP, root.getCorpId() };

            List<Category> adminCategoryList = categoryService.find(null, Id.CORP, Id.ADMIN,
                    Category.TEMPLATE, CategoryMap.TOUCH.getName(), Cnds.NOT_EQ);
            List<Category> rootCategoryList = categoryService.find(null, root.getCorpId(), null,
                    Category.TEMPLATE, CategoryMap.CUSTOM.getName(), Cnds.EQ);
            
            List<Category> recommendCategoryList = Lists.newArrayList();
            for (Category category: adminCategoryList) {
                recommendCategoryList.add(category);
            }
            for (Category category: rootCategoryList) {
                recommendCategoryList.add(category);
            }
            
			Map<String, List<Template>> recommendMap = Categorys.groupMap(
			        recommendCategoryList,
					templateService.find(rootCorpIds, null, null, null));

			List<Template> templateList = templateService.find(null, UserSession.getCorpId(), null, null);
			List<Adapter> adapterList = adapterService.find(UserSession.getCorpId(), UserSession.getUserId());
			
			List<Form> formList = formService.find(UserSession.getCorpId(), null, Value.T);
			Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
			Asserts.isNull(touchCategory, "模板类别");
			List<Template> touchTemplateList = templateService.find(UserSession.getCorpId(), touchCategory.getCategoryId(), Cnds.EQ);
			
			// 触发设置
			List<Setting> templateSets = null;
			List<Setting> phoneSets = null;
			
			int settingSize = 0;
			int phoneSize = 0;
            if (templateId != null) {
                templateSets = settingService.find(UserSession.getCorpId(), UserSession.getUserId(), templateId, Device.PC);
                phoneSets = settingService.find(UserSession.getCorpId(), UserSession.getUserId(), templateId, Device.PHONE);
            }
            
            if (Asserts.empty(templateSets)) {
                templateSets = Lists.newArrayList();
                templateSets.add(new Setting());
                settingSize = 0;
            } else {
                settingSize = templateSets.size();
            }
            
            if (Asserts.empty(phoneSets)) {
                phoneSets = Lists.newArrayList();
                phoneSets.add(new Setting());
                phoneSize = 0;
            } else {
                phoneSize = phoneSets.size();
            }
            
            if (t.equals("copy")) {
                Template copy = new Template();
                BeanUtils.copyProperties(template, copy, new String[] { "templateId", "templateName" });
                copy.setTemplateName(template.getTemplateName() + "副本");
                map.put("template", copy);
            } else {
                map.put("template", template);
            }

            map.put("adapter", adapter);
			map.put("categoryList", categoryList);
            map.put("propListOfOne", propListOfOne);
            map.put("propListOfMany", propListOfMany);
			map.put("recommendMap", recommendMap);
            map.put("templateList", templateList);
            map.put("adapterList", adapterList);
            map.put("formList", formList);
            map.put("touchTemplateList", touchTemplateList);
			map.put("attrs", attrs);
			map.put("phoneAttrs", phoneAttrs);

            map.put("settingList", templateSets);
            map.put("settingSize", settingSize);
            map.put("phoneList", phoneSets);
            map.put("phoneSize", phoneSize);
            
            map.put("fileServer", Webs.fileServer());
            map.put("corpId", UserSession.getCorpId());
			
			return "default/mailing/template_add";
		} catch (Exception e) {
			logger.error("(Template:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer templateId = null;
		String filePath = null;
		String phonePath = null;
		String smsPath = null;
		
		try {
			CSRF.validate(request);
			
			templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
			String orgTemplateName = Https.getStr(request, "orgTemplateName", R.CLEAN, R.LENGTH, "{1,22}", "模板名称");

			Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "模板类别");
			String templateName = Https.getStr(request, "templateName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "模板名称");
			String subject = Https.getStr(request, "subject", R.CLEAN, R.LENGTH, "{1,100}", "邮件主题");
			
			// PC
			String templateContent = request.getParameter("templateContent");
			String editor = Values.get(Https.getStr(request, "editor", R.CLEAN, R.REGEX, "regex:^TEXT|WYSIWYG$"), "WYSIWYG");
            String personal = Values.get(Https.getStr(request, "personal", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
            String next = Values.get(Https.getStr(request, "next", R.CLEAN, R.REGEX, "regex:^on|off$"), "on");
            
            // PHONE
            Integer joinPhone = Https.getInt(request, "joinPhone", R.CLEAN, R.REQUIRED, R.INTEGER, R.SIZE, "{0,1}", "终端类型PHONE");
            String phoneContent = request.getParameter("phoneContent");
            String phoneEditor = Values.get(Https.getStr(request, "phoneEditor", R.CLEAN, R.REGEX, "regex:^TEXT|WYSIWYG$"), "WYSIWYG");
            String phonePersonal = Values.get(Https.getStr(request, "phonePersonal", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
            // SMS
            Integer joinSms = Https.getInt(request, "joinSms", R.CLEAN, R.INTEGER, R.SIZE, "{0,1}", "终端类型短信");
            String smsContent = request.getParameter("smsContent");
            
			Category category = categoryService.get(UserSession.getCorpId(), null, categoryId, Category.TEMPLATE);
			Asserts.isNull(category, "类别");
			boolean isTouch = category.getCategoryName().equals(CategoryMap.TOUCH.getName());
            List<Setting> templateSets = Templates.templateSetting(request, isTouch, "", Device.PC);
            List<Setting> phoneSets = Templates.templateSetting(request, isTouch, "phone_", Device.PHONE);
            
			Uploads.maxSize(templateContent);
            Uploads.maxSize(phoneContent);
            Uploads.maxSize(smsContent);
			
			Asserts.notUnique(templateService.unique(UserSession.getCorpId(), templateName, orgTemplateName), "模板");
			
			Template template = null;
			Adapter adapter = null;
			if (templateId != null) {
				template = templateService.get(null, null, UserSession.getUserId(), templateId);
				Asserts.isNull(template, "模板");
				Category orgCategory = categoryService.get(UserSession.getCorpId(), null, template.getCategoryId(), Category.TEMPLATE);
				if (orgCategory != null) {
    				if (!orgCategory.getCategoryName().equals(category.getCategoryName())) {
    				    if (category.getCategoryName().equals(CategoryMap.BILL.getName())) {
    	                    if (templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 0) {
    	                        throw new Errors("不可再在账单类别下修改模板");
    	                    }
    	                }
    				}
				} else {
				    if (category.getCategoryName().equals(CategoryMap.BILL.getName())) {
                        if (templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 0) {
                            throw new Errors("不可再在账单类别下修改模板");
                        }
                    }
				}
				
				Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
				Templates.touch(template, touchCategory, categoryId);
				adapter = adapterService.get(templateId);
			} else {
			    if (category.getCategoryName().equals(CategoryMap.BILL.getName())) {
                    if (templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 0) {
                        throw new Errors("不可再在账单类别下新建模板");
                    }
                }
				template = new Template();
				template.setCorpId(UserSession.getCorpId());
				template.setUserId(UserSession.getUserId());
			}
			if (adapter == null) {
			    adapter = new Adapter();
			}
			
			String orgFilePath = template.getFilePath();
			String orgPhonePath = adapter.getPhonePath();
			String orgSmsPath = adapter.getSmsPath();
			
			Document doc = Templates.filter(response, mb, templateContent, next);
			if (doc == null) {
			    ObjectMappers.renderJson(response, mb);
			    return;
			}
			Document phoneDoc = Templates.filter(response, mb, phoneContent, next);
            if (phoneDoc == null) {
                ObjectMappers.renderJson(response, mb);
                return;
            }
			
			template.setCategoryId(categoryId);
			template.setTemplateName(templateName);
			template.setFilePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(doc).toString(), Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			template.setSubject(subject);
			
			adapter.setTemplateId(templateId);
			adapter.setJoinPhone(joinPhone);
			adapter.setJoinSms(joinSms);
			if (joinPhone.equals(Value.T)) {
			    adapter.setPhonePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(phoneDoc).toString(), Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			}
			if (joinSms.equals(Value.T)) {
			    adapter.setSmsPath(Uploads.upload(UserSession.getCorpId(), smsContent, Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			}
			
			String touch = templateSets.size() > 0 ? "true" : "false";
			template.setAttrs(Templates.attrs(editor, personal, touch));
			String phoneTouch = phoneSets.size() > 0 ? "true" : "false";
			adapter.setPhoneAttrs(Templates.attrs(phoneEditor, phonePersonal, phoneTouch));
			
			filePath = template.getFilePath();
			phonePath = adapter.getPhonePath();
			smsPath = adapter.getSmsPath();
			
			templateService.save(template, adapter, templateSets, phoneSets);

            Files.delete(Webs.rootPath(), orgFilePath, PathMap.TEMPLATE.getPath());
            Files.delete(Webs.rootPath(), orgPhonePath, PathMap.TEMPLATE.getPath());
            Files.delete(Webs.rootPath(), orgSmsPath, PathMap.TEMPLATE.getPath());
			
			Views.ok(mb, (templateId != null ? "修改" : "新建") + "模板成功");
		} catch (Errors e) {
		    e.printStackTrace();
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (templateId != null ? "修改" : "新建") + "模板失败");
			if (templateId == null) {
			    Files.delete(Webs.rootPath(), filePath, PathMap.TEMPLATE.getPath());
			    Files.delete(Webs.rootPath(), phonePath, PathMap.TEMPLATE.getPath());
			    Files.delete(Webs.rootPath(), smsPath, PathMap.TEMPLATE.getPath());
			}
			logger.error("(Template:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	/*@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/template/preview", token);
			CSRF.generate(request, "/mailing/template/recommend", token);
			CSRF.generate(request, "/mailing/template/history", token);
            CSRF.generate(request, "/mailing/template/charset", token);
            CSRF.generate(request, "/mailing/template/setting", token);

            String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^copy|add$");
            t = Values.get(t, "add");
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
			Template template = null;
			Adapter adapter = null;
			MapBean attrs = new MapBean();
			MapBean phoneAttrs = new MapBean();
			
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
            
			if (templateId != null) {
				template = templateService.get(corpIds, null, UserSession.containsUserId(), templateId);
				template = templateService.get(corpIds, null,null, templateId);
				Asserts.isNull(template, "模板");
				Templates.page(request, map, template);
                Templates.attrs(template, attrs);
                
                adapter = adapterService.get(templateId);
                if (adapter != null) {
                    Templates.page(request, map, adapter);
                    Templates.attrs(adapter, phoneAttrs);
                }
			}
			if (template == null) {
				template = new Template();
			}
			if (adapter == null) {
			    adapter = new Adapter();
			}
			
			List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TEMPLATE, null, null);
			List<Prop> propList = propService.find(UserSession.getCorpId());

            List<Prop> propListOfOne = Lists.newArrayList();
            List<Prop> propListOfMany = Lists.newArrayList();
			
            for (Prop prop : propList) {
                if (prop.getRelation().equals(RelationMap.ONE.getId())) {
                    propListOfOne.add(prop);
                } else {
                    propListOfMany.add(prop);
                }
            }
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            
            Integer[] rootCorpIds = { Id.CORP, root.getCorpId() };
			Map<String, List<Template>> recommendMap = Categorys.groupMap(
					categoryService.find(rootCorpIds, null, Category.TEMPLATE, CategoryMap.TOUCH.getName(), Cnds.NOT_EQ),
					templateService.find(rootCorpIds, null, null, null));

			List<Template> templateList = templateService.find(null, null, UserSession.getUserId(), null);
			List<Adapter> adapterList = adapterService.find(UserSession.getCorpId(), UserSession.getUserId());
			
			List<Form> formList = formService.find(UserSession.getCorpId(), null, Value.T);
			
			Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
			Asserts.isNull(touchCategory, "模板类别");
			List<Template> touchTemplateList = templateService.find(UserSession.getCorpId(), touchCategory.getCategoryId(), Cnds.EQ);
			
			// 触发设置
			List<Setting> templateSets = null;
			List<Setting> phoneSets = null;
			
			int settingSize = 0;
			int phoneSize = 0;
            if (templateId != null) {
                templateSets = settingService.find(UserSession.getCorpId(), UserSession.getUserId(), templateId, Device.PC);
                phoneSets = settingService.find(UserSession.getCorpId(), UserSession.getUserId(), templateId, Device.PHONE);
            }
            
            if (Asserts.empty(templateSets)) {
                templateSets = Lists.newArrayList();
                templateSets.add(new Setting());
                settingSize = 0;
            } else {
                settingSize = templateSets.size();
            }
            
            if (Asserts.empty(phoneSets)) {
                phoneSets = Lists.newArrayList();
                phoneSets.add(new Setting());
                phoneSize = 0;
            } else {
                phoneSize = phoneSets.size();
            }
            
            if (t.equals("copy")) {
                Template copy = new Template();
                BeanUtils.copyProperties(template, copy, new String[] { "templateId", "templateName" });
                copy.setTemplateName(template.getTemplateName() + "副本");
                map.put("template", copy);
            } else {
                map.put("template", template);
            }

            map.put("adapter", adapter);
			map.put("categoryList", categoryList);
            map.put("propListOfOne", propListOfOne);
            map.put("propListOfMany", propListOfMany);
			map.put("recommendMap", recommendMap);
            map.put("templateList", templateList);
            map.put("adapterList", adapterList);
            map.put("formList", formList);
            map.put("touchTemplateList", touchTemplateList);
			map.put("attrs", attrs);
			map.put("phoneAttrs", phoneAttrs);

            map.put("settingList", templateSets);
            map.put("settingSize", settingSize);
            map.put("phoneList", phoneSets);
            map.put("phoneSize", phoneSize);
            
            map.put("fileServer", Webs.fileServer());
            map.put("corpId", UserSession.getCorpId());
			
			return "default/mailing/template_add";
		} catch (Exception e) {
			logger.error("(Template:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer templateId = null;
		String filePath = null;
		String phonePath = null;
		String smsPath = null;
		
		try {
			CSRF.validate(request);
			
			templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
			String orgTemplateName = Https.getStr(request, "orgTemplateName", R.CLEAN, R.LENGTH, "{1,20}", "模板名称");

			Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "模板类别");
			String templateName = Https.getStr(request, "templateName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "模板名称");
			String subject = Https.getStr(request, "subject", R.CLEAN, R.LENGTH, "{1,100}", "邮件主题");
			
			// PC
			String templateContent = request.getParameter("templateContent");
			String editor = Values.get(Https.getStr(request, "editor", R.CLEAN, R.REGEX, "regex:^TEXT|WYSIWYG$"), "WYSIWYG");
            String personal = Values.get(Https.getStr(request, "personal", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
            String next = Values.get(Https.getStr(request, "next", R.CLEAN, R.REGEX, "regex:^on|off$"), "on");
            
            // PHONE
            Integer joinPhone = Https.getInt(request, "joinPhone", R.CLEAN, R.REQUIRED, R.INTEGER, R.SIZE, "{0,1}", "终端类型PHONE");
            String phoneContent = request.getParameter("phoneContent");
            String phoneEditor = Values.get(Https.getStr(request, "phoneEditor", R.CLEAN, R.REGEX, "regex:^TEXT|WYSIWYG$"), "WYSIWYG");
            String phonePersonal = Values.get(Https.getStr(request, "phonePersonal", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
            // SMS
            Integer joinSms = Https.getInt(request, "joinSms", R.CLEAN, R.INTEGER, R.SIZE, "{0,1}", "终端类型短信");
            String smsContent = request.getParameter("smsContent");
            
			Category category = categoryService.get(UserSession.getCorpId(), null, categoryId, Category.TEMPLATE);
			Asserts.isNull(category, "类别");
			boolean isTouch = category.getCategoryName().equals(CategoryMap.TOUCH.getName());
            List<Setting> templateSets = Templates.templateSetting(request, isTouch, "", Device.PC);
            List<Setting> phoneSets = Templates.templateSetting(request, isTouch, "phone_", Device.PHONE);
            
			Uploads.maxSize(templateContent);
            Uploads.maxSize(phoneContent);
            Uploads.maxSize(smsContent);
			
			Asserts.notUnique(templateService.unique(UserSession.getCorpId(), templateName, orgTemplateName), "模板");
			
			Template template = null;
			Adapter adapter = null;
			if (templateId != null) {
				template = templateService.get(null, null, UserSession.getUserId(), templateId);
				Asserts.isNull(template, "模板");
				Category billCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.BILL.getName(), Category.TEMPLATE);
				if (billCategory != null && !billCategory.getCategoryId().equals(category.getCategoryId())) {
				    if (templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 1) {
                        throw new Errors("不可再在账单类别下修改模板类别");
                    }
				}
				Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
				Templates.touch(template, touchCategory, categoryId);
				adapter = adapterService.get(templateId);
			} else {
			    if (category.getCategoryName().equals(CategoryMap.BILL.getName())) {
                    if (templateService.count(UserSession.getCorpId(), category.getCategoryId()) > 1) {
                        throw new Errors("不可再在账单类别下新建模板");
                    }
                }
				template = new Template();
				template.setCorpId(UserSession.getCorpId());
				template.setUserId(UserSession.getUserId());
			}
			if (adapter == null) {
			    adapter = new Adapter();
			}
			
			String orgFilePath = template.getFilePath();
			String orgPhonePath = adapter.getPhonePath();
			String orgSmsPath = adapter.getSmsPath();
			
			Document doc = Templates.filter(response, mb, templateContent, next);
			if (doc == null) {
			    ObjectMappers.renderJson(response, mb);
			    return;
			}
			Document phoneDoc = Templates.filter(response, mb, phoneContent, next);
            if (phoneDoc == null) {
                ObjectMappers.renderJson(response, mb);
                return;
            }
			
			template.setCategoryId(categoryId);
			template.setTemplateName(templateName);
			template.setFilePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(doc).toString(), Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			template.setSubject(subject);
			
			adapter.setTemplateId(templateId);
			adapter.setJoinPhone(joinPhone);
			adapter.setJoinSms(joinSms);
			if (joinPhone.equals(Value.T)) {
			    adapter.setPhonePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(phoneDoc).toString(), Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			}
			if (joinSms.equals(Value.T)) {
			    adapter.setSmsPath(Uploads.upload(UserSession.getCorpId(), smsContent, Webs.rootPath(), PathMap.TEMPLATE.getPath()));
			}
			
			String touch = templateSets.size() > 0 ? "true" : "false";
			template.setAttrs(Templates.attrs(editor, personal, touch));
			String phoneTouch = phoneSets.size() > 0 ? "true" : "false";
			adapter.setPhoneAttrs(Templates.attrs(phoneEditor, phonePersonal, phoneTouch));
			
			filePath = template.getFilePath();
			phonePath = adapter.getPhonePath();
			smsPath = adapter.getSmsPath();
			
			templateService.save(template, adapter, templateSets, phoneSets);

            Files.delete(Webs.rootPath(), orgFilePath, PathMap.TEMPLATE.getPath());
            Files.delete(Webs.rootPath(), orgPhonePath, PathMap.TEMPLATE.getPath());
            Files.delete(Webs.rootPath(), orgSmsPath, PathMap.TEMPLATE.getPath());
			
			Views.ok(mb, (templateId != null ? "修改" : "新建") + "模板成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (templateId != null ? "修改" : "新建") + "模板失败");
			if (templateId == null) {
			    Files.delete(Webs.rootPath(), filePath, PathMap.TEMPLATE.getPath());
			    Files.delete(Webs.rootPath(), phonePath, PathMap.TEMPLATE.getPath());
			    Files.delete(Webs.rootPath(), smsPath, PathMap.TEMPLATE.getPath());
			}
			logger.error("(Template:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}*/

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			int[] templateIds = Converts._toInts(checkedIds);
			
			if (templateIds != null && templateIds.length == 1) {
				Template template = templateService.get(null, null, UserSession.getUserId(), templateIds[0]);
				if (template != null) {
					map.put("checkedName", template.getTemplateName());
				}
			}
			
			Views.map(map, "del", "", "删除", "所选模板", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Template:del) error: ", e);
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
			Integer[] templateIds = Converts._toIntegers(checkedIds);

			boolean lockTag = false;
			boolean lockSetting = false;
			
			if (!Asserts.empty(templateIds)) {
				List<Template> templateList = templateService.find(UserSession.getCorpId(), templateIds);
				List<Integer> templateIdList = taskService.lockTemplate(UserSession.getCorpId(), UserSession.getUserId(), templateIds);
				List<Integer> settingIdList = settingService.lockTemplate(UserSession.getCorpId(), UserSession.getUserId(), templateIds);
				
				if (!Asserts.empty(templateIdList)) {
					lockTag = true;
				}
				if (!Asserts.empty(settingIdList)) {
				    lockSetting = true;
                }
				
				StringBuffer sbff = new StringBuffer();
				for (Template template : templateList) {
					if (lockTag && templateIdList.contains(template.getTemplateId())) {
						continue;
					}
					if (lockSetting && settingIdList.contains(template.getTemplateId())) {
                        continue;
                    }
					sbff.append(template.getTemplateId()).append(",");
				}
				
				checkedIds = sbff.toString();				
				templateIds = Converts._toIntegers(sbff.toString());
				
				if (!Asserts.empty(templateIds)) {
					templateService.delete(UserSession.getUserId(), templateIds);
                    settingService.delete(UserSession.getCorpId(), UserSession.getUserId(), templateIds);
				}
				
				for (Template template : templateList) {
					if (lockTag && templateIdList.contains(template.getTemplateId())) {
                        continue;
                    }
                    if (lockSetting && settingIdList.contains(template.getTemplateId())) {
                        continue;
                    }
					
					Files.delete(Webs.rootPath(), template.getFilePath(), PathMap.TEMPLATE.getPath());
				}
			}

			if (lockTag) {
				Views.error(mb, "所选的部分模板已关联投递任务，禁止删除");
			} else if (lockSetting) {
                Views.error(mb, "所选的部分模板已关联触发计划，禁止删除");
            } else {
				Views.ok(mb, "删除模板成功");
			}
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除模板失败");
			logger.error("(Template:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public String preview(HttpServletRequest request, HttpServletResponse response) {
		try {
			CSRF.auth(request);
			
			String content = request.getParameter("content");
			if (StringUtils.isBlank(content)) {
				content = Value.S;
			}
			
			Document doc = Jsoup.parse(content);
			Modulars.filter(doc);
			ObjectMappers.renderHtml(response, XSS.safety(doc).toString());

			return null;
		} catch (Exception e) {
		    e.printStackTrace();
			logger.error("(Template:preview) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String view(HttpServletRequest request, ModelMap map) {
		try {
			String action = Values.get(Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^template|task|audit|touch$"), "template");
			Integer device = Values.get(Https.getInt(request, "device", R.CLEAN, R.INTEGER, R.REGEX, "regex:^0|1|2$"), Device.PC);
            
			Integer templateId = null;

			String subject = null;
			String senderEmail = null;
			String senderName = null;
			String receiver = null;
			Date createTime = null;

			String style = null;
			String content = null;
			
			Adapter adapter = new Adapter();
			
			if (action.equals("template")) {
				templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
				Asserts.isNull(templateId, "模板");
            } else if (action.equals("touch")) {
                templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
                Asserts.isNull(templateId, "模板");
				Integer touchId = Https.getInt(request, "touchId", R.CLEAN, R.INTEGER);
				Asserts.isNull(touchId, "触发任务");
				map.put("taskId", touchId);
				Touch touch = touchService.get(UserSession.getCorpId(), touchId, templateId);
				Asserts.isNull(touch, "触发任务");
				
				String sid = Tasks.id(UserSession.getCorpId(), touch.getTaskId(), touch.getTemplateId());
				String path = Webs.rootPath() + PathMap.TOUCH.getPath() + "" + sid + "/";
				
				File recent = Files.recent(path);
				if (recent != null) {
				    String filePath = path + recent.getName();
	                if (Files.exists(filePath)) {
	                    Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), touch.getParentId());
	                    Asserts.isNull(task, "任务");
	                    subject = Delivery.node(filePath, "SUBJECT");
	                    senderEmail = Delivery.node(filePath, "SENDER");
	                    senderName = Delivery.node(filePath, "NICKNAME");
	                    String MODULA = modula(adapter, filePath, device);
	                    
	                    Resource resource = resourceService.get(task.getResourceId());
                        String modula = filter(resource, MODULA);
                        
                        Document doc = Jsoup.parse(modula);
                        Modulars.filter(doc);
	                    
	                    content = XSS.safety(doc).body().html();
	                    
	                    receiver = task.getReceiver();
	                    createTime = task.getCreateTime();
                        map.put("touchId", touchId);
                        map.put("templateId", templateId);
	                    templateId = null;
	                }
				}
			} else {
				Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
				Asserts.isNull(taskId, "任务");
				/*Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), taskId);*/
				Task task = taskService.get(null,null, taskId);
				Asserts.isNull(task, "任务");
				
				Integer type = task.getType();
				String sid = Tasks.id(task.getCorpId(), task.getTaskId(), task.getTemplateId());
				
				if (type.equals(Type.JOIN)) {
				    String path = Webs.rootPath() + PathMap.JOIN_API.getPath() + "/" + sid + ".txt";
				    if (Files.exists(path)) {
				        subject = Delivery.node(path, "SUBJECT");
	                    senderEmail = Delivery.node(path, "SENDER");
	                    senderName = Delivery.node(path, "NICKNAME");
	                    String MODULA = modula(adapter, path, device);
	                    content = MODULA;
	                    receiver = task.getReceiver();
	                    createTime = task.getCreateTime();
				    }
				} else {
				    boolean isDelivery = task.getDeliveryTime() != null;
	                String time = isDelivery ? new DateTime(task.getDeliveryTime()).toString("yyyyMM") : Value.S;
	                
	                String path = Webs.rootPath() + PathMap.TASK.getPath() + time + "/" + sid + ".txt";
	                
	                if (isDelivery && Files.exists(path)) {
	                    subject = Delivery.node(path, "SUBJECT");
	                    senderEmail = Delivery.node(path, "SENDER");
	                    senderName = Delivery.node(path, "NICKNAME");
	                    String MODULA = modula(adapter, path, device);
	                    
                        Resource resource = resourceService.get(task.getResourceId());
                        String modula = filter(resource, MODULA);
                        
                        Document doc = Jsoup.parse(modula);
                        Modulars.filter(doc);
                        
                        content = XSS.safety(doc).body().html();
                        
                        List<Touch> touchList = touchService.find(task.getTaskId());
                        if (!Asserts.empty(touchList)) {
                            Setting taskSetting = settingService.get(task.getTaskId());
                            Asserts.isNull(taskSetting, "任务设置");
                            
                            map.put("templateId", task.getTemplateId());
                            map.put("templateName", taskSetting.getTemplate());
                            map.put("action", "touch");
                            
                            List<Setting> settingList = Lists.newArrayList();
                            for (Touch touch : touchList) {
                                Setting setting = new Setting();
                                setting.setTaskId(touch.getTaskId());
                                setting.setTemplateId(touch.getTemplateId());
                                setting.setTemplateName(touch.getTemplateName());
                                settingList.add(setting);
                            }
                            map.put("settingList", settingList);
                        }
	                } else {
	                    templateId = task.getTemplateId();
	                    subject = task.getSubject();
	                    senderEmail = task.getSenderEmail();
	                    senderName = task.getSenderName();
	                    createTime = task.getCreateTime();
	                }
	                
	                receiver = task.getReceiver();
	                createTime = task.getCreateTime();
				}
	            map.put("taskId", taskId);
			}
			
			if (templateId != null) {
			    List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
                Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
				/*Template template = templateService.get(corpIds, UserSession.containsCorpId(), null, templateId);*/	
                Template template = templateService.get(null,null, null, templateId);
				Asserts.isNull(template, "模板");
				Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");

				subject = Values.get(subject, template.getSubject());
				senderEmail = Values.get(senderEmail, template.getSenderEmail());
				senderName = Values.get(senderName, template.getSenderName());
				
				if (createTime == null) {
					createTime = template.getCreateTime();
				}
				
                adapter = adapterService.get(templateId);
                if (adapter == null) {
                    adapter = new Adapter();
                }
                
                String filePath = template.getFilePath();
                if (adapter != null) {
                    if (device.equals(Device.PHONE)) {
                        if (Asserts.exists(Webs.rootPath(), adapter.getPhonePath())) {
                            filePath = adapter.getPhonePath();
                        } else {
                            filePath = null;
                        }
                    } else if (device.equals(Device.SMS)) {
                        if (Asserts.exists(Webs.rootPath(), adapter.getSmsPath())) {
                            filePath = adapter.getSmsPath();
                        } else {
                            filePath = null;
                        }
                    }
                }
                if (StringUtils.isNotBlank(filePath)) {
                    Document doc = Jsoup.parse(Files.get(Webs.rootPath(), filePath));
                    Modulars.filter(doc);
                    content = XSS.safety(doc).body().html();
                } else {
                    content = null;
                }
                
				List<Setting> settingList = settingService.find(UserSession.getCorpId(), UserSession.containsUserId(), templateId, null);
				map.put("settingList", settingList);
				map.put("templateId", templateId);
				map.put("templateName", template.getTemplateName());
			}

            map.put("action", action);
            map.put("subject", subject);
			map.put("senderEmail", senderEmail);
			map.put("senderName", senderName);
			map.put("receiver", receiver);
			map.put("createTime", createTime);
			map.put("style", style);
            map.put("content", content);
            map.put("device", device);
            map.put("adapter", adapter);
			
			return "default/mailing/template_view";
		} catch (Exception e) {
			logger.error("(Template:view) error: ", e);
			return Views._404();
		}
	}
	
	private String modula(Adapter adapter, String path, Integer device) {
	    String content = null;
	    String modula = Delivery.node(path, "MODULA");
	    String modulaQvga = Delivery.node(path, "MODULA_QVGA");
	    String modulaText = Delivery.node(path, "MODULA_TEXT");
        if (device.equals(Device.PHONE)) {
            content = modulaQvga;
        } else if (device.equals(Device.SMS)) {
            content = modulaText;
        } else {
            content = modula;
        }
        
        if (StringUtils.isNotBlank(modulaQvga)) {
            adapter.setJoinPhone(Value.T);
        }
        if (StringUtils.isNotBlank(modulaText)) {
            adapter.setJoinSms(Value.T);
        }
        
        return content;
	}
	
	private String filter(Resource resource, String modula) {
		Document doc = Jsoup.parse(modula);
		// open
		// SMS body 没 children
		if(!StringUtils.isBlank(doc.body().children().html())){
			doc.body().children().first().remove();
		}

		// click
		StringBuffer sbff = new StringBuffer();
		for (String skip : Converts.toStrings(resource.getSkipList())) {
			sbff.append("http://").append(skip).append("/test3.php").append(",");
		}
		String[] skips = Converts._toStrings(sbff.toString());

		// online
		String online = Property.getStr(Config.APP_URL) + "/online";

		Elements links = doc.select("[href]");
		Elements images = doc.select("[src]");

		for (Element link : links) {
			String href = link.attr("abs:href");
			if (Asserts.containsAny(href, skips)) {
				String u = null;
				for (String strs : StringUtils.split(StringUtils.substringAfter(href, "?"), "&")) {
					String k = StringUtils.substringBefore(strs, "=");
					String v = StringUtils.substringAfter(strs, "=");
					if (k.equals("u")) {
						u = new String(Encodes.decodeBase64(v));
						break;
					}
				}
				link.attr("href", u);
			}

			if (StringUtils.startsWith(href, online)) {
				link.attr("href", online);
			}
		}
		for (Element link : images) {
			link.removeAttr("href");
		}

		return doc.toString();
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String file(HttpServletRequest request) {
		try {
			CSRF.generate(request);
			return "default/mailing/template_file"; 
		} catch (Exception e) {
			logger.error("(Template:file) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void file(HttpServletRequest request, HttpServletResponse response, 
			@RequestParam(required = false, value = "upload") MultipartFile upload) {
		Integer code = Value.I;
		String message = Value.S;
		String GB2312 = Value.S;
		String UTF8 = Value.S;
		try {
			CSRF.auth(request);
			
			Files.valid(upload, new MapBean("file", "html,htm"));
			boolean script = false;
			boolean blacklist = false;
			
			Document doc1 = Jsoup.parse(Readers.html(upload.getInputStream(), "GB2312"));
			Document doc2 = Jsoup.parse(Readers.html(upload.getInputStream(), "UTF-8"));
			
			Uploads.maxSize(doc1.toString());
			Uploads.maxSize(doc2.toString());
			
			if (doc1.select("script").size() > 0 || doc2.select("script").size() > 0) {
				script = true;
			}

			// 2) HTML WhiteList
            if (!(Jsoup.isValid(doc1.toString(), Safety.whitelist()) && Jsoup.isValid(doc2.toString(), Safety.whitelist()))) {
                blacklist = true;
            }
            
			Modulars.filter(doc1);
			Modulars.filter(doc2);
			
			GB2312 = XSS.safety(doc1).toString();
			UTF8 = XSS.safety(doc2).toString();

			code = script || blacklist ? 2 : Value.T;
			message = script || blacklist ? Templates.filter(script, blacklist) : "导入文件成功";
		} catch (Errors e) {
			code = Value.F;
			message = e.getMessage();
		} catch (Exception e) {
			code = Value.F;
			message = "导入文件失败";
			logger.error("(Template:file) error: ", e);
		}
		
		StringBuffer sbff = new StringBuffer();
		sbff.append("code:").append(code)
			.append("---------- NextPart ----------")
			.append("message:").append(message)
			.append("---------- NextPart ----------")
			.append(GB2312)
			.append("---------- NextPart ----------")
			.append(UTF8);
		
		ObjectMappers.renderHtml(response, sbff.toString());
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String snatch(HttpServletRequest request) {
		try {
			CSRF.generate(request);
			return "default/mailing/template_snatch"; 
		} catch (Exception e) {
			logger.error("(Template:snatch) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void snatch(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			String url = Https.getStr(request, "snatchUrl", R.REQUIRED, "网页地址");
			boolean script = false;
			boolean blacklist = false;
			
			String s1 = Snatchs.get(url, "GB2312");
			String s2 = Snatchs.get(url, "UTF-8");
			Uploads.maxSize(s1);
			Uploads.maxSize(s2);
			
			Document doc1 = Jsoup.parse(s1, url);
			Document doc2 = Jsoup.parse(s2, url);
			
			// 1) JavaScript
			if (doc1.select("script").size() > 0 || doc2.select("script").size() > 0) {
				script = true;
			}
			
			// 2) HTML WhiteList
			if (!(Jsoup.isValid(doc1.toString(), Safety.whitelist()) && Jsoup.isValid(doc2.toString(), Safety.whitelist()))) {
                blacklist = true;
            }

			filter(doc1.select("[href]"), "abs:href", "href");
			filter(doc1.select("[src]"), "abs:src", "src");
			filter(doc2.select("[href]"), "abs:href", "href");
			filter(doc2.select("[src]"), "abs:src", "src");

			Modulars.filter(doc1);
			Modulars.filter(doc2);

			mb.put("code", script || blacklist ? 2 : Value.T);
			mb.put("message", script || blacklist ? Templates.filter(script, blacklist) : "抓取网页成功");
			
			mb.put("GB2312", XSS.safety(doc1).toString());
			mb.put("UTF8", XSS.safety(doc2).toString());
		} catch (Errors e) {
			mb.put("code", Value.F, "message", e.getMessage());
		} catch (Exception e) {
			mb.put("code", Value.F, "message", "抓取网页失败");
			logger.error("(Template:snatch) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void recommend(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(templateId, "推荐模板");
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
            Template template = templateService.get(corpIds, null, null, templateId);
			Asserts.isNull(template, "推荐模板");
			String content = Files.get(Webs.rootPath(), template.getFilePath());
			
			Document doc = Jsoup.parse(content);
			Modulars.filter(doc);
			
			mb.put("content", XSS.safety(doc).toString());
			mb.put("page_header", StringUtils.contains(content, "page_header") ? "on" : "off");
			mb.put("page_footer", StringUtils.contains(content, "page_footer") ? "on" : "off");
			
			Views.ok(mb, "导入模板成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "导入模板失败");
			logger.error("(Template:recommend) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void history(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
//			CSRF.validate(request);
			
			Integer device = Values.get(Https.getInt(request, "device", R.CLEAN, R.INTEGER, R.REGEX, "regex:^0|1$"), Device.PC);
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(templateId, "历史模板");
			Template template = templateService.get(null, null, UserSession.getUserId(), templateId);
			Asserts.isNull(template, "历史模板");
			
			String content = "";
			
			if (device.equals(Device.PC)) {
    			content = Files.get(Webs.rootPath(), template.getFilePath());
    			Uploads.maxSize(content);
			} else if (device.equals(Device.PHONE)) {
			    Adapter adapter = adapterService.get(templateId);
			    Asserts.isNull(adapter, "历史模板");
			    content = Files.get(Webs.rootPath(), adapter.getPhonePath());
                Uploads.maxSize(content);
			}
			
			Document doc = Jsoup.parse(content);
			Modulars.filter(doc);
			
			mb.put("content", XSS.safety(doc).toString());
			mb.put("page_header", StringUtils.contains(content, "page_header") ? "on" : "off");
			mb.put("page_footer", StringUtils.contains(content, "page_footer") ? "on" : "off");
			
			Views.ok(mb, "导入模板成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "导入模板失败");
			logger.error("(Template:history) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	private void filter(Elements links, String abs, String attr) {
		for (Element link : links) {
			String src = link.attr(abs);
			link.attr(attr, src);
		}
	}
	
}
