package cn.edm.persistence;


import java.util.List;
import java.util.Map;

import cn.edm.domain.LabelHistory;

public interface LabelHistoryMapper {
	
	void insert(LabelHistory labelHistory);
	
	List<LabelHistory> selectList(Map<String,Object> params);
	
	void updateStatus(Map<String,Object> params);
	
	void updateStatusAndLabel(Map<String,Object> params);
	
}
