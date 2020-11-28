package cn.edm.service;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.app.auth.Sents;
import cn.edm.consts.Cnds;
import cn.edm.consts.Id;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.model.Cnd;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Pages;
import cn.edm.web.facade.Templates;
import cn.edm.web.facade.Touchs;

import com.google.common.collect.Lists;

@Transactional
@Service
public class TaskService {

    @Autowired
    private Dao dao;

    public void save(Task task) {
        if (task.getTaskId() != null) {
            dao.update("Task.update", task);
        } else {
            dao.save("Task.save", task);
        }
	}
    
    public void save(Task task, Cnd cnd, Setting setting) {
        save(task);
        cnd.setTaskId(task.getTaskId());
        dao.save("Cnd.save", cnd);
        setting.setTaskId(task.getTaskId());
        dao.save("TaskSetting.save", setting);
    }
    
    public void saveAndPlusSent(Task task, Cnd cnd, Setting setting, int holdCount, int emailCount) {
    	save(task);
		cnd.setTaskId(task.getTaskId());
		dao.save("Cnd.save", cnd);
		setting.setTaskId(task.getTaskId());
		dao.save("TaskSetting.save", setting);
		Sents.plus(task.getStatus(), holdCount, emailCount);
	}
    
    public void saveAndPlusSentWithTouch(Task task, Cnd cnd, Setting setting, int holdCount, int emailCount) {
        saveAndPlusSent(task, cnd, setting, holdCount, emailCount);
        
        List<Setting> settingList = dao.find("TemplateSetting.query",
                new MapBean("corpId", task.getCorpId(), "parentId", task.getTemplateId()));
        
        List<Touch> touchList = touchList(task, settingList);
        
        List<Setting> touchSets = Lists.newArrayList();
        
        Integer taskId = task.getTaskId();
        StringBuilder sbff = new StringBuilder();
        for (Touch touch : touchList) {
            touch.setTaskId(Id.START_TOUCH_ID + taskId);
            touch.setParentId(taskId);
            touch.setTouchCount(Value.I);
            dao.save("Touch.save", touch);
            
            Setting touchSet = new Setting();
            touchSet.setTaskId(touch.getTaskId());
            touchSet.setParentId(touch.getParentTemplateId());
            touchSet.setTemplateId(touch.getTemplateId());
            touchSet.setParentName(touch.getParentTemplateName());
            touchSet.setTemplateName(touch.getTemplateName());
            touchSet.setTouchUrls(touch.getTouchUrls());
            touchSet.setSubject(touch.getSubject());
            touchSet.setAfterMinutes(touch.getAfterMinutes());
            touchSet.setDeviceType(touch.getDeviceType());
            touchSets.add(touchSet);
            sbff.append(touch.getTaskId()).append(",");
        }

        Integer[] taskIds = Converts._toIntegers(sbff.toString());
        dao.delete("TouchSetting.delete", new MapBean("taskIds", taskIds));
        dao.save("TouchSetting.batchSave", touchSets);
        
        MapBean attrs = new MapBean();
        Template template = dao.get("Template.query", new MapBean("templateId", task.getTemplateId()));
        Templates.attrs(template, attrs);
        
        List<Touch> arr = dao.find("Touch.query", new MapBean("parentId", taskId));
        Touchs.create(arr, attrs.getString("personal"));
    }
    
    private List<Touch> touchList(Task task, List<Setting> settingList) {
        List<Touch> touchList = Lists.newArrayList();
        for (Setting setting : settingList) {
            Touch touch = new Touch();
            touch.setCorpId(task.getCorpId());
            touch.setUserId(task.getUserId());
            touch.setTemplateId(setting.getTemplateId());
            touch.setTemplateName(setting.getTemplateName());
            touch.setSubject(setting.getSubject());
            touch.setSenderName(task.getSenderName());
            touch.setSenderEmail(task.getSenderEmail());
            touch.setReceiver(task.getReceiver());
            touch.setParentTemplateId(setting.getParentId());
            touch.setParentTemplateName(setting.getParentName());
            touch.setTouchUrls(setting.getTouchUrls());
            touch.setAfterMinutes(setting.getAfterMinutes());
            touch.setDeviceType(setting.getDeviceType());
            touchList.add(touch);
        }
        return touchList;
    }
    
