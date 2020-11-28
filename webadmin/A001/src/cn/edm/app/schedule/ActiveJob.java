package cn.edm.app.schedule;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;

import cn.edm.app.auth.Auth;
import cn.edm.consts.Config;
import cn.edm.consts.Fqn;
import cn.edm.consts.Quartz;
import cn.edm.consts.Queue;
import cn.edm.model.Copy;
import cn.edm.model.Tag;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.Property;
import cn.edm.repository.TagDao;
import cn.edm.service.CopyService;
import cn.edm.service.TagService;
import cn.edm.utils.Asserts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public class ActiveJob {

	private static final Logger logger = LoggerFactory.getLogger(ActiveJob.class);

	@Autowired
	private CopyService copyService;
	@Autowired
	private Ehcache ehcache;
	@Autowired
	private TagDao tagDao;
	@Autowired
	private TagService tagService;

	public void execute() {
		if (!Auth.isSetup()) {
			return;
		}

		try {
			scan();
			queue();
		} catch (Exception e) {
			logger.error("(ActiveJob:execute) error: ", e);
		}
	}

	@SuppressWarnings("unchecked")
	private void scan() {
		Map<String, Integer> scanMap = (Map<String, Integer>) ehcache.get(Fqn.ACTIVE, Queue.ACTIVE);
		if (scanMap == null) {
			scanMap = Maps.newHashMap();
			ehcache.put(Fqn.ACTIVE, Queue.ACTIVE, scanMap);
		}

		File logs = new File(Property.getStr(Config.ACTIVE_LOGGER));
		if (!logs.isDirectory()) {
			return;
		}

		if (logs.listFiles() != null) {
			for (File log : logs.listFiles()) {
				String name = log.getName();
				if (scanMap.containsKey(name)) {
					continue;
				}
				scanMap.put(name, Quartz.WAITING);
			}
		}

		logger.info("(ActiveJob:scan) scanMap: " + scanMap);
	}

	@SuppressWarnings("unchecked")
	private void queue() {
		Map<String, Integer> scanMap = (Map<String, Integer>) ehcache.get(Fqn.ACTIVE, Queue.ACTIVE);
		if (Asserts.empty(scanMap)) {
			return;
		}

		Iterator<Entry<String, Integer>> iter = scanMap.entrySet().iterator();
		while (iter.hasNext()) {
			Entry<String, Integer> entry = iter.next();
			if (entry.getValue() == null || entry.getValue().equals(Quartz.PROCESSING)) {
				continue;
			}

			try {
				entry.setValue(Quartz.PROCESSING);
				active(entry.getKey());
			} catch (Exception e) {
				throw new Errors("(ActiveJob:queue) name: " + entry.getKey() + ", error: ", e);
			} finally {
				iter.remove();
			}
		}
	}

	private void active(String name) {
		String path = Property.getStr(Config.ACTIVE_LOGGER) + "/" + name;
		BufferedReader reader = null;
		boolean completed = false;

		try {
			reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));

			DateTime t1 = new DateTime();

			String[] nextLine = null;
			String line = null;

			Map<Integer, Tag> tagMap = Maps.newHashMap();
			int batch = 2000;

			List<Copy> copyTagList = Lists.newArrayList(); // CopyTrigger
			
			while ((line = reader.readLine()) != null) {
				nextLine = StringUtils.splitPreserveAllTokens(StringUtils.removeStart(line, "(Active:ok) "), "\t");
				if (nextLine.length != 3) {
					continue;
				}

				String tagId = (String) Validator.validate(nextLine[0], R.REQUIRED, R.INTEGER);
				String email = (String) Validator.validate(nextLine[1], R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}");
				String activeTime = (String) Validator.validate(nextLine[2], R.REQUIRED, R.LONG);

				if (StringUtils.isBlank(tagId) || StringUtils.isBlank(email) || StringUtils.isBlank(activeTime)) {
					continue;
				}

				Tag tag = tagMap.get(tagId);
				if (!tagMap.containsKey(tagId)) {
					Integer id = Integer.valueOf(tagId);
					tag = tagService.get(id);
					tagMap.put(id, tag);
				}

				if (tag == null) {
					continue;
				}

				tagDao.update(tag, email, Long.valueOf(activeTime));
				
				if (tagMap.size() == batch) {
				    for (Tag e : tagMap.values()) {
                        if (e == null) continue;
	                    copyTagList.add(new Copy(e.getCorpId(), e.getDbId(), e.getTagId(), Copy.MODIFY));
	                }
	                copyService.batchSave(Copy.TAG, copyTagList);
                    copyTagList.clear();
					tagMap.clear();
				}
			}
			
            if (!Asserts.empty(tagMap)) {
                for (Tag e : tagMap.values()) {
                    if (e == null) continue;
                    copyTagList.add(new Copy(e.getCorpId(), e.getDbId(), e.getTagId(), Copy.MODIFY));
                }
                copyService.batchSave(Copy.TAG, copyTagList);
                copyTagList.clear();
                tagMap.clear();
            }
			
			completed = true;

			DateTime t2 = new DateTime();
			long seconds = Seconds.secondsBetween(t1, t2).getSeconds();
			logger.info("(ActiveJob:active) name: " + name + ", seconds: " + seconds);
		} catch (Error e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
				if (completed) {
					if (Files.exists(path) && StringUtils.endsWith(path, ".log")) {
						Files.delete(path);
					}
				}
			} catch (IOException e) { logger.error(e.getMessage(), e); }
		}
	}
}