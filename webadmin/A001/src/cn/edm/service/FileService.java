package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Cnds;
import cn.edm.model.File;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class FileService {

    @Autowired
    private Dao dao;

    public void save(File file) {
        if (file.getFileId() != null) {
            dao.update("File.update", file);
        } else {
            dao.save("File.save", file);
        }
    }

    public void delete(int corpId, String userId, Integer fileId) {
        dao.delete("File.delete", new MapBean("corpId", corpId, "userId", userId, "fileId", fileId));
    }

    public void delete(int corpId, String userId, Integer[] fileIds) {
        dao.delete("File.delete", new MapBean("corpId", corpId, "userId", userId, "fileIds", fileIds));
    }

    public File get(int corpId, String userId, Integer fileId) {
        return dao.get("File.query", new MapBean("corpId", corpId, "userId", userId, "fileId", fileId));
    }

	public List<File> find(int corpId, String userId, Integer[] fileIds) {
		return dao.find("File.query", new MapBean("corpId", corpId, "userId", userId, "fileIds", fileIds));
	}
	
    public Page<File> search(Page<File> page, MapBean mb, int corpId, String userId, String fileName, String beginTime, String endTime) {
        Pages.put(mb, "fileName", fileName);
        Pages.put(mb, "beginTime", beginTime);
        Pages.put(mb, "endTime", endTime);
        Pages.search(mb, page);
        Pages.put(mb, "corpId", corpId);
        Pages.put(mb, "userId", userId);
        Pages.put(mb, "nameCnd", Cnds.LIKE);
        Pages.order(page, "modifyTime", Page.DESC);
        return dao.find(page, mb, "File.count", "File.index");
    }
    
    public boolean unique(int corpId, String userId, String fileName, String orgFileName, String suff) {
        if (fileName == null || fileName.equals(orgFileName)) {
            return true;
        }
        
        long count = dao.countResult("File.count", new MapBean("corpId", corpId, "userId", userId, "fileName", fileName, "suff", suff, "nameCnd", Cnds.EQ));
        return count == 0;
    }
}
