package cn.edm.repository;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.regex.Pattern;

import org.apache.commons.lang.StringUtils;
import org.bson.types.ObjectId;
import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Sync;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.TagMap;
import cn.edm.model.Category;
import cn.edm.model.Recipient;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Mongos;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Hashs;
import cn.edm.utils.execute.Imports;

import com.google.common.collect.Lists;
import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.DBObject;

@Repository
public class TagDao {

	@Autowired
	private Dao dao;

	public void save(int corpId) {
		MapBean mb = new MapBean();
		mb.put("corpId", corpId, "categoryName", CategoryMap.UNSUBSCRIBE.getName(), "type", Category.TAG, "nameCnd", Cnds.EQ);
        Category category = dao.get("Category.query", mb);
        
        List<Tag> tags = Lists.newArrayList();
		tags.add(tag(corpId, null, category.getCategoryId(), TagMap.UNSUBSCRIBE.getName(), TagMap.UNSUBSCRIBE.getDesc()));
		dao.save("Tag.batchSave", tags);
	}
	
    private Tag tag(int corpId, String userId, Integer categoryId, String tagName, String tagDesc) {
		Tag tag = new Tag();
		tag.setDbId(Value.I);
		tag.setCategoryId(categoryId);
		tag.setCorpId(corpId);
		tag.setUserId(userId);
		tag.setTagName(tagName);
		tag.setTagDesc(tagDesc);
		tag.setEmailCount(Value.I);
		return tag;
	}
    
	public void start(Map<Integer, DB> dbMap, Map<Integer, DBCollection> collMap, Map<Integer, List<DBObject>> tagMap, 
			Integer corpId, int maxDbId, int maxColls, int maxTags, int emailCount, List<Tag> tags) {
		for (Tag tag : tags) {
			synchronized (Sync.TAG) {
				List<DBObject> tagList = Lists.newArrayList();
				tagMap.put(tag.getTagId(), tagList);

				DB db = getDb(maxDbId, maxColls, tag);
				db.requestStart();
				dbMap.put(tag.getTagId(), db);

				DBCollection coll = getColl(db, corpId, maxTags, emailCount, tag);
				collMap.put(tag.getTagId(), coll);
			}
		}
	}

	public void end(Map<Integer, DB> dbMap) {
		if (!Asserts.empty(dbMap)) {
			for (Integer tagId : dbMap.keySet()) {
				DB db = dbMap.get(tagId);
				db.requestDone();
			}
		}
	}
	
	public DBCollection getColl(Integer dbId, Integer corpId, Integer tagId) {
		DB db = Mongos.db("tag_" + dbId);
		return db.getCollection(corpId + "_" + tagId);
	}
	
	public void save(Map<Integer, DBCollection> collMap, Map<Integer, List<DBObject>> tagMap, List<Tag> tags, boolean eq, int batch) {
		for (Tag tag : tags) {
			List<DBObject> tagList = tagMap.get(tag.getTagId());
			if (eq) {
				if (tagList.size() == batch) {
					collMap.get(tag.getTagId()).insert(tagList);
					tagList.clear();
				}
			} else {
				if (tagList.size() > 0) {
					collMap.get(tag.getTagId()).insert(tagList);
					tagList.clear();
				}
			}
		}
	}
	
	public void save(int maxDbId, Tag tag, Recipient recipient, String orgEmail) {
		int maxColls = Property.getInt(Config.MONGO_COLL_SIZE);
		int maxTags = Property.getInt(Config.MONGO_TAG_SIZE);
		
		DB db = null;
		DBCollection coll = null;
		synchronized (Sync.TAG) {
			db = getDb(maxDbId, maxColls, tag);
			coll = getColl(db, tag.getCorpId(), maxTags, 1, tag);
		}
		
		long millis = new DateTime().getMillis();
		
		if (StringUtils.isNotBlank(orgEmail)) {
			BasicDBObject query = new BasicDBObject();
			query.put("email", orgEmail);
			DBObject next = coll.findOne(query);
			BasicDBObject set = new BasicDBObject();
			set.put("email", recipient.getEmail());
			set.put("create_time", next != null ? Values.get((Long) next.get("create_time"), millis) : millis);
			set.put("modify_time", millis);
            coll.update(query, new BasicDBObject("$set", set), true, false);
		} else {
			coll.insert(Imports.addTag(recipient.getEmail(), millis));
		}

		tag.setEmailCount((int) coll.count());
	}
	
	public void update(Tag tag, String email, long activeTime) {
		if (tag.getDbId().equals(Value.I)) {
			return;
		}
		
		DBCollection coll = getColl(tag.getDbId(), tag.getCorpId(), tag.getTagId());

		BasicDBObject query = new BasicDBObject();
		query.put("email", email);

		BasicDBObject set = new BasicDBObject();
		set.put("active_time", activeTime);

		coll.update(query, new BasicDBObject("$set", set));
	}
	
