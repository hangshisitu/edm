package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.constants.Cnds;
import cn.edm.constants.Options;
import cn.edm.constants.Suffix;
import cn.edm.constants.Value;
import cn.edm.constants.mapper.CategoryMap;
import cn.edm.constants.mapper.PathMap;
import cn.edm.domain.Prop;
import cn.edm.domain.Tag;
import cn.edm.exception.Errors;
import cn.edm.exception.ServiceErrors;
import cn.edm.model.Category;
import cn.edm.model.Copy;
import cn.edm.repository.execute.Excel;
import cn.edm.repository.execute.Text;
import cn.edm.repository.handler.BeforeHandler;
import cn.edm.utils.Asserts;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.execute.Hashs;
import cn.edm.utils.execute.Imports;
import cn.edm.utils.execute.R;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Tags;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;


@Component
public class ImportService {

	private static final Logger logger = LoggerFactory.getLogger(ImportService.class);

	@Autowired
	private BeforeHandler beforeHandler;
	@Autowired
    private CategoryService categoryService;
	@Autowired
	private CopyService copyService;
	@Autowired
	private PropService propService;
	@Autowired
	private RecipientService recipientService;
	@Autowired
	private TagService tagService;
	@Autowired
	private Text text;
	@Autowired
	private Excel excel;
	
	public void execute(String opt, int corpId, String userId, PathMap path, String fileId,
			MultipartFile upload, String charset, String desc, Integer[] propIds, Integer[] excludeIds, Integer tagId, Counter counter) {
		String filePath = path.getPath() + fileId;
		Files.delete(Webs.root(), filePath, path.getPath());
		
		int splitCount = 0;
		int emailCount = 0;
		
		try {
			DateTime start = new DateTime();

			Asserts.isEmpty(propIds, "属性");
			List<Prop> propList = propService.find(corpId, userId, propIds);
			Asserts.isEmpty(propList, "属性");
			
			List<Tag> tags = null;
			if (StringUtils.equals(opt, Options.INPUT)) {
				Asserts.isNull(tagId, "标签");
				Tag tag = tagService.get(corpId, userId, tagId);
				Asserts.isNull(tag, "标签");
				
				tags = Lists.newArrayList();
				tags.add(tag);
				
				emailCount = tag.getEmailCount();
			} 
			
			// 入库前邮箱地址数
			if (counter != null) {
				counter.setStart(emailCount);
			}
			
			Map<Integer, Prop> propMap = Maps.newHashMap();
			int mailPos = Imports.mailPos(propIds, propList, propMap);

			Files.make(Webs.root() + filePath);
			
			String suff = Files.suffix(upload.getOriginalFilename());
			
			int inCount = 0;
			int exCount = 0;
			
			// 1) 获取邮箱
			if (suff.equals(Suffix.TXT) || suff.equals(Suffix.CSV)) {
				inCount = text.mailPos(path, fileId, upload, charset, propIds, propMap, mailPos);
			} else if (suff.equals(Suffix.XLS) || suff.equals(Suffix.XLSX)) {
				inCount = excel.mailPos(path, fileId, upload, charset, propIds, propMap, mailPos);
			} else {
				throw new Errors("文件后缀不是合法值");
			}
			
			if (inCount < 1) {
				throw new Errors("收件人数不能少于1");
			}
			
			List<Tag> exTags = Lists.newArrayList();
			if (!Asserts.empty(excludeIds)) {
				List<Tag> excludeTags = tagService.find(corpId, userId, excludeIds);
				exCount = Tags.count(excludeTags, exTags);
				if (Asserts.hasAny(-1, excludeIds)) {
					Category category = categoryService.get(corpId, userId, CategoryMap.UNSUBSCRIBE.getName(), Category.TAG);
					if (category != null) {
						List<Tag> unsubscribeTags = tagService.find(corpId, userId, category.getCategoryId());
						exCount += Tags.count(unsubscribeTags, exTags);
					}
				}
			}
            
			// 2) 邮箱hash分组
			splitCount = Hashs.splitCount(inCount, exCount);
			beforeHandler.map(Options.IMPORT, userId, splitCount, Webs.root() + filePath, null, exTags, null);
			emailCount = beforeHandler.reduce(opt, userId, splitCount, Webs.root() + filePath, Cnds.OR, 0, null, null, null);
			
			if (!Asserts.empty(tags)) {
				int maxRecipientDbId = recipientService.maxDbId();
				int maxTagDbId = tagService.maxDbId();
				List<Integer> recipientDbIds = Lists.newArrayList();

				// 3) 标签入库
				beforeHandler.db(userId, Webs.root() + filePath, desc, emailCount, tags, maxTagDbId);
				Copy copyTag = new Copy(userId, tags.get(0).getDbId(), tags.get(0).getTagId(), Copy.MODIFY);
				
				// 4) 邮箱入库 
                List<Copy> copyRecipientList = Lists.newArrayList();
				if (suff.equals(Suffix.TXT) || suff.equals(Suffix.CSV)) {
					text.execute(userId, upload, charset, propIds, propMap, mailPos, maxRecipientDbId, recipientDbIds, copyRecipientList);
				} else if (suff.equals(Suffix.XLS) || suff.equals(Suffix.XLSX)) {
					excel.execute(userId, upload, propIds, propMap, mailPos, maxRecipientDbId, recipientDbIds, copyRecipientList);
				}
				
				// 5) 更新邮箱库关联
				recipientService.save(userId, maxRecipientDbId, recipientDbIds);

				// 6) 更新标签邮箱数
				tagService.save(tags.get(0));
				emailCount = tags.get(0).getEmailCount();
				
                // 7) CopyTrigger触发
                copyService.save(Copy.TAG, copyTag);
                copyService.batchSave(Copy.RECIPIENT, copyRecipientList);
                copyRecipientList.clear();
				
				tags.clear();
			}
			
			// 入库后邮箱地址数
			if (counter != null) {
				counter.setEnd(emailCount);
			}
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(Import:execute) suff: " + suff + ", seconds: " + seconds
					+ (counter != null ? ", start: " + counter.getStart() + ", end: " + counter.getEnd() : Value.S));
		} catch (Errors e) {
			logger.error("(Import:execute) error: ", e);
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			logger.error("(Import:execute) error: ", e);
			throw new ServiceErrors(e.getMessage(), e);
		} finally {
			Files.delete(splitCount, Webs.root(), filePath, path);
			if (path.equals(PathMap.EXECUTE) || emailCount < 1) {
				Files.delete(Webs.root(), filePath, path.getPath());
			}
			if (path.equals(PathMap.R) && opt.equals(Options.INPUT)) {
				R.delete(userId, fileId);
			}
		}
	}
	
}
