package cn.edm.repository.handler;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.logging.Filter;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.constants.Cnds;
import cn.edm.constants.Config;
import cn.edm.constants.Options;
import cn.edm.constants.Value;
import cn.edm.constants.mapper.PropMap;
import cn.edm.domain.Prop;
import cn.edm.domain.Tag;
import cn.edm.exception.Errors;
import cn.edm.model.Recipient;
import cn.edm.modules.utils.Props;
import cn.edm.repository.RecipientDao;
import cn.edm.repository.TagDao;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.execute.Exports;
import cn.edm.utils.execute.Hashs;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.execute.Streams;
import cn.edm.utils.file.Files;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;
import com.mongodb.BasicDBList;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;

@Repository
public class BeforeHandler {

	private static final Logger logger = LoggerFactory.getLogger(BeforeHandler.class);
	
	@Autowired
	private RecipientDao recipientDao;
	@Autowired
	private TagDao tagDao;

	public void map(String opt, String userId, int splitCount, String path, List<Tag> inTags, List<Tag> exTags, String activeCnd) {
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

			if (StringUtils.equals(opt, Options.IMPORT)) {
				fis = new FileInputStream(path + "/map.txt");
				br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
				while (br.ready()) {
					String email = br.readLine();
					int index = Hashs.BKDR(email) % splitCount;
					BufferedWriter writer = inBws.get(index);
					writer.write(email + "\r\n");
				}
			} else if (StringUtils.equals(opt, Options.EXPORT)) {
				tagDao.query(inTags, activeCnd, splitCount, inBws);
			} else {
				throw new Errors("Opt(" + opt + ")不是合法值");
			}

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
			logger.info("(BeforeHandler:map) seconds: " + seconds);
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (br != null) {
					br.close();
				}
				if (fis != null) {
					fis.close();
				}
			} catch (IOException e) {
			}
			Streams.closeOutput(inFoss, inBws);
			Streams.closeOutput(exFoss, exBws);
		}
	}
	
	public int reduce(String opt, String userId, int splitCount, String path, String inCnd, int counter, 
			List<Recipient> recipientDbIds, List<Prop> propList, Filter filter) {
		int emailCount = 0;
		
		LinkedList<FileInputStream> inFiss = null;
		LinkedList<BufferedReader> inBrs = null;
		FileInputStream inFis = null;
		BufferedReader inBr = null;
		
		LinkedList<FileInputStream> exFiss = null;
		LinkedList<BufferedReader> exBrs = null;
		FileInputStream exFis = null;
		BufferedReader exBr = null;
		
		FileOutputStream fos = null;
		BufferedWriter bw = null;
		
		try {
			DateTime start = new DateTime();
			
			inFiss = Lists.newLinkedList();
			inBrs = Lists.newLinkedList();
			exFiss = Lists.newLinkedList();
			exBrs = Lists.newLinkedList();

			Map<String, Integer> lineMap = Maps.newHashMap();
			Map<String, Prop> propMap = propMap(propList);
			Map<String, String> filterMap = filterMap(filter);

			BasicDBList emailList = new BasicDBList();
			Map<String, Recipient> recipientMap = Maps.newHashMap();
			Map<String, String> parameterMap = Maps.newHashMap();
			Map<String, String> queryMap = null;
			
			fos = new FileOutputStream(path + "/reduce.txt");
			bw = new BufferedWriter(new OutputStreamWriter(fos, "UTF-8"));

			line(bw, opt, propMap); // 属性
			
			int batch = 500;
			int count = 0;
			
			for (int i = 0; i < splitCount; i++) {
				// 1) 标签筛选
				inFis = new FileInputStream(path + "/in.map." + (i + 1) + ".txt");
				inBr = new BufferedReader(new InputStreamReader(inFis, "UTF-8"));
				inFiss.add(inFis);
				inBrs.add(inBr);
				
				if (StringUtils.equals(inCnd, Cnds.AND)) {
					while (inBr.ready()) {
						String email = StringUtils.substringBefore(inBr.readLine(), ",");
						Integer reappear = 1;
						if (lineMap.containsKey(email)) {
							reappear++;
						}
						lineMap.put(email, reappear);
					}
					Iterator<Entry<String, Integer>> iter = lineMap.entrySet().iterator();
					while (iter.hasNext()) {
						Integer reappear = iter.next().getValue();
						if (reappear != counter) {
							iter.remove();
						}
					}
				} else if (StringUtils.equals(inCnd, Cnds.OR)) {
					while (inBr.ready()) {
						String email = StringUtils.substringBefore(inBr.readLine(), ",");
						if (!lineMap.containsKey(email)) {
							lineMap.put(email, Value.I);
						}
					}
				} else {
					throw new Errors("Cnd(" + inCnd + ")不是合法值");
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
						String email = StringUtils.substringBefore(exBr.readLine(), ",");
						if (lineMap.containsKey(email)) {
							lineMap.remove(email);
						}
					}
				}
				
				// 2) 过滤器筛选
				Exports.email(lineMap, filterMap, propMap);
				
				for (String line : lineMap.keySet()) {
					emailList.add(line);
					count++;
					if (count == batch) {
						recipientDao.filter(opt, bw, userId, recipientDbIds, propMap, filterMap, emailList, recipientMap, parameterMap, queryMap);
						emailCount = emailCount + emailList.size();
					
						clear(emailList, recipientMap);
						count = 0;
					}
				}
				
				if (!Asserts.empty(emailList)) {
					recipientDao.filter(opt, bw, userId, recipientDbIds, propMap, filterMap, emailList, recipientMap, parameterMap, queryMap);
					emailCount = emailCount + emailList.size();
					
					clear(emailList, recipientMap);
					count = 0;
				}

				lineMap.clear();
			}
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(BeforeHandler:reduce) seconds: " + seconds);
			
			return emailCount;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (bw != null) {
					bw.close();
				}
				if (fos != null) {
					fos.close();
				}
			} catch (IOException e) {
			}
			Streams.closeInput(inFiss, inBrs);
			Streams.closeInput(exFiss, exBrs);
		}
	}
	
	private void clear(BasicDBList emailList, Map<String, Recipient> recipientMap) {
		emailList.clear();
		recipientMap.clear();
	}
	
	public void db(String userId, String path, String desc, int emailCount, List<Tag> tagList, int maxTagDbId) {
		Map<Integer, DB> tagDbMap = Maps.newHashMap();
		Map<Integer, DBCollection> tagCollMap = Maps.newHashMap();
		Map<Integer, List<DBObject>> tagMap = Maps.newHashMap();
		
		FileInputStream fis = null;
		BufferedReader br = null;
		try {
			DateTime start = new DateTime();
			
			long now = new DateTime().getMillis();
			
			int maxColls = Props.getInt(Config.MONGO_COLL_SIZE);
			int maxTags = Props.getInt(Config.MONGO_TAG_SIZE);
			int batch = 100;
			
			tagDao.start(tagDbMap, tagCollMap, tagMap, userId, maxTagDbId, maxColls, maxTags, emailCount, tagList);
			
			fis = new FileInputStream(path + "/reduce.txt");
			br = new BufferedReader(new InputStreamReader(fis, "UTF-8"));
			
			while (br.ready()) {
				String email = br.readLine();
				Imports.addTag(tagMap, email, desc, now);
				tagDao.save(tagCollMap, tagMap, tagList, true, batch);
			}
			tagDao.save(tagCollMap, tagMap, tagList, false, 0);

			for (Tag tag : tagList) {
				int count = (int) tagCollMap.get(tag.getTagId()).count();
				tag.setEmailCount(count);
			}
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(BeforeHandler:db) seconds: " + seconds);
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			tagDao.end(tagDbMap);
			try {
				if (br != null) {
					br.close();
				}
				if (fis != null) {
					fis.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
	private Map<String, Prop> propMap(List<Prop> propList) {
		Map<String, Prop> propMap = null;
		if (!Asserts.empty(propList)) {
			propMap = Maps.newHashMap();
			for (Prop prop : propList) {
				propMap.put(prop.getPropName(), prop);
			}
		}

		return propMap;
	}

	private Map<String, String> filterMap(Filter filter) {
		Map<String, String> filterMap = null;
		if (filter != null) {
		}

		return filterMap;
	}
	
	private void line(BufferedWriter bw, String opt, Map<String, Prop> propMap) throws IOException {
		if (StringUtils.equals(opt, Options.OUTPUT)) {
			StringBuffer sbff = new StringBuffer();
			sbff.append(PropMap.EMAIL.getName());
			for (String prop : propMap.keySet()) {
				if (StringUtils.equals(prop, PropMap.EMAIL.getId())) {
					continue;
				}
				sbff.append(",").append(Values.get(PropMap.getName(prop), prop));
			}
			bw.write(sbff.toString() + "\r\n");
		}
	}
}
