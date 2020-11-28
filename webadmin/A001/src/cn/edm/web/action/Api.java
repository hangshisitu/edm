package cn.edm.web.action;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.app.jms.NotifyMessageProducer;
import cn.edm.consts.Config;
import cn.edm.consts.Device;
import cn.edm.consts.HttpCode;
import cn.edm.consts.Id;
import cn.edm.consts.Mail;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.RoleMap;
import cn.edm.consts.mapper.TagMap;
import cn.edm.model.Adapter;
import cn.edm.model.Campaign;
import cn.edm.model.Cnd;
import cn.edm.model.Corp;
import cn.edm.model.Prop;
import cn.edm.model.Recipient;
import cn.edm.model.Resource;
import cn.edm.model.Sender;
import cn.edm.model.Setting;
import cn.edm.model.Tag;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.model.Touch;
import cn.edm.model.User;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.encoder.Md5;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Randoms;
import cn.edm.repository.Ips;
import cn.edm.service.AdapterService;
import cn.edm.service.CampaignService;
import cn.edm.service.CorpService;
import cn.edm.service.EmailService;
import cn.edm.service.FormService;
import cn.edm.service.PropService;
import cn.edm.service.ResourceService;
import cn.edm.service.RobotService;
import cn.edm.service.SenderService;
import cn.edm.service.SettingService;
import cn.edm.service.TagService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.service.TouchService;
import cn.edm.service.UserService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Downloads;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.helper.Forwards;
import cn.edm.utils.helper.JoinApi;
import cn.edm.utils.helper.Modulars;
import cn.edm.utils.helper.Onlines;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Cookies;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validates;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.api.Processings;
import cn.edm.web.facade.Audits;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Recipients;
import cn.edm.web.facade.Tasks;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Maps;

/**
 * API对接类.
 *  
 * @author yjli
 */
@Controller
public class Api {

	private static final Logger logger = LoggerFactory.getLogger(Api.class);
	
	private static final String REGEX_USER_ID = "regex:^[a-zA-Z][a-zA-Z0-9_]+$:只能由字母、数字或下划线组成";
	private static final String REGEX_SID = "regex:^(\\d{1,20}).(\\d{1,20}).(\\d{1,20})$:只能由数字或点组成";
	
	@Autowired
	private AdapterService adapterService;
	@Autowired
	private CorpService corpService;
    @Autowired
    private Ehcache ehcache;
	@Autowired
	private EmailService emailService;
	@Autowired
	private FormService formService;
	@Autowired
	private Ips ips;
	@Autowired
	private PropService propService;
	@Autowired
	private ResourceService resourceService;
	@Autowired
	private RobotService robotService;
	@Autowired
	private SenderService senderService;
	@Autowired
	private SettingService settingService;
	@Autowired
	private TagService tagService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private TemplateService templateService;
	@Autowired
	private TouchService touchService;
	@Autowired
	private UserService userService;
	@Autowired
	private CampaignService campaignService;
	
	private NotifyMessageProducer producer;
	
	@Autowired(required = false)
	public void setProducer(NotifyMessageProducer producer) {
		this.producer = producer;
	}

