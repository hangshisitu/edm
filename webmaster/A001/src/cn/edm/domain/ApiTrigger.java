package cn.edm.domain;
/**
 * API 触发
 * @author Luxh
 *
 */
public class ApiTrigger {
	
	private Integer triggerId; // 映射ID，与triger表和result表的corp_id对应，不是企业ID
	
	private Integer corpId; // 企业ID
	
	private String userId;
	
	private String triggerName;
	
	private Integer status; // 是否启用触发：0为禁用，1为启用

	public Integer getTriggerId() {
		return triggerId;
	}

	public void setTriggerId(Integer triggerId) {
		this.triggerId = triggerId;
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

	public String getTriggerName() {
		return triggerName;
	}

	public void setTriggerName(String triggerName) {
		this.triggerName = triggerName;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}
}
