package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Resource;
import cn.edm.persistence.ResourceMapper;

/**
 * 通道业务接口实现
 * @author Luxh
 *
 */
@Service
public class ResourceServiceImpl implements ResourceService{
	
	@Autowired
	private ResourceMapper resourceMapper;
	
	@Override
	public List<Resource> getAll() {
		return resourceMapper.selectAll();
	}

	@Override
	public Resource getById(String resourceId) {
		return resourceMapper.selectById(resourceId);
	}

}
