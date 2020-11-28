package cn.edm.web.facade;

import java.util.Date;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Minutes;
import org.joda.time.format.DateTimeFormat;
import org.joda.time.format.DateTimeFormatter;
import org.springframework.ui.ModelMap;

import cn.edm.app.auth.Auth;
import cn.edm.consts.MyCode;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.mapper.UrlMap;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Triger;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Cookies;

import com.google.common.collect.Lists;

public class Tasks {

    /**
     * 获取"企业ID.任务ID.模板ID"字符串.
     */
    public static String id(Integer corpId, Integer taskId, Integer templateId) {
        return corpId + "." + taskId + "." + templateId;
    }
    
    /**
     * 获取[企业ID,任务ID,模板ID]数组.
     */
    public static Integer[] id(String SID) {
        return Converts._toIntegers(StringUtils.replace(SID, ".", ","));
    }
    
    /**
     * 获取触发任务的企业ID.
     */
    public static Integer[] corpIds(List<Triger> trigerList) {
        List<Integer> corpIdList = Lists.newArrayList();
        for (Triger triger : trigerList) {
            corpIdList.add(triger.getTrigerCorpId());
        }
        
        return Converts._toIntegers(corpIdList);
    }
    
    /**
     * 锁定定时任务.
     */
    public static void lockSchedule(Task task, Setting setting, DateTime now, Date jobTime) {
        boolean isWaited = task.getStatus().equals(Status.WAITED);
        boolean isSchedule = setting.getSendCode().equals("schedule");
        if (isWaited && isSchedule) {
            if (jobTime != null) {
                int minutes = Minutes.minutesBetween(now, new DateTime(jobTime)).getMinutes();
                if (minutes < 5) {
                    throw new Errors("禁止修改已锁定的任务");
                }
            }
        }
    }
    
    /**
     * 校验定时发送的有效时间范围.
     */
    public static void jobTime(DateTime now, DateTime jobTime) {
        if (jobTime != null) {
            int minutes = Minutes.minutesBetween(now, jobTime).getMinutes();
            if (minutes < 5) {
                DateTime min = now.plusMinutes(6);
                throw new Errors("定时发送时间不能小于" + min.toString("yyyy-MM-dd HH:mm"));
            }
            
//            if (Auth.isAuth() && !Auth.expire(jobTime)) {
//                DateTimeFormatter fmt = DateTimeFormat.forPattern("yyyyMMdd");
//                DateTime expireTime = fmt.parseDateTime(MyCode.get(Auth.MAP.get(UrlMap.EXPIRE.getAction())));
//                throw new Errors("定时发送时间不能大于" + expireTime.toString("yyyy-MM-dd HH:mm"));
//            }
            
            if (Auth.isAuth()) {
                DateTimeFormatter fmt = DateTimeFormat.forPattern("yyyyMMdd");
                DateTime expireTime = fmt.parseDateTime(MyCode.get(Auth.MAP.get(UrlMap.EXPIRE.getAction())));
                throw new Errors("定时发送时间不能大于" + expireTime.toString("yyyy-MM-dd HH:mm"));
            }
        }
    }
    
    /**
     * 获取当前任务的邮箱地址数.
     */
    public static int holdCount(Integer status, Integer emailCount) {
        Integer[] STATUS = { 
                Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED,
                Status.QUEUEING, Status.PROCESSING, Status.PAUSED, Status.CANCELLED, 
                Status.COMPLETED
        };

        int holdCount = 0;
        if (Asserts.hasAny(Values.get(status), STATUS)) {
            holdCount = Values.get(emailCount);
        }

        return holdCount;
    }
    
    /**
     * 是否创建用于扫描文件的基础任务文件
     * @return
     */
    public static boolean createTask(Task task) {
    	Integer[] STATUS = { Status.DRAFTED, Status.PROOFING, Status.UNAPPROVED, Status.REVIEWING, Status.RETURN };
    	Asserts.isNull(task, "任务");
    	return !Asserts.hasAny(Values.get(task.getStatus()), STATUS) && task.getType()!=Type.PLAN;
    }
    
    /**
     * 是否创建用于扫描文件的基础周期文件
     * @return
     */
    public static boolean createPlan(Task task) {
    	Integer[] STATUS = { Status.DRAFTED, Status.PROOFING, Status.UNAPPROVED, Status.REVIEWING, Status.RETURN };
    	Asserts.isNull(task, "任务");
    	return !Asserts.hasAny(Values.get(task.getStatus()), STATUS);
    }
    
    /**
     * 从cookie中获取属性值，并封装为json格式字符串
     * @param request
     * @param map
     */
    public static void getCookies(HttpServletRequest request, ModelMap map, String entry, boolean isSender) {
    	String taskName = Values.get(Cookies.get(request, "taskName"), "");
    	String campId = Values.get(Cookies.get(request, "campaignId"), "");
    	String tempId = null;
    	if("task".equals(entry)) {
    		tempId = Values.get(Cookies.get(request, "templateId"), "");
    	}
    	String subject = Values.get(Cookies.get(request, "subject"), "");
    	String bindCorpId = Values.get(Cookies.get(request, "bindCorpId"), "");
    	String resourceId = Values.get(Cookies.get(request, "resourceId"), "");
    	String senderEmail = Values.get(Cookies.get(request, "senderEmail"), "");
    	String senderName = Values.get(Cookies.get(request, "senderName"), "");
    	String receiver = Values.get(Cookies.get(request, "receiver"), "");
    	String unsubscribeId = Values.get(Cookies.get(request, "unsubscribeId"), "");
    	StringBuffer sb = new StringBuffer("");
    	sb.append("{");
    	sb.append("\"taskName\":\"" + taskName + "\"");
    	sb.append(",");
    	sb.append("\"campaignId\":\"" + campId + "\"");
    	sb.append(",");
    	if("task".equals(entry)) {
	    	sb.append("\"templateId\":\"" + tempId + "\"");
	    	sb.append(",");
    	}
    	sb.append("\"subject\":\"" + subject + "\"");
    	sb.append(",");
    	sb.append("\"bindCorpId\":\"" + bindCorpId + "\"");
    	sb.append(",");
    	sb.append("\"resourceId\":\"" + resourceId + "\"");
    	sb.append(",");
    	if(!isSender){
    		sb.append("\"senderEmail\":\"" + senderEmail + "\"");
    		sb.append(",");
    		sb.append("\"senderName\":\"" + senderName + "\"");
    		sb.append(",");
    	}
    	sb.append("\"receiver\":\"" + receiver + "\"");
    	sb.append(",");
    	sb.append("\"unsubscribeId\":\"" + unsubscribeId + "\"");
    	sb.append("}");
    	map.put("cookieArr", sb.toString());
    }
    
}
