package cn.edm.constant;

public interface ConfigKeys {

	public static final String APPLICATION_LOCATION = "application.location";
	public static final String LOGIN_PASSWORD = "login.password";

	public static final String WEBADMIN_HTTP = "webadmin.http";
	public static final String WEBADMIN_PATH = "webadmin.path";
	public static final String WEBSITE_HTTP = "website.http";
	public static final String TASK_SIZE = "task.size";
	public static final String SEND_HTTP = "send.http";
	public static final String SMTP_FROM = "smtp.from";
	public static final String FORGET = "forget";
	
	public static final String LOG_HTTP = "log.http";
	
	public static final String SMTP_CATALOG_SHOW = "smtp.catalog.show";
	public static final String HR_CATALOG_SHOW = "hr.catalog.show";
	public static final String ROLE_CATALOG_SHOW = "role.catalog.show";
	
	
	public static final String DEFAULT_HELO = "default.helo";
	public static final String DEFAULT_SKIP = "default.skip";
	public static final String DELIVERY_ENGINE = "delivery.engine";
	public static final String DELIVERY_LOGGER = "delivery.logger";
	public static final String DELIVERY_SENDER = "delivery.sender";
	
	public static final String APP_URL = "app.url";
	public static final String APP_URLS = "app.urls";
	
	/**
	 * 属性库url
	 */
	public static final String PROPERTY_URL = "property.url";
	
	/**
	 * 新任务创建多少天后允许导入
	 */
	public static final String PROPERTY_DAYS = "property.days";
	
	/**
	 * 发件域投递监控
	 */
	public static final String FROMDOMAIN_URL = "fromDomain.url";
	
	/**
	 * 上传文件服务器路径
	 * <prev>如：E:/test </prev>
	 */
	public static final String ROOT_PATH = "root.path";
	
	/**
	 * 上传文件服务器地址
	 * <prev>如：http://127.0.0.1:8888/showpic </prev>
	 */
	public static final String FILE_SERVER = "file.server";

}
