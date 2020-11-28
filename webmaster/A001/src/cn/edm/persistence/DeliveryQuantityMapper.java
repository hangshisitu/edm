package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Cos;

public interface DeliveryQuantityMapper {
	
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
	
	
}
