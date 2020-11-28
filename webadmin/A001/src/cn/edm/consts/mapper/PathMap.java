package cn.edm.consts.mapper;

/**
 * 路径.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum PathMap {

    EXECUTE	   ("/WEB-INF/static/execute/"),
    FILE       ("/WEB-INF/static/file/"),
    R          ("/WEB-INF/static/R/"),
    TASK       ("/WEB-INF/static/task/"),
    TEMPLATE   ("/WEB-INF/static/template/"),
    WRITE      ("/WEB-INF/static/write/"),
    FORM       ("/WEB-INF/static/form/"),
    JOIN_API   ("/WEB-INF/static/join_api/"),
    TOUCH      ("/WEB-INF/static/touch/"),
    
    SCAN_TASK   ("/WEB-INF/static/scan/task/"),
    SCAN_PLAN   ("/WEB-INF/static/scan/plan/"),
    SCAN_SELECTION  ("/WEB-INF/static/scan/selection/");

	private final String path;

	private PathMap(String path) {
        this.path = path;
    }
	
	public String getPath() {
		return path;
	}

	public static final String getPath(String path) {
		for (PathMap mapper : PathMap.values()) {
		    if (mapper.name().equals(path)) {
                return mapper.getPath();
            }
		}
		return null;
	}
}
