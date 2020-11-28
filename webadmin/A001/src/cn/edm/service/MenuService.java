package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Menu;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class MenuService {
	
	@Autowired
    private Dao dao;
	
	public List<Menu> find() {
        MapBean mb = new MapBean();
        return dao.find("Menu.joinRM", mb);
    }
	
	public List<Menu> find(Integer[] menuIds) {
		MapBean mb = new MapBean();
		mb.put("menuIds", menuIds);
		return dao.find("Menu.query", mb);
	}
	
	public List<Menu> find(Integer roleId) {
		MapBean mb = new MapBean();
		mb.put("roleId", roleId);
		return dao.find("Menu.joinRM", mb);
	}

}
