package cn.edm.repository;

import static cn.edm.consts.mapper.CategoryMap.BILL;
import static cn.edm.consts.mapper.CategoryMap.DEFAULT;
import static cn.edm.consts.mapper.CategoryMap.DELIVERY;
import static cn.edm.consts.mapper.CategoryMap.FILE;
import static cn.edm.consts.mapper.CategoryMap.FORM;
import static cn.edm.consts.mapper.CategoryMap.TEST;
import static cn.edm.consts.mapper.CategoryMap.TOUCH;
import static cn.edm.consts.mapper.CategoryMap.UNSUBSCRIBE;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Value;
import cn.edm.model.Category;

import com.google.common.collect.Lists;

@Repository
public class CategoryDao {
	
	@Autowired
	private Dao dao;
	
	public void save(int corpId) {
		List<Category> categories = Lists.newArrayList();
		categories.add(category(corpId, null, DELIVERY.getName(), DELIVERY.getDesc(), Category.TAG));
		categories.add(category(corpId, null, TEST.getName(), TEST.getDesc(), Category.TAG));
		categories.add(category(corpId, null, UNSUBSCRIBE.getName(), UNSUBSCRIBE.getDesc(), Category.TAG));
        categories.add(category(corpId, null, FORM.getName(), FORM.getDesc(), Category.TAG));
        categories.add(category(corpId, null, FILE.getName(), FILE.getDesc(), Category.TAG));
        categories.add(category(corpId, null, TOUCH.getName(), TOUCH.getDesc(), Category.TEMPLATE));
        categories.add(category(corpId, null, BILL.getName(), BILL.getDesc(), Category.TEMPLATE));
        categories.add(category(corpId, null, DEFAULT.getName(), DEFAULT.getDesc(), Category.TEMPLATE));
        dao.save("Category.batchSave", categories);
    }
	
    private Category category(int corpId, String userId, String categoryName, String categoryDesc, int type) {
		Category category = new Category();
		category.setCorpId(corpId);
		category.setUserId(userId);
		category.setCategoryName(categoryName);
		category.setCategoryDesc(categoryDesc);
		category.setType(type);
		category.setCount(Value.I);
		return category;
	}
}
