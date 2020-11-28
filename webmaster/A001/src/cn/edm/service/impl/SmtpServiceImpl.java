package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.domain.Smtp;
import cn.edm.persistence.SmtpMapper;
import cn.edm.service.SmtpService;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;

@Service
public class SmtpServiceImpl implements SmtpService {
	
	@Autowired
	private SmtpMapper smtpMapper;

	@Override
	public Smtp getSmtpById(Map<String,Object> paramsMap) {
		return smtpMapper.selectSmtpById(paramsMap);
	}

	@Override
	public List<Smtp> getSmtpsByIp(Map<String, Object> paramsMap) {
		
		return smtpMapper.selectSmtpsByIp(paramsMap);
	}

	@Override
	public void saveOrUpdateSmtp(Smtp smtp) {
		if(smtp.getId() != null){
			smtpMapper.updateSmtp(smtp);
		}else{
			smtpMapper.insertSmtp(smtp);
		}
	}

	@Override
	public Pagination<Smtp> getSmtpPage(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		List<Smtp> smtps = smtpMapper.selectSmtpList(paramsMap);
		Long recordCount = smtpMapper.selectSmtpCount(paramsMap);
		Pagination<Smtp> smtpPage = new Pagination<Smtp>(currentPage, pageSize, recordCount, smtps);
		return smtpPage;
	}

	@Override
	public void deleteSmtpById(Map<String, Object> paramsMap) {
		smtpMapper.deleteSmtpById(paramsMap);
	}

}
