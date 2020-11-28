package cn.edm.service;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Sender;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class SenderService {

    @Autowired
    private Dao dao;

    public void save(Sender sender, String orgSenderEmail) {
        if (StringUtils.isNotBlank(orgSenderEmail)) {
            dao.update("Sender.update", sender);
        } else {
            dao.save("Sender.save", sender);
        }
    }
    
	public void batchUpdate(Integer corpId, String userId) {
		dao.update("Sender.batchUpdate", new MapBean("corpId", corpId, "userId", userId));
	}

    public void delete(String userId, String senderEmail) {
        dao.delete("Sender.delete", new MapBean("userId", userId, "senderEmail", senderEmail));
    }
    
    public Sender get(Integer corpId, String userId, String senderEmail) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        mb.put("senderEmail", senderEmail);
        return dao.get("Sender.query", mb);
    }

    public List<Sender> find(Integer corpId, Integer status) {
    	MapBean mb = new MapBean();
    	Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "status", status);
    	Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Sender.query", mb);
    } 

    public boolean unique(Integer corpId, String senderEmail, String orgSenderEmail) {
        if (senderEmail == null || senderEmail.equals(orgSenderEmail)) {
            return true;
        }
        
        long count = dao.countResult("Sender.count", new MapBean("corpId", corpId, "senderEmail", senderEmail));
        return count == 0;
    }
    
	public boolean max(Integer corpId, String userId, int max) {
		long count = dao.countResult("Sender.count", new MapBean("corpId", corpId, "userId", userId));
		return count < max;
	}
	
	public long countResult(Integer corpId, String userId, Integer status) {
		return dao.countResult("Sender.count", new MapBean("corpId", corpId, "userId", userId, "status", status));
	}
}
