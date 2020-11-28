package cn.edm.service;

import java.util.List;
import java.util.Map;

import cn.edm.domain.Smtp;
import cn.edm.util.Pagination;

/**
 * Smtp接口
 * 
 * @Date 2014年6月3日 下午2:26:42
 * @author Lich
 *
 */
public interface SmtpService {
	/**
	 * 通过ID获得Smtp对象信息
	 * @param smtpId
	 * @return 返回Smtp对象信息
	 */
	public Smtp getSmtpById(Map<String,Object> paramsMap);
	/**
	 * 通过IP获得Smtp列表信息
	 * @param ip
	 * @return Smtp列表信息
	 */
	public List<Smtp> getSmtpsByIp(Map<String,Object> paramsMap);
	/**
	 * 分页查询
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return pagination
	 */
	public Pagination<Smtp> getSmtpPage(Map<String,Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 保存或更新Smtp
	 * @param smtp
	 */
	public void saveOrUpdateSmtp(Smtp smtp);
	/**
	 * 通过ID 删除SMTP
	 * @param paramsMap
	 */
	public void deleteSmtpById(Map<String, Object> paramsMap);

}
