package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Status;
import cn.edm.model.Triger;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class TrigerService {

	@Autowired
	private Dao dao;

    public void saveApi(Triger triger) {
        dao.save("TrigerApi.save", triger);
    }
    
	public Triger get(Integer[] corpIds, Integer corpId, Integer taskId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "taskId", taskId);
		return dao.get("Triger.query", mb);
	}
	
	public Triger getApi(Integer[] corpIds, Integer corpId, Integer trigerCorpId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "trigerCorpId", trigerCorpId);
        return dao.get("Triger.queryApi", mb);
    }
	
	public List<Triger> findApi(Integer[] corpIds, Integer corpId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpIds", corpIds);
		Pages.put(mb, "corpId", corpId);
		return dao.find("Triger.queryApi", mb);
	}

	public boolean isApi(int corpId, String userId) {
		MapBean mb = new MapBean();
		Pages.put(mb, "corpId", corpId);
		Pages.put(mb, "userId", userId);
		Pages.put(mb, "status", Status.ENABLED);
		long count = dao.countResult("Triger.countApi", mb);
		return count > 0;
	}
}