	public void delete(Tag tag, String[] emails) {
		if (tag.getDbId().equals(Value.I)) {
			return;
		}
		
		DBCollection coll = getColl(tag.getDbId(), tag.getCorpId(), tag.getTagId());
		for (String email : emails) {
			coll.remove(new BasicDBObject("email", email));
		}
		tag.setEmailCount((int) coll.count());
	}
	
	public void clear(Tag tag) {
		if (tag.getDbId().equals(Value.I)) {
			return;
		}

		DBCollection coll = getColl(tag.getDbId(), tag.getCorpId(), tag.getTagId());
		coll.drop();
	}
	
	/**
	 * activeCnd(
	 *     -1:不活跃,
	 *     0:全部,
	 *     1:活跃,
	 *     1week:一周以前活跃,
	 *     1month:一个月以前活跃,
	 *     2month:二个月以前活跃,
	 *     3month:三个月以前活跃
	 * )
	 */
	public void query(List<Tag> tags, String activeCnd, int splitCount, LinkedList<BufferedWriter> bws) throws IOException {
		BasicDBObject filter = new BasicDBObject();
		filter.append("email", 1).append("_id", 0);

		BasicDBObject query = null;
		if (StringUtils.isNotBlank(activeCnd)) {
			DateTime now = new DateTime();
			query = new BasicDBObject();
			if (activeCnd.equals("-1")) {
				query.append("active_time", null);
			} else if (activeCnd.equals("0")) {
				query = null;
			} else if (activeCnd.equals("1")) {
				query.append("active_time", new BasicDBObject("$ne", null));
			} else if (activeCnd.equals("1week")) {
				query.append("active_time", new BasicDBObject("$lte", now.plusWeeks(-1).getMillis()));
			} else if (activeCnd.equals("1month")) {
				query.append("active_time", new BasicDBObject("$lte", now.plusMonths(-1).getMillis()));
			} else if (activeCnd.equals("2month")) {
				query.append("active_time", new BasicDBObject("$lte", now.plusMonths(-2).getMillis()));
			} else if (activeCnd.equals("3month")) {
				query.append("active_time", new BasicDBObject("$lte", now.plusMonths(-3).getMillis()));
			}
		}
		
		for (Tag tag : tags) {
			if (tag.getDbId() < 1) {
				continue;
			}
			
			DBCursor cursor = getColl(tag.getDbId(), tag.getCorpId(), tag.getTagId()).find(query, filter);
			cursor.batchSize(500);
			while (cursor.hasNext()) {
				String email = cursor.next().get("email").toString();
				int index = Hashs.BKDR(email) % splitCount;
				BufferedWriter writer = bws.get(index);
				writer.write(email + "\r\n");
			}
		}
	}
	
	public List<String> find(Integer corpId, Tag tag) {
		if (tag.getDbId() < 1) {
			return null;
		}
		
		List<String> emailList = Lists.newArrayList();

		BasicDBObject filter = new BasicDBObject();
		filter.append("email", 1).append("_id", 0);

		DBCursor cursor = getColl(tag.getDbId(), corpId, tag.getTagId()).find(null, filter);
		cursor.batchSize(500);
		while (cursor.hasNext()) {
			String email = cursor.next().get("email").toString();
			emailList.add(email);
		}
		
		return emailList;
	}
	
	public Page<Recipient> find(DBCollection coll, Page<Recipient> page, String activeCnd, String _id, String email, String beginTime, String endTime) {
		List<Recipient> result = Lists.newArrayList();
		BasicDBObject query = new BasicDBObject();
		
		append(query, activeCnd, email, beginTime, endTime);
		query.append("_id", new BasicDBObject("$lt", new ObjectId(_id)));

		// 暂不支持modify_time分页排序
        DBCursor cursor = coll.find(query).sort(new BasicDBObject("_id", -1)).limit(page.getPageSize() * 5);
		cursor.batchSize(100);
		while (cursor.hasNext()) {
			DBObject next = cursor.next();
			if (result.size() < page.getPageSize()) {
				addRecipient(result, next);
			}
		}
		page.setResult(result);
		return page;
	}
	
	private void addRecipient(List<Recipient> result, DBObject next) {
		Recipient r = new Recipient();
		r.setEmail(next.get("email").toString());
		if (next.get("create_time") != null) r.setCreateTime(Calendars.date(next.get("create_time").toString()));
		if (next.get("modify_time") != null) r.setModifyTime(Calendars.date(next.get("modify_time").toString()));
        if (next.get("active_time") != null) r.setActiveTime(Calendars.date(next.get("active_time").toString()));
        if (next.get("desc") != null)  r.setDesc(next.get("desc").toString());
		result.add(r);
	}
	
	public long count(Tag tag, String email) {
		if (tag.getDbId().equals(Value.I)) {
			return 0;
		}
		
		DBCollection coll = getColl(tag.getDbId(), tag.getCorpId(), tag.getTagId());
		return coll.count(new BasicDBObject("email", email));
	}
	
