package cn.edm.domain;

import java.util.Date;

import org.apache.commons.lang.StringUtils;

import cn.edm.constants.mapper.WeekMap;
import cn.edm.utils.Asserts;

/**
 * 计划.
 * corn(调度. 格式: 分 时 日 周 月, *表示不受限制, -1表示最后, 集合用","分隔.
 *      eg1:
 *          20 8 1 * 1
 *      eg2:
 *          20 8 1 1,2,3 *
 *      )
 * 
 * @author yjli
 */
public class Plan {

	private Integer planId;
    private Integer corpId;
    private String userId;
    private String cron;
    private Date beginTime;
    private Date endTime;

    private Integer taskCount;
    
    private String translateCron;
    
    private Integer touchCount;
    

	public String getTranslateCron() {
		return translateCron;
	}
    /**
     * 翻译周期cron.
     */
    public void setTranslateCron(String cron) {
        String cronStr = null;
        if (StringUtils.isNotBlank(cron)) {
            String[] crons = StringUtils.splitPreserveAllTokens(cron, " ");
            if (!Asserts.empty(crons) && crons.length > 4) {
            	String minute = crons[0];
            	String hour = crons[1];
            	String day = crons[2];
            	String week = crons[3];
            	String month = crons[4];
            	
            	// month
            	if (!day.equals("*")) {
            		cronStr = "每月";
            		if (day.equals("-1")) cronStr += "最后一天";
            		else cronStr += day + "日";
            	}
            	// week
            	if (!week.equals("*")) {
            		cronStr = "每周";
            		if (week.equals("1,2,3,4,5")) cronStr += "工作日（周一至周五）";
            		else if (week.equals("6,7")) cronStr += "休假（周六至周日）";
            		else if (StringUtils.splitPreserveAllTokens(week, ",").length == 1)
            			cronStr += "周" + WeekMap.week(week);
            	}
            	// day
            	if (month.equals("*") && week.equals("*") && day.equals("*")) {
            		cronStr = "每日";
            	}
            	
            	if (StringUtils.isNotBlank(cronStr)) {
            		if (hour.length() == 1) hour = "0" + hour;
            		if (minute.length() == 1) minute = "0" + minute;
            		cronStr += hour + ":" + minute;                
            	}
            }
        }
        this.translateCron = cronStr;
    }
    
    public Integer getPlanId() {
        return planId;
    }
    
    public void setPlanId(Integer planId) {
        this.planId = planId;
    }

    public Integer getCorpId() {
        return corpId;
    }
    
    public void setCorpId(Integer corpId) {
        this.corpId = corpId;
    }

    public String getUserId() {
        return userId;
    }
    
    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getCron() {
        return cron;
    }
    
    public void setCron(String cron) {
        this.cron = cron;
    }

    public Date getBeginTime() {
        return beginTime;
    }
    
    public void setBeginTime(Date beginTime) {
        this.beginTime = beginTime;
    }

    public Date getEndTime() {
        return endTime;
    }

    public void setEndTime(Date endTime) {
        this.endTime = endTime;
    }

    public Integer getTaskCount() {
        return taskCount;
    }

    public void setTaskCount(Integer taskCount) {
        this.taskCount = taskCount;
    }
	public Integer getTouchCount() {
		return touchCount;
	}
	public void setTouchCount(Integer touchCount) {
		this.touchCount = touchCount;
	}

}
