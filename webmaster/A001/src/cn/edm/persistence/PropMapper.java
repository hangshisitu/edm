package cn.edm.persistence;


import java.util.List;

import cn.edm.domain.Prop;

public interface PropMapper {
	
	void insert(Prop prop);
	
	void insertBatch(List<Prop> propList);
	
	void delPropByUserId(String userId);
}
