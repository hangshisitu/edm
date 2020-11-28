package cn.edm.constants.mapper;

public enum TagMap {

	UNSUBSCRIBE("默认退订", "系统默认退订标签");

	private final String name;
	private final String desc;

	private TagMap(String name, String desc) {
		this.name = name;
		this.desc = desc;
	}

	public String getName() {
		return name;
	}

	public String getDesc() {
		return desc;
	}

	public static final String getName(String name) {
		for (TagMap mapper : TagMap.values()) {
			if (mapper.getName().equals(name)) {
				return mapper.getName();
			}
		}
		return null;
	}
}
