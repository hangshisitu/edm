package cn.edm.web.facade;

import static cn.edm.consts.mapper.FilterMap.CONTAINS;
import static cn.edm.consts.mapper.FilterMap.END;
import static cn.edm.consts.mapper.FilterMap.EQ;
import static cn.edm.consts.mapper.FilterMap.GT;
import static cn.edm.consts.mapper.FilterMap.LT;
import static cn.edm.consts.mapper.FilterMap.RANGE;
import static cn.edm.consts.mapper.FilterMap.START;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.FilterMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.TypeMap;
import cn.edm.model.Prop;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Validates;

import com.google.common.collect.Maps;

public class Props {

    /**
     * 获取过滤器正文.
     */
    public static String settings(String props) {
        if (StringUtils.isBlank(props)) return null;
        
        StringBuffer sbff = new StringBuffer();
        for (String prop : StringUtils.splitPreserveAllTokens(props, "&")) {
            String p = StringUtils.substringBefore(prop, ":");
            String cnd = StringUtils.substringBefore(StringUtils.substringAfter(prop, ":"), "=");
            String val = StringUtils.substringAfter(StringUtils.substringAfter(prop, ":"), "=");
            val = StringUtils.replace(val, ",", "至");
            sbff.append(Values.get(PropMap.getName(p), p))
                .append(" ")
                .append(FilterMap.getName(cnd)).append(" ").append(val)
                .append("；");
        }
        props = sbff.toString();
        if (StringUtils.endsWith(props, "；"))
            props = StringUtils.removeEnd(props, "；");
        
        return props;
    }
    
    /**
     * 设置属性正文.
     */
    public static void mapper(List<Prop> propList) {
        if (Asserts.empty(propList)) return;
        for (Prop prop : propList) {
            prop.setPropName(Values.get(PropMap.getName(prop.getPropName()), prop.getPropName()));
        }
    }

    /**
     * Map存储属性集合.
     */
    public static Map<String, String> map(String propStrs) {
        Map<String, String> propMap = Maps.newHashMap();
        if (StringUtils.isNotBlank(propStrs)) {
            for (String prop : StringUtils.splitPreserveAllTokens(propStrs, "&")) {
                String settings = settings(prop);
                if (StringUtils.isNotBlank(settings)) {
                    prop = StringUtils.replace(prop, ":", ".");
                    prop = StringUtils.replace(prop, "=", ",");
                    propMap.put(prop, settings);
                }
            }
        } else {
            propMap.put(Value.S, Value.S);
        }

        return propMap;
    }
    
    /**
     * 属性规则.
     */
    public static void rule(String cnd, String val, String type) {
        if (StringUtils.equals(type, TypeMap.STRING.getType())) {
            if (!Asserts.hasAny(cnd, new String[] { EQ.getId(), START.getId(), END.getId(), CONTAINS.getId() })) {
                throw new Errors("过滤条件只能为" + EQ.getName() + "、" + START.getName() + "、" + END.getName() + "或" + CONTAINS.getName());
            }
        } else if (StringUtils.equals(type, TypeMap.INTEGER.getType())) {
            if (!Asserts.hasAny(cnd, new String[] { EQ.getId(), LT.getId(), GT.getId(), RANGE.getId() })) {
                throw new Errors("过滤条件只能为" + EQ.getName() + "、" + LT.getName() + "、" + GT.getName() + "或" + RANGE.getName());
            }
            String message = "过滤内容" + FilterMap.getName(cnd);
            if (StringUtils.equals(cnd, RANGE.getId())) {
                String[] range = StringUtils.splitPreserveAllTokens(val, ",");
                if (range.length != 2) {
                    throw new Errors(message + "没有最小值及最大值");
                }
                if (!Validates.integer(range[0])) {
                    throw new Errors(message + "最小值不是合法值");
                }
                if (!Validates.integer(range[1])) {
                    throw new Errors(message + "最大值不是合法值");
                }
                if (Integer.valueOf(range[0]) >= Integer.valueOf(range[1])) {
                    throw new Errors(message + "最小值只能小于最大值");
                }
            } else {
                if (!Validates.integer(val)) {
                    throw new Errors(message + "不是合法值");
                }
            }
        } else if (StringUtils.equals(type, TypeMap.DATE.getType())) {
            if (!Asserts.hasAny(cnd, new String[] { EQ.getId(), LT.getId(), GT.getId(), RANGE.getId() })) {
                throw new Errors("过滤条件只能为" + EQ.getName() + "、" + LT.getName() + "、" + GT.getName() + "或" + RANGE.getName());
            }
            String message = "过滤内容" + FilterMap.getName(cnd);
            if (StringUtils.equals(cnd, RANGE.getId())) {
                String[] range = StringUtils.splitPreserveAllTokens(val, ",");
                if (range.length != 2) {
                    throw new Errors(message + "没有起始日期及结束日期");
                }
                if (!Validates.date(range[0])) {
                    throw new Errors(message + "起始日期不是合法值");
                }
                if (!Validates.date(range[1])) {
                    throw new Errors(message + "结束日期不是合法值");
                }
                DateTime min = Calendars.parse(range[0], Calendars.DATE);
                DateTime max = Calendars.parse(range[1], Calendars.DATE);
                
                if (!min.isBefore(max)) {
                    throw new Errors(message + "起始日期只能早于结束日期");
                }
            } else {
                if (!Validates.date(val)) {
                    throw new Errors(message + "不是合法值");
                }
            }
        } else {
            throw new Errors("过滤条件不是合法值");
        }
    }
    
    /**
     * 获取模板标题和内容的变量.
     */
    public static Map<Integer, String> parameters(String subject, String templateContent, List<Prop> propList) {
        Map<Integer, String> propMap = Maps.newHashMap();
        int i = 0;
        for (Prop prop : propList) {
            if (!StringUtils.contains(subject + "," + templateContent, "$" + prop.getPropName() + "$")) {
                continue;
            }
            propMap.put(i, prop.getPropName());
            i++;
        }
        
        return propMap;
    }
}
