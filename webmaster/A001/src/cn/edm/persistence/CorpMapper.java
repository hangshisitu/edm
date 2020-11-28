package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Corp;


public interface CorpMapper {
	
	/**
	 * 根据企业id查询企业
	 * @param corpId 
	 * @return
	 */
	Corp selectByCorpId(Integer corpId);
	
	/**
	 * 更新企业状态
	 * @param paramsMap
	 */
	void updateCorpStatus(Map<String, Object> paramsMap);	
	
	/**
	 * 添加
	 * @param corp
	 */
	void insertCorp(Corp corp);
	
	/**
	 * 更新
	 * @param corp
	 */
	void updateCorp(Corp corp);
	
	/**
	 * 更改通道
	 * @param paramsMap
	 */
	void changeRes(Map<String, Object> paramsMap);
	
	
	Corp isCorpExist(Map<String, Object> paramsMap);
	
	void changeSenderValid(Map<String, Object> paramsMap);
	/**
	 * 删除机构
	 * @param corpId
	 */
	void deleteByCorpId(String corpId);
	
	void deleteCampaignByCorpId(String corpId);
	void deleteCategoryByCorpId(String corpId);
	void deleteFileByCorpId(String corpId);
	void deleteFilterByCorpId(String corpId);
	void deleteFormByCorpId(String corpId);
	void deleteLabelHistoryByCorpId(String corpId);
	
	void deleteLoginHistoryByCorpId(String corpId);
	void deletePlanByCorpId(String corpId);
	void deletePropByCorpId(String corpId);
	void deleteResultByCorpId(String corpId);
	void deleteResultByBrowserByCorpId(String corpId);	
	
	void deleteResultByDomainByCorpId(String corpId);
	void deleteResultByHourByCorpId(String corpId);
	void deleteResultByLangByCorpId(String corpId);
	void deleteResultByOsByCorpId(String corpId);
	void deleteResultByRegionByCorpId(String corpId);
	void deleteResultByUrlByCorpId(String corpId);
	void deleteRobotByCorpId(String corpId);
	
	void deleteSelectionByCorpId(String corpId);
	void deleteSenderByCorpId(String corpId);
	void deleteStatHaviorByCorpId(String corpId);
	void deleteTagByCorpId(String corpId);
	void deleteTaskByCorpId(String corpId);
	void deleteTemplateByCorpId(String corpId);
	
	void deleteTemplateSettingByCorpId(String corpId);
	void deleteTouchByCorpId(String corpId);
	void deleteTrigerApiByCorpId(String corpId);
	void deleteUnsubscribeByCorpId(String corpId);
	void deleteUserByCorpId(String corpId);
	
	public List<Corp> getSubCorp(String corpId);
	
	public List<Corp> getAllCorp();
	
	public List<Corp> getOneCorp();
	
	public List<Corp> getCorpList(Map<String, Object> paramsMap);
	
	public long getCorpCount(Map<String, Object> paramsMap);
	
}
