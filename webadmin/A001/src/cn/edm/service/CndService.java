package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Cnd;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;

@Transactional
@Service
public class CndService {

    @Autowired
    private Dao dao;
    
	public Cnd get(Integer taskId) {
		return dao.get("Cnd.query", new MapBean("taskId", taskId));
	}
	
	public List<Cnd> find(List<String> fileIds) {
        return dao.find("Cnd.query", new MapBean("fileIds", fileIds));
    }
}
