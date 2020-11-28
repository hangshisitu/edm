package cn.edm.service;


import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.google.common.collect.Maps;
import com.googlecode.ehcache.annotations.TriggersRemove;

import cn.edm.domain.Corp;
import cn.edm.persistence.CorpMapper;


/**
 * 企业业务接口实现
 * @author Luxh
 *
 */
@Service
public class CorpServiceImpl implements CorpService{
	
	@Autowired
	private CorpMapper corpMapper;
	
	@Override
	public Corp getByCorpId(Integer corpId) {
		return corpMapper.selectByCorpId(corpId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	@TriggersRemove(cacheName="resCache",removeAll=true)
	public void changeRes(Map<String, Object> paramsMap) {
		corpMapper.changeRes(paramsMap);
	}

	@Override
	public boolean isCorpExist(Map<String, Object> paramsMap) {
		Corp corp = corpMapper.isCorpExist(paramsMap);
		return corp != null?true:false;
	}

	@Override
	public void changeSenderValidate(Integer corpId, String senderValidate) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("corpId", corpId);
		paramsMap.put("senderValidate", senderValidate);
		corpMapper.changeSenderValid(paramsMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void deleteByCorpId(String corpId) {				
		corpMapper.deleteByCorpId(corpId);
		
		corpMapper.deleteCampaignByCorpId(corpId);
		corpMapper.deleteCategoryByCorpId(corpId);
		corpMapper.deleteFileByCorpId(corpId);
		corpMapper.deleteFilterByCorpId(corpId);
		corpMapper.deleteFormByCorpId(corpId);
		corpMapper.deleteLabelHistoryByCorpId(corpId);		
		
		corpMapper.deleteLoginHistoryByCorpId(corpId);
		corpMapper.deletePlanByCorpId(corpId);
		corpMapper.deletePropByCorpId(corpId);
		corpMapper.deleteResultByCorpId(corpId);
		corpMapper.deleteResultByBrowserByCorpId(corpId);		
		
		corpMapper.deleteResultByDomainByCorpId(corpId);
		corpMapper.deleteResultByHourByCorpId(corpId);
		corpMapper.deleteResultByLangByCorpId(corpId);
		corpMapper.deleteResultByOsByCorpId(corpId);
		corpMapper.deleteResultByRegionByCorpId(corpId);
		corpMapper.deleteResultByUrlByCorpId(corpId);
		corpMapper.deleteRobotByCorpId(corpId);
		
		corpMapper.deleteSelectionByCorpId(corpId);
		corpMapper.deleteSenderByCorpId(corpId);
		corpMapper.deleteStatHaviorByCorpId(corpId);
		corpMapper.deleteTagByCorpId(corpId);
		corpMapper.deleteTaskByCorpId(corpId);
		corpMapper.deleteTemplateByCorpId(corpId);
		
		corpMapper.deleteTemplateSettingByCorpId(corpId);
		corpMapper.deleteTouchByCorpId(corpId);
		corpMapper.deleteTrigerApiByCorpId(corpId);
		corpMapper.deleteUnsubscribeByCorpId(corpId);
		corpMapper.deleteUserByCorpId(corpId);		
		
	}

	@Override
	public List<Corp> getSubCorp(String corpId) {				
		return corpMapper.getSubCorp(corpId);
	}

	@Override
	public List<Corp> getAllCorp(){
		return corpMapper.getAllCorp();
	}

	
	
	@Override
	public List<Corp> getOneCorp() {
		return corpMapper.getOneCorp();
	}

	@Override
	public List<Corp> getCorpByTopLevel(Map<String, Object> paramsMap) {
		List<Corp> corpList = corpMapper.getCorpList(paramsMap);
		List<Corp> allCorpList = corpMapper.getAllCorp();
		for(Corp corp : corpList){
			List<Integer> branchCorp = new ArrayList<Integer>();
			branchCorp.add(corp.getCorpId());//加入一级机构id
			for(Corp subCorp : allCorpList){//遍历所有机构列表,加入从属子机构id
				if(branchCorp.contains(subCorp.getParentId())){
					branchCorp.add(subCorp.getCorpId());
				}			
			}
			corp.setBranchCorp(branchCorp);			
		}		
		return corpList;
	}

}
