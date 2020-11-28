package cn.edm.consts.mapper;

/**
 * 过滤规则.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum FilterMap {

	EQ					("eq", "等于"),
	LT					("lt", "小于"),
	GT					("gt", "大于"),
	RANGE				("range", "在此之间"),
	START				("start", "开头为"),
	END					("end", "结尾为"),
	CONTAINS			("contains", "包含关键词"),
	IN					("in", "包含元素集"),
	NONE				("none", "为空"),
	NOT_END_CONTAINS	("not_end_contains", "结尾不包含关键词");
	
	private final String id;
	private final String name;

	private FilterMap(String id, String name) {
		this.id = id;
		this.name = name;
	}

	public String getId() {
		return id;
	}

	public String getName() {
		return name;
	}

	public static final String getId(String name) {
		for (FilterMap mapper : FilterMap.values()) {
			if (mapper.getName().equals(name)) {
				return mapper.getId();
			}
		}
		return null;
	}

	public static final String getName(String id) {
		for (FilterMap mapper : FilterMap.values()) {
			if (mapper.getId().equals(id)) {
				return mapper.getName();
			}
		}
		return null;
	}
}
