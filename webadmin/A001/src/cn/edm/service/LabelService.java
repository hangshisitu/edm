package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Label;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class LabelService {

	@Autowired
	private Dao dao;

    public List<Label> find() {
        MapBean mb = new MapBean();
        Pages.order(mb, "categoryId", Page.ASC);
        return dao.find("Label.query", mb);
    }

}
