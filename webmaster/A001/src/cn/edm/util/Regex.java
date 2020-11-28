package cn.edm.util;

public class Regex {

	public static final String EMAIL = "^[a-z0-9!#$%&'*+/=?^_`{|}~-]+(\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@([a-z0-9!#$%&'*+/=?^_`{|}~-]+(\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\])$";
	public static final String PHONE = "^([\\+][0-9]{1,3}[ \\.\\-])?([\\(]{1}[0-9]{2,6}[\\)])?([0-9 \\.\\-\\/]{3,20})((x|ext|extension)[ ]?[0-9]{1,4})?$";
	public static final String GENDER = "^(?i)male|female|女|男|0|1$";
	public static final String MALE = "^(?i)male|男|1$";
	public static final String FEMALE = "^(?i)female|女|0$";
	public static final String AGE = "^120|((1[0-1]|\\d)?\\d)$";
	public static final String ACCOUNT = ("[0-9A-Za-z_]*");
	public static final String URLS= "http://(([a-zA-z0-9]|-){1,}\\.){1,}[a-zA-z0-9]{1,}-*";
	/**
	 * 大写
	 */
	public static final String UPPERCASE = "[A-Z]";

	
	/**匹配用户名*/
	public static final String USER_NAME = "[A-Za-z][0-9A-Za-z_]*";
	
	/**密码不能为汉字*/
	public static final String PASSWORD =  "[\u4e00-\u9fa5]";
	
	/**匹配数字*/
	public static final String NUMBER = "[0-9]{1,8}";
	
	/**匹配文件名*/
	public static final String FILENAME = "^\\d+(\\.csv)$";
	
	/**所有输入框*/
	public static final String ALL = "^([\\s\\w]|[\\-(),.@]|[\u4e00-\u9fa5]|[^\\x00-\\x80])*$";
	
	
}
