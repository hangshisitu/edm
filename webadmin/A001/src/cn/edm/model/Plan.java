package cn.edm.model;

import java.util.Date;

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
    
    private Integer touchCount;
    
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
