package cn.edm.domain;

import java.io.Serializable;
import java.util.Date;
import java.util.List;
import java.util.Set;

/**
 * 企业.
 * auditPath(审核路径. 空值:不需要上级审核, 非空值:需要上级审核. eg. /1/100/需要100和1的企业ID审核)
 * 
 * @author yjli
 */
public class Corp implements Serializable{

	private static final long serialVersionUID = 2394815054043085383L;
	private Integer corpId;
	private Integer parentId;//父机构
    private Integer cosId;//套餐id
    private String company;//机构名称
    private String corpPath;//机构全称
    private String website;//网站名称
    private String address;//联系地址
    private String contact;//联系人
    private String email;//邮箱
    private String telephone;//电话
    private String mobile;//手机号码
    private String fax;//传真
    private String zip;//邮编
    private String industry;//行业
    private String emailQuantity;//邮箱数量
    private String sendQuantity;//月发送量
    private String understand;//了解途径
    private String promise;//保证
    private String agreement;//协议
    private String way;//发现途径
    private String auditPath;//审核路径
    private String formalId;//正式发送id
    private String testedId;//测试发送id
    private Integer status;//0=无效,1=有效,2=冻结,3=禁用
    private Date createTime;//创建时间
    private Date expireTime;//有效时间
    
    private String isNeedAudit;//系统侧审核,1=需要，0=不需要
    private String managerId;//建机构的管理员账号
    private Date modifyTime;//
    private String exist;    	
    
    private Integer isTriggerApi = 0; // 触发：0为否，1为是

    private Integer isGroupsApi = 0; // 群发：0为否，1为是
    
    private String[] robots;
    
    private Integer openSms;
    
    private List<Integer> branchCorp;//该机构与从属子机构id的集合
    
        
	public List<Integer> getBranchCorp() {
		return branchCorp;
	}

	public void setBranchCorp(List<Integer> branchCorp) {
		this.branchCorp = branchCorp;
	}

	public String[] getRobots() {
		return robots;
	}

	public void setRobots(String[] robots) {
		this.robots = robots;
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

	public String getExist() {
		return exist;
	}

	public void setExist(String exist) {
		this.exist = exist;
	}

	/**
     * 发件人验证,1=需要，0=不需要
     */
    private Integer senderValidate;

    
    private List<User> userList;
    
    private Integer joinApi; // 是否开通API群发功能：0为否，1为是
    
    public String getManagerId() {
		return managerId;
	}

	public void setManagerId(String managerId) {
		this.managerId = managerId;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}
   
    public Integer getParentId() {
		return parentId;
	}

	public void setParentId(Integer parentId) {
		this.parentId = parentId;
	}

	public String getCorpPath() {
		return corpPath;
	}

	public void setCorpPath(String corpPath) {
		this.corpPath = corpPath;
	}

	public Integer getSenderValidate() {
		return senderValidate;
	}

	public void setSenderValidate(Integer senderValidate) {
		this.senderValidate = senderValidate;
	}

	public String getIsNeedAudit() {
		return isNeedAudit;
	}

	public void setIsNeedAudit(String isNeedAudit) {
		this.isNeedAudit = isNeedAudit;
	}

	public Integer getCorpId() {
        return corpId;
    }
    
    public void setCorpId(Integer corpId) {
        this.corpId = corpId;
    }

    public Integer getCosId() {
        return cosId;
    }
    
    public void setCosId(Integer cosId) {
        this.cosId = cosId;
    }

    public String getCompany() {
        return company;
    }
    
    public void setCompany(String company) {
        this.company = company;
    }

    public String getWebsite() {
        return website;
    }
    
    public void setWebsite(String website) {
        this.website = website;
    }

    public String getAddress() {
        return address;
    }
    
    public void setAddress(String address) {
        this.address = address;
    }

    public String getContact() {
        return contact;
    }
    
    public void setContact(String contact) {
        this.contact = contact;
    }

    public String getEmail() {
        return email;
    }
    
    public void setEmail(String email) {
        this.email = email;
    }

    public String getTelephone() {
        return telephone;
    }
    
    public void setTelephone(String telephone) {
        this.telephone = telephone;
    }

    public String getMobile() {
        return mobile;
    }
    
    public void setMobile(String mobile) {
        this.mobile = mobile;
    }

    public String getFax() {
        return fax;
    }
    
    public void setFax(String fax) {
        this.fax = fax;
    }

    public String getZip() {
        return zip;
    }
    
    public void setZip(String zip) {
        this.zip = zip;
    }

    public String getIndustry() {
        return industry;
    }
    
    public void setIndustry(String industry) {
        this.industry = industry;
    }

    public String getEmailQuantity() {
        return emailQuantity;
    }
    
    public void setEmailQuantity(String emailQuantity) {
        this.emailQuantity = emailQuantity;
    }

    public String getSendQuantity() {
        return sendQuantity;
    }
    
    public void setSendQuantity(String sendQuantity) {
        this.sendQuantity = sendQuantity;
    }

    public String getUnderstand() {
        return understand;
    }
    
    public void setUnderstand(String understand) {
        this.understand = understand;
    }

    public String getPromise() {
        return promise;
    }
    
    public void setPromise(String promise) {
        this.promise = promise;
    }

    public String getAgreement() {
        return agreement;
    }
    
    public void setAgreement(String agreement) {
        this.agreement = agreement;
    }

    public String getWay() {
        return way;
    }
    
    public void setWay(String way) {
        this.way = way;
    }

    public String getAuditPath() {
        return auditPath;
    }
    
    public void setAuditPath(String auditPath) {
        this.auditPath = auditPath;
    }

    public String getFormalId() {
        return formalId;
    }
    
    public void setFormalId(String formalId) {
        this.formalId = formalId;
    }

    public String getTestedId() {
        return testedId;
    }
    
    public void setTestedId(String testedId) {
        this.testedId = testedId;
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

    public List<User> getUserList() {
        return userList;
    }

    public void setUserList(List<User> userList) {
        this.userList = userList;
    }

	public Integer getJoinApi() {
		return joinApi;
	}

	public void setJoinApi(Integer joinApi) {
		this.joinApi = joinApi;
	}

}
