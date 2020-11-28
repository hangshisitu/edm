package cn.edm.repository.execute;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Config;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Copy;
import cn.edm.model.Prop;
import cn.edm.modules.utils.Property;
import cn.edm.repository.RecipientDao;
import cn.edm.utils.Asserts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.execute.Pos;
import cn.edm.utils.file.BOM;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;

@Repository
public class Text {

	private static final Logger logger = LoggerFactory.getLogger(Text.class);

	@Autowired
	private RecipientDao recipientDao;
	
	public int mailPos(PathMap path, String fileId, MultipartFile upload, String charset, Integer[] propIds, Map<Integer, Prop> propMap, int mailPos) {
		int emailCount = 0;
		String filePath = path.getPath() + fileId;
		BufferedReader reader = null;
		BufferedWriter writer = null;
		
		try {
			DateTime start = new DateTime();
			
			reader = new BufferedReader(new InputStreamReader(charset.equals("UTF-8") ? 
					BOM.charset(upload.getInputStream()).getInputStream() : upload.getInputStream(), charset));
			writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + filePath + "/map.txt"), "UTF-8"));
			
			String[] nextLine = null;
			String line = null;
			boolean hasParameters = Imports.hasParameters(propMap, mailPos);
			
			while ((line = reader.readLine()) != null) {
				nextLine = StringUtils.splitPreserveAllTokens(line, ",");
				
				// blank
				if (Asserts.empty(nextLine)) {
					continue;
				}

				// recipient
				if (nextLine.length < mailPos) {
					continue;
				}

				// email
				String email = StringUtils.trim(nextLine[mailPos]);
				email = StringUtils.lowerCase(email);
				if (!Imports.isEmail(email)) {
					continue;
				}
				
				if (path.equals(PathMap.R) && hasParameters) {
					StringBuffer sbff = new StringBuffer();
					sbff.append(email).append(",");
					for (int i = 0; i < nextLine.length; i++) {
						if (i > propIds.length - 1) {
							break;
						}

						String value = StringUtils.trim(nextLine[i]);
						if(value.contains("=")) {
							String[] values = StringUtils.splitPreserveAllTokens(value, "=");
							value = values[1];
						}
						if (!Imports.hasParameter(i, mailPos, propMap, value, sbff)) {
							continue;
						}
					}
					writer.write(sbff.toString() + "\r\n");
				} else {
					writer.write(email + "\r\n");
				}
				emailCount++;
			}
			
			writer.flush();
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(" + Files.suffix(upload.getOriginalFilename()) + ":mailPos) seconds: " + seconds);
			
			return emailCount;
		} catch (Error e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
				if (writer != null) {
					writer.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
	public void execute(Integer corpId, MultipartFile upload, String charset, Integer[] propIds, Map<Integer, Prop> propMap, int mailPos, 
			Integer maxRecipientDbId, List<Integer> recipientDbIds, List<Copy> copyRecipientList) {
		List<DB> recipientDbs = Lists.newLinkedList();
		List<DBCollection> recipientColls = Lists.newLinkedList();
		
		BufferedReader reader = null;
		
		try {
			DateTime start = new DateTime();
			
			reader = new BufferedReader(new InputStreamReader(charset.equals("UTF-8") ? 
					BOM.charset(upload.getInputStream()).getInputStream() : upload.getInputStream(), charset));
			List<DBObject> recipientList = Lists.newArrayList();
			
			String[] nextLine = null;
			String line = null;
			
			boolean hasParameters = Imports.hasParameters(propMap, mailPos);
			int maxColls = Property.getInt(Config.MONGO_COLL_SIZE);
			int maxRecipients = Property.getInt(Config.MONGO_RECIPIENT_SIZE);
			int batch = 100;
			
			Pos pos = new Pos();
			recipientDao.start(recipientDbs, recipientColls, corpId, maxRecipientDbId, recipientDbIds, maxColls, pos);
			
			Map<String, Integer[]> copyMap = Maps.newHashMap(); // CopyTrigger
			
			while ((line = reader.readLine()) != null) {
				nextLine = StringUtils.splitPreserveAllTokens(line, ",");
				
				// blank
				if (Asserts.empty(nextLine)) {
					continue;
				}

				// recipient
				if (nextLine.length < mailPos) {
					continue;
				}

				// email
				String email = StringUtils.trim(nextLine[mailPos]);
				email = StringUtils.lowerCase(email);
				if (!Imports.isEmail(email)) {
					continue;
				}

				if (hasParameters) {
					StringBuffer sbff = new StringBuffer();
					for (int i = 0; i < nextLine.length; i++) {
						if (i > propIds.length - 1) {
							break;
						}

						String value = StringUtils.trim(nextLine[i]);
						if (!Imports.hasParameter(i, mailPos, propMap, value, sbff)) {
							continue;
						}
					}
					
					Imports.addRecipient(recipientList, email, sbff.toString());
					int recipientCount = pos.getCount();
					pos.setCount(recipientCount + 1);
				}
				
				if (recipientList.size() == batch) {
					recipientDao.save(recipientDbs, recipientColls, maxColls, maxRecipients, corpId, recipientList, recipientDbIds, pos);
					int maxDbId = pos.getMaxDbId();
                    int lastColl = pos.getLastColl();
                    copyMap.put(maxDbId + "_" + lastColl, new Integer[] { maxDbId, lastColl });
				}
			}
			
			if (recipientList.size() > 0) {
				recipientDao.save(recipientDbs, recipientColls, maxColls, maxRecipients, corpId, recipientList, recipientDbIds, pos);
				int maxDbId = pos.getMaxDbId();
                int lastColl = pos.getLastColl();
                copyMap.put(maxDbId + "_" + lastColl, new Integer[] { maxDbId, lastColl });
			}
			
			if (!Asserts.empty(copyMap)) {
                for (Integer[] arr : copyMap.values()) {
                    copyRecipientList.add(new Copy(corpId, arr[0], arr[1], Copy.MODIFY));
                }
            }
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(" + Files.suffix(upload.getOriginalFilename()) + ":execute) seconds: " + seconds);
		} catch (Error e) {
			logger.error("(" + Files.suffix(upload.getOriginalFilename()) + ":execute) error: ", e);
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			logger.error("(" + Files.suffix(upload.getOriginalFilename()) + ":execute) error: ", e);
			throw new ServiceErrors(e);
		} finally {
			recipientDao.end(recipientDbs);
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
}
