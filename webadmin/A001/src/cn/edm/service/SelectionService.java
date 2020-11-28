package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Status;
import cn.edm.consts.mapper.SelectionMap;
import cn.edm.model.Selection;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class SelectionService {

    @Autowired
    private Dao dao;

    public void save(Selection selection) {
        if (selection.getSelectionId() != null) {
            dao.update("Selection.update", selection);
        } else {
            dao.save("Selection.save", selection);
        }
    }

    public Selection get(int corpId, String userId, Integer selectionId) {
        return dao.get("Selection.query", new MapBean("corpId", corpId, "userId", userId, "selectionId", selectionId));
    }
    
    public Selection get(Integer selectionId) {
        return dao.get("Selection.query", new MapBean("selectionId", selectionId));
    }

    public void delete(String userId, int day) {
        dao.delete("Selection.delete", new MapBean("userId", userId, "day", day));
    }
    
    public Page<Selection> search(Page<Selection> page, MapBean mb, int corpId, String userId) {
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "Selection.count", "Selection.index");
    }
    
    public long processingCount(int corpId, String userId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        mb.put("userId", userId);
        mb.put("status", Status.DISABLED);
        return dao.countResult("Selection.count", mb);
    }
    
    public List<Selection> find(int corpId, String userId) {
        MapBean mb = new MapBean();
        mb.put("corpId", corpId);
        mb.put("userId", userId);
        mb.put("type", SelectionMap.EXPORT.getId());
        mb.put("status", Status.ENABLED);
        Pages.order(mb, "modifyTime", Page.DESC);
        return dao.find("Selection.query", mb);
    }
    
    public List<Integer> scan() {
        return dao.find("Selection.scan");
    }
    
    public List<Integer> scan(List<Integer> selectionIds) {
    	return dao.find("Selection.scan", new MapBean("selectionIds", selectionIds));
    }
    
    public List<Selection> find(List<String> fileIds) {
        return dao.find("Selection.query", new MapBean("fileIds", fileIds));
    }
}
