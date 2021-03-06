package cn.edm.web.facade;

import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Prop;
import cn.edm.model.Recipient;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.MailBox;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Readers;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;

import com.google.common.collect.Maps;

public class Recipients {

    /**
     * 获取有效的邮箱地址.
     */
    public static String[] validate(String emailList) {
        if (StringUtils.contains(emailList, ";"))
            emailList = StringUtils.replace(emailList, ";", ",");

        MapBean map = new MapBean();

        for (String email : StringUtils.splitPreserveAllTokens(emailList, ",")) {
            email = StringUtils.lowerCase(StringUtils.trim(email));
            if (MailBox.validate(email)) {
                if (!map.containsKey(email)) {
                    map.put(email, email);
                }
            }
        }

        Asserts.isEmpty(map, "好友邮箱");
        return Converts._toStrings(Converts.toString(map));
    }
    
    /**
     * Map存储parameterList.
     */
    public static void parameterMap(Map<String, String> parameterMap, Recipient recipient) {
        if (recipient != null && StringUtils.isNotBlank(recipient.getEmail())) {
            if (StringUtils.isNotBlank(recipient.getParameterList())) {
                for (String prop : StringUtils.splitPreserveAllTokens(recipient.getParameterList(), ",")) {
                    String k = StringUtils.substringBefore(prop, "=");
                    String v = StringUtils.substringAfter(prop, "=");
                    if (StringUtils.isBlank(k)) {
                        continue;
                    }
                    parameterMap.put(Values.get(PropMap.getName(k), k), v);
                }
            } else {
                parameterMap.put(Value.S, Value.S);
            }
        } else {
            parameterMap.put(Value.S, Value.S);
        }
    }
    
    /**
     * 定位导入文件的属性.
     */
    public static Integer[] propIds(HttpServletRequest request, MultipartFile upload, String charset, Prop email) {
        Asserts.isNull(email, "邮箱属性");
        
        String[] lines = Readers.lines(upload, charset, 1);
        String[] props = StringUtils.splitPreserveAllTokens(lines[0], ",");
        
        Map<String, Integer> propIdMap = Maps.newHashMap();
        boolean mailPos = false;
        
        if (props.length > 1) {
            Map<String, Object> propMap = Servlets.getParametersStartingWith(request, "prop_");
            Map<String, Object> parameterMap = Servlets.getParametersStartingWith(request, "parameter_");
            
            for (Entry<String, Object> entry : propMap.entrySet()) {
                String propId = (String) entry.getValue();
                propId = (String) Validator.validate(propId, R.CLEAN, R.INTEGER, "属性");

                if (StringUtils.isBlank(propId)) {
                    continue;
                }
                
                String parameter = (String) parameterMap.get(entry.getKey());
                parameter = (String) Validator.validate(parameter, R.CLEAN, R.LENGTH, "{1,20}", "标题");
                
                if (StringUtils.isBlank(parameter)) {
                    continue;
                }
                
                if (email.getPropId().equals(Integer.valueOf(propId))) {
                    mailPos = true;
                }

                propIdMap.put(parameter, Integer.valueOf(propId));
            }
        } else {
            propIdMap.put(props[0], Integer.valueOf(email.getPropId()));
            mailPos = true;
        }
        
        Asserts.isEmpty(propIdMap, "属性");
        
        if (!mailPos) {
            throw new Errors("数据标题没对应邮箱属性");
        }
        
        StringBuffer sbff = new StringBuffer();
        for (String prop : props) {
            Integer propId = propIdMap.get(prop);
            if (propId == null) {
                propId = Value.I;
            }
            sbff.append(propId).append(",");
        }
        
        return Converts._toIntegers(sbff.toString());
    }
}
