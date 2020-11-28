package cn.edm.constant;

public enum CategoryDefine {

	DELIVERY       ("投递", "系统默认投递类别"),
	TEST           ("测试", "系统默认测试类别"),
	FORM           ("表单收集","系统默认表单收集类别"),
	UNSUBSCRIBE    ("退订", "系统默认退订类别"),
	TOUCH          ("触发计划", "触发计划的子模板，不可单独投递"),
	TEMP        ("临时文件", "系统默认临时文件类别"),
	//农信银专有
	BILL        ("账单", "系统默认模板类别"),
    DEFAULT     ("默认", "系统默认模板类别"),
	USER_DEFINED     ("自定义推荐", "系统默认模板类别");
	private final String key;
	private final String val;

	private CategoryDefine(String key, String val) {
		this.key = key;
		this.val = val;
	}

	public String getKey() {
		return key;
	}

	public String getVal() {
		return val;
	}

	public static final String getKey(String key) {
		for (CategoryDefine mapper : CategoryDefine.values()) {
			if (mapper.getKey().equals(key)) {
				return mapper.getKey();
			}
		}
		return null;
	}
}
