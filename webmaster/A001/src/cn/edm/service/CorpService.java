package cn.edm.service;


import java.util.List;
import java.util.Map;

import cn.edm.domain.Corp;


/**
 * 企业业务接口
 * @author Luxh
 *
 */
public interface CorpService {
	
	/**
	 * 根据企业id获取企业
	 * @param corpId
	 * @return
	 */
	Corp getByCorpId(Integer corpId);
	
	/**
	 * 更改通道
	 * @param paramsMap
	 */
	void changeRes(Map<String,Object> paramsMap);
	
	boolean isCorpExist(Map<String, Object> paramsMap);
	
	void changeSenderValidate(Integer corpId,String senderValidate);
	
	void deleteByCorpId(String corpId);
	/**
	 * 获取子机构
	 */
	List<Corp> getSubCorp(String corpId);
	
	List<Corp> getAllCorp();
	List<Corp> getOneCorp();
	/**
	 * 通过最高层级机构id获取机构属性一级所属子机构id集合
	 */
	List<Corp> getCorpByTopLevel(Map<String, Object> paramsMap);
	

}
