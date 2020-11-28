package cn.edm.repository.handler;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Config;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Recipient;
import cn.edm.model.Tag;
import cn.edm.modules.utils.Property;
import cn.edm.repository.RecipientDao;
import cn.edm.repository.TagDao;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Exports;
import cn.edm.utils.execute.Hashs;
import cn.edm.utils.execute.R;
import cn.edm.utils.execute.Streams;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;
import com.mongodb.BasicDBList;

@Repository
public class AfterHandler {

	private static final Logger logger = LoggerFactory.getLogger(AfterHandler.class);

	@Autowired
	private RecipientDao recipientDao;
	@Autowired
	private TagDao tagDao;

	public void map(String userId, String SID, int splitCount, String path, Map<String, String> emailMap, String fileId, List<Tag> inTags, List<Tag> exTags) {
		LinkedList<FileOutputStream> inFoss = null;
		LinkedList<BufferedWriter> inBws = null;
		FileOutputStream inFos = null;
		BufferedWriter inBw = null;
		
		LinkedList<FileOutputStream> exFoss = null;
		LinkedList<BufferedWriter> exBws = null;
		FileOutputStream exFos = null;
		BufferedWriter exBw = null;
		
		FileInputStream fis = null;
		BufferedReader br = null;
		
		BufferedReader reader = null;
		
		try {
			DateTime start = new DateTime();
			
			inFoss = Lists.newLinkedList();
			inBws = Lists.newLinkedList();
			
			exFoss = Lists.newLinkedList();
			exBws = Lists.newLinkedList();
			
			Files.make(path);
			
			for (int i = 0; i < splitCount; i++) {
				inFos = new FileOutputStream(path + "/in.map." + (i + 1) + ".txt", false);
				inBw = new BufferedWriter(new OutputStreamWriter(inFos, "UTF-8"));
				inFoss.add(inFos);
				inBws.add(inBw);
			}

			// 1) 邮箱地址
			if (!Asserts.empty(emailMap)) {
				for (Entry<String, String> entry : emailMap.entrySet()) {
					String email = entry.getKey();
					int index = Hashs.BKDR(email) % splitCount;
					BufferedWriter writer = inBws.get(index);
					writer.write(email + "\r\n");
				}
			}
			
			// 2) 收件人地址
			if (StringUtils.isNotBlank(fileId)) {
				String r = Webs.rootPath() + PathMap.R.getPath() + userId + "/" + fileId + "/reduce.txt";
				if (Files.exists(r)) {
					reader = new BufferedReader(new InputStreamReader(new FileInputStream(r), "UTF-8"));
					String line = null;
					while ((line = reader.readLine()) != null) {
						if (StringUtils.isBlank(line)) {
							continue;
						}
						String email = line;
						int index = Hashs.BKDR(email) % splitCount;
						BufferedWriter writer = inBws.get(index);
						writer.write(email + "\r\n");
					}
				}
			}
			
			// 3) 包含标签
			if (!Asserts.empty(inTags)) {
				tagDao.query(inTags, null, splitCount, inBws);
			}
			
			// 4) 排除标签
			if (!Asserts.empty(exTags)) {
				for (int i = 0; i < splitCount; i++) {
					exFos = new FileOutputStream(path + "/ex.map." + (i + 1) + ".txt", false);
					exBw = new BufferedWriter(new OutputStreamWriter(exFos, "UTF-8"));
					exFoss.add(exFos);
					exBws.add(exBw);
				}
				
				tagDao.query(exTags, null, splitCount, exBws);
			}
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(AfterHandler:map) SID: " + SID + ", seconds: " + seconds);
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (br != null) {
					br.close();
				}
				if (fis != null) {
					fis.close();
				}
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
			Streams.closeOutput(inFoss, inBws);
			Streams.closeOutput(exFoss, exBws);
		}
	}
	
