package cn.edm.utils.execute;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;

import cn.edm.constant.Options;
import cn.edm.constant.mapper.FilterMap;
import cn.edm.constant.mapper.PropMap;
import cn.edm.constant.mapper.TypeMap;
import cn.edm.domain.Prop;
import cn.edm.domain.Tag;
import cn.edm.exception.Errors;
import cn.edm.model.Recipient;
import cn.edm.util.Asserts;
import cn.edm.util.Calendars;
import cn.edm.util.Converts;
import cn.edm.util.Values;

import com.mongodb.BasicDBList;

public class Exports {

	public static int recipientCount(List<Tag> tagList, int recipientCount, String message) {
		int count = 0;
		for (Tag tag : tagList) {
			count = count + tag.getEmailCount();
		}
		if (count > recipientCount) {
			throw new Errors(message + "的收件人总数不能多于" + recipientCount);
		}

		return count;
	}

	public static void email(Map<String, Integer> lineMap, Map<String, String> filterMap, Map<String, Prop> propMap) {
		if (!Asserts.empty(filterMap) && StringUtils.isNotBlank(filterMap.get(PropMap.EMAIL.getId()))) {
			String email = filterMap.get(PropMap.EMAIL.getId());
			String key = StringUtils.substringBefore(email, "=");
			String value = StringUtils.substringAfter(email, "=");
			
			if (StringUtils.isNotBlank(key) && StringUtils.isNotBlank(value)) {
				String coll = "," + value + ",";
				String[] ends = Converts._toStrings(value);
				
				if (!Asserts.empty(propMap)) {
					Prop prop = propMap.get(PropMap.EMAIL.getId());
					if (prop == null) {
						return;
					}
				}
				Iterator<Entry<String, Integer>> iter = lineMap.entrySet().iterator();
				while (iter.hasNext()) {
					String line = iter.next().getKey();
					if (email(line, key, value, coll, ends)) {
						iter.remove();
					}
				}
			}
		}
	}
	
	private static boolean email(String line, String key, String value, String coll, String[] ends) {
		boolean remove = true;
		if (StringUtils.equals(key, FilterMap.EQ.getId()) && StringUtils.equals(line, value)) {
			remove = false;
		} else if (StringUtils.equals(key, FilterMap.START.getId()) && StringUtils.startsWith(line, value)) {
			remove = false;
		} else if (StringUtils.equals(key, FilterMap.END.getId()) && StringUtils.endsWith(line, value)) {
			remove = false;
		} else if (StringUtils.equals(key, FilterMap.CONTAINS.getId()) && StringUtils.contains(line, value)) {
			remove = false;
		} else if (StringUtils.equals(key, FilterMap.IN.getId()) && StringUtils.contains(line, coll)) {
			remove = false;
		} else if (StringUtils.equals(key, FilterMap.NOT_END_CONTAINS.getId()) && !StringUtils.endsWithAny(line, ends)) {
			remove = false;
		}
		
		return remove;
	}
	
	public static void prop(String opt, BufferedWriter writer, BasicDBList emailList, Map<String, Recipient> recipientMap,
			Map<String, String> parameterMap, Map<String, String> filterMap, Map<String, Prop> propMap) throws IOException {
		if (!Asserts.empty(recipientMap)) {
			for (String email : recipientMap.keySet()) {
				Recipient recipient = recipientMap.get(email);
				String parameters = recipient.getParameterList();
				
				for (String parameter : StringUtils.splitPreserveAllTokens(parameters, ",")) {
					String key = StringUtils.substringBefore(parameter, "=");
					String value = StringUtils.substringAfter(parameter, "=");
					if (StringUtils.isBlank(key)) {
						continue;
					}
					parameterMap.put(key, value);
				}
				
				if (!Asserts.empty(filterMap)) {
					for (String key : filterMap.keySet()) {
						if (StringUtils.equals(key, PropMap.EMAIL.getId())) {
							continue;
						}
						
						String value = filterMap.get(key);
						String k = StringUtils.substringBefore(value, "=");
						String v = StringUtils.substringAfter(value, "=");
						
						String parameter = Values.get(parameterMap.get(key));
						if (StringUtils.isNotBlank(k)) {
							Prop prop = propMap.get(key);
							if (prop != null) {
								String type = prop.getType();
								boolean remove = true;
								if (StringUtils.equals(type, TypeMap.STRING.getType())) {
									remove = string(parameter, k, v);
								} else if (StringUtils.equals(type, TypeMap.INTEGER.getType())) {
									remove = integer(parameter, k, v);
								} else if (StringUtils.equals(type, TypeMap.DATE.getType())) {
									remove = date(parameter, k, v);
								} else {
									remove = true;
								}
								
								if (remove && emailList.contains(email)) {
									emailList.remove(email);
								}
							}
						}
					}
				}
				
				if (emailList.contains(email)) {
					if (StringUtils.equals(opt, Options.INPUT)) {
						writer.write(email + "\r\n");
					} else {
						StringBuffer sbff = new StringBuffer();
						for (String prop : propMap.keySet()) {
							if (StringUtils.equals(prop, PropMap.EMAIL.getId())) {
								continue;
							}
							sbff.append(",").append(Values.get(parameterMap.get(prop)));
						}

						writer.write(email + sbff.toString() + "\r\n");
					}
				}
				
				parameterMap.clear();
			}
		}
	}
	
