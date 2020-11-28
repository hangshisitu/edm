package cn.edm.persistence;

import java.util.List;

import cn.edm.domain.Os;

public interface OsMapper {
	
	
	List<Os> selectOs(Integer campaignId);
}
