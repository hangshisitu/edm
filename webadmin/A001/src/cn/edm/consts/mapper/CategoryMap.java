package cn.edm.consts.mapper;

/**
 * 类别.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum CategoryMap {

	DELIVERY	("投递", "系统默认投递类别"),
	TEST		("测试", "系统默认测试类别"),
	UNSUBSCRIBE	("退订", "系统默认退订类别"),
	
    RED         ("红名单", "系统默认红名单类别"),
    BLACK       ("黑名单", "系统默认黑名单类别"),
    REFUSE      ("拒收", "系统默认拒收类别"),
    EXPOSE      ("举报", "系统默认报类别"),
	
    FORM        ("表单收集", "系统默认表单收集类别"),
    INPUT       ("表单填写", "表单填写界面"),
    SUBMIT      ("表单提交", "表单提交界面"),
    PROMPT      ("邮件提示", "邮件提示界面"),
    
    TOUCH       ("触发计划", "触发计划的子模板，不可单独投递"),
    BILL        ("账单", "系统默认模板类别"),
    DEFAULT     ("默认", "系统默认模板类别"),
    CUSTOM      ("自定义推荐", "系统默认模板类别，用于存放自定义推荐模板"),
	
    FILE        ("临时文件", "系统默认临时文件类别");

	private final String name;
	private final String desc;

	private CategoryMap(String name, String desc) {
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
		for (CategoryMap mapper : CategoryMap.values()) {
			if (mapper.getName().equals(name)) {
				return mapper.getName();
			}
		}
		return null;
	}
}
