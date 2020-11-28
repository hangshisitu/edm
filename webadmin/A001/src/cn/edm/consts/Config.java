package cn.edm.consts;

public interface Config {

	public static final String APP_URL = "app.url";
	public static final String APP_URLS = "app.urls";
    public static final String WEBSITE_URL = "website.url";
    public static final String RESOURCE_URLS = "resource.urls";
    public static final String UNIQUE_CODE = "unique.code";
    public static final String CATEGORY_BILL= "category.bill";
	public static final String CONCURRENCY_COUNT = "concurrency.count";
	public static final String RECIPIENT_COUNT = "recipient.count";
	public static final String TAG_COUNT = "tag.count";
	public static final String REPEAT_SIZE = "repeat.size";
	public static final String UPLOAD_SIZE = "upload.size";
	public static final String PIPE_SIZE = "pipe.size";
	public static final String TEMPLATE_SIZE = "template.size";
	public static final String FILTER_PROPS = "filter.props";
    public static final String SELECTION_COUNT = "selection.count";
    public static final String SENDER_COUNT = "sender.count";
    public static final String ACTIVE_LOGGER = "active.logger";
    public static final String FORM_LOGGER = "form.logger";
	
	public static final String DEFAULT_HELO = "default.helo";
    public static final String DEFAULT_SKIP = "default.skip";
    public static final String DEFAULT_OPEN = "default.open";
    public static final String DEFAULT_CLICK = "default.click";
    public static final String DELIVERY_API = "delivery.api";
    public static final String DELIVERY_TOUCH = "delivery.touch";
    public static final String DELIVERY_ENGINE = "delivery.engine";
	public static final String DELIVERY_LOGGER = "delivery.logger";
	public static final String DELIVERY_SENDER = "delivery.sender";

    public static final String TOUCH_ENGINE = "touch.engine";
    
	public static final String SMTP_HOST = "smtp.host";
	public static final String SMTP_PORT = "smtp.port";
	public static final String SMTP_FROM = "smtp.from";
	public static final String REGIST = "regist";
	public static final String FORGET = "forget";
	public static final String SENDER = "sender";
	public static final String REMIND = "remind";

	public static final String MONGO_HOST = "mongo.host";
	public static final String MONGO_PORT = "mongo.port";
	public static final String MONGO_POOL_SIZE = "mongo.pool.size";
	public static final String MONGO_COLL_SIZE = "mongo.coll.size";
	public static final String MONGO_RECIPIENT_SIZE = "mongo.recipient.size";
	public static final String MONGO_TAG_SIZE = "mongo.tag.size";
	
	/**
	 * 生成任务文件路径
	 * <prev>如：E:/test </prev>
	 */
	public static final String ROOT_PATH = "root.path";
	/**
	 * 上传文件服务器地址
	 * <prev>如：http://127.0.0.1:8888/showpic </prev>
	 */
	public static final String FILE_SERVER = "file.server";
	/** 每次扫描任务的基础文件个数 */
	public static final String SCAN_SIZE = "scan.size";
	/** 针对某个版本的菜单显示配置，是否显示菜单。值为：false|true，不设置则默认为true*/
	public static final String HR_CATALOG_SHOW = "hr.catalog.show";
	
	public static final String NGINX_SERVER ="nginx.server";
	public static final String NGINX_PATH ="nginx.path";
}