	public int reduce(PrintWriter writer, Integer corpId, String userId, String SID, int splitCount, String path, String fileId, List<Recipient> recipientDbIds, String receiver, Map<Integer, String> propMap) {
		LinkedList<FileInputStream> inFiss = null;
		LinkedList<BufferedReader> inBrs = null;
		FileInputStream inFis = null;
		BufferedReader inBr = null;
		
		LinkedList<FileInputStream> exFiss = null;
		LinkedList<BufferedReader> exBrs = null;
		FileInputStream exFis = null;
		BufferedReader exBr = null;
		
		try {
			DateTime start = new DateTime();
			
			inFiss = Lists.newLinkedList();
			inBrs = Lists.newLinkedList();

			exFiss = Lists.newLinkedList();
			exBrs = Lists.newLinkedList();
			
			Map<String, Integer> lineMap = Maps.newHashMap();
			Map<String, String> filterMap = filterMap(Property.getStr(Config.FILTER_PROPS));
			
			Map<String, String> parameterMap = Maps.newHashMap();
			Map<String, String> emailMap = Maps.newHashMap();
			BasicDBList emailList = new BasicDBList();
			
			int counter = 0;
			int batch = 1000;
			int count = 0;
			
			boolean hasParameters = hasParameters(receiver, propMap);
			
			String r = "";
			if (StringUtils.isNotBlank(fileId) && (StringUtils.startsWith(fileId, "IN.") || StringUtils.startsWith(fileId, "EXPORT_"))) {
				r = Webs.rootPath() + PathMap.R.getPath() + userId + "/" + fileId + "/map.txt";
			}
			
			for (int i = 0; i < splitCount; i++) {
				inFis = new FileInputStream(path + "/in.map." + (i + 1) + ".txt");
				inBr = new BufferedReader(new InputStreamReader(inFis, "UTF-8"));
				inFiss.add(inFis);
				inBrs.add(inBr);
				
				while (inBr.ready()) {
					String email = inBr.readLine();
					if (!lineMap.containsKey(email)) {
						lineMap.put(email, Value.I);
					}
				}

				if (Asserts.empty(lineMap)) {
					continue;
				}
				
				if (new File(path + "/ex.map." + (i + 1) + ".txt").exists()) {
					exFis = new FileInputStream(path + "/ex.map." + (i + 1) + ".txt");
					exBr = new BufferedReader(new InputStreamReader(exFis, "UTF-8"));
					exFiss.add(exFis);
					exBrs.add(exBr);
					
					while (exBr.ready()) {
						String email = exBr.readLine();
						if (lineMap.containsKey(email)) {
							lineMap.remove(email);
						}
					}
				}

				Exports.email(lineMap, filterMap, null);
				
				for (Entry<String, Integer> entry : lineMap.entrySet()) {
					String email = entry.getKey();
					emailList.add(email);
					
					count++;
					if (count == batch) {
						counter += content(writer, corpId, r, recipientDbIds, receiver, propMap, emailList, parameterMap, emailMap, hasParameters);
						count = 0;
					}
				}
				
				if (count > 0) {
					counter += content(writer, corpId, r, recipientDbIds, receiver, propMap, emailList, parameterMap, emailMap, hasParameters);
					count = 0;
				}
				
				lineMap.clear();
			}

			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(AfterHandler:reduce) SID: " + SID + ", seconds: " + seconds);
			
			return counter;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			Streams.closeInput(inFiss, inBrs);
			Streams.closeInput(exFiss, exBrs);
		}
	}
	
	private boolean hasParameters(String receiver, Map<Integer, String> propMap) {
		boolean hasParameters = false;
		
		if (receiver.equals("true_name")) {
			hasParameters = true;
		} else if (receiver.equals("nick_name")) {
			hasParameters = true;
		}
		
		if (!Asserts.empty(propMap)) {
			if (propMap.containsKey(PropMap.EMAIL.getId())) {
				if (propMap.size() > 1) {
					hasParameters = true;
				}
			} else {
				if (propMap.size() > 0) {
					hasParameters = true;
				}
			}
		}
		
		return hasParameters;
	}
	
	private int content(PrintWriter writer, Integer corpId, String r, List<Recipient> recipientDbIds, String receiver, Map<Integer, String> propMap, 
			BasicDBList emailList, Map<String, String> parameterMap, Map<String, String> emailMap, boolean hasParameters) {
		int counter = 0;

		if (hasParameters && !Asserts.empty(recipientDbIds)) {
			for (Recipient dbId : recipientDbIds) {
				Map<String, String> queryMap = recipientDao.query(corpId, emailList, dbId.getDbId());
				emailMap(emailMap, queryMap);
			}
		}
		
		if (hasParameters && StringUtils.isNotBlank(r) && Files.exists(r)) {
			Map<String, String> queryMap = R.query(r, emailList);
			emailMap(emailMap, queryMap);
		}
		
		for (Object e : emailList) {
			String email = (String) e;
			String parameters = Value.S;
			if (!Asserts.empty(emailMap) && emailMap.containsKey(email)) {
				parameters = emailMap.get(email);
			}
			Delivery.CONTENT(writer, receiver, propMap, parameterMap, email, parameters);
			parameterMap.clear();
			counter++;
		}
		
		emailMap.clear();
		emailList.clear();
		return counter;
	}
	
	private void emailMap(Map<String, String> emailMap, Map<String, String> queryMap) {
		if (!Asserts.empty(queryMap)) {
			for (Entry<String, String> entry : queryMap.entrySet()) {
				String email = entry.getKey();
				String parameter = entry.getValue();
				String parameters = Values.get(emailMap.get(email));
				parameters = parameters + parameter;
				emailMap.put(email, parameters);
			}
		}
		queryMap.clear();
	}
	
	private Map<String, String> filterMap(String props) {
		Map<String, String> filterMap = null;
		if (StringUtils.isNotBlank(props)) {
			filterMap = Maps.newHashMap();
			for (String prop : StringUtils.split(props, "&")) {
				String key = StringUtils.substringBefore(prop, ":");
				String value = StringUtils.substringAfter(prop, ":");
				filterMap.put(key, value);
			}
		}

		return filterMap;
	}
}
