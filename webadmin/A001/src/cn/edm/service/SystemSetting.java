package cn.edm.service;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import cn.edm.consts.Status;
import cn.edm.model.CustomEnterprise;
@Service
public class SystemSetting{
	
	private static Logger log = LoggerFactory.getLogger(SystemSetting.class);
	
	@Autowired
	private CustomEnterpriseService customEnterpriseService;
	
	public  String getLogoUrl(){
		
		CustomEnterprise customEnterprise = customEnterpriseService.selectByCustomEnterpriseStatus(Status.ENABLED);
		if(customEnterprise == null){			
			return "";
		}else{
			return customEnterprise.getLogoUrl();
		}	
	}

}