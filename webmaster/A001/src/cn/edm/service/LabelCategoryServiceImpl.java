package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.LabelCategory;
import cn.edm.persistence.LabelCategoryMapper;

@Service
public class LabelCategoryServiceImpl implements LabelCategoryService{
	
	@Autowired
	private LabelCategoryMapper labelCategoryMapper;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<LabelCategory> getAll() {
		return labelCategoryMapper.selectAll();
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public LabelCategory getById(Integer id) {
		return labelCategoryMapper.selectById(id);
	}

}
