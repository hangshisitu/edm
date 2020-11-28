package cn.edm.util;

public class Params {
	public static final String sid(Integer corpId, Integer taskId, Integer templateId) {
		return corpId + "." + taskId + "." + templateId;
	}
}
