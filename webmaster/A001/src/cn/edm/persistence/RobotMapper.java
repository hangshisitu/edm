package cn.edm.persistence;


import java.util.List;
import java.util.Map;

import cn.edm.domain.Robot;



public interface RobotMapper {
	
	/**
	 * 添加
	 * @param corp
	 */
	void insertRobot(Robot robot);
	
	void insertBatch(List<Robot> robotList);
	
	List<Robot> selectRobotList(Map<String,Object> params);
	
	void deleteByCorpId(Map<String,Object> params);
}
