package cn.edm.repository;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.apache.commons.lang.math.NumberUtils;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Config;
import cn.edm.consts.Options;
import cn.edm.consts.Sync;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.model.Copy;
import cn.edm.model.Prop;
import cn.edm.model.Recipient;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Asserts;
import cn.edm.utils.Mongos;
import cn.edm.utils.Values;
import cn.edm.utils.execute.Exports;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.execute.Pos;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;
import com.mongodb.BasicDBList;
import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import com.mongodb.WriteResult;

@Repository
public class RecipientDao {

	public void start(List<DB> dbs, List<DBCollection> colls, Integer corpId, int maxDbId, 
			List<Integer> dbIds, int maxColls, Pos pos) {
		synchronized (Sync.RECIPIENT) {
			DB db = getDb(corpId, maxDbId, maxColls, dbIds, pos);
			db.requestStart();
			dbs.add(db);

			DBCollection coll = getColl(dbs.get(dbs.size() - 1), corpId, pos.getLastColl());
			colls.add(coll);
			
			pos.setCount((int) colls.get(colls.size() - 1).count());
		}
	}
	
	public void end(List<DB> dbs) {
		if (!Asserts.empty(dbs)) {
			for (DB db : dbs) {
				db.requestDone();
			}
		}
	}

	public int max(DB db, Integer corpId) {
		int max = 0;
		Set<String> sets = db.getCollectionNames();
		for (String s : sets) {
			if (StringUtils.startsWith(s, corpId + "_")) {
				String count = StringUtils.removeStart(s, corpId + "_");
				if (NumberUtils.isDigits(count)) {
					if (Integer.valueOf(count) > max) {
						max = Integer.valueOf(count);
					}
				}
			}
		}

		return max;
	}
	
	public void save(List<DB> dbs, List<DBCollection> colls, int maxColls, int maxRecipients, 
			Integer corpId, List<DBObject> recipientList, List<Integer> dbIds, Pos pos) {
		int count = pos.getCount();
		int lastColl = pos.getLastColl();
		int maxDbId = pos.getMaxDbId();
		
		if (count > maxRecipients) {
			int batch = recipientList.size();
			int balance = count - maxRecipients;
			if (balance > 0 && balance < batch) {
				List<DBObject> balanceList = Lists.newArrayList();
				
				for (int i = 0; i < (batch - balance); i++) {
					balanceList.add(recipientList.get(i));
				}
				for (int i = 0; i < (batch - balance); i++) {
					recipientList.remove(0);
				}
				colls.get(colls.size() - 1).insert(balanceList);
				balanceList.clear();
			}
			
			synchronized (Sync.RECIPIENT) {
				if (dbs.get(dbs.size() - 1).getCollectionNames().size() > maxColls) {
					maxDbId++;
					dbIds.add(maxDbId);
					DB nextRecipientDb = Mongos.db("recipient_" + maxDbId);
					dbs.add(nextRecipientDb);
					dbs.get(dbs.size() - 1).requestStart();

					lastColl = 1;
				} else {
					lastColl++;
				}
				DBCollection nextColl = dbs.get(dbs.size() - 1).getCollection(corpId + "_" + lastColl);
				nextColl.ensureIndex(new BasicDBObject("email", "1"), "idx");
				colls.add(nextColl);
			}
			
			count = recipientList.size();
		}

		if (recipientList.size() > 0) {
			colls.get(colls.size() - 1).insert(recipientList);
			recipientList.clear();
		}
		
		pos.setCount(count);
		pos.setLastColl(lastColl);
		pos.setMaxDbId(maxDbId);
	}
	
	public void save(int maxDbId, Recipient recipient, List<Integer> dbIds, List<Copy> copyRecipientList) {
		int maxColls = Property.getInt(Config.MONGO_COLL_SIZE);
		int maxRecipients = Property.getInt(Config.MONGO_RECIPIENT_SIZE);

		Pos pos = new Pos();

		DB db = getDb(recipient.getCorpId(), maxDbId, maxColls, dbIds, pos);
		DBCollection coll = getColl(db, recipient.getCorpId(), pos.getLastColl());
		
		synchronized (Sync.RECIPIENT) {
			if (pos.getCount() + 1 > maxRecipients) {
				if (db.getCollectionNames().size() > maxColls) {
					maxDbId++;
					dbIds.add(maxDbId);
					db = Mongos.db("recipient_" + maxDbId);
					pos.setLastColl(1);
				} else {
					pos.setLastColl(pos.getLastColl() + 1);
				}
				coll = getColl(db, recipient.getCorpId(), pos.getLastColl());
			}
		}

		coll.insert(Imports.addRecipient(recipient.getEmail(), recipient.getParameterList()));
		copyRecipientList.add(new Copy(recipient.getCorpId(), pos.getMaxDbId(), pos.getLastColl(), Copy.MODIFY));
	}
	
