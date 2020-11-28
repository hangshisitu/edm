package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Robot;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class RobotService {

	@Autowired
	private Dao dao;

	public Robot get(int corpId, String email) {
		return dao.get("Robot.query", new MapBean("corpId", corpId, "email", email));
	}

	public List<Robot> find(int corpId) {
		return dao.find("Robot.query", new MapBean("corpId", corpId));
	}
}
