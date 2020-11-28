package cn.edm.constant;

public enum Role {

	ADM(1, "ROLE_ADM"), // 超级管理员
	MGR(2, "ROLE_MGR"), // 管理员
/*	OPT(7, "ROLE_OPT"), // 操作人员*/	
	BCT(3, "ROLE_BCT"), // 群发人员	
	ADT(4, "ROLE_ADT"), // 运营专员
	SEE(5, "ROLE_SEE"), //后台监控专员(后台子账号)
	RDR(6, "ROLE_RDR"); // 企业只读人员

	private final Integer id;
	private final String value;

	private Role(Integer id, String value) {
		this.id = id;
		this.value = value;
	}

	public Integer getId() {
		return id;
	}

	public String getValue() {
		return value;
	}

	public static final Integer getId(String roleName) {
		Integer roleId = null;
		for (Role role : Role.values()) {
			if (role.getValue().equals(roleName)) {
				roleId = role.getId();
				break;
			}
		}
		return roleId;
	}

	public static final String getName(Integer roleId) {
		String name = "";
		for (Role role : Role.values()) {
			if (role.getId().equals(roleId)) {
				name = role.name();
				break;
			}
		}
		return name;
	}

	public static void main(String[] args) {
		System.out.println(Role.ADM.getValue());
	}
}