	@RequestMapping(value = "/task_callback", method = RequestMethod.GET)
	public void taskCallback(HttpServletRequest request, HttpServletResponse response) {
		String ip = request.getRemoteAddr();
		ip = (ip == null ? Value.S : ip);

		if (!ip.equals("127.0.0.1")) {
			logger.info("(Api:Callback) ip: " + ip + ", error: forbid");
			ObjectMappers.renderText(response, HttpCode._500);
			return;
		}

		String SID = null;
		String action = null;
		String info = null;
		
		try {
			SID = Https.getStr(request, "taskId", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", REGEX_SID, "SID");
			action = Https.getStr(request, "action", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^process|pause|restart|stop$", "action");
            info = Https.getStr(request, "info", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{1,60}", REGEX_SID, "info");
			
			Integer[] ids = Tasks.id(SID);
			int corpId = ids[0];
			int taskId = ids[1];
			int templateId = ids[2];

			Task task = taskService.get(taskId);
			Asserts.isNull(task, "任务");

			if (!task.getCorpId().equals(corpId) || !task.getTemplateId().equals(templateId)) {
				throw new Errors("SID不是合法值");
			}

			boolean change = false;
			Integer status = task.getStatus();
			
			if(info.equals(0)){ //对应的任务文件存在且操作成功
			    if (action.equals("process")) {
				  if (status.equals(Status.PROCESSING)) {
					change = true;
				  }
			    } else if (action.equals("pause")) {
				  if (status.equals(Status.PROCESSING)) {
					task.setStatus(Status.PAUSED);
					taskService.save(task);
					change = true;
				  }
			    } else if (action.equals("restart")) {
				  if (status.equals(Status.PAUSED)) {
					task.setStatus(Status.PROCESSING);
					taskService.save(task);
					change = true;
				  }
			    } else if (action.equals("stop")) {
				  if (Asserts.hasAny(status, new Integer[] { Status.PROCESSING, Status.PAUSED })) {
					task.setStatus(Status.CANCELLED);
					taskService.save(task);
					change = true;
				  }
			    }
			
			    if (change) {
				  logger.info("(Api:Callback) action: " + action + ", SID: " + SID + ", status before: " + status + ", status after: " + task.getStatus());
				  ObjectMappers.renderText(response, HttpCode._200);
				  return;
			    } else {
				  logger.info("(Api:Callback) action: " + action + ", SID: " + SID + ", error: status is not changed");
				  ObjectMappers.renderText(response, HttpCode._500);
				  return;
			    }
		    }else if(info.equals(1)){ //对应的任务文件不存在
		    	task.setStatus(Status.NOTFIND);
				taskService.save(task);
				logger.info("(Api:Callback) action: " + action + ", SID: " + SID +", info: " + info+ ", error: file is not find");
				ObjectMappers.renderText(response, HttpCode._500);
				return;		    	
		    }
		} catch (Errors e) {
            logger.warn("(Api:Callback) action: " + action + ", SID: " + SID + ", warn: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
            return;
        } catch (Exception e) {
            logger.error("(Api:Callback) action: " + action + ", SID: " + SID + ", error: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
            return;
        }
	}

    @RequestMapping(value = "/join_api", method = RequestMethod.GET)
    public void join(HttpServletRequest request, HttpServletResponse response) {
        String filePath = null;
        try {
        	filePath = Https.getStr(request, "filePath", R.REQUIRED, "API封装文件");
        	String audit = Https.getStr(request, "audit", R.REQUIRED, R.REGEX, "regex:^true|false$", "是否需要审核");
        	String fileId = null;
        	String deliveryIP = Property.getStr(Config.DELIVERY_API);
        	// 判断deliveryIP是否是IP
        	// 是则调用对应的接口查找文件
        	// 否则按路径查找文件
        	if(Validates.matches(deliveryIP, Validates.REGEX_IP)){
        		String ip = request.getRemoteAddr();
        		ip = (ip == null ? Value.S : ip);
        		
        		if (!ip.equals(deliveryIP)) {
        			logger.info("(Api:join) ip: " + ip + ", error: forbid");
        			ObjectMappers.renderText(response, HttpCode._500);
        			return;
        		}
        		
        		fileId = JoinApi.snatch(filePath);
        	}else{
        		File f = new File(deliveryIP);
        		if(f.isDirectory() && !f.canRead() && !f.canWrite() ){
        			logger.info("(Api:join) 路径: " + deliveryIP + ", error: forbid");
        			ObjectMappers.renderText(response, HttpCode._500);
        			return;
        		}
        		
        		fileId = JoinApi.read(filePath);
        	}
            String path = Webs.rootPath() + PathMap.JOIN_API.getPath() + fileId;
            
            Integer[] arr = Tasks.id(Delivery.node(path, "ID"));

            int corpId = arr[0];
            int taskId = arr[1];
            int templateId = arr[2];
            
            String ad = Delivery.node(path, "AD");
            String replier = Delivery.node(path, "REPLIER");
            String rand = Delivery.node(path, "RAND");
            String robot = Delivery.node(path, "ROBOT");
            
            String content = Delivery.node(path, "ORIGINAL");
            String phoneContent = Delivery.node(path, "ORIGINAL_QVGA");
            String smsContent = Delivery.node(path, "ORIGINAL_TEXT");
            
            Corp corp = corpService.get(corpId);
            Asserts.isNull(corp, "企业");
            Task task = taskService.get(taskId);
            Asserts.isNull(task, "任务");
            Template template = templateService.get(templateId);
            Asserts.isNull(template, "模板");
            User user = userService.get(task.getUserId());
            Asserts.isNull(user, "用户");
            
            String templatePath = template.getFilePath();
            if (task.getType().equals(Type.JOIN)) {
                template.setUserId(task.getUserId());
                template.setFilePath(Uploads.upload(corpId, content, Webs.rootPath(), PathMap.TEMPLATE.getPath()));
                templateService.save(template);
                Files.delete(Webs.rootPath(), templatePath, PathMap.TEMPLATE.getPath());
            
                Adapter adapter = adapterService.get(templateId);
                if (adapter == null) {
                    adapter = new Adapter();
                }
                
                adapter.setTemplateId(templateId);
                adapter.setJoinPhone(phoneContent != null ? Value.T : Value.F);
                adapter.setJoinSms(smsContent != null ? Value.T : Value.F);
                if (adapter.getJoinPhone().equals(Value.T)) {
                    adapter.setPhonePath(Uploads.upload(corpId, phoneContent, Webs.rootPath(), PathMap.TEMPLATE.getPath()));
                }
                if (adapter.getJoinSms().equals(Value.T)) {
                    adapter.setSmsPath(Uploads.upload(corpId, smsContent, Webs.rootPath(), PathMap.TEMPLATE.getPath()));
                }
                adapterService.save(adapter);
            }
            
            if (audit.equals("false")) {
                logger.info("(Api:join) filePath: " + filePath + ", audit: false");
                ObjectMappers.renderText(response, HttpCode._200);
                return;
            }
            
            Cnd cnd = new Cnd();
            Setting setting = new Setting();
            
            cnd.setFileId(fileId);
            cnd.setIncludeIds(null);
            cnd.setExcludeIds(null);
            cnd.setIncludes(null);
            cnd.setExcludes(null);
            
            if(task.getCampaignId() != null){
            	Campaign campaign = campaignService.get(corpId, null, task.getCampaignId());
            	setting.setCampaign(campaign.getCampaignName());
            }else{
            	setting.setCampaign(null);
            }
            
            setting.setTemplate(template.getTemplateName());
            setting.setAd(ad);
            setting.setSendCode("current");
            setting.setSms("0");
            setting.setReplier(replier);
            setting.setRand(rand);
            setting.setRobot(StringUtils.replace(robot, ";", ","));
            setting.setTagIds(null);
            setting.setUnsubscribeId(null);
            setting.setUnsubscribe(null);
            setting.setLabelIds(null);
            
            String auditPath = null;
//            if (Asserts.hasAny(user.getRoleId(), new Integer[] { RoleMap.MGR.getId(), RoleMap.ALL.getId() })) {
            if(user.getRoleType().equals(1)){
                auditPath = corp.getAuditPath();
            }
            auditPath = Audits.remove(auditPath, corpId);
//            if (Asserts.hasAny(user.getRoleId(), new Integer[] { RoleMap.BUK.getId(), RoleMap.BSN.getId(), RoleMap.ALL.getId() })
//                    && Values.get(user.getModerate()).equals(Value.T)) {
            if(user.getRoleType().equals(0) && Values.get(user.getModerate()).equals(Value.T)) {
                auditPath = Audits.add(auditPath, corpId);
            }
            task.setResourceId("api");
            task.setAuditPath(auditPath);
            task.setModeratePath(null);
            task.setType(Type.JOIN);
            task.setStatus(Audits.moderate(setting.getSendCode(), auditPath));
            taskService.save(task, cnd, setting);
            
            logger.info("(Api:join) filePath: " + filePath + ", audit: true");
            ObjectMappers.renderText(response, HttpCode._200);
        } catch (Errors e) {
            logger.warn("(Api:join) filePath: " + filePath + ", warn: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
            return;
        } catch (Exception e) {
            logger.warn("(Api:join) filePath: " + filePath + ", warn: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
            return;
        }
    }
	
    @RequestMapping(value = "/template", method = RequestMethod.GET)
    public void template(HttpServletRequest request, HttpServletResponse response) {
        try {
        	String deliveryIP = Property.getStr(Config.DELIVERY_API);
	      	// 判断deliveryIP是否是IP
	      	// 是IP，才对源进行验证
	      	if(Validates.matches(deliveryIP, Validates.REGEX_IP)){
	      		String ip = request.getRemoteAddr();
	      		ip = (ip == null ? Value.S : ip);
	      		
	      		if (!ip.equals(deliveryIP)) {
	      			logger.info("(Api:template) ip: " + ip + ", error: forbid");
	      			ObjectMappers.renderText(response, HttpCode._500);
	      			return;
	      		}
	      	}
            
            String templateId = Https.getStr(request, "templateId", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,32}", "模板ID");
            Asserts.isNull(templateId, "模板");
            templateId = templateId.toLowerCase();
            Template template = templateService.md5(templateId);
            Asserts.isNull(template, "模板");
            Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
            
            String content = "<$ORIGINAL>\n" + Files.get(Webs.rootPath(), template.getFilePath()) + "\n</$ORIGINAL>\n";
            
            Adapter adapter = adapterService.get(template.getTemplateId());
            if (adapter != null) {
                if (adapter.getJoinPhone().equals(Value.T)) {
                    Asserts.notExists(Webs.rootPath(), adapter.getPhonePath(), "PHONE模板");
                    content += "<$ORIGINAL_QVGA>\n" + Files.get(Webs.rootPath(), adapter.getPhonePath()) + "\n</$ORIGINAL_QVGA>\n";
                }
                if (adapter.getJoinSms().equals(Value.T)) {
                    Asserts.notExists(Webs.rootPath(), adapter.getSmsPath(), "短信模板");
                    content += "<$ORIGINAL_TEXT>\n" + Files.get(Webs.rootPath(), adapter.getSmsPath()) + "\n</$ORIGINAL_TEXT>\n";
                }
            }
            
            ObjectMappers.renderText(response, content);
        } catch (Exception e) {
            logger.error("(Api:template) error: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
        }
    }
    
    @RequestMapping(value = "/touch", method = RequestMethod.GET)
    public void touch(HttpServletRequest request, HttpServletResponse response) {
        String ids = null;
        String email = null;
        
        PrintWriter writer = null;
        
        try {
            String ip = request.getRemoteAddr();
            ip = (ip == null ? Value.S : ip);
            String touchIpStr = Property.getStr(Config.DELIVERY_TOUCH);
            String[] touchIps = Converts._toStrings(touchIpStr);
            boolean isPass = false;
            for(String touchIp : touchIps) {
            	if(ip.equals(touchIp)){
            		isPass = true;
            		break;
            	}
            }
            if (!isPass) {
                logger.info("(Api:touch) ip: " + ip + ", error: forbid");
                ObjectMappers.renderText(response, HttpCode._500);
                return;
            }
            
            ids = Https.getStr(request, "ids", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", REGEX_SID, "ids");
            String email64 = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, "{1,200}", "邮箱");
            email = Validator.getStr(new String(Encodes.decodeBase64(email64)), R.CLEAN, R.REQUIRED, R.MAIL, "{1,64}", "邮箱");
            
            Integer[] arr = Tasks.id(ids);
            int corpId = arr[0];
            int taskId = arr[1];
            int templateId = arr[2];

            DateTime now = new DateTime();
            
            if (isRepeat(ids, email64)) {
                logger.info("(Api:touch) email: " + email + ", ids: " + ids + ", is repeat");
                ObjectMappers.renderText(response, HttpCode._200);
                return;
            }
            
            Touch touch = touchService.get(corpId, taskId, templateId);
            Asserts.isNull(touch, "触发计划");
            Task task = taskService.get(touch.getParentId());
            Asserts.isNull(task, "任务");
            Setting touchSet = settingService.touchGet(taskId, templateId, Device.PC);
            Asserts.isNull(touchSet, "触发计划模板");
            
            String fileName = "~" + now.plusMinutes(touchSet.getAfterMinutes()).toString("yyyyMMddHHmm") + "_" + email64 + ".txt";
            
            String path = PathMap.TOUCH.getPath() + ids + "/";
            Files.make(Webs.webRoot() + path);

            writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.webRoot() + path + fileName), "UTF-8")));
            writer.flush();
            
            logger.info("(Api:touch) email: " + email + ", ids: " + ids);
            ObjectMappers.renderText(response, HttpCode._200);
        } catch (Errors e) {
            logger.warn("(Api:touch) EMAIL: " + email + ", ids: " + ids + ", warn: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
        } catch (Exception e) {
            logger.error("(Api:touch) email: " + email + ", ids: " + ids + ", error: ", e);
            ObjectMappers.renderText(response, HttpCode._500);
        } finally {
            if (writer != null) {
                writer.close();
            }
        }
    }
    
    private boolean isRepeat(String ids, String email64) {
        String path = Webs.webRoot() + PathMap.TOUCH.getPath() + ids;
        if (!Files.exists(path)) {
            return false;
        }
        
        File current = new File(path);
        if (current.listFiles() != null) {
            for (File file : current.listFiles()) {
                String fileName = file.getName();
                if (fileName.equals("template.txt")) continue;
                if (StringUtils.startsWith(fileName, "~")) fileName = StringUtils.removeStart(fileName, "~");
                if (StringUtils.endsWith(fileName, ".txt")) fileName = StringUtils.removeEnd(fileName, ".txt");
                
                String pref = StringUtils.substringBefore(fileName, "_");
                String suff = StringUtils.substringBefore(fileName.substring(pref.length() + 1), "_");

                if (suff.equals(email64)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
	@RequestMapping(value = "/forget", method = RequestMethod.POST)
	public void forget(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			String userId = Https.getStr(request, "userId", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}", "用户名");
			User user = userService.get(userId);
			Asserts.isNull(user, "此用户");
			
			if (!user.getStatus().equals(Status.ENABLED)) {
				String error = null;
				if (user.getStatus().equals(Status.FREEZE)) {
					error = "冻结";
				} else {
					error = "失效";
				}
				throw new Errors("此用户已" + error);
			}
			Validator.validate(user.getEmail(), R.CLEAN, R.REQUIRED, R.MAIL, "注册邮箱");

			user.setRandCode(Randoms.getRandomString(32));
			userService.save(user, userId);

			forgetNotify(user);
			Views.ok(mb, "密码重置邮件已发送，请注意查收。");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
			logger.warn("(Api:forget) warn: ", e);
		} catch (Exception e) {
			Views.error(mb, "邮件发送出现异常");
			logger.error("(Api:forget) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(value = "/settings", method = RequestMethod.POST)
	public void settings(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String sid = null;
		String r = null;
		try {
			CSRF.validate(request);
			
			sid = Https.getStr(request, "SID", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}", "SID");
			r = Https.getStr(request, "R", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,32}", "R");

			String email = new String(Encodes.decodeBase64(sid));
			Validator.validate(email, R.CLEAN, R.REQUIRED, R.MAIL, "注册邮箱");
			String password = Https.getStr(request, "password", R.CLEAN, R.REQUIRED, R.LENGTH, "{6,18}", "密码");
			Https.getStr(request, "passwordConfirm", R.CLEAN, R.REQUIRED, R.LENGTH, R.EQ, "{6,18}", "eq:" + password, "重复密码");

			User user = userService.getByEmail(email);
			Asserts.isNull(user, "此用户");

			if (!user.getStatus().equals(Status.ENABLED)) {
				throw new Errors("此用户已失效");
			}
			if (StringUtils.isBlank(user.getRandCode()) || !user.getRandCode().equals(r)) {
				throw new Errors("校验码已失效");
			}

			Md5 md5 = new Md5();
			user.setPassword(md5.encode(password));
			user.setRandCode(null);
			userService.save(user, user.getUserId());

			Views.ok(mb, "您的密码已经重置成功。请重新<a href='" + Property.getStr(Config.APP_URL) + "/login'><b>登录</b></a>");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
			logger.warn("(Api:settings) SID: " + sid + ", warn: ", e);
		} catch (Exception e) {
			Views.error(mb, "重置密码失败");
			logger.error("(Api:settings) SID: " + sid + ", error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(value = "/regist", method = RequestMethod.POST)
	public void regist(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String userId = null;
		String email = null;
		try {
			userId = Https.getStr(request, "User", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
			email = Https.getStr(request, "Email", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,60}", "邮箱");
			
			if (Asserts.containsAny(userId, new String[] { "mtarget", "admin" })) {
				throw new Errors("注册账号不能含有mtarget或admin关键字");
			}

			String password = Https.getStr(request, "password", R.CLEAN, R.REQUIRED, R.LENGTH, "{6,18}", "密码");
			String job = Https.getStr(request, "Position", R.CLEAN, R.LENGTH, "{1,20}", "职位");
			String company = Https.getStr(request, "Company", R.CLEAN, R.LENGTH, "{1,100}", "公司");
			String website = Https.getStr(request, "WebSite", R.CLEAN, R.LENGTH, "{1,100}", "网址");
			String address = Https.getStr(request, "Address", R.CLEAN, R.LENGTH, "{1,100}", "联系地址");
			String contact = Https.getStr(request, "ContactName", R.CLEAN, R.LENGTH, "{1,20}", "联系人");
			String telephone = Https.getStr(request, "Telephone", R.CLEAN, R.LENGTH, "{1,20}", "电话");
			String industry = Https.getStr(request, "Industry", R.CLEAN, R.LENGTH, "{1,20}", "行业");
			String emailQuantity = Https.getStr(request, "Quantity", R.CLEAN, R.LENGTH, "{1,20}", "邮件地址量");
			String sendQuantity = Https.getStr(request, "SendNum", R.CLEAN, R.LENGTH, "{1,20}", "每月发送量");
			String understand = Https.getStr(request, "Understand", R.CLEAN, R.LENGTH, "{1,20}", "了解途径");
			String promise = Https.getStr(request, "Promise", R.CLEAN, R.LENGTH, "{1,20}", "承诺");
			String agreement = Https.getStr(request, "Agreement", R.CLEAN, R.LENGTH, "{1,20}", "协议");
			String way = Https.getStr(request, "Way", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "了解途径");

			Md5 md5 = new Md5();

			Corp corp = new Corp();
			User user = new User();

			corp.setCompany(company);
			corp.setWebsite(website);
			corp.setAddress(address);
			corp.setContact(contact);
			corp.setTelephone(telephone);
			corp.setIndustry(industry);
			corp.setEmailQuantity(emailQuantity);
			corp.setSendQuantity(sendQuantity);
			corp.setUnderstand(understand);
			corp.setPromise(promise);
			corp.setAgreement(agreement);
			corp.setWay(way);
			corp.setStatus(Status.FREEZE);
			corp.setCosId(Id.COS);
			corp.setFormalId("normal");
			corp.setTestedId("test");
			corp.setAuditPath("/" + Id.CORP + "/");
			corp.setJoinApi(Value.F);

			user.setUserId(userId);
			user.setRoleId(RoleMap.MGR.getId());
			user.setPassword(md5.encode(password));
			user.setEmail(email);
			user.setJob(job);
			user.setStatus(Status.FREEZE);
			user.setRandCode(Randoms.getRandomString(32));

			if (!userService.unique("email", email, null) || !userService.unique("userId", user.getUserId(), null)) {
				throw new Errors("账号或邮箱已存在");
			}

			userService.save(corp, user, null);

			registNotify(user);

			Views.ok(mb, "注册新用户成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
			logger.warn("(Api:regist) user_id: " + userId + ", email: " + email + ", warn: ", e);
		} catch (Exception e) {
			Views.error(mb, "注册新用户失败");
			logger.error("(Api:regist) user_id: " + userId + ", email: " + email + ", error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(value = "/ajax_call", method = RequestMethod.POST)
	public void ajaxCall(HttpServletRequest request, HttpServletResponse response) {
		String userId = null;
		String email = null;
		try {
			userId = Https.getStr(request, "User", R.CLEAN, R.LENGTH, R.REGEX, "{4,60}", "用户名", REGEX_USER_ID);
			email = Https.getStr(request, "Email", R.CLEAN, R.LENGTH, R.MAIL, "{1,64}", "邮箱");

			if (StringUtils.isNotBlank(userId) && userService.unique("userId", userId, null)) {
				ObjectMappers.renderText(response, "true");
				logger.info("(Api:ajaxCall) user_id: " + userId + ", email: " + email);
				return;
			}

			if (StringUtils.isNotBlank(email) && userService.unique("email", email, null)) {
				ObjectMappers.renderText(response, "true");
				logger.info("(Api:ajaxCall) user_id: " + userId + ", email: " + email);
				return;
			}
		} catch (Errors e) {
            logger.warn("(Api:ajaxCall) user_id: " + userId + ", email: " + email + ", warn: ", e);
        } catch (Exception e) {
            logger.error("(Api:ajaxCall) user_id: " + userId + ", email: " + email + ", error: ", e);
        }

		ObjectMappers.renderText(response, "false");
	}

	@RequestMapping(value = "/forward", method = RequestMethod.GET)
	public String forward(HttpServletRequest request, ModelMap map) {
		try {
			/*CSRF.generate(request);*/ //网页登录各邮件服务商查收邮件时候打开的refer不是app.url(类似foxmail则不会出现这种情况)
			map.put("SID", Https.getStr(request, "SID", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", "regex:^(\\d{1,20}).(\\d{1,20}).(\\d{1,20})$"));
			return "forward";
		} catch (Exception e) {
			logger.error("(Api:forward) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "/forward", method = RequestMethod.POST)
	public void forward(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String SID = null;
		String EMAIL = null;
		try {
			/*CSRF.validate(request);*/
			
			SID = Https.getStr(request, "SID", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", REGEX_SID, "SID");
			EMAIL = Https.getStr(request, "EMAIL", R.CLEAN, R.REQUIRED, "好友邮箱");
			
			String[] emailList = Recipients.validate(EMAIL);

			Integer[] ids = Tasks.id(SID);
			int corpId = ids[0];
			int taskId = ids[1];
			int templateId = ids[2];

			Task task = taskService.get(taskId);
			Asserts.isNull(task, "任务");
			Setting setting = settingService.get(taskId);
			Asserts.isNull(setting, "发送设置");
			
			if (!task.getCorpId().equals(corpId) || !task.getTemplateId().equals(templateId)) {
				throw new Errors("SID不是合法值");
			}

			Corp corp = corpService.get(corpId);
			Asserts.isNull(corp, "企业");
			Template template = templateService.get(templateId);
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
			
            Resource resource = null;
            if (task.getType().equals(Type.JOIN)) {
                String path = Webs.rootPath() + PathMap.JOIN_API.getPath() + Tasks.id(task.getCorpId(), task.getTaskId(), task.getTemplateId()) + ".txt";
                resource = new Resource();
                resource.setSenderList(Delivery.node(path, "SENDER"));
                resource.setHeloList(Delivery.node(path, "HELO"));
                resource.setSkipList(Property.getStr(Config.DEFAULT_SKIP));
            } else {
                resource = resourceService.get(task.getResourceId());
            }
			
			Asserts.isNull(resource, "通道");
			resource.setResourceId(corp.getTestedId());
			
			List<Prop> propList = propService.find(task.getCorpId());
			Asserts.isEmpty(propList, "属性");
			
			String subject = Values.get(task.getSubject());
			String content = Values.get(Files.get(Webs.rootPath(), template.getFilePath()));
			Map<Integer, String> propMap = Props.parameters(subject, content, propList);
			
			MapBean attrs = new MapBean();
	        Templates.attrs(template, attrs);
	        Map<String, Integer> touchMap = Maps.newHashMap();
	        if (attrs.get("touch").equals("true")) {
	            List<Setting> settingList = settingService.touchFind(task.getCorpId(), task.getUserId(), task.getTemplateId(), Device.PC);
	            if (!Asserts.empty(settingList)) {
	                for (Setting e : settingList) {
	                    for (String touchUrl : Converts._toStrings(e.getTouchUrls())) {
	                        touchMap.put(touchUrl, e.getTemplateId());
	                    }
	                }
	            }
	        }
            
			String path = Forwards.write(task, setting, template, touchMap, robotService.find(corpId), resource, propMap, emailList);
			String code = Processings.processing(path);
			
			logger.info("(Api:forward) EMAIL: " + Converts.toString(emailList) + ", SID: " + SID + ", code: " + code);
		} catch (Errors e) {
            Views.error(mb, "转发邮件失败");
            logger.warn("(Api:forward) EMAIL: " + EMAIL + ", SID: " + SID + ", warn: ", e);
        } catch (Exception e) {
            Views.error(mb, "转发邮件失败");
            logger.error("(Api:forward) EMAIL: " + EMAIL + ", SID: " + SID + ", error: ", e);
        }

		Views.ok(mb, "转发邮件成功");
		
		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(value = "/online", method = RequestMethod.GET)
	public void online(HttpServletRequest request, HttpServletResponse response) {
		String SID = null;
		String EMAIL = null;
		try {
			SID = Https.getStr(request, "SID", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", REGEX_SID, "SID");
			EMAIL = Https.getStr(request, "EMAIL", R.CLEAN, R.REQUIRED, R.LENGTH, R.MAIL, "{1,60}", "EMAIL");

			Integer[] ids = Tasks.id(SID);
			int corpId = ids[0];
			int taskId = ids[1];
			int templateId = ids[2];

			Task task = taskService.get(taskId);
			Asserts.isNull(task, "任务");

			if (!task.getCorpId().equals(corpId) || !task.getTemplateId().equals(templateId)) {
				throw new Errors("SID不是合法值");
			}

			Template template = templateService.get(templateId);
			Asserts.isNull(template, "模板");
			Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");

			Document doc = Jsoup.parse(Files.get(Webs.rootPath(), template.getFilePath()));
			
            doc.select("#page_header").remove();
            doc.select("#page_footer").remove();
			
			Modulars.filter(doc);
			
			ObjectMappers.renderHtml(response, Onlines.filter(XSS.safety(doc).toString(), EMAIL));
		} catch (Errors e) {
			ObjectMappers.renderText(response, e.getMessage());
            logger.warn("(Api:online) EMAIL: " + EMAIL + ", SID: " + SID + ", warn: ", e);
		} catch (Exception e) {
			ObjectMappers.renderText(response, "您访问的网页版本已失效");
			logger.error("(Api:online) EMAIL: " + EMAIL + ", SID: " + SID + ", error: ", e);
		}
	}

	@RequestMapping(value = "/unsubscribe", method = RequestMethod.GET)
	public String unsubscribe(HttpServletRequest request) {
		action(request, "unsubscribe");
		return "unsubscribe";
	}
	
	@RequestMapping(value= "/complain", method = RequestMethod.GET)
	public String complain(HttpServletRequest request) {
		action(request, "complain");
		return "complain";
	}

	@RequestMapping(value= "/download", method = RequestMethod.GET)
	public String download(HttpServletRequest request, HttpServletResponse response) {
		try {
			String type = Https.getStr(request, "t", R.CLEAN, R.REGEX, "regex:^txt|csv|xlsx|template.design$");
			type = Values.get(type, "txt");
			if (type.equals("template.design")) {
	            Downloads.download(response, Webs.rootPath(), "/static/res/编写邮件html规范.docx", "编写邮件html规范.docx");
			} else {
			    Downloads.download(response, Webs.rootPath(), "/static/res/模板." + type, "模板." + type);
			}
			return null;
		} catch (Exception e) {
			logger.error("(Api:download) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "/m", method = RequestMethod.GET)
    public String m(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        String t = null;
        String e = null;
        String r = null;
        
        String email = null;
        
        try {
            t = Https.getStr(request, "t", R.CLEAN, R.REQUIRED, R.REGEX, "t", "regex:^active|settings|sender$");
            t = Values.get(t, "active");
            e = Https.getStr(request, "e", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}", "e");
            r = Https.getStr(request, "R", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,32}", "R");

            email = new String(Encodes.decodeBase64(e));
            
            if (t.equals("active")) {
                User user = userService.getByEmail(email);
                Asserts.isNull(user, "此用户");
                Validator.validate(email, R.CLEAN, R.REQUIRED, R.MAIL, "注册邮箱");
                
                active(response, user, r);
                return Views.redirect("login");
            } else if (t.equals("settings")) {
                String token = Randoms.getRandomString(32);
                request.setAttribute("token", token);
                CSRF.generate(request, "/settings", token);
                
                User user = userService.getByEmail(email);
                Asserts.isNull(user, "此用户");
                Validator.validate(email, R.CLEAN, R.REQUIRED, R.MAIL, "注册邮箱");
                
                settings(user, r, map);
                return "settings";
            } else if (t.equals("sender")) {
                String userId = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}", "id");
                User user = userService.get(userId);
                Asserts.isNull(user, "此用户");
                Validator.validate(email, R.CLEAN, R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}", "发件人邮箱");
                
                Sender sender = senderService.get(user.getCorpId(), user.getUserId(), email);
                sender(response, sender, r);
                return "sender";
            } else {
                throw new Errors("t(" + t + ")不是合法值");
            }
        } catch (Errors ex) {
            logger.warn("(Api:m) t: " + t + ", email: " + email + ", warn: ", ex);
            return Views._404();
        } catch (Exception ex) {
            logger.error("(Api:m) t: " + t + ", email: " + email + ", error: ", ex);
            return Views._404();
        }
    }
	
	private void action(HttpServletRequest request, String action) {
		String SID = null;
		Integer UID = null;
		String EMAIL = null;
		
		try {
		    String ip = request.getRemoteAddr();
	        ip = (ip == null ? Value.S : ip);
	        
	        Map<String, Integer> ipMap = ips.query();
	        
	        if (ipMap.containsKey(ip)) throw new Errors("ip[" + ip + "] is forbidden");
		    
			SID = Https.getStr(request, "SID", R.CLEAN, R.REQUIRED, R.LENGTH, R.REGEX, "{5,60}", REGEX_SID, "SID");
			UID = Https.getInt(request, "UID", R.CLEAN, R.INTEGER);
			EMAIL = Https.getStr(request, "EMAIL", R.CLEAN, R.REQUIRED, "EMAIL");

			Integer[] ids = Tasks.id(SID);
			int corpId = ids[0];
			int taskId = ids[1];
			int templateId = ids[2];

			Task task = taskService.get(taskId);
			Asserts.isNull(task, "任务");

			if (!task.getCorpId().equals(corpId) || !task.getTemplateId().equals(templateId)) {
				throw new Errors("SID不是合法值");
			}

			Tag tag = null;
			
            if (UID != null)
                tag = tagService.get(task.getCorpId(), task.getUserId(), UID);
            if (tag == null)
                tag = tagService.get(task.getCorpId(), task.getUserId(), TagMap.UNSUBSCRIBE.getName());
            
            Asserts.isNull(tag, "退订标签");
            UID = tag.getTagId();
            
            Recipient recipient = new Recipient();
            recipient.setEmail(EMAIL);
			emailService.save(tag, recipient, false);
			
			logger.info("(Api:" + action + ") SID: " + SID + ", UID: " + UID + ", EMAIL: " + EMAIL);
		} catch (Errors e) {
            logger.warn("(Api:" + action + ") SID: " + SID + ", UID: " + UID + ", EMAIL: " + EMAIL  + ", warn: ", e);
        } catch (Exception e) {
            logger.error("(Api:" + action + ") SID: " + SID + ", UID: " + UID + ", EMAIL: " + EMAIL  + ", error: ", e);
        }
	}
	
	private void registNotify(User user) {
		if (producer != null) {
			try {
				Map<String, Object> map = Maps.newHashMap();
				map.put(Mail.TEMPLATE_FILE_NAME, "regist.vm");
				map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
				map.put(Mail.TO, user.getEmail());
				map.put(Mail.SUBJECT, Property.getStr(Config.REGIST));
				map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
				map.put(Mail.USER_NAME, user.getUserId());
				map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(user.getEmail().getBytes()));
				map.put(Mail.RAND_CODE, user.getRandCode());
				producer.sendTopic(map);
			} catch (Exception e) {
				throw new Errors("(Api:registNotify) error: ", e);
			}
		}
	}

	private void forgetNotify(User user) {
		if (producer != null) {
			try {
				Map<String, Object> map = Maps.newHashMap();
				map.put(Mail.TEMPLATE_FILE_NAME, "forget.vm");
				map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
				map.put(Mail.TO, user.getEmail());
				map.put(Mail.SUBJECT, Property.getStr(Config.FORGET));
				map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
				map.put(Mail.USER_NAME, user.getUserId());
				map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(user.getEmail().getBytes()));
				map.put(Mail.RAND_CODE, user.getRandCode());
				producer.sendTopic(map);
			} catch (Exception e) {
				throw new Errors("(Api:forgetNotify) error: ", e);
			}
		}
	}
	
	private void active(HttpServletResponse response, User user, String r) {
		if (user.getStatus().equals(Status.DISABLED) && StringUtils.isNotBlank(user.getRandCode()) && user.getRandCode().equals(r)) {
			Corp corp = corpService.get(user.getCorpId());
			corp.setStatus(Status.ENABLED);
			user.setStatus(Status.ENABLED);
			user.setRandCode(null);
			userService.save(corp, user, user.getUserId());
			response.addCookie(Cookies.add(Cookies.USER_ID, user.getUserId()));
		} else {
			throw new Errors("此用户已失效");
		}
	}
	
	private void sender(HttpServletResponse response, Sender sender, String r) {
		if (sender.getStatus().equals(Status.DISABLED) && StringUtils.isNotBlank(sender.getRandCode()) && sender.getRandCode().equals(r)) {
			sender.setStatus(Status.ENABLED);
			sender.setRandCode(null);
			senderService.save(sender, sender.getSenderEmail());
		} else {
			throw new Errors("此发件人邮箱已失效");
		}
	}

	private void settings(User user, String r, ModelMap map) {
		if (!user.getStatus().equals(Status.ENABLED)) {
			throw new Errors("此用户已失效");
		}
		
		map.put("userId", user.getUserId());
		map.put("email", user.getEmail());
		map.put("SID", Encodes.encodeBase64(user.getEmail().getBytes()));
		map.put("R", r);
	}
}