	private static boolean string(String parameter, String k, String v) {
		boolean remove = true;
		if (StringUtils.equals(k, FilterMap.EQ.getId()) && StringUtils.equals(parameter, v)) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.START.getId()) && StringUtils.startsWith(parameter, v)) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.END.getId()) && StringUtils.endsWith(parameter, v)) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.CONTAINS.getId()) && StringUtils.contains(parameter, v)) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.IN.getId()) && StringUtils.contains(v, "," + parameter + ",")) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.NONE.getId()) && StringUtils.isBlank(parameter)) {
			remove = false;
		}
		return remove;
	}
	
	private static boolean integer(String parameter, String k, String v) {
		boolean remove = true;
		Integer i1 = Values.integer(parameter);
		
		if (i1 != null && StringUtils.equals(k, FilterMap.EQ.getId())) {
			Integer i2 = Values.integer(v);
			if (i2 != null && i1 == i2) {
				remove = false;
			}
		} else if (i1 != null && StringUtils.equals(k, FilterMap.LT.getId())) {
			Integer i2 = Values.integer(v);
			if (i2 != null && i1 < i2) {
				remove = false;
			}
		} else if (i1 != null && StringUtils.equals(k, FilterMap.GT.getId())) {
			Integer i2 = Values.integer(v);
			if (i2 != null && i1 > i2) {
				remove = false;
			}
		} else if (i1 != null && StringUtils.equals(k, FilterMap.RANGE.getId())) {
			int min = Values.get(Values.integer(StringUtils.substringBefore(v, ",")), 0);
			int max = Values.get(Values.integer(StringUtils.substringAfter(v, ",")), Integer.MAX_VALUE);
			if (i1 >= min && i1 <= max) {
				remove = false;
			}
		} else if (i1 != null && StringUtils.equals(k, FilterMap.IN.getId()) && StringUtils.contains(v, "," + parameter + ",")) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.NONE.getId()) && StringUtils.isBlank(parameter)) {
			remove = false;
		}
		return remove;
	}
	
	private static boolean date(String parameter, String k, String v) {
		boolean remove = true;
		DateTime d1 = Calendars.parse(parameter, Calendars.DATE);
		
		if (d1 != null && StringUtils.equals(k, FilterMap.EQ.getId())) {
			DateTime d2 = Calendars.parse(parameter, Calendars.DATE);
			if (d2 != null && d1.equals(d2)) {
				remove = false;
			}
		} else if (d1 != null && StringUtils.equals(k, FilterMap.GT.getId())) {
			DateTime d2 = Calendars.parse(parameter, Calendars.DATE);
			if (d2 != null && d1.isAfter(d2)) {
				remove = false;
			}
		} else if (d1 != null && StringUtils.equals(k, FilterMap.LT.getId())) {
			DateTime d2 = Calendars.parse(parameter, Calendars.DATE);
			if (d2 != null && d1.isBefore(d2)) {
				remove = false;
			}
		} else if (d1 != null && StringUtils.equals(k, FilterMap.RANGE.getId())) {
			DateTime min = Calendars.parse(StringUtils.substringBefore(v, ","), Calendars.DATE);
			DateTime max = Calendars.parse(StringUtils.substringAfter(v, ","), Calendars.DATE);
			if (min != null && max != null) {
				if ((d1.isEqual(min) || d1.isAfter(min)) && (d1.isEqual(max) || d1.isBefore(max))) {
					remove = false;
				}
			}
		} else if (StringUtils.equals(k, FilterMap.IN.getId()) && StringUtils.contains(v, "," + parameter + ",")) {
			remove = false;
		} else if (StringUtils.equals(k, FilterMap.NONE.getId()) && StringUtils.isBlank(parameter)) {
			remove = false;
		}
		
		return remove;
	}
}
