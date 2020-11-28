/**
 * 
 */
package cn.edm.consts.mapper;

/**
 * 属性关系. one or many.
 * 
 * @author SuperScott@Yeah.Net
 */
public enum RelationMap {

    ONE(0, "one"), MANY(1, "many");

    private final Integer id;
    private final String name;

    private RelationMap(Integer id, String name) {
        this.id = id;
        this.name = name;
    }

    public Integer getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public static final Integer getId(String name) {
        for (RelationMap mapper : RelationMap.values()) {
            if (mapper.getName().equals(name)) {
                return mapper.getId();
            }
        }
        return null;
    }

    public static final String getName(String id) {
        for (RelationMap mapper : RelationMap.values()) {
            if (mapper.getId().equals(id)) {
                return mapper.getName();
            }
        }
        return null;
    }
}
