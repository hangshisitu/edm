package cn.edm.app.schedule;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Minutes;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;

import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Cnd;
import cn.edm.model.Plan;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.service.CndService;
import cn.edm.service.PlanService;
import cn.edm.service.SettingService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Validates;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public class PlanJob {

	private static final Logger logger = LoggerFactory.getLogger(PlanJob.class);
	
	private static boolean setup = false;

	@Autowired
	private CndService cndService;
	@Autowired
	private PlanService planService;
	@Autowired
	private SettingService settingService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private TemplateService templateService;

	public void execute() {
		try {
			if (setup) {
				return;
			}
			setup = true;
			scan();
		} catch (Exception e) {
			logger.error("(PlanJob:execute) error: ", e);
		} finally {
			setup = false;
		}
	}

	private void scan() {
		List<Plan> planList = null;

		DateTime now = new DateTime();
//		DateTime now = Calendars.parse("2014-06-12 15:00", Calendars.DATE_TIME);
		String date = now.toString(Calendars.DATE);

		int last = 0;
		Map<Integer, String> cronMap = Maps.newHashMap();
		Map<Integer, String> completedMap = Maps.newHashMap();
		
		int row = 2000;

		while (true) {
			String path = Webs.webRoot() + PathMap.SCAN_PLAN.getPath();
			List<Integer> planIds = Files.scan(path);
			planList = planService.iter(Converts._toIntegers(planIds), date, date, last, row);
			if (Asserts.empty(planList)) {
				break;
			}

			for (Plan plan : planList) {
				Integer planId = plan.getPlanId();
				Integer excludePlanId = planService.exclude(planId);
				// 对状态task_status是27并且不是type=2的周期父任务进行删除基础扫描文件
				if(excludePlanId != null) deleteFile(excludePlanId);
				
				last = planId;
				
				String jobTime = cron(now, plan);
				if (StringUtils.isBlank(jobTime)) {
					continue;
				}
				cronMap.put(planId, jobTime);
				
                DateTime completed = new DateTime(plan.getEndTime());
                completedMap.put(planId, completed.toString(Calendars.DATE));
			}

            logger.info("(PlanJob:scan) cronMap: " + cronMap);
			
			copy(cronMap, completedMap, now);

			cronMap.clear();
		}
	}
	
	private static String cron(DateTime now, Plan plan) {
		String[] crons = StringUtils.splitPreserveAllTokens(plan.getCron(), " ");
		if (Asserts.empty(crons) || crons.length < 5) {
            logger.warn("(PlanJob:cron) plan_id: " + plan.getPlanId() + ", cron: " + plan.getCron() + ", warn: cron is empty|error");
			return null;
		}

		String minute = crons[0];
		String hour = crons[1];
		String day = crons[2];
		String week = crons[3];
		String month = crons[4];
		
		// month
		boolean isMonth = false;
		if (month.equals("*")) isMonth = true;
		if (Validates.integer(month) && Integer.valueOf(month).equals(now.getMonthOfYear())) isMonth = true;

		// week
		boolean isWeek = false;
		if (week.equals("*")) isWeek = true;
		if (Validates.matches(week, "^[1-7,]+$"))
		    for (String e : StringUtils.splitPreserveAllTokens(week, ","))
                if (Validates.integer(e) && Integer.valueOf(e).equals(now.getDayOfWeek())) isWeek = true;
		
		// day
		boolean isDay = false;
		DateTime last = now.dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue();
		if (day.equals("*")) isDay = true;
		if (day.equals("-1") && now.getDayOfMonth() == last.getDayOfMonth()) isDay = true;
		if (Validates.integer(day) && Integer.valueOf(day).equals(now.getDayOfMonth())) isDay = true;

		// hour
		boolean isHour = false;
		if (Validates.formatter(hour, "H")) isHour = true;
		
		// minute
		boolean isMinute = false;
        if (Validates.formatter(minute, "m")) isMinute = true;
        
        if (hour.length() == 1) hour = "0" + hour;
        if (minute.length() == 1) minute = "0" + minute;
        
        if (!(isMonth && isWeek && isDay && isHour && isMinute)) {
            logger.warn("(PlanJob:cron) plan_id: " + plan.getPlanId() + ", cron: " + plan.getCron()
                    + ", now: " + now.toString(Calendars.DATE_TIME) + ", warn: cron is not time to plan");
            return null;
        }
        
        String timeStr = now.toString(Calendars.DATE) + " " + hour + ":" + minute;
        DateTime jobTime = Calendars.parse(timeStr, Calendars.DATE_TIME);
        
        // 若 jobTime - now < 5, 执行调度.
        int minutes = Minutes.minutesBetween(now, jobTime).getMinutes();
        if (minutes < 5 && minutes > -1) {
            return timeStr;
        } else {
            logger.warn("(PlanJob:cron) plan_id: " + plan.getPlanId() + ", cron: " + plan.getCron() 
                    + ", job_time: " + jobTime.toString(Calendars.DATE_TIME)
                    + ", now: " + now.toString(Calendars.DATE_TIME) + ", warn: job_time - now > 5");
        }
        
        return null;
    }
	
	private void copy(Map<Integer, String> cronMap, Map<Integer, String> completedMap, DateTime now) {
		List<Integer> planIds = Lists.newArrayList();
		for (Entry<Integer, String> entry : cronMap.entrySet()) {
			planIds.add(entry.getKey());
		}

		List<Task> taskList = taskService.referers(Converts._toIntegers(planIds));
		if (Asserts.empty(taskList)) {
		    logger.warn("(PlanJob:copy) warn: task_list is empty");
			return;
		}

		for (Task task : taskList) {
			try {
				String jobTime = cronMap.get(task.getPlanId());
				
				if (StringUtils.isBlank(jobTime)) {
					continue;
				}
				
				String planReferer = task.getTaskId() + "#" + jobTime.substring(0, 10);

				if (!taskService.unique(planReferer)) {
				    logger.warn("(PlanJob:copy) plan_id: " + task.getPlanId() + ", plan_referer: " + planReferer + ", job_time: " + jobTime + ", warn: plan is repeat");
					continue;
				}

				Cnd cnd = cndService.get(task.getTaskId());
				Asserts.isNull(cnd, "收件人设置");
				Setting setting = settingService.get(task.getTaskId());
				Asserts.isNull(setting, "发送设置");
				
				Task copy1 = new Task();
				Cnd copy2 = new Cnd();
				Setting copy3 = new Setting();

                BeanUtils.copyProperties(task, copy1, new String[] { "taskId", "campaignId", "type", "status", "modifyTime" });
				BeanUtils.copyProperties(cnd, copy2, new String[] { "taskId", "fileId" });
				BeanUtils.copyProperties(setting, copy3, new String[] { "taskId", "campaign" });
				
				copy1.setPlanId(task.getPlanId());
				copy1.setPlanReferer(planReferer);
				copy1.setJobTime(Calendars.parse(jobTime, Calendars.DATE_TIME).toDate());
				copy1.setType(Type.COLL);
				copy1.setStatus(Status.WAITED);
				copy1.setModifyTime(now.toDate());

				// 若now=plan.endTime, 周期任务为已完成.
                if (now.toString(Calendars.DATE).equals(completedMap.get(task.getPlanId()))) {
                    task.setStatus(Status.COMPLETED);
                    taskService.save(task);
                    // 扫描入库成功后，删除扫描文件
                    deleteFile(task.getPlanId());
                    logger.info("(PlanJob:copy) plan_id: " + task.getPlanId() + ", ok: plan is completed");
                }
				
                Template template = templateService.get(task.getTemplateId());
                Asserts.isNull(template, "模板");
                MapBean attrs = new MapBean();
                Templates.attrs(template, attrs);
                
                if (attrs.getString("touch").equals("true")) {
                    taskService.saveAndPlusSentWithTouch(copy1, copy2, copy3, Value.I, task.getEmailCount());
                    Files.createTask(copy1.getTaskId());
                } else {
                    taskService.saveAndPlusSent(copy1, copy2, copy3, Value.I, task.getEmailCount());
                    if(copy1.getType() == Type.COLL) Files.createTask(copy1.getTaskId());
                }
			} catch (Errors e) {
                logger.warn("(PlanJob:copy) warn: ", e);
            } catch (Exception e) {
                logger.error("(PlanJob:copy) error: ", e);
            }
		}
	}
	
	private void deleteFile(Integer planId) {
		String planidPath = Webs.webRoot() + PathMap.SCAN_PLAN.getPath() + planId + ".txt";
		Files.delete(planidPath);
	}
}
