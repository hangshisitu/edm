package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Resource;

public interface ResourceMapper {
	
	/**
	 * 查询所有
	 * @return
	 */
	List<Resource> selectAll();
	
	String selectHeloList(String resourceId);
	
	public Resource selectById(String resourceId);
	
}
