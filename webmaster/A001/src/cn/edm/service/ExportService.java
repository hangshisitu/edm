//package cn.edm.service;
//
//import java.util.List;
//import java.util.logging.Filter;
//
//import org.apache.commons.lang.StringUtils;
//import org.joda.time.DateTime;
//import org.joda.time.Seconds;
//import org.slf4j.Logger;
//import org.slf4j.LoggerFactory;
//import org.springframework.beans.factory.annotation.Autowired;
//import org.springframework.stereotype.Service;
//
//import cn.edm.constants.Cnds;
//import cn.edm.constants.Config;
//import cn.edm.constants.Options;
//import cn.edm.constants.Value;
//import cn.edm.constants.mapper.PathMap;
//import cn.edm.domain.Prop;
//import cn.edm.domain.Tag;
//import cn.edm.exception.Errors;
//import cn.edm.exception.ServiceErrors;
//import cn.edm.model.Recipient;
//import cn.edm.modules.utils.Props;
//import cn.edm.repository.handler.BeforeHandler;
//import cn.edm.utils.Asserts;
//import cn.edm.utils.Converts;
//import cn.edm.utils.Values;
//import cn.edm.utils.execute.Counter;
//import cn.edm.utils.execute.Exports;
//import cn.edm.utils.execute.Hashs;
//import cn.edm.utils.file.Files;
//import cn.edm.utils.web.Webs;
//
//import com.google.common.collect.Lists;
//
//@Service
//public class ExportService {
//
//	private static final Logger logger = LoggerFactory.getLogger(ExportService.class);
//	
//	@Autowired
//	private BeforeHandler beforeHandler;
//	@Autowired
//	private PropService propService;
//	@Autowired
//	private RecipientService recipientService;
//	@Autowired
//	private TagService tagService;
//
//	public void execute(String opt, int corpId, String userId, PathMap path, String fileId,
//			Integer[] includeTagIds, Integer[] excludeTagIds, String activeCnd, String inCnd, Integer filterId, Integer tagId, Counter counter) {
//		String filePath = path.getPath() + fileId;
//		Files.delete(Webs.root(), filePath, path.getPath());
//		
//		int splitCount = 0;
//		int emailCount = 0;
//		
//		try {
//			DateTime start = new DateTime();
//			
//			Asserts.isEmpty(includeTagIds, "包含标签");
//			
//			Integer[] inTagIds = Converts.unique(includeTagIds, excludeTagIds);
//			Integer[] exTagIds = Converts.unique(excludeTagIds, includeTagIds);
//
//			Asserts.isEmpty(inTagIds, "包含标签");
//			
//			List<Tag> inTags = tagService.find(corpId, userId, inTagIds);
//			List<Tag> exTags = null;
//
//			List<Prop> propList = propService.find(corpId, userId);
//			Asserts.isEmpty(propList, "属性");
//			
//			Filter filter = null;
//			if (filterId != null) {
//			}
//
//			List<Tag> tags = Lists.newArrayList();
//			if (StringUtils.equals(opt, Options.INPUT)) {
//				Asserts.isNull(tagId, "标签");
//				Tag tag = tagService.get(corpId, userId, tagId);
//				Asserts.isNull(tag, "标签");
//				tags.add(tag);
//				
//				// 入库前邮箱地址数
//				if (counter != null) {
//					counter.setStart(tag.getEmailCount());
//				}
//			} else {
//				// 导出前邮箱地址数
//				if (counter != null) {
//					counter.setStart(Value.I);
//				}
//			}
//			
//			List<Recipient> recipientDbIds = recipientService.find(userId);
//			int recipientCount = Values.get(Props.getInt(Config.RECIPIENT_COUNT), 0);
//			int inCount = Exports.recipientCount(inTags, recipientCount, "包含标签");
//			int exCount = 0;
//			if (!Asserts.empty(exTagIds)) {
//				exTags = tagService.find(corpId, userId, exTagIds);
//				exCount = Exports.recipientCount(exTags, recipientCount, "排除标签");
//			}
//
//			// 1) 邮箱hash分组
//			splitCount = Hashs.splitCount(inCount, exCount);
//			beforeHandler.map(Options.EXPORT, userId, splitCount, Webs.root() + filePath, inTags, exTags, activeCnd);
//			
//			// 2) 标签/过滤器筛选
//			int count = 0;
//			if (StringUtils.equals(inCnd, Cnds.AND)) {
//				List<Tag> tagList = tagService.find(corpId, userId, includeTagIds);
//				Asserts.isEmpty(tagList, "包含标签");
//				count = tagList.size();
//			}
//			emailCount = beforeHandler.reduce(opt, userId, splitCount, Webs.root() + filePath, inCnd, count, recipientDbIds, propList, filter);
//			
//			if (StringUtils.equals(opt, Options.INPUT)) {
//				// 3) 标签入库
//				int maxTagDbId = tagService.maxDbId();
//				beforeHandler.db(userId, Webs.root() + filePath, emailCount, tags, maxTagDbId);
//
//				Tag tag = tags.get(0);
//				tagService.save(tag);
//
//				// 入库后邮箱地址数
//				if (counter != null) {
//					counter.setEnd(tag.getEmailCount());
//				}
//			} else {
//				// 导出后邮箱地址数
//				if (counter != null) {
//					counter.setEnd(emailCount);
//				}
//			}
//
//			DateTime end = new DateTime();
//			long seconds = Seconds.secondsBetween(start, end).getSeconds();
//			logger.info("(Export:execute) seconds: " + seconds
//					+ (counter != null ? ", start: " + counter.getStart() + ", end: " + counter.getEnd() : Value.S));
//		} catch (Errors e) {
//			logger.error("(Export:execute) error: ", e);
//			throw new Errors(e.getMessage(), e);
//		}  catch (Exception e) {
//			logger.error("(Export:execute) error: ", e);
//			throw new ServiceErrors(e);
//		} finally {
//			Files.delete(splitCount, Webs.root(), filePath, path);
//		}
//	}
//}
