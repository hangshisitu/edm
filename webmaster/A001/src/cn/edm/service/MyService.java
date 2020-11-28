package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class MyService {

    @Autowired
    private Dao dao;

    public List<User> find(Integer status) {
        MapBean mb = new MapBean();
        mb.put("status", status);
        Pages.order(mb, "company", "asc");
        return dao.find("User.query", mb);
    }

    public User getBSN(Integer roleId, Integer status) {
        MapBean mb = new MapBean();
        mb.put("roleId", roleId, "status", status);
        Pages.order(mb, "userId", "asc");
        List<User> userList = dao.find("User.query", mb);
        if (userList != null && userList.size() >0) {
            return userList.get(0);
        }
        return null;
    }
}
