package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Cos;
import cn.edm.util.Pagination;

/**
 * 套餐业务接口
 * @author Luxh
 *
 */
public interface CosService {
	
	/**
	 * 根据类型查找
	 * @param type
	 * @return
	 */
	List<Cos> getByType(int type);
	
	List<Cos> selectByTypeAndStatus(Map<String, Object> paramsMap);
	
	/**
	 * 查询所有
	 * @return
	 */
	List<Cos> selectAll();
	
	/**
	 * 根据id查询
	 * @param cosId
	 * @return
	 */
	Cos getCosByCosId(int cosId);
	
	/**
	 * 保存自定义套餐
	 * @param cos
	 */
	void save(Cos cos);
	
	
	/**
	 * 分页查询套餐
	 * @param paramsMap		查询参数
	 * @param orderBy		排序
	 * @param currentPage	当前页
	 * @param pageSize		每页显示数
	 * @return
	 */
	Pagination<Cos> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	void updateCosStatus(Map<String,Object> paramsMap);
	
	boolean deleteCosById(Map<String, Object> paramsMap);
	
}
