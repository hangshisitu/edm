package cn.edm.web.facade;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.mapper.CategoryMap;
import cn.edm.model.Category;
import cn.edm.model.Label;
import cn.edm.model.Tag;
import cn.edm.model.Template;
import cn.edm.utils.Asserts;
import cn.edm.utils.except.Errors;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public class Categorys {

    /**
     * 是否为系统定义类别.
     */
    public static void isKeyword(String orgCategoryName, String categoryName) {
        boolean isKeyword = false;
        for (CategoryMap mapper : CategoryMap.values()) {
            if (StringUtils.isNotBlank(orgCategoryName)) {
                if (StringUtils.equals(orgCategoryName, mapper.getName())) {
                    if (!StringUtils.equals(orgCategoryName, categoryName)) {
                        throw new Errors("不能修改系统定义的类别名称");
                    }
                } else {
                    if (!StringUtils.equals(orgCategoryName, categoryName)) {
                        if (StringUtils.equals(categoryName, mapper.getName())) {
                            isKeyword = true;
                        }
                    }
                }
            } else {
                if (StringUtils.equals(categoryName, mapper.getName())) {
                    isKeyword = true;
                }
            }
            
            if (isKeyword) {
                throw new Errors("类别名称不能含有系统定义的" + mapper.getName() + "关键词");
            }
        }
    }

    /**
     * 类别分组. T:Label|Tag|Template
     */
    public static <T> Map<String, List<T>> groupMap(List<Category> categories, List<T> objects) {
        Map<String, List<T>> map = Maps.newLinkedHashMap();
        if (Asserts.empty(categories) || Asserts.empty(objects))
            return map;
        for (Category c : categories) {
            List<T> list = Lists.newLinkedList();
            for (T e : objects) {
                Integer id = null;

                if (e instanceof Label) id = ((Label) e).getCategoryId();
                else if (e instanceof Tag) id = ((Tag) e).getCategoryId();
                else if (e instanceof Template) id = ((Template) e).getCategoryId();

                if (id.equals(c.getCategoryId()))
                    list.add(e);
            }
            map.put(c.getCategoryName(), list);
        }

        return map;
    }
}
