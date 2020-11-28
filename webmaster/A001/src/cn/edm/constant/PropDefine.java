package cn.edm.constant;

public enum PropDefine {

	EMAIL("email", "邮箱"),
	TRUE_NAME("true_name", "姓名"),
	NICK_NAME("nick_name", "昵称"),
	GENDER("gender", "性别"),
	JOB("job", "职业"),
	AGE("age", "年龄"),
	BIRTHDAY("birthday", "生日");
	
	private final String key;
	private final String val;

	private PropDefine(String key, String val) {
		this.key = key;
		this.val = val;
	}

	public String getKey() {
		return key;
	}

	public String getVal() {
		return val;
	}

	public static final String getKey(String val) {
		for (PropDefine mapper : PropDefine.values()) {
			if (mapper.getVal().equals(val)) {
				return mapper.getKey();
			}
		}
		return null;
	}

	public static final String getVal(String key) {
		for (PropDefine mapper : PropDefine.values()) {
			if (mapper.getKey().equals(key)) {
				return mapper.getVal();
			}
		}
		return null;
	}
}
