package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Lang;


public interface LangMapper {
	
	List<Lang> selectLang(Integer campaignId);
	
}
