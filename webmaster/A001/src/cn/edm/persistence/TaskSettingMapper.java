package cn.edm.persistence;


import cn.edm.domain.TaskSetting;

public interface TaskSettingMapper {
	
	
	TaskSetting selectByTaskId(int taskId);
	
	
}
