package cn.edm.web.action.mailing;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.multipart.commons.CommonsMultipartFile;

import cn.edm.app.auth.Auth;
import cn.edm.app.security.Securitys;
import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Fqn;
import cn.edm.consts.HttpCode;
import cn.edm.consts.Options;
import cn.edm.consts.Permissions;
import cn.edm.consts.Quartz;
import cn.edm.consts.Queue;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.RoleMap;
import cn.edm.model.Campaign;
import cn.edm.model.Category;
import cn.edm.model.Cnd;
import cn.edm.model.Corp;
import cn.edm.model.Cos;
import cn.edm.model.Domain;
import cn.edm.model.Plan;
import cn.edm.model.Prop;
import cn.edm.model.Resource;
import cn.edm.model.Robot;
import cn.edm.model.Selection;
import cn.edm.model.Sender;
import cn.edm.model.Setting;
import cn.edm.model.Tag;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.DateUtil;
import cn.edm.utils.Ntps;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Lines;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Cookies;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.UserSession;
import cn.edm.web.View;
import cn.edm.web.action.Action;
import cn.edm.web.api.Processings;
import cn.edm.web.facade.Audits;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Plans;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Senders;
import cn.edm.web.facade.Tasks;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("MailingTask")
@RequestMapping("/mailing/task/**")
public class TaskAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(TaskAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Task> page = new Page<Task>();
			Pages.page(request, page);

			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^task|campaign$");
			action = Values.get(action, "task");
			
			String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^proofing|waited|processing|completed$");
			m = Values.get(m, "all");
			
			Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
			String taskName = Https.getStr(request, "taskName", R.CLEAN, R.LENGTH, "{1,20}");
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			Integer[] statuses = Status.task(m);
			String nameCnd = Https.getStr(request, "nameCnd", R.CLEAN, R.REGEX, "regex:^" + Cnds.EQ + "|" + Cnds.LIKE + "$");
			nameCnd = Values.get(nameCnd, Cnds.LIKE);
			Integer type = Https.getInt(request, "type", R.CLEAN, R.REGEX, "regex:^0|2|4$");
            
			Integer campaignId = null;
            Campaign campaign = null;
            if (action.equals("campaign")) {
                campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
                Asserts.isNull(campaignId, "活动");
                campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
                Asserts.isNull(campaign, "活动");
            }

            MapBean mb = new MapBean();
            corpId = UserSession.getCorpId(mb, corpId);
            
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
			page = taskService.search(page, mb, corpIds, corpId,
			        campaignId, taskName, beginTime, endTime, statuses, nameCnd, type);
			
			if (m.equals("processing")) {
				domain(corpIds, corpId, page.getResult());
			}
            if (action.equals("campaign")) {
                if (mb.containsKey("statuses")) {
                    page.setSearch(true);
                }
            }
            
            map.put("campaign", campaign);
            map.put("page", page);
			map.put("mb", mb);
            map.put("action", action);
            map.put("m", m);
            map.put("corp", corp);
            map.put("corpChildren", corpChildren);
            
			return "default/mailing/task_page";
		} catch (Exception e) {
			logger.error("(Task:page) error: ", e);
			return Views._404();
		}
	}
	
	private void domain(Integer[] corpIds, Integer corpId, List<Task> taskList) {
		StringBuffer sbff = new StringBuffer();
		for (Task task : taskList) {
			sbff.append(task.getTaskId()).append(",");
		}
		
		List<Domain> domainList = domainService.group(corpIds, corpId, UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
		Map<Integer, Domain> domainMap = Maps.newHashMap();
		for (Domain domain : domainList) {
			domainMap.put(domain.getTaskId(), domain);
		}
		
		for (Task task : taskList) {
			Domain domain = domainMap.get(task.getTaskId());
			if (domain != null) {
				task.setSentCount(Values.get(domain.getSentCount()));
			}
		}
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/account/sender/array", token);
            CSRF.generate(request, "/mailing/task/selection", token);
            CSRF.generate(request, "/mailing/task/refresh", token);
			
            String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^task|campaign$");
            action = Values.get(action, "task");
            
			String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^proofing|waited|processing|completed$");
			m = Values.get(m, "all");
			
			String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^copy|add$");
			t = Values.get(t, "add");

			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.INTEGER);
			Integer campaignId = null;
			if (action.equals("campaign")) {
                campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            }
			
			Task task = null;
			Cnd cnd = null;
			Setting setting = null;
            
			if (taskId != null) {
				/*task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);*/
				task = taskService.get(UserSession.getCorpId(),null, taskId);
				cnd = cndService.get(taskId);
				setting = settingService.get(taskId);
			}
			task = (task == null ? new Task() : task);
			cnd = (cnd == null ? new Cnd() : cnd);
			setting = (setting == null ? new Setting() : setting);
			
			if (taskId == null) {
                if (templateId != null) {
    				Template template = templateService.get(null, null, UserSession.getUserId(), templateId);
    				Asserts.isNull(template, "模板");
    				task.setTemplateId(template.getTemplateId());
    				task.setSubject(template.getSubject());
			    }
				task.setCampaignId(campaignId);
			}
			
			Category touchCategory = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TOUCH.getName(), Category.TEMPLATE);
            Asserts.isNull(touchCategory, "模板类别");

			List<Campaign> campaignList = campaignService.find(UserSession.getCorpId());
			List<Template> templateList = templateService.find(UserSession.getCorpId(), touchCategory.getCategoryId(), Cnds.NOT_EQ);
			List<Prop> propList = propService.find(UserSession.getCorpId());
			
			List<Robot> robotList = robotService.find(UserSession.getCorpId());

			Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.UNSUBSCRIBE.getName(), Category.TAG);
			Corp corp = corpService.get(UserSession.getCorpId());
			
			List<Sender> senderList = senderService.find(UserSession.getCorpId(), Status.ENABLED);
			Sender sender = Senders.getRequired(senderList);

			List<Selection> selectionList = selectionService.find(UserSession.getCorpId(), UserSession.getUserId());
			
			if (t.equals("copy")) {
				Task copy = new Task();
				BeanUtils.copyProperties(task, copy, new String[] { "taskId", "planId", "taskName" });
				Senders.copy(sender, copy);
				map.put("task", copy);
			} else {
			    Senders.copy(sender, task);
				map.put("task", task);
			}
			
			Plan plan = null;
            if (task.getPlanId() != null) {
                plan = planService.get(null, UserSession.getCorpId(), task.getPlanId());
            } else {
                plan = new Plan();
            }
            
            map.put("category", category);
            map.put("campaignList", campaignList);
            map.put("templateList", templateList);
			map.put("propList", propList);
			map.put("cnd", cnd);
			map.put("setting", setting);
			map.put("robotList", robotList);
			map.put("formalId", corp.getFormalId());
			map.put("senderList", senderList);
			map.put("selectionList", selectionList);
			map.put("plan", plan);
			map.put("openSms", UserSession.openSms());
			
			if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				List<Corp> bindCorpList = corpService.sees();
				List<Resource> resourceList = resourceService.find();
				map.put("bindCorpList", bindCorpList);
				map.put("resourceList", resourceList);
				map.put("labelMap", Categorys.groupMap(categoryService.labelFind(), labelService.find()));
			}

            map.put("action", action);
            map.put("m", m);
            
            // 从cookie读数据，设置记忆功能
            if(taskId == null){
            	Tasks.getCookies(request, map, "task", sender!=null);
            }
            
			return "default/mailing/task_add";
		} catch (Exception e) {
			logger.error("(Task:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String t = null;
		Integer taskId = null;
		
		try {
			CSRF.validate(request);
			
			int corpId = UserSession.getCorpId();
	        Corp testCorp = corpService.get(corpId);
	        Cos  testCos  = cosService.get(testCorp.getCosId());
	        Integer type = testCos.getType();
	        String  status = testCos.getStatus();
	        //测试账号 如有提醒选项 提前七天登录弹框提示
	        if(status.equals("0")){
	        	logger.info("[ 试用套餐 ] : 账号("+UserSession.getUserId()+") 试用套餐已到期或已被冻结 - 请联系相关人员咨询"); 
	        	mb.put("test","OUTDATE");
	        	Views.error(mb,"账号 ("+UserSession.getUserId()+") 试用套餐已到期或已被冻结 - 请联系相关人员咨询!");
	        }else if(type == 0){  
	        	int deadLine = DateUtil.compareMills(testCos.getEndTime(),new Date());
	        	int effect = DateUtil.compareMills(testCos.getStartTime(),new Date());
	        	if(effect > 0){
	            	logger.info("[ 试用套餐 ] : 账号("+UserSession.getUserId()+") 试用套餐还未到生效时间 - 请耐心等待");
/*	            	if(testCos.getRemind() != null && testCos.getRemind() != "" && testCos.getRemind().equals("1")){ 
	            		  mb.put("test","NOTEFFECT");
	        		} */  
	            	Views.error(mb,"账号 ("+UserSession.getUserId()+") 试用套餐还未到生效时间 - 请耐心等待!");
	        	}else if(deadLine <=0){
	            	logger.info("[ 试用套餐 ] : "+UserSession.getUserId()+"账号试用套餐已经到期 - 请联系相关人员续费套餐!");
/*	            	if(testCos.getRemind() != null && testCos.getRemind() != "" && testCos.getRemind().equals("1")){ 
	            		  mb.put("test","OUTDATE");
	        		}   */
	            	Views.error(mb,"账号 ("+UserSession.getUserId()+") 试用套餐已经到期 - 请联系相关人员续费套餐!");
	        	}else{
	        		createTask(mb,t,taskId,request,response);	
	        	}
        	}else{
        		createTask(mb,t,taskId,request,response);
        	}
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, t.equals("add") ? ((taskId != null ? "修改" : "新建") + "任务失败") : "自动保存草稿失败");
			logger.error("(Task:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.POST)
    public void refresh(HttpServletRequest request, HttpServletResponse response) {
	    MapBean mb = new MapBean();
        try {
            CSRF.validate(request);
            
    	    List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TAG, null, null);
            List<Tag> tagList = tagService.find(UserSession.getCorpId());
    	    List<Selection> selectionList = selectionService.find(UserSession.getCorpId(), UserSession.getUserId());
    	    
            Map<String, List<MapBean>> categoryMap = new LinkedHashMap<String, List<MapBean>>();
            Map<String, List<MapBean>>  fileMap = new LinkedHashMap<String, List<MapBean>>();
            Map<String, MapBean>  selectionMap = new LinkedHashMap<String, MapBean>();
    	    
    	    XSS xss = new XSS();
    	    View view = new View();
    	    
    	    for (Category category : categoryList) {
                List<MapBean> list = Lists.newLinkedList();
                for (Tag tag : tagList) {
                    if (tag.getCategoryId().equals(category.getCategoryId())) {
                    	if (category.getCategoryName().equals(CategoryMap.FILE.getName())) {
                    		list.add(new MapBean(
                    				"id", tag.getTagDesc(),
                    				"title", xss.filter(tag.getTagDesc()) + "_" + xss.filter(tag.getTagName()) + "（" + tag.getEmailCount() + "）",
                    				"count", tag.getEmailCount(),
                    				"type", "file"
                    		));
                    	}else{
	                        list.add(new MapBean(
	                            "id", tag.getTagId(),
	                            "title", xss.filter(tag.getTagName()) + "（" + tag.getEmailCount() + "）",
	                            "count", tag.getEmailCount()
	                        ));
                    	}
                    }
                }
                if (category.getCategoryName().equals(CategoryMap.UNSUBSCRIBE.getName())) {
                    list.add(new MapBean("id", -1, "title", "全部退订", "count", 0));
                }
                categoryMap.put(category.getCategoryName(), list);
            }
    	    
    	    List<MapBean> list = Lists.newLinkedList();
            for (Selection s : selectionList) {
                list.add(new MapBean(
                    "id", s.getFileId(),
                    "title", s.getFileId(),
                    "type", "file"
                ));
                selectionMap.put(
                    s.getFileId(), new MapBean(
                        "includes", view.value(s.getIncludes(), "无"),
                        "activeCnd", view.active_cnd(s.getActiveCnd()),
                        "inCnd", view.in_cnd(s.getInCnd()),
                        "excludes", view.value(s.getExcludes(), "无"),
                        "filter", view.value(s.getFilter(), "无"),
                        "emailCount", s.getEmailCount(),
                        "repeat", "已过滤去重"
                    )
                );
            }
            fileMap.put("筛选文件", list);
            
            mb.put("categoryMap", categoryMap, "fileMap", fileMap, "selectionMap", selectionMap);
            
    	    Views.ok(mb, "刷新成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
            logger.error("(Task:refresh) error: ", e);
        } catch (Exception e) {
            Views.error(mb, "刷新失败");
            logger.error("(Task:refresh) error: ", e);
        }
        ObjectMappers.renderJson(response, mb);
	}
	
    @RequestMapping(method = RequestMethod.POST)
    public void selection(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);
            
            String includeIds = Https.getStr(request, "includeIds", R.CLEAN, R.REQUIRED, "包含标签");
            String excludeIds = Https.getStr(request, "excludeIds", R.CLEAN, R.REGEX, "regex:^[0-9,-]+$:不是合法值", "排除标签");
            
            String now = new DateTime().toString("yyyyMMddHHmmss");
            Counter counter = new Counter();
            
            if (StringUtils.startsWith(includeIds, "EXPORT.") || StringUtils.startsWith(includeIds, "EXPORT_")) {
            	String fileId = (String) Validator.validate(includeIds, R.LENGTH, "{1,30}", "文件");
            	String path = null;
            	String fileIdPre = null;
            	Prop email = propService.get(UserSession.getCorpId(), null, PropMap.EMAIL.getId());
            	Integer[] propIds = { email.getPropId() };
            	if(StringUtils.startsWith(includeIds, "EXPORT_")){
            		path = PathMap.R.getPath() + "export/" + UserSession.getUserId() + "/" + fileId + "/map.txt";
            		fileIdPre = "EXPORT_";
            		propIds = getPropIds(Webs.rootPath() + path);
            	}else{
            		path = PathMap.EXECUTE.getPath() + "export/" + UserSession.getUserId() + "/" + fileId + "/reduce.txt";
            		fileIdPre = "EXPORT.";
                }
                Asserts.notExists(Webs.rootPath(), path, "临时");
                MultipartFile upload = new CommonsMultipartFile(Files.create(Webs.rootPath() + path, "reduce.txt"));
                importService.execute(Options.EXPORT, UserSession.getCorpId(), UserSession.getUserId(), PathMap.R, UserSession.getUserId() + "/" + fileIdPre + now, 
                        upload, "UTF-8", propIds, null, null, counter);
                mb.put("fileId", fileIdPre + now);
                mb.put("count", counter.getEnd());
            } else {
                includeIds = (String) Validator.validate(includeIds, R.REGEX, "regex:^[0-9,]+$:不是合法值", "包含标签");
                Integer[] inTagIds = Converts._toIntegers(includeIds);
                Asserts.isEmpty(inTagIds, "包含标签");
                Asserts.isEmpty(tagService.find(UserSession.getCorpId(), null, inTagIds), "包含标签");
                Integer[] exTagIds = Converts._toIntegers(excludeIds);
                
                if (Asserts.equals(inTagIds, exTagIds)) throw new Errors("包含标签不能与排除标签一致");
                exportService.execute(Options.EXPORT, UserSession.getCorpId(), UserSession.getUserId(), PathMap.R, UserSession.getUserId() + "/" + "EXPORT." + now,
                        inTagIds, exTagIds, "0", "or", null, null, counter);
                mb.put("fileId", "EXPORT." + now);
                mb.put("count", counter.getEnd());
            }
            
            Views.ok(mb, "筛选收件人成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
            logger.error("(Task:selection) error: ", e);
        } catch (Exception e) {
            Views.error(mb, "筛选收件人失败");
            logger.error("(Task:selection) error: ", e);
        }
        ObjectMappers.renderJson(response, mb);
    }
    
	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);

			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			Integer[] taskIds = Converts._toIntegers(checkedIds);
			
			if (!Asserts.empty(taskIds) && taskIds.length == 1) {
				Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskIds[0]);
				if (task != null) {
					map.put("checkedName", task.getTaskName());
				}
			}

			Views.map(map, "del", "", "删除", "任务", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Task:del) error: ", e);
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
			Integer[] taskIds = Converts._toIntegers(checkedIds);
			
			if (!Asserts.empty(taskIds) && taskIds.length == 1) {
				Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskIds[0]);
				Asserts.isNull(task, "任务");

				if (!Asserts.hasAny(task.getStatus(), new Integer[] { Status.DRAFTED, Status.UNAPPROVED, Status.RETURN })) {
					throw new Errors("禁止删除非待处理的任务");
				}
				
				task.setStatus(Status.DELETED);
				taskService.save(task);
			}

			Views.ok(mb, "删除任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除任务失败");
			logger.error("(Task:del) error: ", e);
		}
		
		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
    public String clear(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);

            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            Integer[] taskIds = Converts._toIntegers(checkedIds);
            
            if (!Asserts.empty(taskIds) && taskIds.length == 1) {
                Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskIds[0]);
                if (task != null) {
                    map.put("checkedName", task.getTaskName());
                }
            }

            Views.map(map, "clear", "", "移除", "任务", "checkedIds", checkedIds);
            return "default/common_form";
        } catch (Exception e) {
            logger.error("(Task:clear) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(method = RequestMethod.POST)
    public void clear(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        String checkedIds = null;
        
        try {
            CSRF.validate(request);
            
            checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            Integer[] taskIds = Converts._toIntegers(checkedIds);
            
            if (!Asserts.empty(taskIds) && taskIds.length == 1) {
                Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskIds[0]);
                Asserts.isNull(task, "任务");
                task.setCampaignId(null);
                taskService.save(task);
            }

            Views.ok(mb, "移除任务成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "移除任务失败");
            logger.error("(Task:clear) error: ", e);
        }
        
        mb.put("checkedIds", checkedIds);
        ObjectMappers.renderJson(response, mb);
    }
	
	@RequestMapping(method = RequestMethod.GET)
	public String test(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^task|step|audit$");
			action = Values.get(action, "task");
			
			if (action.equals("audit")) {
				Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
				Asserts.isNull(taskId, "任务");
				Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), taskId);
				Asserts.isNull(task, "任务");
				Setting setting = settingService.get(taskId);
				Asserts.isNull(setting, "发送设置");
				
				map.put("templateId", task.getTemplateId());
                map.put("taskName", task.getTaskName());
                map.put("subject", task.getSubject());
				map.put("bindCorpId", task.getBindCorpId());
				map.put("senderEmail", task.getSenderEmail());
				map.put("senderName", task.getSenderName());
				map.put("receiver", task.getReceiver());
				map.put("setting", setting);
			} else {
				map.put("setting", new Setting());
			}

			map.put("action", action);
			
			return "default/mailing/task_test";
		} catch (Exception e) {
			logger.error("(Task:test) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void test(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^task|audit$");
			action = Values.get(action, "task");

			String taskName = Https.getStr(request, "taskName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "任务名称");
			Integer tagId = Https.getInt(request, "includeIds", R.CLEAN, R.INTEGER);
			String includes = Https.getStr(request, "includes", R.CLEAN, R.LENGTH, "{1,512}", "包含标签");
			 
			Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(templateId, "模板");
			Corp corp = corpService.get(UserSession.getCorpId());
			Template template = templateService.get(null, UserSession.getCorpId(), null, templateId);
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
			String subject = Https.getStr(request, "subject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
			String ad = Values.get(Https.getStr(request, "ad", R.CLEAN, R.REGEX, "regex:^true|false$", "AD设置"), "false");
			
			Integer bindCorpId = null;
//			if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
            if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
				bindCorpId = Https.getInt(request, "bindCorpId", R.CLEAN, R.REQUIRED, R.INTEGER);
				Asserts.isNull(bindCorpId, "所属企业");
				Corp bindCorp = corpService.get(bindCorpId);
				Asserts.isNull(bindCorp, "所属企业");
			}
			String senderEmail = Https.getStr(request, "senderEmail", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "发件人邮箱");
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
			String replier = Https.getStr(request, "replier", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}", "回复邮箱");
			String receiver = Https.getStr(request, "receiver", R.CLEAN, R.LENGTH, "{1,20}", "收件人昵称");
			String sms = Values.get(Https.getStr(request, "sms", R.CLEAN, R.REGEX, "regex:^0|19|11|14|15|13$:不是合法值", "短信通知"), "0");
			String email_list = Https.getStr(request, "email_list", R.CLEAN, R.LENGTH, "{1, 6400}", "邮箱地址");
			
			MapBean emailMap = new MapBean();
			for (String email : StringUtils.split(email_list, ",")) {
				email = StringUtils.lowerCase(email);
				Validator.validate(email, R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "邮箱地址");
				emailMap.put(email, email);
			}
			
			int emailCount = Value.I;

			if (tagId != null) {
				Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TEST.getName(), Category.TAG);
				Asserts.isNull(category, "类别");
				Tag tag = tagService.get(UserSession.getCorpId(), null, tagId);
				Asserts.isNull(tag, "标签");
				
				if (!category.getCategoryId().equals(tag.getCategoryId())) {
					throw new Errors("禁止发送非测试类别标签");
				}

				emailCount = tag.getEmailCount();
				if (emailCount > 100) {
					throw new Errors("测试邮箱地址最多100个");
				}

				List<String> emailList = emailService.find(UserSession.getCorpId(), tag);
				for (String email : emailList) {
					emailMap.remove(email);
				}
			}

			emailCount = emailMap.size() + emailCount;
			
			if (emailCount < 1) {
				throw new Errors("测试邮箱地址最少1个");
			}
			if (emailCount > 100) {
				throw new Errors("测试邮箱地址最多100个");
			}
			
			List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(corp.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            List<Integer> corpIdList = Lists.newArrayList();
            corpService.cross(corpList, root.getCorpId(), corpIdList);
            corpIdList.add(root.getCorpId()); //算发送量的时候需要加上顶级父机构的发送量
            Integer[] corpIds = Converts._toIntegers(corpIdList);
            
			cosService.balance(corpIds, root.getCosId(), Value.I, emailCount, new DateTime());

			Cnd cnd = new Cnd();
			Setting setting = new Setting();

			cnd.setEmailList(Converts.toString(emailMap));
            cnd.setIncludeIds(Values.get(tagId, null));
            cnd.setExcludeIds("-1");
            cnd.setIncludes(includes);
            cnd.setExcludes("全部退订");
            
            setting.setAd(ad);
            setting.setSendCode("current");
            setting.setSms(sms);
            setting.setReplier(replier);
            setting.setRand(rand);
            setting.setRobot(Values.get(robot, null));
            setting.setTemplate(template.getTemplateName());
			
			Task task = new Task();
			task.setCorpId(UserSession.getCorpId());
			task.setBindCorpId(bindCorpId);
			task.setUserId(UserSession.getUserId());
			task.setTemplateId(templateId);
			task.setTaskName(taskName);
			task.setSubject(subject);
			task.setSenderName(senderName);
			task.setSenderEmail(senderEmail);
			task.setReceiver(receiver);
			task.setEmailCount(emailCount);
			
			task.setResourceId(corpService.get(UserSession.getCorpId()).getTestedId());
			task.setType(Type.TEST);
			task.setStatus(Status.TEST);
			
			MapBean attrs = new MapBean();
            Templates.attrs(template, attrs);
            if (attrs.getString("touch").equals("true")) {
                taskService.saveAndPlusSentWithTouch(task, cnd, setting, Value.I, emailCount);
            } else {
                taskService.saveAndPlusSent(task, cnd, setting, Value.I, emailCount);
            }
            
			Resource resource = resourceService.get(task.getResourceId());
			Asserts.isNull(resource, "通道");
			
			List<Prop> propList = propService.find(task.getCorpId());
			Asserts.isEmpty(propList, "属性");
			
			subject = Values.get(subject);
			String content = Values.get(Files.get(Webs.rootPath(), template.getFilePath()));
			Map<Integer, String> propMap = Props.parameters(subject, content, propList);
			
			String path = writerService.write(task, template, resource, propMap);
			
			if (task.getEmailCount() < 1) {
				taskService.save(task);
				throw new Errors("测试邮箱地址已退订");
			}
			
			String code = Processings.processing(path);
			code = HttpCode._200;
			if (StringUtils.isNotBlank(code) && StringUtils.equals(code, HttpCode._200)) {
				Date now = new Date();
				task.setStatus(Status.PROCESSING);
                task.setDeliveryTime(now);
                taskService.save(task);
				Views.ok(mb, "邮件已发送");
			} else {
				Views.error(mb, "邮件发送失败");
			}
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "邮件发送失败");
			logger.error("(Task:test) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String cancel(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/task/callback", token);
			
			String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^all|processing|completed$");
			t = Values.get(t, "all");
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);

			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			map.put("checkedName", task.getTaskName());
			map.put("status", task.getStatus());
			map.put("t", t);
			
			Views.map(map, "cancel", "", "取消", "所选任务", "taskId", taskId);
			return "default/mailing/task_status";
		} catch (Exception e) {
			logger.error("(Task:cancel) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void cancel(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			boolean isWaited = task.getStatus().equals(Status.WAITED);
			boolean isPlan = task.getPlanId() != null && task.getPlanReferer().equals(Value.NONE);
			
			if (isWaited) {
                if (!isPlan) {
	                synchronized (taskId) {
	                    Map<Integer, Integer> scanMap = scanMap();
	                    scanMap.put(taskId, Quartz.CANCELLED);
	                }
	            }
			    task.setStatus(Status.CANCELLED);
                taskService.save(task);
                if(isPlan){
                	deletePlanFile(task.getPlanId());
                }else{
                	deleteTaskFile(taskId);
                }
			} else {
				throw new Errors("禁止取消非待发送的任务");
			}
			
			Views.ok(mb, "取消任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "取消任务失败");
			logger.error("(Task:cancel) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String pause(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/task/callback", token);
			
			String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^all|processing|completed$");
			t = Values.get(t, "all");
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			map.put("checkedName", task.getTaskName());
			map.put("t", t);
			
			Views.map(map, "pause", "", "暂停", "所选任务", "taskId", taskId);
			return "default/mailing/task_status";
		} catch (Exception e) {
			logger.error("(Task:pause) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void pause(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");

			boolean isWaited = task.getStatus().equals(Status.WAITED);
			boolean isQueueing = task.getStatus().equals(Status.QUEUEING);
			boolean isProcessing = task.getStatus().equals(Status.PROCESSING);
			boolean isDelivery = (task.getDeliveryTime() != null);
			boolean isPlan = task.getPlanId() != null && task.getPlanReferer().equals(Value.NONE);
			
			if (isWaited && isPlan) {
                task.setStatus(Status.PAUSED);
                taskService.save(task);
                deletePlanFile(task.getPlanId());
                
            } else if (isQueueing && !isPlan) {
    	        synchronized (taskId) {
                    Map<Integer, Integer> scanMap = scanMap();
                    scanMap.put(taskId, Quartz.PAUSED);
                }
    			task.setStatus(Status.PAUSED);
    			taskService.save(task);
    			deleteTaskFile(taskId);
			} else if (isProcessing && !isPlan) {
				if (isDelivery) {
					String code = Processings.pause(task.getCorpId(), task.getTaskId(), task.getTemplateId());
					if (!StringUtils.equals(code, HttpCode._200)) {
						throw new Errors("暂停任务失败");
					}
				} else {
					task.setStatus(Status.PAUSED);
					taskService.save(task);
					deleteTaskFile(taskId);
				}
			} else {
				throw new Errors("禁止暂停非发送中的任务");
			}

			Views.ok(mb, "暂停任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "暂停任务失败");
			logger.error("(Task:pause) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String restart(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/task/callback", token);
			
			String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^all|processing|completed$");
			t = Values.get(t, "all");
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			map.put("checkedName", task.getTaskName());
			map.put("t", t);
			
			Views.map(map, "restart", "", "重启", "所选任务", "taskId", taskId);
			return "default/mailing/task_status";
		} catch (Exception e) {
			logger.error("(Processing:restart) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void restart(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			Setting setting = settingService.get(taskId);
			Asserts.isNull(setting, "发送设置");
			
			boolean isPaused = task.getStatus().equals(Status.PAUSED);
			boolean isPlan = task.getPlanId() != null && task.getPlanReferer().equals(Value.NONE); 
			
			if (isPaused) {
			    if (isPlan) {
			        task.setStatus(Status.WAITED);
                    taskService.save(task);
                    Files.createPlan(task.getPlanId());
			    } else {
			        boolean isDelivery = (task.getDeliveryTime() != null);
	                if (isDelivery) {
	                    String code = Processings.restart(task.getCorpId(), task.getTaskId(), task.getTemplateId());
	                    if (!StringUtils.equals(code, HttpCode._200)) {
	                        throw new Errors("重启任务失败");
	                    }
	                } else {
	                    synchronized (taskId) {
	                        Map<Integer, Integer> scanMap = scanMap();
	                        scanMap.put(taskId, Quartz.WAITING);
	                    }
	                    task.setStatus(Status.PROCESSING);
	                    taskService.save(task);
	                    Files.createTask(task.getTaskId());
	                }
			    }
			} else {
				throw new Errors("禁止重启非发送中的任务");
			}

			Views.ok(mb, "重启任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "重启任务失败");
			logger.error("(Task:restart) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String stop(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/mailing/task/callback", token);
			
			String t = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^all|processing|completed$");
			t = Values.get(t, "all");
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			map.put("checkedName", task.getTaskName());
			map.put("t", t);
			
			Views.map(map, "stop", "", "取消", "所选任务", "taskId", taskId);
			return "default/mailing/task_status";
		} catch (Exception e) {
			logger.error("(Task:stop) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void stop(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");

            boolean isWaited = task.getStatus().equals(Status.WAITED);
            boolean isQueueing = task.getStatus().equals(Status.QUEUEING);
			boolean isProcessing = Asserts.hasAny(task.getStatus(), new Integer[] { Status.PROCESSING, Status.PAUSED });
			boolean isDelivery = (task.getDeliveryTime() != null);
			
            boolean isPlan = task.getPlanId() != null && task.getPlanReferer().equals(Value.NONE); 
            
            if (isWaited && isPlan) {
                task.setStatus(Status.CANCELLED);
                taskService.save(task);
                deletePlanFile(task.getPlanId());
            } else if (isQueueing && !isPlan) {
				synchronized (taskId) {
					Map<Integer, Integer> scanMap = scanMap();
					scanMap.put(taskId, Quartz.CANCELLED);
				}
				task.setStatus(Status.CANCELLED);
				taskService.save(task);
				deleteTaskFile(taskId);
			} else if (isProcessing && !isPlan) {
				if (isDelivery) {
					String code = Processings.stop(task.getCorpId(), task.getTaskId(), task.getTemplateId());
					if (!StringUtils.equals(code, HttpCode._200)) {
						throw new Errors("取消任务失败");
					}
				} else {
					task.setStatus(Status.CANCELLED);
					taskService.save(task);
					deleteTaskFile(taskId);
				}
			} else {
				throw new Errors("禁止取消非发送中的任务");
			}
			
			Views.ok(mb, "取消任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "取消任务失败");
			logger.error("(Task:stop) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	@SuppressWarnings("unchecked")
	private Map<Integer, Integer> scanMap() {
		Map<Integer, Integer> scanMap = (Map<Integer, Integer>) cache.get(Fqn.TASK, Queue.TASK);
		if (scanMap == null) {
			scanMap = Maps.newHashMap();
			cache.put(Fqn.TASK, Queue.TASK, scanMap);
		}
		return scanMap;
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void callback(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String m = Value.S;
		try {
			CSRF.validate(request);
			
			m = Values.get(Https.getStr(request, "m", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^cancel|pause|restart|stop$"), "cancel");
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.getUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			mb.put("status", task.getStatus());
			
			Views.ok(mb, opt(m) + "任务成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, opt(m) + "任务失败");
			logger.error("(Task:callback) error: ", e);
		}
		
		ObjectMappers.renderJson(response, mb);
	}
	
	private String opt(String m) {
		String result = Value.S;
		if (m.equals("cancel")) {
			result = "取消";
		} else if (m.equals("pause")) {
			result = "暂停";
		} else if (m.equals("restart")) {
			result = "重启";
		} else if (m.equals("stop")) {
			result = "取消";
		}

		return result;
	}
	
	private void deleteTaskFile(Integer taskId) {
		String taskidPath = Webs.webRoot() + PathMap.SCAN_TASK.getPath() + taskId + ".txt";
		Files.delete(taskidPath);
	}
	
	private void deletePlanFile(Integer planId) {
		String taskidPath = Webs.webRoot() + PathMap.SCAN_PLAN.getPath() + planId + ".txt";
		Files.delete(taskidPath);
	}
	
	private Integer[] getPropIds(String path) {
		if(Files.exists(path)){
			File file = new File(path);
			BufferedReader reader = null;
	        try {
	        	List<Prop> propList = Lists.newArrayList();
	        	propList.addAll(propService.find(UserSession.getCorpId(), new String[]{ "email" }));
	            reader = new BufferedReader(new FileReader(file));
	            String line = null;
	            int count = 1;
	            while ((line = reader.readLine()) != null) {
	            	if(count == 2) break;
	            	if (StringUtils.isBlank(line)) {
						continue;
					}
	            	String[] props = StringUtils.split(line, ",");
	            	for(String prop : props) {
	            		if (Imports.isEmail(prop)) {
	    					continue;
	    				}
	            		String[] p = StringUtils.split(prop, "=");
	            		propList.addAll(propService.find(UserSession.getCorpId(), new String[]{ p[0] }));
	            	}
	            	count++;
	            }
	            reader.close();
	            String idStr = "";
	            for(Prop p : propList) {
	            	idStr += p.getPropId() + ",";
	            }
	            return Converts._toIntegers(idStr);
	        } catch (IOException e) {
	            e.printStackTrace();
	        } finally {
	            if (reader != null) {
	                try {
	                    reader.close();
	                } catch (IOException e1) {
	                }
	            }
	        }
		}
		return null;
	}
	
	
	public void createTask(MapBean mb,String t,Integer taskId,HttpServletRequest request, HttpServletResponse response){
		
		t = Values.get(Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^add|drafted$"), "add"); // add:正式发送, drafted:保存草稿	
		taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
		
		// 邮件设置
		String taskName = Https.getStr(request, "taskName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "任务名称");
		String orgTaskName = Https.getStr(request, "orgTaskName", R.CLEAN, R.LENGTH, "{1,60}");
	    Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
	    String campaignName = null;
	    if (campaignId != null) {
	        Campaign campaign = campaignService.get(UserSession.getCorpId(),null, campaignId); //同机构下活动共享
	        Asserts.isNull(campaign, "活动");
	        campaignName = campaign.getCampaignName();
	    }
		Integer templateId = Https.getInt(request, "templateId", R.CLEAN, R.REQUIRED, R.INTEGER);
		Asserts.isNull(templateId, "模板");
		Template template = templateService.get(null, UserSession.getCorpId(), null, templateId);
		Asserts.isNull(template, "模板");
		Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
		String subject = Https.getStr(request, "subject", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,100}", "邮件主题");
		String ad = Values.get(Https.getStr(request, "ad", R.CLEAN, R.REGEX, "regex:^true|false$"), "false");
		
		MapBean attrs = new MapBean();
		Templates.attrs(template, attrs);
		
		// 发件人设置
		Integer bindCorpId = null;
		String bindResourceId = null;
//		if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
		if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
			bindCorpId = Https.getInt(request, "bindCorpId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(bindCorpId, "所属企业");
			Corp bindCorp = corpService.get(bindCorpId);
			Asserts.isNull(bindCorp, "所属企业");
			
			bindResourceId = Https.getStr(request, "resourceId", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}");
			Asserts.isNull(bindResourceId, "投递通道");
			Resource resource = resourceService.get(bindResourceId);
			Asserts.isNull(resource, "投递通道");
			
		}
		String senderEmail = Https.getStr(request, "senderEmail", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,64}", "发件人邮箱");
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
		String replier = Https.getStr(request, "replier", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}", "回复邮箱");
		
		DateTime now = !Auth.isAuth() ? new DateTime() : Ntps.get();
		
		// 发送设置   
		String sendCode = Https.getStr(request, "sendCode", R.CLEAN, R.REGEX, "regex:^current|schedule|plan$:不是合法值", "发送方式");
		String includeIds = Https.getStr(request, "includeIds", R.CLEAN, R.REQUIRED, "包含标签");
		sendCode = Values.get(sendCode, "current");
		String jobTime = Value.S;
		Plan plan = null;

		Integer planId = Https.getInt(request, "planId", R.CLEAN, R.INTEGER, "周期设置");
		if (StringUtils.equals(sendCode, "schedule")) {
			jobTime = Https.getStr(request, "jobTime", R.REQUIRED, R.LENGTH, R.DATETIME, "{1,20}", "发送时间");
		} else if(StringUtils.equals(sendCode, "plan")) {
			if (sendCode.equals("plan") && (StringUtils.startsWith(includeIds, "EXPORT.") || StringUtils.startsWith(includeIds, "EXPORT_"))) throw new Errors("周期发送禁止选择临时文件");
	        String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.REQUIRED, R.DATE, "开始日期");
	        String endTime = Https.getStr(request, "endTime", R.CLEAN, R.REQUIRED, R.DATE, "结束日期");

	        if (beginTime.compareTo(endTime) > -1) throw new Errors("有效期限开始日期必须小于结束日期");
/*	        if (beginTime.compareTo(now.toString(Calendars.DATE)) < 1) throw new Errors("有效期限开始日期必须大于当前日期");*/
	        
	        if (planId != null)
	            plan = planService.get(null, UserSession.getCorpId(), planId);
	        if (plan == null)
	        	plan = new Plan();
	        
	        plan.setCorpId(UserSession.getCorpId());
	        plan.setUserId(UserSession.getUserId());
	        plan.setCron(Plans.cron(request, R.REQUIRED));
	        plan.setBeginTime(Calendars.parse(beginTime, Calendars.DATE).toDate());
	        plan.setEndTime(Calendars.parse(endTime, Calendars.DATE).toDate());
	        planService.save(plan);
		}
		
		String sms = Values.get(Https.getStr(request, "sms", R.CLEAN, R.REGEX, "regex:^0|19|11|14|15|13$:不是合法值", "短信通知"), "0");
		
		// 收件人设置
	    Integer REQUIRED = R.CLEAN;
	    if (t.equals("add")) REQUIRED = R.REQUIRED;
	    String fileId = Https.getStr(request, "fileId", R.CLEAN, REQUIRED, R.LENGTH, "{1,30}", "筛选文件");
	    String excludeIds = Https.getStr(request, "excludeIds", R.CLEAN, R.REGEX, "regex:^[0-9,-]+$:不是合法值", "排除标签");
	    String includes = Https.getStr(request, "includes", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,512}", "包含标签");
	    String excludes = Https.getStr(request, "excludes", R.CLEAN, R.LENGTH, "{1,512}", "排除标签");
	    
	    int emailCount = 0;
	    
	    if (StringUtils.startsWith(includeIds, "EXPORT.") || StringUtils.startsWith(includeIds, "EXPORT_")) {
	    	String path = null;
	    	if(StringUtils.startsWith(includeIds, "EXPORT_")){
	    		path = PathMap.R.getPath() + "export/" + UserSession.getUserId() + "/" + includeIds + "/reduce.txt";
	    	}else{
	    		path = PathMap.EXECUTE.getPath() + "export/" + UserSession.getUserId() + "/" + includeIds + "/reduce.txt";
	        }
	        Asserts.notExists(Webs.rootPath(), path, "临时");
	        if (sendCode.equals("plan")) throw new Errors("周期发送禁止选择临时文件");
	    } else {
	        includeIds = (String) Validator.validate(includeIds, R.REGEX, "regex:^[0-9,]+$:不是合法值", "包含标签");
	        Integer[] inTagIds = Converts._toIntegers(includeIds);
	        Asserts.isEmpty(inTagIds, "包含标签");
	        Asserts.isEmpty(tagService.find(UserSession.getCorpId(), null, inTagIds), "包含标签");
	        Integer[] exTagIds = Converts._toIntegers(excludeIds);
	        if (Asserts.equals(inTagIds, exTagIds)) throw new Errors("包含标签不能与排除标签一致");
	    }
	    
	    String receiver = Https.getStr(request, "receiver", R.CLEAN, R.LENGTH, "{1,20}", "收件人昵称");
	    Integer unsubscribeId = Https.getInt(request, "unsubscribeId", R.CLEAN, R.INTEGER, "退订设置");
	    String unsubscribe = Https.getStr(request, "unsubscribe", R.CLEAN, R.LENGTH, "{1,60}", "退订设置");
	    if (unsubscribeId != null) {
	        Asserts.isNull(tagService.get(UserSession.getCorpId(), null, unsubscribeId), "退订标签");
	    }
		
		// 属性库设置
		String labelIds = null;
//		if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
		if (UserSession.hasAnyPermission(new String[] { Permissions.BSN.getName() })) {
			labelIds = Https.getStr(request, "labelIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "关联属性");
		}
		
		Asserts.notUnique(taskService.unique(UserSession.getCorpId(), UserSession.getUserId(), taskName, orgTaskName), "任务");

		if (t.equals("add")) {
	        String path = PathMap.R.getPath() + UserSession.getUserId() + "/" + fileId + "/reduce.txt";
	        Asserts.notExists(Webs.rootPath(), path, "投递");
	        emailCount = Lines.count(Webs.rootPath() + path, "UTF-8");
	        if (emailCount < 1) {
	            throw new Errors("收件人数不能少于1");
	        }
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
			if (task.getModifyTime() != null) {
				balanceTime = new DateTime(task.getModifyTime());
			} else {
				balanceTime = now;
			}
		}
		
		String auditPath = null;
//		if (Securitys.hasAnyRole(new String[] { RoleMap.MGR.getName(), RoleMap.ALL.getName() })) {
//		if (user.getRoleType().equals(1)) {
//			auditPath = corp.getAuditPath();
//		}
		auditPath = Audits.remove(auditPath, UserSession.getCorpId());
//		if (Securitys.hasAnyRole(new String[] { RoleMap.BUK.getName(), RoleMap.BSN.getName(), RoleMap.ALL.getName() })
//				&& Values.get(user.getModerate()).equals(Value.T)) {
		if (Securitys.hasAnyRole(new String[] { RoleMap.BUK.getName(), RoleMap.ALL.getName() })
	            && Values.get(user.getModerate()).equals(Value.T)) {
			auditPath = Audits.add(auditPath, UserSession.getCorpId());
		}
		
		String resourceId = corp.getTestedId();
		if (emailCount > Property.getInt(Config.PIPE_SIZE)) {
			resourceId = corp.getFormalId();
		} else {
			auditPath = null;
		}
		
//		if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
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
			cosService.balance(corpIds, root.getCosId(), holdCount, emailCount, balanceTime);
		}
		
		Cnd cnd = new Cnd();
		Setting setting = new Setting();
		
		cnd.setFileId(Values.get(fileId, null));
		cnd.setIncludeIds(Values.get(includeIds, null));
	    cnd.setExcludeIds(Values.get(excludeIds, null));
	    cnd.setIncludes(Values.get(includes, null));
	    cnd.setExcludes(Values.get(excludes, null));
		
	    setting.setTemplate(template.getTemplateName());
	    setting.setCampaign(campaignName);
		setting.setAd(ad);
		setting.setSendCode(sendCode);
		setting.setSms(sms);
		setting.setReplier(Values.get(replier, null));
		setting.setRand(rand);
		setting.setRobot(Values.get(robot, null));
		setting.setTagIds(Values.get(includeIds, null));
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
	    
		if (sendCode.equals("plan") && plan != null && plan.getPlanId() != null) {
	        task.setPlanId(plan.getPlanId());
	        task.setPlanReferer(Value.NONE);
	    }
		
//		if (Securitys.hasAnyRole(new String[] { RoleMap.BSN.getName() })) {
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
	    
	    // 将一些值保存到cookie中
	    Cookies.add(response, "taskName", Values.get(taskName, ""));
	    Cookies.add(response, "campaignId", Values.get(campaignId, ""));
	    Cookies.add(response, "templateId", Values.get(templateId, ""));
	    Cookies.add(response, "subject", Values.get(subject, ""));
		Cookies.add(response, "bindCorpId", Values.get(bindCorpId, ""));
		Cookies.add(response, "resourceId", Values.get(bindResourceId, ""));
		Cookies.add(response, "senderEmail", Values.get(senderEmail, ""));
		Cookies.add(response, "senderName", Values.get(senderName, ""));
		Cookies.add(response, "receiver", Values.get(receiver, ""));
		Cookies.add(response, "unsubscribeId", Values.get(unsubscribeId, ""));
	    
		mb.put("taskId", task.getTaskId());
		mb.put("taskName", task.getTaskName());
	    if (plan != null) {
	        mb.put("planId", plan.getPlanId());
	    } else {
	        mb.put("planId", "");
	    }   
		Views.ok(mb, t.equals("add") ? ((taskId != null ? "修改" : "新建") + "任务成功") : "自动保存草稿成功");	
	  }
	
	
}