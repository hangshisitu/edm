package cn.edm.constants;

public interface Config {

	public static final String APP_URL = "app.url";
	public static final String APP_URLS = "app.urls";
    public static final String WEBSITE_URL = "website.url";
    public static final String RESOURCE_URLS = "resource.urls";
    
	public static final String CONCURRENCY_COUNT = "concurrency.count";
	public static final String RECIPIENT_COUNT = "recipient.count";
	public static final String TAG_COUNT = "tag.count";
	public static final String REPEAT_SIZE = "repeat.size";
	public static final String UPLOAD_SIZE = "upload.size";
	public static final String PIPE_SIZE = "pipe.size";
	public static final String TEMPLATE_SIZE = "template.size";
	public static final String FILTER_PROPS = "filter.props";
    public static final String SELECTION_COUNT = "selection.count";
    public static final String ACTIVE_LOGGER = "active.logger";
    public static final String FORM_LOGGER = "form.logger";
	
	public static final String DEFAULT_HELO = "default.helo";
    public static final String DEFAULT_SKIP = "default.skip";
    public static final String DEFAULT_OPEN = "default.open";
    public static final String DEFAULT_CLICK = "default.click";
	public static final String DELIVERY_ENGINE = "delivery.engine";
	public static final String DELIVERY_LOGGER = "delivery.logger";
	public static final String DELIVERY_SENDER = "delivery.sender";

	public static final String SMTP_HOST = "smtp.host";
	public static final String SMTP_PORT = "smtp.port";
	public static final String SMTP_FROM = "smtp.from";
	public static final String REGIST = "regist";
	public static final String FORGET = "forget";
	public static final String SENDER = "sender";

	public static final String MONGO_HOST = "mongo.host";
	public static final String MONGO_PORT = "mongo.port";
	public static final String MONGO_POOL_SIZE = "mongo.pool.size";
	public static final String MONGO_COLL_SIZE = "mongo.coll.size";
	public static final String MONGO_RECIPIENT_SIZE = "mongo.recipient.size";
	public static final String MONGO_TAG_SIZE = "mongo.tag.size";
}
