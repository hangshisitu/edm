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

import cn.edm.consts.Cnds;
import cn.edm.consts.Id;
import cn.edm.consts.Options;
import cn.edm.consts.Permissions;
import cn.edm.consts.Suffix;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Category;
import cn.edm.model.Copy;
import cn.edm.model.Prop;
import cn.edm.model.Tag;
import cn.edm.model.User;
import cn.edm.repository.execute.Excel;
import cn.edm.repository.execute.Text;
import cn.edm.repository.handler.BeforeHandler;
import cn.edm.utils.Asserts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.ServiceErrors;
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
    private UserService userService;
	@Autowired
	private Text text;
	@Autowired
	private Excel excel;
	
	public void execute(String opt, int corpId, String userId, PathMap path, String fileId,
			MultipartFile upload, String charset, Integer[] propIds, Integer[] excludeIds, Integer tagId, Counter counter) {
		String filePath = path.getPath() + fileId;
		Files.delete(Webs.rootPath(), filePath, path.getPath());
		
		int splitCount = 0;
		int emailCount = 0;
		
		try {
			DateTime start = new DateTime();

			Asserts.isEmpty(propIds, "属性");
			List<Prop> propList = propService.find(corpId, null, propIds);
			Asserts.isEmpty(propList, "属性");
			
			List<Tag> tags = null;
			if (StringUtils.equals(opt, Options.INPUT)) {
				Asserts.isNull(tagId, "标签");
				Tag tag = tagService.get(corpId, null, tagId);
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

			Files.make(Webs.rootPath() + filePath);
			
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
				List<Tag> excludeTags = tagService.find(corpId, null, excludeIds);
				exCount = Tags.count(excludeTags, exTags);
				if (Asserts.hasAny(-1, excludeIds)) {
					Category category = categoryService.get(corpId, null, CategoryMap.UNSUBSCRIBE.getName(), Category.TAG);
					if (category != null) {
						List<Tag> unsubscribeTags = tagService.find(corpId, category.getCategoryId());
						exCount += Tags.count(unsubscribeTags, exTags);
					}
				}
			}

            if (opt.equals(Options.EXPORT)) {
                // 1) 排除红名单|黑名单|举报 (运营用户)
                User user = userService.get(userId);
                List<Tag> robotTags = tagService.find(Id.ROBOT_CORP, Id.ROBOT_USER);
                String[] bsnTags = { CategoryMap.RED.getName(), CategoryMap.BLACK.getName(), CategoryMap.EXPOSE.getName() };
                String[] allTags = { CategoryMap.REFUSE.getName() };
                
                if (user != null) {
                    if (user.getPermissionId() != null && user.getPermissionId().equals(Permissions.BSN.getId())) {
                        for (Tag tag : robotTags) {
                            if (Asserts.hasAny(tag.getTagName(), bsnTags)) {
                                if (tag.getDbId() > 0) {
                                    exTags.add(tag);
                                    exCount += tag.getEmailCount();
                                }
                            }
                        }
                    }
                }
                
                // 2) 排除拒收  (所有用户)
                for (Tag tag : robotTags) {
                    if (Asserts.hasAny(tag.getTagName(), allTags)) {
                        if (tag.getDbId() > 0) {
                            exTags.add(tag);
                            exCount += tag.getEmailCount();
                        }
                    }
                }
            }
			
			// 2) 邮箱hash分组
			splitCount = Hashs.splitCount(inCount, exCount);
			beforeHandler.map(Options.IMPORT, splitCount, Webs.rootPath() + filePath, null, exTags, null);
			emailCount = beforeHandler.reduce(opt, corpId, splitCount, Webs.rootPath() + filePath, Cnds.OR, 0, null, null, null);
			
			if (!Asserts.empty(tags)) {
				int maxRecipientDbId = recipientService.maxDbId();
				int maxTagDbId = tagService.maxDbId();
				List<Integer> recipientDbIds = Lists.newArrayList();

				// 3) 标签入库
				beforeHandler.db(corpId, Webs.rootPath() + filePath, emailCount, tags, maxTagDbId);
				Copy copyTag = new Copy(corpId, tags.get(0).getDbId(), tags.get(0).getTagId(), Copy.MODIFY);
				
				// 4) 邮箱入库 
                List<Copy> copyRecipientList = Lists.newArrayList();
				if (suff.equals(Suffix.TXT) || suff.equals(Suffix.CSV)) {
					text.execute(corpId, upload, charset, propIds, propMap, mailPos, maxRecipientDbId, recipientDbIds, copyRecipientList);
				} else if (suff.equals(Suffix.XLS) || suff.equals(Suffix.XLSX)) {
					excel.execute(corpId, upload, propIds, propMap, mailPos, maxRecipientDbId, recipientDbIds, copyRecipientList);
				}
				
				// 5) 更新邮箱库关联
				recipientService.save(corpId, maxRecipientDbId, recipientDbIds);

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
			Files.delete(splitCount, Webs.rootPath(), filePath, path);
			if (path.equals(PathMap.EXECUTE) || emailCount < 1) {
				Files.delete(Webs.rootPath(), filePath, path.getPath());
			}
			if (path.equals(PathMap.R) && opt.equals(Options.INPUT)) {
				R.delete(userId, fileId);
			}
		}
	}
	
	public void executeTemp(String opt, int corpId, String userId, PathMap path, String fileId,
			MultipartFile upload, String charset, Integer[] propIds, Integer tagId, Counter counter) {
		String filePath = path.getPath() + fileId;
		
		int splitCount = 0;
		int emailCount = 0;
		
		try {
			DateTime start = new DateTime();
			
			Asserts.isEmpty(propIds, "属性");
			List<Prop> propList = propService.find(corpId, null, propIds);
			Asserts.isEmpty(propList, "属性");
			
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(corpId, null, tagId);
			Asserts.isNull(tag, "标签");
			
			// 入库前邮箱地址数
			if (counter != null) {
				counter.setStart(emailCount);
			}
			
			Map<Integer, Prop> propMap = Maps.newHashMap();
			int mailPos = Imports.mailPos(propIds, propList, propMap);
			
			Files.make(Webs.rootPath() + filePath);
			
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
			
			List<Tag> tags = Lists.newArrayList();
			tags.add(tag);
			
			// 2) 邮箱hash分组
			splitCount = Hashs.splitCount(inCount, exCount);
			beforeHandler.map(Options.IMPORT, splitCount, Webs.rootPath() + filePath, tags, null, null);
			emailCount = beforeHandler.reduce(opt, corpId, splitCount, Webs.rootPath() + filePath, Cnds.OR, 0, null, null, null);
			
			// 6) 更新标签邮箱数
			tag.setEmailCount(emailCount);
			tagService.save(tag);
			
			// 入库后邮箱地址数
			if (counter != null) {
				counter.setEnd(emailCount);
			}
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(Export:execute) suff: " + suff + ", seconds: " + seconds
					+ (counter != null ? ", start: " + counter.getStart() + ", end: " + counter.getEnd() : Value.S));
		} catch (Errors e) {
			logger.error("(Export:execute) error: ", e);
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			logger.error("(Export:execute) error: ", e);
			throw new ServiceErrors(e.getMessage(), e);
		} finally {
			Files.delete(splitCount, Webs.rootPath(), filePath, path);
		}
	}
	
}
