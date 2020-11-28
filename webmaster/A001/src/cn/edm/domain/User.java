package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;

/**
 * 用户
 * @author Luxh
 */
public class User implements Serializable{
	
	private static final long serialVersionUID = 1922085053903136015L;

	public static final String AUTHORITY_PREFIX = "ROLE_";
	
	/**用户id*/
    private String userId;
    
    /**父id*/
    private String managerId;    	

	/**企业id*/
    private Integer corpId;
    
    /**角色id*/
    private Integer roleId;
    
    /**密码*/
    private String password;
    
    /**邮箱*/
    private String email;
    
    /**姓名*/
    private String trueName;
    
    /**联系电话*/
    private String phone;
    
    /**职位*/
    private String job;
    
    /**忘记密码验证*/
    private String randCode;
    
    /**状态 0-无效、1-有效、2-冻结、3-禁用*/
    private Integer status;
    
    /**创建时间*/
    private Date createTime;
    
    /**更新时间**/
    private Date updateTime;        

	/**过期时间*/
    private Date expireTime;
    
    /**公司名称*/
    private String company;
    
    /**注册方式*/
    private String way;
    
    /**投递通道*/
    private String formalId;
    
    /**套餐类型*/
    private String cosName;
    
    /**子账号数量*/
    private Integer subUserCount;
    
    /**通道名称*/
    private String formalName;
    
    private String cosId;
    
    
    private String isNeedApi;
    
    private String triggerType;
    
    private String otherTriggerType;
    
    private String contact;
    
    private String customerType;
    
    private Integer moderate;
    
    private String[] robots;
    
    private Integer openSms;
    
    private Integer isTriggerApi = 0; // 触发：0为否，1为是

    private Integer isGroupsApi = 0; // 群发：0为否，1为是
    
    private String roleName;//角色名称
    
    private String corpPath;//机构全称

    public String getCorpPath() {
		return corpPath;
	}

	public void setCorpPath(String corpPath) {
		this.corpPath = corpPath;
	}

	public String getManagerId() {
		return managerId;
	}

	public void setManagerId(String managerId) {
		this.managerId = managerId;
	}
	
    public Date getUpdateTime() {
		return updateTime;
	}

	public void setUpdateTime(Date updateTime) {
		this.updateTime = updateTime;
	}
    
	public String getRoleName() {
		return roleName;
	}

	public void setRoleName(String roleName) {
		this.roleName = roleName;
	}

	public String[] getRobots() {
		return robots;
	}

	public void setRobots(String[] robots) {
		this.robots = robots;
	}

	public Integer getModerate() {
		return moderate;
	}

	public void setModerate(Integer moderate) {
		this.moderate = moderate;
	}

	public String getCustomerType() {
		return customerType;
	}

	public void setCustomerType(String customerType) {
		this.customerType = customerType;
	}

	public String getContact() {
		return contact;
	}

	public void setContact(String contact) {
		this.contact = contact;
	}

	public String getIsNeedApi() {
		return isNeedApi;
	}

	public void setIsNeedApi(String isNeedApi) {
		this.isNeedApi = isNeedApi;
	}

	public String getTriggerType() {
		return triggerType;
	}

	public void setTriggerType(String triggerType) {
		this.triggerType = triggerType;
	}

	public String getOtherTriggerType() {
		return otherTriggerType;
	}

	public void setOtherTriggerType(String otherTriggerType) {
		this.otherTriggerType = otherTriggerType;
	}

	public String getCosId() {
		return cosId;
	}

	public void setCosId(String cosId) {
		this.cosId = cosId;
	}

	public String getFormalName() {
		return formalName;
	}

	public void setFormalName(String formalName) {
		this.formalName = formalName;
	}

	private String confirmPassword;
    
    private String exist;
    
    private String auditPath;

    
    public String getAuditPath() {
		return auditPath;
	}

	public void setAuditPath(String auditPath) {
		this.auditPath = auditPath;
	}

	@Override
   	public boolean equals(Object obj) {
   		// TODO Auto-generated method stub
       	User user = (User) obj;
   		return this.userId.equals(user.getUserId());
   	}

	public String getExist() {
		return exist;
	}

	public void setExist(String exist) {
		this.exist = exist;
	}

	public String getConfirmPassword() {
		return confirmPassword;
	}

	public void setConfirmPassword(String confirmPassword) {
		this.confirmPassword = confirmPassword;
	}

	public Integer getSubUserCount() {
		return subUserCount;
	}

	public void setSubUserCount(Integer subUserCount) {
		this.subUserCount = subUserCount;
	}

	public String getWay() {
		return way;
	}

	public void setWay(String way) {
		this.way = way;
	}

	public String getFormalId() {
		return formalId;
	}

	public void setFormalId(String formalId) {
		this.formalId = formalId;
	}

	public String getCosName() {
		return cosName;
	}

	public void setCosName(String cosName) {
		this.cosName = cosName;
	}

	public String getCompany() {
		return company;
	}

	public void setCompany(String company) {
		this.company = company;
	}

	public String getUserId() {
        return userId;
    }
    
    public void setUserId(String userId) {
        this.userId = userId;
    }

    public Integer getCorpId() {
        return corpId;
    }
    
    public void setCorpId(Integer corpId) {
        this.corpId = corpId;
    }

    public Integer getRoleId() {
        return roleId;
    }
    
    public void setRoleId(Integer roleId) {
        this.roleId = roleId;
    }

    public String getPassword() {
        return password;
    }
    
    public void setPassword(String password) {
        this.password = password;
    }

    public String getEmail() {
        return email;
    }
    
    public void setEmail(String email) {
        this.email = email;
    }

    public String getTrueName() {
        return trueName;
    }
    
    public void setTrueName(String trueName) {
        this.trueName = trueName;
    }

    public String getPhone() {
        return phone;
    }
    
    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getJob() {
        return job;
    }
    
    public void setJob(String job) {
        this.job = job;
    }

    public String getRandCode() {
        return randCode;
    }
    
    public void setRandCode(String randCode) {
        this.randCode = randCode;
    }

    public Integer getStatus() {
        return status;
    }
    
    public void setStatus(Integer status) {
        this.status = status;
    }

    public Date getCreateTime() {
        return createTime;
    }
    
    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }

    public Date getExpireTime() {
        return expireTime;
    }
    
    public void setExpireTime(Date expireTime) {
        this.expireTime = expireTime;
    }

    public Integer getOpenSms() {
        return openSms;
    }

    public void setOpenSms(Integer openSms) {
        this.openSms = openSms;
    }

	public Integer getIsTriggerApi() {
		return isTriggerApi;
	}

	public void setIsTriggerApi(Integer isTriggerApi) {
		this.isTriggerApi = isTriggerApi;
	}

	public Integer getIsGroupsApi() {
		return isGroupsApi;
	}

	public void setIsGroupsApi(Integer isGroupsApi) {
		this.isGroupsApi = isGroupsApi;
	}

}
