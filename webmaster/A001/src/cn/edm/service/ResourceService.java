package cn.edm.service;

import java.util.List;

import cn.edm.domain.Resource;

/**
 * 通道业务接口
 * @author Luxh
 */
public interface ResourceService {
	
	/**
	 * 查询所有通道
	 * @return
	 */
	List<Resource> getAll();
	
	public Resource getById(String resourceId);
	
}
