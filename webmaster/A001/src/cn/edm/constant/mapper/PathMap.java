package cn.edm.constant.mapper;

public enum PathMap {

	EXECUTE	("/WEB-INF/static/execute/"),
	FILE	("/WEB-INF/static/file/"),
	R		("/WEB-INF/static/R/"),
	TASK	("/WEB-INF/static/task/"),
	TEMPLATE("/WEB-INF/static/template/"),
	WRITE	("/WEB-INF/static/write/"),
	FORM    ("/WEB-INF/static/form/"),
	TOUCH   ("/WEB-INF/static/touch/");

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
