package cn.edm.web.action.mailing;

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
import cn.edm.consts.Status;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.model.Category;
import cn.edm.model.Corp;
import cn.edm.model.Setting;
import cn.edm.model.Tag;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Audits;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Tasks;

@Controller
@RequestMapping("/mailing/audit/**")
public class AuditAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(AuditAction.class);
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Task> page = new Page<Task>();
			Pages.page(request, page);

			String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^waited|completed|none$");
			m = Values.get(m, "waited");
			
			Integer corpId  = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
			String taskName = StreamToString.getStringByUTF8(Https.getStr(request, "taskName", R.CLEAN, R.LENGTH, "{1,20}"));
			String subject  = StreamToString.getStringByUTF8(Https.getStr(request, "subject", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			Integer[] statuses = Status.audit(m);
			String nameCnd = Https.getStr(request, "nameCnd", R.CLEAN, R.REGEX, "regex:^" + Cnds.EQ + "|" + Cnds.LIKE + "$");
			nameCnd = Values.get(nameCnd, Cnds.LIKE);
			
            MapBean mb = new MapBean();
            corpId = UserSession.getCorpId(mb, corpId);
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);

            page = taskService.search(page, mb, corpIds, corpId, UserSession.getCorpId(), taskName, subject, beginTime, endTime, statuses, m, nameCnd);
			
			Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.TEST.getName(), Category.TAG);
			List<Tag> tagList = null;
			if (category != null) {
				tagList = tagService.find(UserSession.getCorpId(), category.getCategoryId());
			}

			map.put("page", page);
			map.put("tagList", tagList);
			map.put("mb", mb);
			map.put("m", m);
			map.put("corp", corp);
            map.put("corpChildren", corpChildren);
			
			return "default/mailing/audit_page";
		} catch (Exception e) {
			logger.error("(Audit:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(value = "not_passed", method = RequestMethod.GET)
	public String notPassed(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			map.put("taskId", taskId);
			return "default/mailing/audit_not_passed";
		} catch (Exception e) {
			logger.error("(Audit:notPassed) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(value = "not_passed", method = RequestMethod.POST)
	public void notPassed(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer taskId = null;
		try {
			CSRF.validate(request);
			
			taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			String suggestion = Https.getStr(request, "suggestion", R.CLEAN, R.LENGTH, "{1,60}", "审核意见");
			
			Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), taskId);
			Asserts.isNull(task, "任务");
			
			validateStatus(task.getStatus());
			
			task.setStatus(Status.UNAPPROVED);
//			if (Securitys.hasAnyRole(new String[] { RoleMap.ADM.getName() })) {
			if (UserSession.getUser().getRole().getRoleId().equals(0)) {
				task.setStatus(Status.RETURN);
			}
			
			task.setAuditPath(Audits.remove(task.getAuditPath(), UserSession.getCorpId()));
			task.setModeratePath(moderate(task.getModeratePath(), UserSession.getCorpId()));
			task.setSuggestion(suggestion);
			
			taskService.saveAndMinusSent(task, task.getEmailCount());
			
			Views.ok(mb, "审核不通过成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "审核不通过失败");
			logger.error("(Audit:notPassed) error: ", e);
		}
		
		mb.put("taskId", taskId);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String passed(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			if (taskId != null) {
				Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), taskId);
				if (task != null) {
					map.put("checkedName", task.getTaskName());
				}
			}
			
			map.put("taskId", taskId);
			return "default/mailing/audit_passed";
		} catch (Exception e) {
			logger.error("(Audit:passed) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void passed(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer taskId = null;
		try {
			CSRF.validate(request);
			
			taskId = Https.getInt(request, "taskId", R.CLEAN, R.INTEGER);
			Asserts.isNull(taskId, "任务");
			Task task = taskService.get(UserSession.getCorpId(), UserSession.containsUserId(), taskId);
			Asserts.isNull(task, "任务");
			Setting setting = settingService.get(taskId);
			Asserts.isNull(setting, "发送设置");
			
			validateStatus(task.getStatus());

			if (task.getEmailCount() < 1) {
				throw new Errors("收件人数不能少于1");
			}
			
			Template template = templateService.get(task.getTemplateId());
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
			
			task.setAuditPath(Audits.remove(task.getAuditPath(), UserSession.getCorpId()));
			task.setModeratePath(moderate(task.getModeratePath(), UserSession.getCorpId()));
			task.setStatus(Audits.moderate(setting.getSendCode(), task.getAuditPath()));
			taskService.save(task);

			// 判断是否生成周期任务的基础文件
			if(task.getType()==2){
				if(Tasks.createPlan(task)) Files.createPlan(task.getPlanId());
			}else{
				Files.createTask(task.getTaskId());
			}
			
			Views.ok(mb, "审核通过成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "审核通过失败");
			logger.error("(Audit:passed) error: ", e);
		}
		
		mb.put("taskId", taskId);
		ObjectMappers.renderJson(response, mb);
	}
	
	private String moderate(String moderatePath, int corpId) {
		if (StringUtils.isNotBlank(moderatePath)) {
			String path = "/" + corpId + "/";
			String pref = StringUtils.endsWith(path, "/") ? StringUtils.removeEnd(path, "/") : path;
			String suff = StringUtils.startsWith(moderatePath, "/") ? StringUtils.removeStart(moderatePath, "/") : moderatePath;
			moderatePath = pref + "/" + suff;
		} else {
			moderatePath = "/" + corpId + "/";
		}
		return moderatePath;
	}
	
	private void validateStatus(Integer status) {
		boolean audit = true;
//		if (Securitys.hasAnyRole(new String[] { RoleMap.ADM.getName() })) {
//		if (UserSession.getUser().getRole().getRoleId().equals(0)) {
//			if (!status.equals(Status.REVIEWING)) {
//				audit = false;
//			}
//		} else {
//			if (!status.equals(Status.PROOFING)) {
//				audit = false;
//			}
//		}
		if (!status.equals(Status.REVIEWING)) {
            audit = false;
        }
		if (!audit) {
			throw new Errors("禁止审核非待审核状态的任务");
		}
	}
}
