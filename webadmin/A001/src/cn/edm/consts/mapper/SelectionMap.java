package cn.edm.consts.mapper;

import cn.edm.consts.Options;
import cn.edm.utils.Asserts;

/**
 * 筛选.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum SelectionMap {

    CREATE(0, "生成新标签"),
    IMPORT(1, "导入到标签"),
    EXPORT(2, "生成文件");

    private final Integer id;
    private final String name;

    private SelectionMap(Integer id, String name) {
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
        for (SelectionMap mapper : SelectionMap.values()) {
            if (mapper.getName().equals(name)) {
                return mapper.getId();
            }
        }
        return null;
    }
    
    public static final String getName(Integer id) {
        for (SelectionMap mapper : SelectionMap.values()) {
            if (mapper.getId().equals(id)) {
                return mapper.getName();
            }
        }
        return null;
    }

    public static String option(Integer type) {
        if (Asserts.hasAny(type, new Integer[] { CREATE.getId(), IMPORT.getId() })) {
            return Options.INPUT;
        }
        return Options.OUTPUT;
    }
}
