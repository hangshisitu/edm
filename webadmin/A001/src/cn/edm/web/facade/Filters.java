package cn.edm.web.facade;

import java.util.List;

import cn.edm.model.Filter;
import cn.edm.utils.Asserts;

public class Filters {
    
    /**
     * 设置过滤器机制正文.
     */
    public static void props(List<Filter> filterList) {
        if (Asserts.empty(filterList)) return;
        for (Filter prop : filterList) {
            prop.setProps(Props.settings(prop.getProps()));
        }
    }
}
