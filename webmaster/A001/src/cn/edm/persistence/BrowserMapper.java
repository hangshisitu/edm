package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Browser;


public interface BrowserMapper {
	
	List<Browser> selectBrowser(Integer campaignId);
	
}
