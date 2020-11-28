package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Robot;
import cn.edm.persistence.RobotMapper;
@Service
public class RobotServiceImpl implements RobotService{
	
	@Autowired
	private RobotMapper robotMapper;

	@Override
	public List<Robot> getRobotList(Map<String, Object> params) {
		return robotMapper.selectRobotList(params);
	}

}
