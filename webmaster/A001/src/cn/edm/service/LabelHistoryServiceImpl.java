package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.google.common.collect.Maps;

import cn.edm.constant.PropertyConstant;
import cn.edm.domain.LabelHistory;
import cn.edm.persistence.LabelHistoryMapper;
import cn.edm.persistence.TaskMapper;

@Service
public class LabelHistoryServiceImpl implements LabelHistoryService{

	@Autowired
	private LabelHistoryMapper labelHistoryMapper;
	
	@Autowired
	private TaskMapper taskMapper;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void save(LabelHistory labelHistory) {
		labelHistoryMapper.insert(labelHistory);
		Map<String,Object> params = Maps.newHashMap();
		params.put("corpId", labelHistory.getCorpId());
		params.put("taskId", labelHistory.getTaskId());
		params.put("templateId", labelHistory.getTemplateId());
		params.put("labelStatus", PropertyConstant.HANDLING);
		taskMapper.updateLabelStatus(params);
		
	}

	@Override
	public List<LabelHistory> getLabelHistory(Map<String, Object> params) {
		return labelHistoryMapper.selectList(params);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void updateStatus(Map<String, Object> params) {
		labelHistoryMapper.updateStatus(params);
		taskMapper.updateLabelStatus(params);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void updateStatusAndLabel(Map<String, Object> params) {
		labelHistoryMapper.updateStatusAndLabel(params);
		taskMapper.updateLabelStatus(params);
	}

	

}
