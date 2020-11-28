package cn.edm.service;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.consts.Config;
import cn.edm.consts.Device;
import cn.edm.consts.Id;
import cn.edm.consts.Permissions;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Adapter;
import cn.edm.model.Category;
import cn.edm.model.Cnd;
import cn.edm.model.Recipient;
import cn.edm.model.Resource;
import cn.edm.model.Setting;
import cn.edm.model.Tag;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.User;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Property;
import cn.edm.repository.handler.AfterHandler;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.Hashs;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Lines;
import cn.edm.utils.helper.Charts;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Resources;
import cn.edm.web.facade.Robots;
import cn.edm.web.facade.Tags;
import cn.edm.web.facade.Tasks;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Transactional
@Service
public class WriterService {

	private static final Logger logger = LoggerFactory.getLogger(WriterService.class);
	
	@Autowired
	private AdapterService adapterService;
	@Autowired
	private CategoryService categoryService;
	@Autowired
	private CndService cndService;
	@Autowired
	private RecipientService recipientService;
	@Autowired
	private RobotService robotService;
	@Autowired
	private SettingService settingService;
	@Autowired
	private TagService tagService;
	@Autowired
	private AfterHandler afterHandler;
	@Autowired
	private UserService userService;
	
	public String write(Task task, Template template, Resource resource, Map<Integer, String> propMap) {
		DateTime now = new DateTime();
		
		String SID = Tasks.id(task.getCorpId(), task.getTaskId(), task.getTemplateId()) + ".txt";
		String path = PathMap.TASK.getPath() + now.toString("yyyyMM") + "/";
		
		String write = PathMap.WRITE.getPath() + Tasks.id(task.getCorpId(), task.getTaskId(), task.getTemplateId());
		Files.delete(Webs.rootPath(), write, PathMap.WRITE.getPath());
		
		Cnd cnd = cndService.get(task.getTaskId());
		Asserts.isNull(cnd, "收件人设置");
		Setting setting = settingService.get(task.getTaskId());
		Asserts.isNull(setting, "发送设置");
		String[] emailList = Converts._toStrings(cnd.getEmailList());
		String fileId = cnd.getFileId();
		Integer[] includeIds = Converts._toIntegers(cnd.getIncludeIds());
		Integer[] excludeIds = Converts._toIntegers(cnd.getExcludeIds());
        
        MapBean attrs = new MapBean();
        Templates.attrs(template, attrs);
        boolean base64 = attrs.get("personal").equals("false");
        Map<String, Integer> touchMap = Maps.newHashMap();
        if (attrs.get("touch").equals("true")) {
            List<Setting> settingList = settingService.find(task.getCorpId(), task.getUserId(), task.getTemplateId(), Device.PC);
            if (!Asserts.empty(settingList)) {
                for (Setting e : settingList) {
                    for (String touchUrl : Converts._toStrings(e.getTouchUrls())) {
                        touchMap.put(touchUrl, e.getTemplateId());
                    }
                }
            }
        }
        
        // 终端适配
        Adapter adapter = adapterService.get(template.getTemplateId());
        if (adapter != null) {
            if (adapter.getJoinPhone().equals(Value.T)) {
                Asserts.notExists(Webs.rootPath(), adapter.getPhonePath(), "PHONE模板");
            }
            if (adapter.getJoinSms().equals(Value.T)) {
                Asserts.notExists(Webs.rootPath(), adapter.getSmsPath(), "短信模板");
            }
        }
        
        MapBean phoneAttrs = new MapBean();
        boolean phoneBase64 = false;
        Map<String, Integer> phoneTouchMap = Maps.newHashMap();
        if (adapter != null) {
            Templates.attrs(adapter, phoneAttrs);  
            phoneBase64 = phoneAttrs.get("personal").equals("false");
            if (phoneAttrs.get("touch").equals("true")) {
                List<Setting> settingList = settingService.find(task.getCorpId(), task.getUserId(), task.getTemplateId(), Device.PHONE);
                if (!Asserts.empty(settingList)) {
                    for (Setting e : settingList) {
                        for (String touchUrl : Converts._toStrings(e.getTouchUrls())) {
                            phoneTouchMap.put(touchUrl, e.getTemplateId());
                        }
                    }
                }
            }
        }
        
		int inCount = 0;
		int exCount = 0;
		
		// 1) 邮箱地址
		Map<String, String> emailMap = Maps.newHashMap();
		if (!Asserts.empty(emailList)) {
			for (String email : emailList) {
				emailMap.put(email, email);
			}
		}

		// 2) 收件人地址
		boolean hasFile = false;
		if (StringUtils.isNotBlank(fileId)) {
			String r = Webs.rootPath() + PathMap.R.getPath() + task.getUserId() + "/" + fileId + "/reduce.txt";
			hasFile = Files.exists(r);
			if (hasFile) inCount = Lines.count(r, "UTF-8");
		}
		
		// 3) 包含标签
		List<Tag> inTags = Lists.newArrayList();
		if (!hasFile && !Asserts.empty(includeIds)) {
			List<Tag> tagList = tagService.find(task.getCorpId(), task.getUserId(), includeIds);
			inCount = Tags.count(tagList, inTags);
		}
		
		// 4) 排除标签
		List<Tag> exTags = Lists.newArrayList();
		if (!hasFile && !Asserts.empty(excludeIds)) {
			List<Tag> excludeTagList = tagService.find(task.getCorpId(), task.getUserId(), excludeIds);
			exCount = Tags.count(excludeTagList, exTags);
			if (Asserts.hasAny(-1, excludeIds)) {
				Category category = categoryService.get(null, task.getUserId(), CategoryMap.UNSUBSCRIBE.getName(), Category.TAG);
				if (category != null) {
					List<Tag> unsubscribeTagList = tagService.find(task.getCorpId(), category.getCategoryId());
                    exCount += Tags.count(unsubscribeTagList, exTags);
                }
			}
		}
		
		// 5) 排除红名单|黑名单|举报 (运营用户)
        User user = userService.get(task.getUserId());
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
		
		// 6) 排除拒收  (所有用户)
        for (Tag tag : robotTags) {
            if (Asserts.hasAny(tag.getTagName(), allTags)) {
                if (tag.getDbId() > 0) {
                    exTags.add(tag);
                    exCount += tag.getEmailCount();
                }
            }
        }
        
		int splitCount = Hashs.splitCount(inCount, exCount);
		
		List<Recipient> recipientDbIds = recipientService.find(task.getUserId());
		
		boolean completed = false;
		PrintWriter writer = null;
		
		int emailCount = 0;
		
		try {
			Files.make(Webs.rootPath() + path);
			
			if (Files.exists(Webs.rootPath() + path + SID) && task.getDeliveryTime() != null) {
				completed = true;
				return path + SID;
			}

			DateTime start = new DateTime();
			
			String label_ids = (String) Validator.validate(setting.getLabelIds(), R.CLEAN, R.REGEX, "regex:^[0-9,]+$");

			int rnd = Resources.rnd(resource.getSenderList(), resource.getHeloList(), resource.getSkipList());
			String helo = Resources.get(resource.getHeloList(), rnd);
			String skip = Resources.get(resource.getSkipList(), rnd);
			
			String SENDER = null;
			String ROBOT = null;
			String RAND = null;
			
			String robot = StringUtils.replace(Robots.get(robotService.find(task.getCorpId())), ",", ";");
			if (StringUtils.isBlank(robot) || Asserts.hasAny(task.getStatus(), new Integer[] { Status.TEST }))
				ROBOT = StringUtils.replace(resource.getSenderList(), ",", ";");
			else {
				if (StringUtils.isNotBlank(setting.getRobot()))
					ROBOT = StringUtils.replace(setting.getRobot(), ",", ";");
				else
					ROBOT = robot;
			}
			
			if (StringUtils.isNotBlank(task.getSenderEmail()))
				SENDER = task.getSenderEmail();
			else
				SENDER = ROBOT;
			RAND = setting.getRand();
			
			writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + path + SID), "UTF-8")));
			Delivery.ID(writer, String.valueOf(task.getCorpId()), String.valueOf(task.getTaskId()), String.valueOf(task.getTemplateId()));
			Delivery.TIME(writer, task.getJobTime());
			Delivery.AD(writer, setting.getAd());
			Delivery.SMS(writer, setting.getSms());
			Delivery.SUBJECT(writer, task.getSubject());
			Delivery.SENDER(writer, SENDER);
			Delivery.NICKNAME(writer, task.getSenderName());
			Delivery.REPLIER(writer, setting.getReplier());
			Delivery.PIPE(writer, resource.getResourceId());
			Delivery.HELO(writer, helo);
			Delivery.ROBOT(writer, ROBOT);
			Delivery.RAND(writer, RAND);
			
			Delivery.MAILLIST_BEGIN(writer, propMap);
			
			afterHandler.map(task.getUserId(), SID, splitCount, Webs.rootPath() + write, emailMap, fileId, inTags, exTags);
			emailCount = afterHandler.reduce(writer, task.getCorpId(), task.getUserId(), SID, splitCount, Webs.rootPath() + write, fileId, recipientDbIds, task.getReceiver(), propMap);
			
			Delivery.MAILLIST_END(writer);
			
            String test = "1";
            if (emailCount > Property.getInt(Config.PIPE_SIZE)) {
                test = "0";
            }
			Delivery.CHART(writer, Charts.cost(Webs.rootPath(), template.getFilePath()));
			
			if (adapter != null) {
			    if (adapter.getJoinSms().equals(Value.T)) {
			        Delivery.MODULA_TEXT(writer, Files.get(Webs.rootPath(), adapter.getSmsPath()));
			    }
			    if (adapter.getJoinPhone().equals(Value.T)) {
			        Delivery.MODULA_QVGA(writer, Modulars.filterPath(task.getCorpId(), task.getTaskId(), task.getTemplateId(), 
                            skip, adapter.getPhonePath(), phoneBase64, 
                            phoneTouchMap,
                            setting.getUnsubscribeId(), label_ids, test));
			    }
			}
			Delivery.MODULA(writer, Modulars.filterPath(task.getCorpId(), task.getTaskId(), task.getTemplateId(), 
			        skip, template.getFilePath(), base64, 
			        touchMap,
			        setting.getUnsubscribeId(), label_ids, test));
            
			writer.flush();
			
			task.setEmailCount(emailCount);
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			logger.info("(Writer:write) SID: " + SID + ", seconds: " + seconds + ", counter: " + emailCount);
			
			completed = true;
			
			return path + SID;
		} catch (Exception e) {
			throw new ServiceErrors("(Writer:write) error:", e);
		} finally {
			if (writer != null) {
				writer.close();
			}
			if (!completed) {
				if (Files.exists(Webs.rootPath() + path + SID) && StringUtils.endsWith(path + SID, ".txt")) {
					Files.delete(Webs.rootPath(), path + SID, PathMap.TASK.getPath());
				}
			}
			Files.delete(Webs.rootPath(), write, PathMap.WRITE.getPath());
		}
	}
}
