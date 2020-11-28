package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Smtp;

/**
 * 
 * @Date 2014年6月3日 上午11:41:21
 * @author Lich
 *
 */
public interface SmtpMapper {
	/**
	 * 通过ID获得Smtp对象信息
	 * @param smtpId
	 * @return 返回Smtp对象信息
	 */
	public Smtp selectSmtpById(Map<String, Object> paramsMap);
	/**
	 * 通过IP获得Smtp列表信息
	 * @param paramsMap
	 * @return 返回Smtp列表信息
	 */
	public List<Smtp> selectSmtpsByIp(Map<String, Object> paramsMap);
	/**
	 * 查询记录列表
	 * @param paramsMap
	 * @return Smtp列表
	 */
	public List<Smtp> selectSmtpList(Map<String, Object> paramsMap);
	/**
	 * 查询记录总数
	 * @param paramsMap
	 * @return 记录总数
	 */
	public Long selectSmtpCount(Map<String, Object> paramsMap);
	/**
	 * 保存Smtp
	 * @param smtp
	 */
	public void insertSmtp(Smtp smtp);
	/**
	 * 更新Smtp
	 * @param smtp
	 */
	public void updateSmtp(Smtp smtp);
	/**
	 * 通过ID 删除SMTP
	 * @param paramsMap
	 */
	public void deleteSmtpById(Map<String, Object> paramsMap);
	
}
