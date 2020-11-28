package cn.edm.constants.mapper;

public enum PropMap {

	EMAIL		("email", "邮箱", "系统默认邮箱"),
	TRUE_NAME	("true_name", "姓名", "系统默认姓名"),
	NICK_NAME	("nick_name", "昵称", "系统默认昵称"),
	GENDER		("gender", "性别", "系统默认性别"),
	JOB			("job", "职业", "系统默认职业"),
	AGE			("age", "年龄", "系统默认年龄"),
	BIRTHDAY	("birthday", "生日", "系统默认生日");
	
	private final String id;
	private final String name;
	private final String desc;

	private PropMap(String id, String name, String desc) {
		this.id = id;
		this.name = name;
		this.desc = desc;
	}

	public String getId() {
		return id;
	}

	public String getName() {
		return name;
	}
	
	public String getDesc() {
		return desc;
	}

	public static final String getId(String name) {
		for (PropMap mapper : PropMap.values()) {
			if (mapper.getName().equals(name)) {
				return mapper.getId();
			}
		}
		return null;
	}

	public static final String getName(String id) {
		for (PropMap mapper : PropMap.values()) {
			if (mapper.getId().equals(id)) {
				return mapper.getName();
			}
		}
		return null;
	}
}
