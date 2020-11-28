package cn.edm.persistence;


import cn.edm.domain.Tag;

public interface TagMapper {
	
	void insertTag(Tag tag);
	
	void delTagByUserId(String userId);
	
}
