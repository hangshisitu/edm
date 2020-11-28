package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Robot;

public interface RobotService {
	
	List<Robot> getRobotList(Map<String,Object> params);
	
}
