package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Cnd;





public interface CndMapper {
	
	List<Cnd> selectByTaskIds(Map<String,Object> params);
	
	
}
