package cn.edm.service;

import java.util.Map;

import cn.edm.domain.Cnd;

public interface CndService {
	
	Map<Integer,Cnd> getCndByTaskIds(Integer[] taskIds);
}
