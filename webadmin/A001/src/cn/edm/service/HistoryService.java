package cn.edm.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.History;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class HistoryService {

    @Autowired
    private Dao dao;

	public void save(int corpId, String userId, History history) {
		if (history.getHistoryId() != null) {
			// update
		} else {
			dao.save("History.save", history);
			dao.delete("History.delete", new MapBean("corpId", corpId, "userId", userId));
		}
	}

    public Page<History> search(Page<History> page, MapBean mb, int corpId, String userId) {
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.order(page, "loginTime", Page.DESC);
        return dao.find(page, mb, "History.count", "History.index");
    }
}
