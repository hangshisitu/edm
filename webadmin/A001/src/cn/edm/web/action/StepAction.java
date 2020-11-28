package cn.edm.web.action;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;
import org.jsoup.nodes.Document;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.app.auth.Auth;
import cn.edm.app.security.Securitys;
import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Device;
import cn.edm.consts.Fqn;
import cn.edm.consts.Id;
import cn.edm.consts.Options;
import cn.edm.consts.Permissions;
import cn.edm.consts.Status;
import cn.edm.consts.Suffix;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.RoleMap;
import cn.edm.consts.mapper.TypeMap;
import cn.edm.model.Adapter;
import cn.edm.model.Campaign;
import cn.edm.model.Category;
import cn.edm.model.Cnd;
import cn.edm.model.Corp;
import cn.edm.model.Form;
import cn.edm.model.Plan;
import cn.edm.model.Prop;
import cn.edm.model.Resource;
import cn.edm.model.Robot;
import cn.edm.model.Sender;
import cn.edm.model.Setting;
import cn.edm.model.Step;
import cn.edm.model.Tag;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Randoms;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.Ntps;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Cookies;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.UserSession;
import cn.edm.web.facade.Audits;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Plans;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Recipients;
import cn.edm.web.facade.Senders;
import cn.edm.web.facade.Tasks;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/step/**")
public class StepAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(StepAction.class);

	@RequestMapping(value = "1", method = RequestMethod.GET)
	public String _1(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/template/preview", token);
			CSRF.generate(request, "/mailing/template/recommend", token);
			CSRF.generate(request, "/mailing/template/history", token);

			String id = Https.getStr(request, "id", R.CLEAN, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$");

			Step step = null;
			if (StringUtils.isNotBlank(id)) {
				step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
				Asserts.isNull(step, "流水号");
			}
			if (step == null) {
				step = new Step();
				id = Randoms.getRandomNumber(20);
				step.setId(id);
				step.setUserId(UserSession.getUserId());
				cache.put(Fqn.R, step.getUserId() + ":" + step.getId(), step); // 新建流水号
			}

			Integer templateId = step.getTemplateId();
			Template template = null;
			Adapter adapter = null;
			MapBean attrs = new MapBean();
            MapBean phoneAttrs = new MapBean();
			if (templateId != null) {
				template = templateService.get(null, UserSession.getCorpId(), null, templateId);
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
			
			Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
            Asserts.isNull(touchCategory, "模板类别");
            
			List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TEMPLATE, CategoryMap.TOUCH.getName(), Cnds.NOT_EQ);
			List<Prop> propList = propService.find(UserSession.getCorpId());
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Integer[] corpIds = { Id.CORP, root.getCorpId() };
			
			Map<String, List<Template>> recommendMap = Categorys.groupMap(
					categoryService.find(corpIds, null, Category.TEMPLATE, CategoryMap.TOUCH.getName(), Cnds.NOT_EQ),
					templateService.find(corpIds, null, null, null));
			List<Template> templateList = templateService.find(UserSession.getCorpId(), touchCategory.getCategoryId(), Cnds.NOT_EQ);
			List<Form> formList = formService.find(UserSession.getCorpId(), null, Value.T);
            List<Template> touchTemplateList = templateService.find(UserSession.getCorpId(), touchCategory.getCategoryId(), Cnds.EQ);
            
            // 触发设置
            List<Setting> templateSets = null;
            List<Setting> phoneSets = null;
            int settingSize = 0;
            int phoneSize = 0;
            if (templateId != null) {
                templateSets = settingService.find(UserSession.getCorpId(), null, templateId, Device.PC);
                phoneSets = settingService.find(UserSession.getCorpId(), null, templateId, Device.PHONE);
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
            
			map.put("step", step);
			map.put("template", template);
			map.put("attrs", attrs);
            map.put("phoneAttrs", phoneAttrs);
            
			map.put("categoryList", categoryList);
			map.put("propList", propList);
			map.put("recommendMap", recommendMap);
            map.put("templateList", templateList);
            map.put("formList", formList);
            map.put("touchTemplateList", touchTemplateList);

            map.put("settingList", templateSets);
            map.put("settingSize", settingSize);
            map.put("phoneList", phoneSets);
            map.put("phoneSize", phoneSize);
            
			return "step_1";
		} catch (Exception e) {
			logger.error("(Step:1) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "1", method = RequestMethod.POST)
	public void _1(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer templateId = null;
		String filePath = null;
        String phonePath = null;
        String smsPath = null;
		
		try {
			CSRF.validate(request);
			
			String id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$:不是合法值", "流水号");
			
			Step step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
			Asserts.isNull(step, "流水号");
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^create|select$");
            action = Values.get(action, "create");
            
            step.set_1(action);
            
            Template template = null;
//            Adapter adapter = null;
            
            if (action.equals("create")) {
                templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
                Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "模板类别");
                String templateName = Https.getStr(request, "templateName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "模板名称");
                String orgTemplateName = Https.getStr(request, "orgTemplateName", R.CLEAN, R.LENGTH, "{1,20}", "模板名称");
                String subject = Https.getStr(request, "subject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
                String templateContent = request.getParameter("templateContent");
                String editor = Values.get(Https.getStr(request, "editor", R.CLEAN, R.REGEX, "regex:^TEXT|WYSIWYG$"), "WYSIWYG");
                String personal = Values.get(Https.getStr(request, "personal", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
                String next = Values.get(Https.getStr(request, "next", R.CLEAN, R.REGEX, "regex:^on|off$"), "on");
                
                Category category = categoryService.get(UserSession.getCorpId(), null, categoryId, Category.TEMPLATE);
                Asserts.isNull(category, "类别");
                boolean isTouch = category.getCategoryName().equals(CategoryMap.TOUCH.getName());
                List<Setting> templateSets = Templates.templateSetting(request, isTouch, "", Device.PC);
                List<Setting> phoneSets = Templates.templateSetting(request, isTouch, "phone_", Device.PHONE);
                
                Uploads.maxSize(templateContent);
                
                Asserts.notUnique(templateService.unique(UserSession.getCorpId(), templateName, orgTemplateName), "模板");

                if (templateId != null) {
                    template = templateService.get(null, UserSession.getCorpId(), null, templateId);
                    Asserts.isNull(template, "模板");
                    Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
                    Templates.touch(template, touchCategory, categoryId);
//                    adapter = adapterService.get(templateId);
                } else {
                    template = new Template();
                    template.setCorpId(UserSession.getCorpId());
                    template.setUserId(UserSession.getUserId());
                }
//                if (adapter == null) {
//                    adapter = new Adapter();
//                }
                
                String orgFilePath = template.getFilePath();
//                String orgPhonePath = adapter.getPhonePath();
//                String orgSmsPath = adapter.getSmsPath();
                
                Document doc = Templates.filter(response, mb, templateContent, next);
                if (doc == null) {
                    ObjectMappers.renderJson(response, mb);
                    return;
                }
//                Document phoneDoc = Templates.filter(response, mb, phoneContent, next);
//                if (phoneDoc == null) {
//                    ObjectMappers.renderJson(response, mb);
//                    return;
//                }
                
                template.setCategoryId(categoryId);
                template.setTemplateName(templateName);
                template.setFilePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(doc).toString(), Webs.rootPath(), PathMap.TEMPLATE.getPath()));
                template.setSubject(subject);
                
//                adapter.setTemplateId(templateId);
//                adapter.setJoinPhone(joinPhone);
//                adapter.setJoinSms(joinSms);
//                adapter.setPhonePath(Uploads.upload(UserSession.getCorpId(), XSS.safety(phoneDoc).toString(), Webs.getRealPath(request), PathMap.TEMPLATE.getPath()));
//                adapter.setSmsPath(Uploads.upload(UserSession.getCorpId(), smsContent, Webs.getRealPath(request), PathMap.TEMPLATE.getPath()));
                
                String touch = templateSets.size() > 0 ? "true" : "false";
                template.setAttrs(Templates.attrs(editor, personal, touch));
//                String phoneTouch = phoneSets.size() > 0 ? "true" : "false";
//                adapter.setPhoneAttrs(Templates.attrs(phoneEditor, phonePersonal, phoneTouch));
                
                filePath = template.getFilePath();
//                phonePath = adapter.getPhonePath();
//                smsPath = adapter.getSmsPath();

                templateService.save(template, null, templateSets, phoneSets);
                
                Files.delete(Webs.rootPath(), orgFilePath, PathMap.TEMPLATE.getPath());
//                Files.delete(Webs.getRealPath(request), orgPhonePath, PathMap.TEMPLATE.getPath());
//                Files.delete(Webs.getRealPath(request), orgSmsPath, PathMap.TEMPLATE.getPath());
                
                step.setSubject(template.getSubject());
            } else {
                templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
                template = templateService.get(null, UserSession.getCorpId(), null, templateId);
                Asserts.isNull(template, "模板");
                Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
                String subject = Https.getStr(request, "subject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
                step.setSubject(subject);
            }
			
			step.setTemplateId(template.getTemplateId());
			
			cache.put(Fqn.R, UserSession.getUserId() + ":" + id, step); // 更新流水号
			
			Views.ok(mb, "设计模板成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "设计模板失败");
			if (templateId == null) {
			    Files.delete(Webs.rootPath(), filePath, PathMap.TEMPLATE.getPath());
                Files.delete(Webs.rootPath(), phonePath, PathMap.TEMPLATE.getPath());
                Files.delete(Webs.rootPath(), smsPath, PathMap.TEMPLATE.getPath());
			}
			logger.error("(Step:1) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(value = "2", method = RequestMethod.GET)
	public String _2(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/datasource/selection/upload", token);
			
			String id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$:不是合法值", "流水号");
			
			Step step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
			Asserts.isNull(step, "流水号");
			
			Integer templateId = step.getTemplateId();
			Asserts.isNull(templateId, "模板");
			Template template = templateService.get(null, null, UserSession.getUserId(), templateId);
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
			
			List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TAG, CategoryMap.FILE.getName(), Cnds.NOT_EQ);
			List<Tag> tagList = tagService.find(UserSession.getCorpId());
			Map<String, List<Tag>> groupMap = Categorys.groupMap(categoryList, tagList);
			List<Prop> propList = propService.find(UserSession.getCorpId());
			String parameters = parameters(request, step.getSubject(), template.getFilePath(), propList);

			Props.mapper(propList);

			map.put("step", step);
			map.put("groupMap", groupMap);
			map.put("propList", propList);
			map.put("categoryList", categoryList);
			map.put("types", TypeMap.values());
			map.put("parameters", parameters);
			
			return "step_2";
		} catch (Exception e) {
			logger.error("(Step:2) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "2", method = RequestMethod.POST)
	public void _2(HttpServletRequest request, HttpServletResponse response,
			@RequestParam(required = false, value = "file") MultipartFile upload) {
		MapBean mb = new MapBean();
		
		try {
			CSRF.auth(request);
			
			String id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$:不是合法值", "流水号");
			
			Step step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
			Asserts.isNull(step, "流水号");
			
			String fileId = step.getFileId();
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^file|tag$");
			action = Values.get(action, "file");
			step.set_2(action);
			
			String now = new DateTime().toString("yyyyMMddHHmmss");
			Counter counter = new Counter();
			
			if (action.equals("file")) {
				step.setIncludeIds(null);
				step.setExcludeIds(null);
				step.setIncludes(null);
				step.setExcludes(null);
				step.setActiveCnd(null);
				step.setInCnd(null);
				
				if (!Asserts.empty(upload)) {
					String opt = Https.getStr(request, "opt", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^" + Options.INPUT + "|" + Options.EXPORT + "$");
					opt = Values.get(opt, Options.INPUT);
					
					Integer unsubscribeId = Https.getInt(request, "excludeIds", R.CLEAN, R.INTEGER);
					if (unsubscribeId != null && unsubscribeId != -1) {
						Tag unsubscribe = tagService.get(UserSession.getCorpId(), null, unsubscribeId);
						Asserts.isNull(unsubscribe, "排除退订");
					}
					
					String charset = Https.getStr(request, "charset", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^GB2312|UTF-8$");
					charset = Values.get(charset, "UTF-8");
					
					Files.valid(upload, new MapBean("file", Suffix.FILES));
					Uploads.maxSize(upload);
					
					Prop email = propService.get(UserSession.getCorpId(), null, PropMap.EMAIL.getId());
					Integer[] propIds = Recipients.propIds(request, upload, charset, email);

					if (opt.equals(Options.INPUT)) {
						Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
						Asserts.isNull(tagId, "标签");
						Tag tag = tagService.get(UserSession.getCorpId(), null, tagId);
						Asserts.isNull(tag, "标签");
		                if (StringUtils.equals(tag.getCategoryName(), CategoryMap.FORM.getName())) {
		                    throw new Errors("标签类别不能为表单收集类别");
		                }
						importService.execute(Options.INPUT, UserSession.getCorpId(), tag.getUserId(), PathMap.R, UserSession.getUserId() + "/IN." + now, 
								upload, charset, propIds, null, tagId, null);
					}
					
					String excludeIds = Values.get(unsubscribeId, Value.S);
					importService.execute(Options.EXPORT, UserSession.getCorpId(), UserSession.getUserId(), PathMap.R, UserSession.getUserId() + "/IN." + now, 
							upload, charset, propIds, Converts._toIntegers(excludeIds), null, counter);
					
					step.setFileName(upload.getOriginalFilename());
					step.setFileId("IN." + now);
					step.setIncludes("IN." + now);
					step.setEmailCount(counter.getEnd());
				}
				
				if (StringUtils.isBlank(step.getFileName())) {
					throw new Errors("文件不存在");
				}
			} else {
				step.setFileName(null);
				step.setIncludes(null);
				
				String includeIds = Https.getStr(request, "includeTagIds", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^[0-9,]+$:不是合法值", "包含标签");
				String excludeIds = Https.getStr(request, "excludeTagIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "排除标签");
				String includes = Https.getStr(request, "includes", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,512}", "包含标签");
	            String excludes = Https.getStr(request, "excludes", R.CLEAN, R.LENGTH, "{1,512}", "排除标签");
				String activeCnd = Https.getStr(request, "activeCnd", R.CLEAN, R.REQUIRED, "regex:^-1|0|1week|1month|2month|3month$");
				String inCnd = Https.getStr(request, "inCnd", R.CLEAN, R.REQUIRED, "regex:^or|and$");
				activeCnd = Values.get(activeCnd, "0");
				inCnd = Values.get(inCnd, "or");
				
				Integer[] inTagIds = Converts._toIntegers(includeIds);
				Asserts.isEmpty(inTagIds, "包含标签");
				Integer[] exTagIds = Converts._toIntegers(excludeIds);

				Integer[] inTagIds2 = Converts._toIntegers(step.getIncludeIds());
				Integer[] exTagIds2 = Converts._toIntegers(step.getExcludeIds());
				String activeCnd2 = step.getActiveCnd();
				String inCnd2 = step.getInCnd();
				
				boolean in = Asserts.equals(inTagIds, inTagIds2);
				boolean ex = Asserts.equals(exTagIds, exTagIds2);
				boolean ac = StringUtils.equals(activeCnd, activeCnd2);
				boolean ic = StringUtils.equals(inCnd, inCnd2);
				
				if (!in || !ex || !ac || !ic) {
					exportService.execute(Options.EXPORT, UserSession.getCorpId(), UserSession.getUserId(), PathMap.R, UserSession.getUserId() + "/OUT." + now,
							inTagIds, exTagIds, activeCnd, inCnd, null, null, counter);
					
					step.setIncludeIds(includeIds);
					step.setExcludeIds(excludeIds);
					step.setIncludes(includes);
					step.setExcludes(excludes);
					step.setActiveCnd(activeCnd);
					step.setInCnd(inCnd);
					
					step.setFileId("OUT." + now);
					step.setEmailCount(counter.getEnd() - counter.getStart());
				}
			}
			
			int emailCount = Values.get(step.getEmailCount(), 0);
			if (emailCount < 1) {
				throw new Errors("邮箱地址不能少于1");
			}
			
			cache.put(Fqn.R, UserSession.getUserId() + ":" + id, step); // 更新流水号

			if (StringUtils.isNotBlank(fileId) && !step.getFileId().equals(fileId)) {
				String filePath = PathMap.R.getPath() + UserSession.getUserId() + "/" + fileId;
				Files.delete(Webs.rootPath(), filePath, PathMap.R.getPath());
			}
			
			mb.put("emailCount", step.getEmailCount());
			Views.ok(mb, "选择收件人成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "选择收件人失败");
			logger.error("(Step:2) error: ", e);
		}
		
		ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
	}
	
	@RequestMapping(value = "3", method = RequestMethod.GET)
	public String _3(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/account/sender/array", token);
			
			String id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$:不是合法值", "流水号");
			
			Step step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
			Asserts.isNull(step, "流水号");
			
			Task task = new Task();
			Cnd cnd = new Cnd();
			Setting setting = new Setting();
			Plan plan = new Plan();
			
			Asserts.isNull(step.getTemplateId(), "模板");
			Template template = templateService.get(null, UserSession.getCorpId(), null, step.getTemplateId());
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
			
			task.setTaskId(step.getTaskId());
			task.setBindCorpId(step.getBindCorpId());
			task.setCampaignId(step.getCampaignId());
			task.setTemplateId(step.getTemplateId());
			task.setTaskName(step.getTaskName());
			task.setSubject(step.getSubject());
			task.setSenderName(step.getSenderName());
			task.setSenderEmail(step.getSenderEmail());
			task.setReceiver(step.getReceiver());
			task.setResourceId(step.getBindResourceId());
			task.setJobTime(StringUtils.isNotBlank(step.getJobTime()) ?  Calendars.parse(step.getJobTime(), Calendars.DATE_TIME).toDate() : null);
			setting.setAd(step.getAd());
			setting.setSendCode(step.getSendCode());
			setting.setSms(step.getSms());
			setting.setReplier(step.getReplier());
			setting.setRand(step.getRand());
			setting.setRobot(step.getRobot());
			setting.setUnsubscribeId(step.getUnsubscribeId());
			setting.setLabelIds(step.getLabelIds());
			
			plan.setPlanId(step.getPlanId());
			plan.setCron(step.getCron());
			plan.setBeginTime(StringUtils.isNotBlank(step.getBeginTime()) ? Calendars.parse(step.getBeginTime(), Calendars.DATE).toDate() : null);
			plan.setEndTime(StringUtils.isNotBlank(step.getEndTime()) ? Calendars.parse(step.getEndTime(), Calendars.DATE).toDate() : null);
			
			List<Campaign> campaignList = campaignService.find(UserSession.getCorpId());
			List<Prop> propList = propService.find(UserSession.getCorpId());
			List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TAG, CategoryMap.FORM.getName(), Cnds.NOT_EQ);
			List<Tag> tagList = tagService.find(UserSession.getCorpId());
			Map<String, List<Tag>> groupMap = Categorys.groupMap(categoryList, tagList);
			Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.UNSUBSCRIBE.getName(), Category.TAG);
			Corp corp = corpService.get(UserSession.getCorpId());
			List<Sender> senderList = senderService.find(UserSession.getCorpId(), Status.ENABLED);
			Sender sender = Senders.getRequired(senderList);

			List<Robot> robotList = robotService.find(UserSession.getCorpId());

			Senders.copy(sender, task);

			map.put("step", step);
			map.put("task", task);
			
			map.put("category", category);
            map.put("campaignList", campaignList);
			map.put("propList", propList);
			map.put("groupMap", groupMap);
			map.put("cnd", cnd);
			map.put("setting", setting);
			map.put("robotList", robotList);
			map.put("formalId", corp.getFormalId());
			map.put("senderList", senderList);
			map.put("plan", plan);
			map.put("openSms", UserSession.openSms());
			
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
			if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				List<Corp> bindCorpList = corpService.sees();
				List<Resource> resourceList = resourceService.find();
				map.put("bindCorpList", bindCorpList);
				map.put("resourceList", resourceList);
				map.put("labelMap", Categorys.groupMap(categoryService.labelFind(), labelService.find()));
			}
			
			// 从cookie读数据，设置记忆功能
			Tasks.getCookies(request, map, "step", sender!=null);
			
			return "step_3";
		} catch (Exception e) {
			logger.error("(Step:3) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(value = "3", method = RequestMethod.POST)
	public void _3(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String t = null;
		
		try {
			CSRF.auth(request);
			
			String id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,20}", "regex:^[0-9]+$:不是合法值", "流水号");
			
			Step step = (Step) cache.get(Fqn.R, UserSession.getUserId() + ":" + id);
			Asserts.isNull(step, "流水号");
			
			t = Values.get(Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^add|drafted|2$"), "add"); // add:正式发送, drafted:保存草稿, 2:选择收件人
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			
			boolean next = Asserts.hasAny(t, new String[] { "add", "drafted" });
			
			Integer REQUIRED = R.REQUIRED;
			if (!next) {
				REQUIRED = R.NONE;
			}
			
			String taskName = Https.getStr(request, "taskName", R.CLEAN, REQUIRED, R.LENGTH, "{1,20}", "任务名称");
			String orgTaskName = Https.getStr(request, "orgTaskName", R.CLEAN, R.LENGTH, "{1,60}");
			
			// 邮件设置
			Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
			String campaignName = null;
            if (campaignId != null) {
                Campaign campaign = campaignService.get(UserSession.getCorpId(), null, campaignId);
                Asserts.isNull(campaign, "活动");                
                campaignName = campaign.getCampaignName();
            }
            
			Integer templateId = step.getTemplateId();
			String templateName = null;
			String subject = null;

            MapBean attrs = new MapBean();
            attrs.put("touch", "false"); // 初始化
            
			if (next) {
				Asserts.isNull(templateId, "模板");
				Template template = templateService.get(null, UserSession.getCorpId(), null, templateId);
				Asserts.isNull(template, "模板");
				Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
				Templates.attrs(template, attrs);
				templateName = template.getTemplateName();
				subject = (String) Validator.validate(step.getSubject(), R.CLEAN, REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
			}
			
			String ad = Values.get(Https.getStr(request, "ad", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
            
			// 发件人设置
			Integer bindCorpId = null;
			String bindResourceId = null;
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
            if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				bindCorpId = Https.getInt(request, "bindCorpId", R.CLEAN, REQUIRED, R.INTEGER);
				bindResourceId = Https.getStr(request, "resourceId", R.CLEAN, REQUIRED, R.LENGTH, "{1,20}");
				
				if (next) {
					Asserts.isNull(bindCorpId, "所属企业");
					Asserts.isNull(bindResourceId, "投递通道");
					Corp bindCorp = corpService.get(bindCorpId);
					Asserts.isNull(bindCorp, "所属企业");
					Resource resource = resourceService.get(bindResourceId);
					Asserts.isNull(resource, "投递通道");
				}
			}
			String senderEmail = Https.getStr(request, "senderEmail", R.CLEAN, REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "发件人邮箱");
			String senderName = Https.getStr(request, "senderName", R.CLEAN, R.LENGTH, "{1,20}", "发件人昵称");
			String robot = Https.getStr(request, "robot", R.CLEAN, R.LENGTH, "{1,512}", "网络发件人");
			if (StringUtils.isNotBlank(robot)) {
                if (robot.equals("-1")) {
                    robot = null;
                } else {
                    for (String email : StringUtils.splitPreserveAllTokens(robot, ",")) {
                        Validator.validate(email, R.REQUIRED, R.MAIL, "网络发件人");
                    }
                }
            }
			
			String rand = Values.get(Https.getStr(request, "rand", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
			String replier = Https.getStr(request, "replier", R.CLEAN, R.LENGTH, R.MAIL, "{1,64}", "回复邮箱");
			
			// 收件人设置
			String fileId = (String) Validator.validate(step.getFileId(), R.CLEAN, R.REQUIRED, R.REGEX, "regex:^(IN|OUT)[.\\d]+$:不是合法值", "收件人");
			String receiver = Https.getStr(request, "receiver", R.CLEAN, R.LENGTH, "{1,20}", "收件人昵称");
			Integer unsubscribeId = Https.getInt(request, "unsubscribeId", R.CLEAN, R.INTEGER, "退订标签");
			String unsubscribe = Https.getStr(request, "unsubscribe", R.CLEAN, R.LENGTH, "{1,60}", "退订设置");
			if (next && unsubscribeId != null) {
				Asserts.isNull(tagService.get(UserSession.getCorpId(), null, unsubscribeId), "退订标签");
			}
			
			DateTime now = !Auth.isAuth() ? new DateTime() : Ntps.get();
			
			// 发送设置
            String sendCode = Https.getStr(request, "sendCode", R.CLEAN, R.REGEX, "regex:^current|schedule|plan$:不是合法值", "发送方式");
			sendCode = Values.get(sendCode, "current");
			String jobTime = Value.S;
			Plan plan = null;
			
			Integer planId = null;
			String cron = null;
            String beginTime = null;
            String endTime = null;

            boolean isPlan = false;
            
			if (StringUtils.equals(sendCode, "schedule")) {
				jobTime = Https.getStr(request, "jobTime", REQUIRED, R.LENGTH, R.DATETIME, "{1,20}", "发送时间");
			} else if(StringUtils.equals(sendCode, "plan")) {
                planId = Https.getInt(request, "planId", R.CLEAN, R.INTEGER, "周期设置");
                beginTime = Https.getStr(request, "beginTime", R.CLEAN, REQUIRED, R.DATE, "开始日期");
                endTime = Https.getStr(request, "endTime", R.CLEAN, REQUIRED, R.DATE, "结束日期");
                
                cron = Plans.cron(request, REQUIRED);
                
                if (REQUIRED.equals(R.REQUIRED)) {
                    if (beginTime.compareTo(endTime) > -1) throw new Errors("有效期限开始日期必须小于结束日期");
                    if (beginTime.compareTo(now.toString(Calendars.DATE)) < 1) throw new Errors("有效期限开始日期必须大于当前日期");
                }
                isPlan = true;
            }
			
			String sms = Values.get(Https.getStr(request, "sms", R.CLEAN, R.REGEX, "regex:^0|19|11|14|15|13$:不是合法值", "短信通知"), "0");
			
			// 属性库设置
			String labelIds = null;
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
            if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				labelIds = Https.getStr(request, "labelIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "关联属性");
			}
			
			step.setTaskId(taskId);
			step.setBindCorpId(bindCorpId);
			step.setBindResourceId(bindResourceId);
			step.setTemplateId(templateId);
			step.setTaskName(taskName);
			step.setSenderName(senderName);
			step.setSenderEmail(senderEmail);
			step.setReceiver(receiver);
			step.setJobTime(jobTime);
			step.setCampaignId(campaignId);
			step.setAd(ad);
			step.setSendCode(sendCode);
			step.setSms(sms);
			step.setReplier(replier);
			step.setRand(rand);
			step.setRobot(Values.get(robot, null));
			step.setUnsubscribeId(unsubscribeId);
			step.setLabelIds(labelIds);
			step.setPlanId(planId);
			step.setCron(cron);
			step.setBeginTime(beginTime);
			step.setEndTime(endTime);
			
			if (!next) {
				cache.put(Fqn.R, UserSession.getUserId() + ":" + id, step); // 更新流水号
				Views.ok(mb, "上一步成功");
				ObjectMappers.renderJson(response, mb);
				return;
			}
			
			Asserts.notUnique(taskService.unique(UserSession.getCorpId(), UserSession.getUserId(), taskName, orgTaskName), "任务");

			int emailCount = Values.get(step.getEmailCount(), Value.I);
			if (emailCount < 1) {
				throw new Errors("收件人数不能少于1");
			}
			
			Task task = null;
			
			if (taskId != null) {
				task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
				Asserts.isNull(task, "任务");

				if (!Asserts.hasAny(task.getStatus(), new Integer[] { Status.DRAFTED, Status.UNAPPROVED, Status.RETURN, Status.WAITED })) {
					throw new Errors("禁止修改已锁定的任务");
				}

				Setting setting = settingService.get(task.getTaskId());
				Asserts.isNull(setting, "发送设置");
				
				Tasks.lockSchedule(task, setting, now, task.getJobTime());
			} else {
				task = new Task();
				task.setCorpId(UserSession.getCorpId());
				task.setUserId(UserSession.getUserId());
			}

			Corp corp = corpService.get(UserSession.getCorpId());
			User user = userService.get(UserSession.getUserId());
			
			DateTime balanceTime = null;
			if (StringUtils.isNotBlank(jobTime)) {
				DateTimeFormatter fmt = DateTimeFormat.forPattern("yyyy-MM-dd HH:mm");
				balanceTime = fmt.parseDateTime(jobTime);
				Tasks.jobTime(now, balanceTime);
			} else {
				if (task.getCreateTime() != null) {
					balanceTime = new DateTime(task.getCreateTime());
				} else {
					balanceTime = now;
				}
			}
			
			String auditPath = null;
			if (Securitys.hasAnyRole(new String[] { RoleMap.MGR.getName(), RoleMap.ALL.getName() })) {
				auditPath = corp.getAuditPath();
			}
			auditPath = Audits.remove(auditPath, UserSession.getCorpId());
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BUK.getName(), RoleMap.BSN.getName(), RoleMap.ALL.getName() })
//					&& Values.get(user.getModerate(), Value.I).equals(Value.T)) {
			if (Securitys.hasAnyRole(new String[] { RoleMap.BUK.getName(), RoleMap.ALL.getName() })
                    && Values.get(user.getModerate(), Value.I).equals(Value.T)) {
				auditPath = Audits.add(auditPath, UserSession.getCorpId());
			}
			
			String resourceId = corp.getTestedId();
			if (emailCount > Property.getInt(Config.PIPE_SIZE)) {
				resourceId = corp.getFormalId();
			} else {
				auditPath = null;
			}
			
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
            if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				resourceId = bindResourceId;
			}
			
			int holdCount = Tasks.holdCount(task.getStatus(), task.getEmailCount());
			if (t.equals("add")) {
			    List<Corp> corpList = corpService.find();
                Corp currentCorp = corpService.get(corp.getCorpId());
                Corp root = Corps.getRoot(corpList, currentCorp);
                List<Integer> corpIdList = Lists.newArrayList();
                corpService.cross(corpList, root.getCorpId(), corpIdList);
                corpIdList.add(root.getCorpId()); //算发送量的时候需要加上顶级父机构的发送量
                Integer[] corpIds = Converts._toIntegers(corpIdList);
                
				cosService.balance(corpIds, corp.getCosId(), holdCount, emailCount, balanceTime);
			}
			
			Cnd cnd = new Cnd();
			Setting setting = new Setting();
			
			cnd.setFileId(Values.get(fileId, null));
            cnd.setIncludeIds(Values.get(step.getIncludeIds(), null));
            cnd.setExcludeIds(Values.get(step.getExcludeIds(), null));
			cnd.setIncludes(Values.get(step.getIncludes(), null));
			cnd.setExcludes(Values.get(step.getExcludes(), null));
            
			setting.setTemplate(templateName);
			setting.setCampaign(campaignName);
			setting.setAd(ad);
			setting.setSendCode(sendCode);
			setting.setSms(sms);
			setting.setReplier(Values.get(replier, null));
			setting.setRand(rand);
			setting.setRobot(Values.get(robot, null));
			setting.setTagIds(Values.get(step.getIncludeIds(), null));
			setting.setUnsubscribeId(unsubscribeId);
			setting.setUnsubscribe(Values.get(unsubscribe, null));
			setting.setLabelIds(Values.get(labelIds, null));
			
			task.setBindCorpId(bindCorpId);
			task.setCampaignId(campaignId);
			task.setTemplateId(templateId);
			task.setTaskName(taskName);
			task.setSubject(subject);
			task.setSenderName(senderName);
			task.setSenderEmail(senderEmail);
			task.setReceiver(receiver);
			task.setEmailCount(emailCount);
			task.setAuditPath(auditPath);
			task.setModeratePath(null);
			task.setResourceId(resourceId);
			task.setType(sendCode.equals("plan") ? Type.PLAN : Type.FORMAL);
			task.setJobTime(StringUtils.isNotBlank(jobTime) ? balanceTime.toDate() : null);
			
			if (isPlan) {
			    if (planId != null)
	                plan = planService.get(null, UserSession.getCorpId(), planId);
	            if (plan == null)
	                plan = new Plan();
	            plan.setCorpId(UserSession.getCorpId());
	            plan.setUserId(UserSession.getUserId());
	            plan.setCron(cron);
	            plan.setBeginTime(Calendars.parse(beginTime, Calendars.DATE).toDate());
	            plan.setEndTime(Calendars.parse(endTime, Calendars.DATE).toDate());
	            planService.save(plan);
                task.setPlanId(plan.getPlanId());
                task.setPlanReferer(Value.NONE);
            }
			
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
            if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				task.setLabelStatus(Status.ENABLED);
			}
			task.setStatus(t.equals("add") ? Audits.moderate(setting.getSendCode(), auditPath) : Status.DRAFTED);
           
			// 判断是否生成周期任务的基础文件
			if(StringUtils.equals(sendCode, "plan") && plan!=null && task.getType()==2){
				if(Tasks.createPlan(task)) Files.createPlan(plan.getPlanId());
			}
						
			if (!sendCode.equals("plan") && attrs.getString("touch").equals("true")) {
                taskService.saveAndPlusSentWithTouch(task, cnd, setting, holdCount, emailCount);
                if(Tasks.createTask(task)) Files.createTask(task.getTaskId());
            } else {
                taskService.saveAndPlusSent(task, cnd, setting, holdCount, emailCount);
                if(Tasks.createTask(task)) Files.createTask(task.getTaskId());
            }
			
			mb.put("taskId", task.getTaskId());
			mb.put("taskName", task.getTaskName());
			if (plan != null) {
                mb.put("planId", plan.getPlanId());
            } else {
                mb.put("planId", "");
            }
			
			if (t.equals("add")) {
				cache.remove(Fqn.R, UserSession.getUserId() + ":" + id); // 删除流水号
			} else {
				step.setTaskId(task.getTaskId());
				cache.put(Fqn.R, UserSession.getUserId() + ":" + id, step); // 更新流水号	
			}
			
			// 将一些值保存到cookie中
			Cookies.add(response, "taskName", Values.get(taskName, ""));
            Cookies.add(response, "campaignId", Values.get(campaignId, ""));
//            Cookies.add(response, "templateId", Values.get("", ""));
            Cookies.add(response, "subject", Values.get(subject, ""));
			Cookies.add(response, "bindCorpId", Values.get(bindCorpId, ""));
			Cookies.add(response, "resourceId", Values.get(bindResourceId, ""));
			Cookies.add(response, "senderEmail", Values.get(senderEmail, ""));
			Cookies.add(response, "senderName", Values.get(senderName, ""));
			Cookies.add(response, "receiver", Values.get(receiver, ""));
			Cookies.add(response, "unsubscribeId", Values.get(unsubscribeId, ""));
			
			Views.ok(mb, t.equals("add") ? "投递设置成功" : "自动保存草稿成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			String error = "投递设置失败";
			if (t.equals("drafted")) {
				error = "自动保存草稿失败";
			} else if (t.equals("2")) {
				error = "上一步失败";
			}
			Views.error(mb, error);
			logger.error("(Step:3) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	private String parameters(HttpServletRequest request, String subject, String filePath, List<Prop> propList) {
		String content = Values.get(Files.get(Webs.rootPath(), filePath));
		
		Map<Integer, String> propMap = Props.parameters(subject, content, propList);
		
		StringBuffer sbff = new StringBuffer();
		int i = 0;
		for (Entry<Integer, String> entry : propMap.entrySet()) {
			i++;
			String prop = entry.getValue();
			sbff.append(Values.get(PropMap.getName(prop), prop));
			if (i < propMap.size()) {
				sbff.append(",");
			}
		}

		return sbff.toString();
	}
}
