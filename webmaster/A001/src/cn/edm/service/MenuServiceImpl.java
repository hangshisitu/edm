package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.Menu;
import cn.edm.persistence.MenuMapper;

@Service
public class MenuServiceImpl implements MenuService {

	@Autowired
	private MenuMapper menuMapper;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Menu> getAllMenu() {		
		return menuMapper.selectMenuList();
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Menu> selectMenuByRoleId(int roleId) {		
		return menuMapper.selectMenuByRoleId(roleId);
	}

}