	public long count(DBCollection coll, Page<Recipient> page, String activeCnd, String email, String beginTime, String endTime) {
		BasicDBObject query = new BasicDBObject();
		append(query, activeCnd, email, beginTime, endTime);
		return coll.count(query);
	}
	
	public void slider(boolean isNext, DBCollection coll,
			Page<Recipient> page, Map<Integer, String> mapper, String activeCnd, String _id,
			String email, String beginTime, String endTime) {
		int offset = isNext ? (page.getPageNo() + 1) : (page.getPageNo() - 1);
		int pos = 0;

		BasicDBObject query = new BasicDBObject();
		query.append("_id", new BasicDBObject(isNext ? "$lt" : "$gt", new ObjectId(_id)));
		append(query, activeCnd, email, beginTime, endTime);

		DBCursor cursor = coll.find(query).sort(new BasicDBObject("_id", isNext ? -1 : 1)).limit(page.getPageSize() * 5);
		cursor.batchSize(100);

		while (cursor.hasNext()) {
			DBObject next = cursor.next();
			pos++;
			if (pos == page.getPageSize()) {
				mapper.put(isNext ? offset++ : offset--, next.get("_id").toString());
				pos = 0;
			}
		}

		Iterator<Entry<Integer, String>> iter = mapper.entrySet().iterator();
		while (iter.hasNext()) {
			Integer pageNo = iter.next().getKey();
			if (pageNo == 1 || pageNo == (int) page.getTotalPages()) {
				continue;
			}
			if (pageNo > (page.getPageNo() + 5) || pageNo < (page.getPageNo() - 5)) {
				iter.remove();
			}
		}
	}
	
	public String last(DBCollection coll, Page<Recipient> page, String activeCnd, String email, String beginTime, String endTime) {
		String last = Value.S;
		
		BasicDBObject query = new BasicDBObject();
		query.append("_id", new BasicDBObject("$gt", new ObjectId("000000000000000000000000")));
		
		append(query, activeCnd, email, beginTime, endTime);
		
		int offset = (int) ((page.getTotalPages() - 1) * page.getPageSize());
		int pos = (int) page.getTotalItems();
		
		DBCursor cursor = coll.find(query).sort(new BasicDBObject("_id", 1)).limit(page.getPageSize());
		cursor.batchSize(100);
		
		while (cursor.hasNext()) {
			DBObject next = cursor.next();
			if (pos == offset) {
				last = next.get("_id").toString();
				break;
			}
			pos--;
		}
		
		return last;
	}

	private void append(BasicDBObject query, String activeCnd, String email, String beginTime, String endTime) {
		if (activeCnd.equals("active")) {
			query.append("active_time", new BasicDBObject("$ne", null));
		} else if (activeCnd.equals("inactive")) {
			query.append("active_time", null);
		}
		if (StringUtils.isNotBlank(email)) {
			query.append("email", Pattern.compile("^.*" + email + ".*$"));
		}
		BasicDBObject time = new BasicDBObject();
		if (StringUtils.isNotBlank(beginTime)) {
			time.append("$gt", Calendars.millis(beginTime + " 00:00:00", "yyyy-MM-dd HH:mm:ss"));
		}
		if (StringUtils.isNotBlank(endTime)) {
			time.append("$lt", Calendars.millis(endTime + " 23:59:59", "yyyy-MM-dd HH:mm:ss"));
		}
		if (!time.isEmpty()) {
			query.append("modify_time", time);
		}
	}
	
	private DB getDb(int maxDbId, int maxColls, Tag tag) {
		DB db = null;
		if (tag.getDbId() > 0) {
			db = Mongos.db("tag_" + tag.getDbId());
		} else {
			db = Mongos.db("tag_" + maxDbId);
			if (db.getCollectionNames().size() > maxColls) {
				maxDbId++;
				db = Mongos.db("tag_" + maxDbId);
			}

			tag.setDbId(maxDbId);
		}

		return db;
	}
	
	private DBCollection getColl(DB db, Integer corpId, int maxTags, int emailCount, Tag tag) {
		DBCollection coll = db.getCollection(corpId + "_" + tag.getTagId());
		int count = (int) coll.count();
		if ((count + emailCount) > maxTags) {
			throw new Errors(tag.getTagName() + "的收件人不能多于" + maxTags);
		}
		/*coll.ensureIndex(new BasicDBObject("email", "1"), "idx1", true);
		coll.ensureIndex(new BasicDBObject("modify_time", "1"), "idx2");
		coll.ensureIndex(new BasicDBObject("active_time", "1"), "idx3");*/
		coll.createIndex(new BasicDBObject("email",1));
		coll.createIndex(new BasicDBObject("modify_time",1));
		coll.createIndex(new BasicDBObject("active_time",1));
		return coll;
	}
}
