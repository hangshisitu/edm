package cn.edm.constant;

import cn.edm.util.PropertiesUtil;

/**
 * 获取路径常量类
 * 
 * @Date 2014年12月3日 下午4:34:11
 * @author Lich
 *
 */
public class PathCons {

	/**
	 * 上传文件根路径
	 * <prev>如：E:/test </prev>
	 */
	public static final String ROOT_PATH = PropertiesUtil.get(ConfigKeys.ROOT_PATH);
	/** execute目录  */
	public static final String EXECUTE = "/WEB-INF/static/execute/";
	/** file目录  */
	public static final String FILE = "/WEB-INF/static/file/";
	/** R目录  */
	public static final String R = "/WEB-INF/static/R/";
	/** task目录  */
	public static final String TASK = "/WEB-INF/static/task/";
	/** template目录  */
	public static final String TEMPLATE = "/WEB-INF/static/template/";
	/** write目录  */
	public static final String WRITE = "/WEB-INF/static/write/";
	/** form目录  */
	public static final String FORM = "/WEB-INF/static/form/";
	/** touch目录  */
	public static final String TOUCH = "/WEB-INF/static/touch/";
	/** task扫描路径 */
	public static final String SCAN_TASK = "/WEB-INF/static/scan/task/";
	/** plan扫描路径 */
	public static final String SCAN_PLAN = "/WEB-INF/static/scan/plan/";
	/** touch扫描路径 */
	public static final String SCAN_TOUCH = "/WEB-INF/static/scan/touch/";
	/** 前台部署的路径 */
	public static final String WEBADMIN_PATH = PropertiesUtil.get(ConfigKeys.WEBADMIN_PATH);
	/**
	 * 上传文件服务器地址
	 * <prev>如：http://127.0.0.1:8888/showpic </prev>
	 */
	public static final String FILE_SERVER = PropertiesUtil.get(ConfigKeys.FILE_SERVER);

}