	public void delete(Recipient recipient, int maxDbId, List<Copy> copyRecipientList) {
		for (int i = 0; i < maxDbId; i++) {
			DB db = Mongos.db("recipient_" + (i + 1));
			int max = max(db, recipient.getCorpId());
			for (int j = 0; j < max; j++) {
				DBCollection coll = getColl(db, recipient.getCorpId(), (j + 1));
				WriteResult wr = coll.remove(new BasicDBObject("email", recipient.getEmail()));
                if (wr.getN() > 0) {
                    copyRecipientList.add(new Copy(recipient.getCorpId(), i + 1, j + 1, Copy.MODIFY));
                }
			}
		}
	}
	
	public Map<String, String> query(Integer corpId, BasicDBList emailList, Integer dbId) {
		DB db = Mongos.db("recipient_" + dbId);
		int max = max(db, corpId);
		
		Map<String, String> parameterMap = Maps.newLinkedHashMap();
		
		for (int i = 0; i < max; i++) {
			DBCollection coll = db.getCollection(corpId + "_" + (i + 1));
			BasicDBObject query = new BasicDBObject();
			query.put("email", new BasicDBObject("$in", emailList));
			
			DBCursor cursor = coll.find(query);
			cursor.batchSize(500);
			while (cursor.hasNext()) {
				DBObject next = cursor.next();
				String email = next.get("email").toString();
				String parameter = Values.get(next.get("parameter_list").toString());
				String parameters = Values.get(parameterMap.get(email));
				parameters = parameters + parameter;
				parameterMap.put(email, parameters);
			}
		}

		return parameterMap;
	}
	
	public String getParameters(Integer corpId, String email, List<Recipient> dbIds) {
		String parameters = Value.S;
		Map<String, String> queryMap = null;
		
		BasicDBList emailList = new BasicDBList();
		emailList.add(email);
		
		for (Recipient dbId : dbIds) {
			queryMap = query(corpId, emailList, dbId.getDbId());
			if (!Asserts.empty(queryMap) && queryMap.containsKey(email)) {
				String parameter = Values.get(queryMap.get(email));
				parameters = parameters + parameter;
				queryMap.clear();
			}
		}
		
		return parameters;
	}
	
	public void filter(String opt, BufferedWriter writer, Integer corpId,
			List<Recipient> recipientDbIds, Map<String, Prop> propMap, Map<String, String> filterMap, 
			BasicDBList emailList, Map<String, Recipient> recipientMap, Map<String, String> parameterMap, Map<String, String> queryMap)
			throws IOException {
		boolean input = StringUtils.equals(opt, Options.INPUT)
				&& !(Asserts.empty(filterMap) || (filterMap.size() == 1 && filterMap.containsKey(PropMap.EMAIL.getId())));
		boolean output = StringUtils.equals(opt, Options.OUTPUT);

		if (input || output) {
			for (Recipient dbId : recipientDbIds) {
				queryMap = query(corpId, emailList, dbId.getDbId());
				boolean empty = Asserts.empty(queryMap);
				
				for (Object line : emailList) {
					String email = line.toString();
					Recipient recipient = recipientMap.get(email);
					if (recipient == null) {
						recipient = new Recipient();
						recipient.setEmail(email);
					}
					
					String parameters = Value.S;
					if (!empty) {
						parameters = Values.get(queryMap.get(line));
					}
					
					String parameter = Values.get(recipient.getParameterList());
					parameters = parameter + parameters;

					recipient.setParameterList(parameters);
					recipientMap.put(email, recipient);
				}
				
				if (!empty) {
					queryMap.clear();
				}
			}
			
			Exports.prop(opt, writer, emailList, recipientMap, parameterMap, filterMap, propMap);
		} else {
			for (Object email : emailList) {
				writer.write(email.toString() + "\r\n");
			}
		}
	}
	
	private DB getDb(Integer corpId, int maxDbId, int maxColls, List<Integer> dbIds, Pos pos) {
		DB db = Mongos.db("recipient_" + maxDbId);

		int lastColl = max(db, corpId);
		if (lastColl == 0) {
			if (db.getCollectionNames().size() > maxColls) {
				maxDbId++;
				dbIds.add(maxDbId);
				db = Mongos.db("recipient_" + maxDbId);
			}

			lastColl = 1;
		}

		pos.setLastColl(lastColl);
		pos.setMaxDbId(maxDbId);

		return db;
	}
	
	private DBCollection getColl(DB db, Integer corpId, int lastColl) {
		DBCollection coll = db.getCollection(corpId + "_" + lastColl);
		coll.ensureIndex(new BasicDBObject("email",1), "idx");
		return coll;
	}
}
