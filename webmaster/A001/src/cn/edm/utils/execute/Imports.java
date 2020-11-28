package cn.edm.utils.execute;

import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;

import cn.edm.constants.Value;
import cn.edm.constants.mapper.PropMap;
import cn.edm.constants.mapper.TypeMap;
import cn.edm.domain.Prop;
import cn.edm.exception.Errors;
import cn.edm.utils.Values;
import cn.edm.utils.web.Validates;

import com.google.common.collect.Maps;
import com.mongodb.BasicDBObject;
import com.mongodb.DBObject;


public class Imports {

	public static int mailPos(Integer[] propIds, List<Prop> propList, Map<Integer, Prop> props) {
		Map<Integer, Prop> propMap = Maps.newHashMap();
		for (Prop prop : propList) {
			propMap.put(prop.getPropId(), prop);
		}

		// 邮箱属性定位
		int recipient = -1;
		for (int i = 0; i < propIds.length; i++) {
			Prop prop = propMap.get(propIds[i]);
			props.put(i, prop);
			if (prop != null && prop.getPropName().equals(PropMap.EMAIL.getId())) {
				recipient = i;
			}
		}
		if (recipient == -1) {
			throw new Errors("邮箱属性不能为空值");
		}
		
		return recipient;
	}

	public static boolean hasParameters(Map<Integer, Prop> propMap, int mailPos) {
		boolean hasParameters = false;
		Iterator<Entry<Integer, Prop>> iter = propMap.entrySet().iterator();

		while (iter.hasNext()) {
			Entry<Integer, Prop> entry = iter.next();

			if (entry.getKey() == mailPos) {
				continue;
			}
			if (entry.getValue() != null) {
				hasParameters = true;
				break;
			}
		}

		return hasParameters;
	}
	
	public static boolean isEmail(String email) {
		if (StringUtils.isBlank(email) || email.length() > TypeMap.EMAIL_LENGTH || !Validates.email(email)) {
			return false;
		}
		return true;
	}
	
	public static boolean hasParameter(int i, int mailPos, Map<Integer, Prop> propMap, String value, StringBuffer sbff) {
		if (i == mailPos) {
			return false;
		}
		
		Prop prop = propMap.get(i);

		if (prop == null) {
			return false;
		}
		
		if (!isParameter(prop, value)) {
			return false;
		}

		sbff.append(prop.getPropName()).append("=").append(Values.get(value)).append(",");
		return true;
	}
	
	public static boolean isParameter(Prop prop, String parameter) {
		// default value
		if (StringUtils.isBlank(parameter) && StringUtils.isNotBlank(prop.getDefaultValue())) {
			parameter = prop.getDefaultValue();
		}

		// required
		if (prop.getRequired().equals(Value.T) && StringUtils.isBlank(parameter)) {
			return false;
		}

		// type & length
		if (StringUtils.isNotBlank(parameter)) {
			if (StringUtils.equals(prop.getType(), TypeMap.STRING.getType())) {
				if (!Validates.clean(parameter)) {
					return false;
				}
				if (parameter.length() > TypeMap.STRING_LENGTH || (prop.getLength() != null && parameter.length() > prop.getLength())) {
					return false;
				}
			} else if (StringUtils.equals(prop.getType(), TypeMap.INTEGER.getType())) {
				if (!Validates.integer(parameter) || (prop.getLength() != null && Integer.valueOf(parameter) > prop.getLength())) {
					return false;
				}
//			} else if (StringUtils.equals(prop.getType(), Types.BIG_INTEGER.getType())) {
//				if (!Validates.biginteger(value) || (prop.getLength() != null && Long.valueOf(value) > prop.getLength())) {
//					return false;
//				}
			} else if (StringUtils.equals(prop.getType(), TypeMap.DATE.getType())) {
				if (!Validates.date(parameter)) {
					return false;
				}
//			} else if (StringUtils.equals(prop.getType(), Types.DATE_TIME.getType())) {
//				if (!Validates.datetime(value)) {
//					return false;
//				}
			} else {
				return false;
			}
		}
		
		return true;
	}
	
	public static void isParameter(Prop prop, String parameter, String error) {
		if (StringUtils.isBlank(parameter) && StringUtils.isNotBlank(prop.getDefaultValue())) {
			parameter = prop.getDefaultValue();
		}

		if (prop.getRequired().equals(Value.T) && StringUtils.isBlank(parameter)) {
			throw new Errors(error + "不能为空值");
		}

		if (StringUtils.isNotBlank(parameter)) {
			if (StringUtils.equals(prop.getType(), TypeMap.STRING.getType())) {
				if (parameter.length() > TypeMap.STRING_LENGTH || (prop.getLength() != null && parameter.length() > prop.getLength())) {
					throw new Errors(error + "的字符长度只能介于1到255之间");
				}
			} else if (StringUtils.equals(prop.getType(), TypeMap.INTEGER.getType())) {
				if (!Validates.integer(parameter) || (prop.getLength() != null && Integer.valueOf(parameter) > prop.getLength())) {
					throw new Errors(error + "不是合法的整数值");
				}
			} else if (StringUtils.equals(prop.getType(), TypeMap.DATE.getType())) {
				if (!Validates.date(parameter)) {
					throw new Errors(error + "不是合法的日期");
				}
			} else {
				throw new Errors("Type(" + prop.getType() + ")不是合法值");
			}
		}
	}
	
	public static void addRecipient(List<DBObject> recipientList, String email, String parameterList) {
		recipientList.add(addRecipient(email, parameterList));
	}
	
	public static BasicDBObject addRecipient(String email, String parameterList) {
		BasicDBObject recipient = new BasicDBObject();
		recipient.put("email", email);
		recipient.put("parameter_list", parameterList);
		return recipient;
	}

	public static void addTag(Map<Integer, List<DBObject>> tagMap, String email, String desc, long time) {
		BasicDBObject tag = addTag(email, desc, time);
		for (Integer tagId : tagMap.keySet()) {
			List<DBObject> tagList = tagMap.get(tagId);
			tagList.add(tag);
		}
	}
	
	public static BasicDBObject addTag(String email, String desc, long time) {
		BasicDBObject tag = new BasicDBObject();
		tag.put("email", email);
        if (StringUtils.isNotBlank(desc)) {
            tag.put("desc", desc);
        }
		tag.put("create_time", time);
		tag.put("modify_time", time);
		return tag;
	}
}
