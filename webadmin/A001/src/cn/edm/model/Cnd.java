package cn.edm.model;

/**
 * 任务条件.
 * 
 * @author yjli
 */
public class Cnd {

	private Integer taskId;
	private String emailList;
	private String fileId;
	private String includeIds;
	private String excludeIds;
	private String includes;
	private String excludes;
	
	private Integer status;
	
	public Integer getTaskId() {
		return taskId;
	}

	public void setTaskId(Integer taskId) {
		this.taskId = taskId;
	}

	public String getEmailList() {
		return emailList;
	}

	public void setEmailList(String emailList) {
		this.emailList = emailList;
	}

	public String getFileId() {
		return fileId;
	}

	public void setFileId(String fileId) {
		this.fileId = fileId;
	}

	public String getIncludeIds() {
		return includeIds;
	}

	public void setIncludeIds(String includeIds) {
		this.includeIds = includeIds;
	}

	public String getExcludeIds() {
		return excludeIds;
	}

	public void setExcludeIds(String excludeIds) {
		this.excludeIds = excludeIds;
	}

    public String getIncludes() {
        return includes;
    }

    public void setIncludes(String includes) {
        this.includes = includes;
    }

    public String getExcludes() {
        return excludes;
    }

    public void setExcludes(String excludes) {
        this.excludes = excludes;
    }

    public Integer getStatus() {
        return status;
    }

    public void setStatus(Integer status) {
        this.status = status;
    }

}
