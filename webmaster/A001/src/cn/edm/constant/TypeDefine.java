package cn.edm.constant;

public enum TypeDefine {
	
	STRING("String", "字符串"),
	INTEGER("Integer", "整数"),
//	BIG_INTEGER("Big integer", "长整数"),
	DATE("Date (yyyy-MM-dd)", "日期 (yyyy-MM-dd)");
//	DATE_TIME("Datetime (yyyy-MM-dd HH:mm:ss)", "日期时间 (yyyy-MM-dd HH:mm:ss)");

	public static final int STRING_LENGTH = 255;
	public static final int EMAIL_LENGTH = 64;
	
	private final String key;
	private final String val;

	private TypeDefine(String key, String val) {
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
		for (TypeDefine mapper : TypeDefine.values()) {
			if (mapper.getVal().equals(val)) {
				return mapper.getKey();
			}
		}
		return null;
	}

	public static final String getVal(String key) {
		for (TypeDefine mapper : TypeDefine.values()) {
			if (mapper.getKey().equals(key)) {
				return mapper.getVal();
			}
		}
		return null;
	}
}
