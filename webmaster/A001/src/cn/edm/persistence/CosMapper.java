package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Cos;

public interface CosMapper {
	
	/**
	 * 根据类型查找
	 * @param type
	 * @return
	 */
	List<Cos> selectByTypeAndStatus(Map<String, Object> paramsMap);
	
	/**
	 * 根据类型查找
	 * @param type
	 * @return
	 */
	List<Cos> selectByType(int type);
	
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
	Cos selectByCosId(int cosId);
	
	/**
	 * 保存自定义套餐
	 * @param cos
	 */
	void insertCos(Cos cos);
	
	/**
	 * 分页查询记录数据
	 * @param paramsMap
	 * @return
	 */
	List<Cos> selectCoses(Map<String, Object> paramsMap);
	
	/**
	 * 查询记录数
	 * @param paramsMap
	 * @return
	 */
	long selectCosesCount(Map<String, Object> paramsMap);
	
	/**
	 * 更新账号状态
	 * @param paramsMap
	 */
	void updateCosStatus(Map<String, Object> paramsMap);
	
	
	void deleteCosById(Map<String, Object> paramsMap);
}
