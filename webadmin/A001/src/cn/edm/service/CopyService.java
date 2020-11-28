package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Copy;
import cn.edm.repository.Dao;

@Transactional
@Service
public class CopyService {

    @Autowired
    private Dao dao;

    public void save(String name, Copy copy) {
        if (copy != null) {
            dao.save("Copy" + name + ".save", copy);
        }
    }

    public void batchSave(String name, List<Copy> copyList) {
        if (copyList != null && copyList.size() > 0) {
            dao.save("Copy" + name + ".batchSave", copyList);
        }
    }
}
