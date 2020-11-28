package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.consts.Id;
import cn.edm.model.Resource;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class ResourceService {

    @Autowired
    private Dao dao;

    public Resource get(String resourceId) {
        return dao.get("Resource.query", new MapBean("resourceId", resourceId, "idCnd", Cnds.EQ));
    }

    public List<Resource> find() {
        return dao.find("Resource.query", new MapBean("resourceId", Id.API, "idCnd", Cnds.NOT_EQ));
    } 
}
