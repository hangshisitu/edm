package cn.edm.constant;

public interface PropertyConstant {
	
	/**
	 * 属性库查总数
	 */
	String TYPE_COUNT = "count";
	/**
	 * 属性库查明细
	 */
	String TYPE_DETAIL = "detail";
	
	/**
	 * 属性库查询
	 */
	String METHOD_QUERY = "/query?";
	
	/**
	 * 属性库插入
	 */
	String METHOD_INSERT = "/insert?";
	
	
	String INDUSTRY = "行业兴趣";
	
	String SOCIETY = "社会身份";
	
	String AGE = "年龄段";
	
	String GENDER = "性别";
	
	int NOT_HANDLE = 0;//未处理
	int HANDLED_SUCCESS = 1;//已处理(成功)
	int HANDLING = 2;//处理中
	int HANDLED_FAILURE = 3;//已处理（失败）
	
	
	String PROP_FILE_PATH = "/file";
	
}
