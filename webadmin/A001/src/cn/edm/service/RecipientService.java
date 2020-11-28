package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Value;
import cn.edm.model.Recipient;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.Values;

import com.google.common.collect.Lists;

@Transactional
@Service
public class RecipientService {

	@Autowired
	private Dao dao;
	
	public void save(Integer corpId, int maxDbId, List<Integer> dbIds) {
		List<Recipient> recipientList = Lists.newArrayList();
		
		dbIds.add(maxDbId);
		for (Integer dbId : dbIds) {
			Recipient mapper = new Recipient();
			mapper.setCorpId(corpId);
			mapper.setDbId(dbId);
			recipientList.add(mapper);
		}
		
		dao.save("Recipient.batchSave", recipientList);
	}
	
	public List<Recipient> find(String userId) {
		return dao.find("Recipient.query", new MapBean("userId", userId));
	}
	
	public int maxDbId() {
		int maxDbId = Values.get((Integer) dao.get("Recipient.maxDbId", Value.I));
		return maxDbId > 0 ? maxDbId : 1;
	}
}
