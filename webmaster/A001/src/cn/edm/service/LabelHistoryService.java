package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.LabelHistory;

public interface LabelHistoryService {
	
	void save(LabelHistory labelHistory);
	
	List<LabelHistory> getLabelHistory(Map<String,Object> params);
	
	void updateStatus(Map<String,Object> params);
	
	void updateStatusAndLabel(Map<String,Object> params);
	
}