    public void saveAndMinusSent(Task task, int emailCount) {
    	save(task);
        Sents.minus(emailCount);
	}
    
    public Task get(int corpId, String userId, Integer taskId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("taskId", taskId);
        return dao.get("Task.query", mb);
    }
    
    public Task get(Integer[] corpIds, Integer corpId, Integer taskId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("taskId", taskId);
        return dao.get("Task.query", mb);
    }

    public Task referer(Integer[] corpIds, Integer corpId, Integer planId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("planId", planId);
        mb.put("type", Type.PLAN, "typeCnd", Cnds.EQ);
        return dao.get("Task.query", mb);
    }
    
	public Task get(Integer taskId) {
		return dao.get("Task.query", new MapBean("taskId", taskId));
	}

    public List<Task> campaignNull(int corpId, Integer[] statuses) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        mb.put("campaignCnd", Cnds.IS_NULL);
        mb.put("type", Type.FORMAL, "typeCnd", Cnds.EQ);
        Pages.put(mb, "statuses", statuses);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Task.query", mb);
    }
    
    public List<Task> find(int corpId, Integer[] taskIds) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "taskIds", taskIds);
        return dao.find("Task.query", mb);
    }
	   
	public List<Task> referers(Integer[] planIds) {
	    MapBean mb = new MapBean();
	    mb.put("planIds", planIds);
        mb.put("type", Type.PLAN, "typeCnd", Cnds.EQ);
	    mb.put("statuses", Status.task("waited"));
		return dao.find("Task.query", mb);
	}

	public boolean unique(String planReferer) {
		long count = dao.countResult("Task.count", new MapBean("planReferer", planReferer));
		return count == 0;
	}
	
	public Page<Task> search(Page<Task> page, MapBean mb, Integer[] corpIds, Integer corpId, Integer campaignId,
			String taskName, String beginTime, String endTime,
			Integer[] statuses, String nameCnd, Integer type) {
        Pages.put(mb, "taskName", taskName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.put(mb, "type", type);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "campaignId", campaignId);
        
        if (type == null) {
            mb.put("types", new Integer[] { Type.FORMAL, Type.PLAN, Type.JOIN });
        } else {
            mb.put("type", type, "typeCnd", Cnds.EQ);
        }
        
        Pages.put(mb, "statuses", statuses);
        Pages.put(mb, "nameCnd", nameCnd);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Task.count", "Task.index");
    }

	public Page<Task> search(Page<Task> page, MapBean mb, Integer[] corpIds, Integer corpId,
	        Integer auditCorpId,
			String taskName, String subject, String beginTime, String endTime,
			Integer[] statuses, String module, String nameCnd) {
        Pages.put(mb, "taskName", taskName);
        Pages.put(mb, "subject", subject);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("types", new Integer[] { Type.FORMAL, Type.PLAN, Type.JOIN });
        Pages.put(mb, "statuses", statuses);
		path(module, mb, "/" + auditCorpId + "/");
		Pages.put(mb, "nameCnd", nameCnd);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Task.count", "Task.index");
    }
    
    public boolean unique(int corpId, String userId, String taskName, String orgTaskName) {
        if (taskName == null || taskName.equals(orgTaskName)) {
            return true;
        }
        
        MapBean mb = new MapBean();
        mb.put("corpId", corpId, "userId", userId);
        mb.put("taskName", taskName, "nameCnd", Cnds.EQ);
        mb.put("status", Status.DELETED, "statusCnd", Cnds.NOT_EQ);
        mb.put("type", Type.TEST, "typeCnd", Cnds.NOT_EQ);
        long count = dao.countResult("Task.count", mb);
        return count == 0;
    }
    
	public List<Integer> scan() {
		return dao.find("Task.scan");
	}

	public Integer scan(Integer taskId) {
		return dao.get("Task.scan", new MapBean("taskId", taskId));
	}
	
	public List<Integer> scan(Integer[] taskIds) {
		return dao.find("Task.scan", new MapBean("taskIds", taskIds));
	}
	
	public boolean lockTag(int corpId, String userId, Integer tagId, boolean isUnsubscribe) {
		Integer[] statuses = { Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED, Status.QUEUEING, Status.PROCESSING, Status.PAUSED };
		
		MapBean mb = new MapBean();
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "userId", userId);
		Pages.put(mb, "statuses", statuses);
		
		Pages.put(mb, "tagId", tagId);
		int count = (int) dao.countResult("Task.lockTag", mb);
		
		if (isUnsubscribe) {
			Pages.put(mb, "tagId", -1);
			return count > 0 || (int) dao.countResult("Task.lockTag", mb) > 0;
		}

		return count > 0;
	}
	
	public List<Integer> lockTemplate(int corpId, String userId, Integer[] templateIds) {
		Integer[] statuses = { Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED, Status.QUEUEING, Status.PROCESSING, Status.PAUSED };
		
		MapBean mb = new MapBean();
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "userId", userId);
		Pages.put(mb, "templateIds", templateIds);
		Pages.put(mb, "statuses", statuses);
		return dao.find("Task.lockTemplate", mb);
	}

    public boolean lockSender(int corpId, String userId, String senderEmail) {
        Integer[] statuses = { Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED, Status.QUEUEING, Status.PROCESSING, Status.PAUSED };
        
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "senderEmail", senderEmail);
        Pages.put(mb, "statuses", statuses);
        int count = (int) dao.countResult("Task.lockSender", mb);
        return count > 0;
    }
    
	public int sentCount(Integer[] corpIds, DateTime beginTime, DateTime endTime) {
		Integer[] processings = {
				Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED,
				Status.QUEUEING, Status.PROCESSING, Status.PAUSED 
		};

		Integer[] completeds = { Status.CANCELLED, Status.COMPLETED };

		MapBean mb = new MapBean();
        mb.put("corpIds", corpIds);
        mb.put("types", new Integer[] { Type.FORMAL, Type.COLL });
		
		if (beginTime != null) {
			mb.put("beginTime", beginTime.toString(Calendars.DATE));
		}
		if (endTime != null) {
			mb.put("endTime", endTime.toString(Calendars.DATE));
		}
		
		mb.put("statuses", processings);
		long processingCount = dao.countResult("Task.processing", mb);
		
		mb.put("statuses", completeds);
		long completedCount = dao.countResult("Result.completed", mb);
		
		return (int) (processingCount + completedCount);
	}
	
    public int refererCount(Integer[] corpIds, Integer corpId, Integer planId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("planId", planId);
        mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        mb.put("statuses", Status.task("processing|completed"));
        long count = dao.countResult("Task.count", mb);
        return (int) count;
    }
    
	private void path(String module, MapBean mb, String path) {
		if (StringUtils.equals(module, "waited")) {
			Pages.put(mb, "auditPath", path);
			Pages.put(mb, "auditCnd", Cnds.LIKE);
		} else if (StringUtils.equals(module, "completed")) {
			Pages.put(mb, "moderatePath", path);
			Pages.put(mb, "moderateCnd", Cnds.LIKE);
		} else if (StringUtils.equals(module, "none")) {
			Pages.put(mb, "auditPath", path);
			Pages.put(mb, "auditCnd", Cnds.NOT_LIKE);
			Pages.put(mb, "moderatePath", path);
			Pages.put(mb, "moderateCnd", Cnds.NOT_LIKE);
		}
	}
	
	public List<Task> find(Integer otherId, String type) {
		MapBean mb = new MapBean();
        if("plan".equals(type)) {
    		mb.put("planId", otherId);
    		mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
    		mb.put("statuses", Status.task("processing|completed"));
    		return dao.find("Task.query", mb);
    	}else if("campaign".equals(type)){
    		mb.put("campaignId", otherId);
    		//mb.put("type", Type.FORMAL, "typeCnd", Cnds.EQ);
    		mb.put("statuses", Status.task("completed"));
    		return dao.find("Task.touchQuery", mb);
    	}
        return null;
	}
	
    public int touchCount(Integer[] corpIds, Integer corpId, Integer otherId, String type) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("statuses", Status.task("completed"));
        if ("plan".equals(type)) {
            mb.put("planId", otherId);
            mb.put("type", Type.COLL, "typeCnd", Cnds.EQ);
        } else if ("campaign".equals(type)) {
            mb.put("campaignId", otherId);
        } else if ("task".equals(type)) {
            mb.put("taskId", otherId);
        } else {
            return 0;
        }

        return (int) dao.countResult("Task.touchCount", mb);
    }
	
	public List<Integer> exclude(Integer[] taskIds) {
		return dao.find("Task.exclude", new MapBean("taskIds", taskIds));
	}
	
}
