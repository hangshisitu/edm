package cn.edm.consts.mapper;

/**
 * 表单.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum FormMap {
    
    REGIST      (0, "注册"),
    SUBSCRIBE   (1, "订阅");

    private final Integer id;
    private final String name;

    private FormMap(Integer id, String name) {
        this.id = id;
        this.name = name;
    }

    public Integer getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public static final String name(Integer id) {
        for (SelectionMap mapper : SelectionMap.values()) {
            if (mapper.getId().equals(id)) {
                return mapper.name();
            }
        }
        return null;
    }
    
    public static final Integer getId(String name) {
        for (FormMap mapper : FormMap.values()) {
            if (mapper.getName().equals(name)) {
                return mapper.getId();
            }
        }
        return null;
    }
    
    public static final String getName(Integer id) {
        for (FormMap mapper : FormMap.values()) {
            if (mapper.getId().equals(id)) {
                return mapper.getName();
            }
        }
        return null;
    }
}
