package cn.edm.web.action.account;

import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.app.jms.NotifyMessageProducer;
import cn.edm.consts.Config;
import cn.edm.consts.Fqn;
import cn.edm.consts.Mail;
import cn.edm.consts.Permissions;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.model.Sender;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Randoms;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;

import com.google.common.collect.Maps;

@Controller
@RequestMapping("/account/sender/**")
public class SenderAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(SenderAction.class);
	
	@Autowired(required = false)
	public void setProducer(NotifyMessageProducer producer) {
		this.producer = producer;
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
            _throw();
			List<Sender> senderList = senderService.find(UserSession.getCorpId(), null);
			map.put("senderList", senderList);
			return "default/account/sender_page";
		} catch (Exception e) {
			logger.error("(Sender:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void array(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
		    _throw();
			CSRF.validate(request);

			List<Sender> senderList = senderService.find(UserSession.getCorpId(), Status.ENABLED);
			Asserts.isEmpty(senderList, "发件人");
			MapBean senderMaps = new MapBean();
			for (Sender sender : senderList) {
				senderMaps.put(sender.getSenderEmail(), sender.getSenderName());
			}
			mb.put("senderMaps", senderMaps);
			Views.ok(mb, "");
		} catch (Exception e) {
			logger.error("(Sender:array) error: ", e);
			Views.error(mb, "");
		}

		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
		    _throw();
			CSRF.generate(request);
			
			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^sender|task$");
			action = Values.get(action, "sender");
			
			String senderEmail = Https.getStr(request, "senderEmail", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}");
			Sender sender = null;

			if (StringUtils.isNotBlank(senderEmail)) {
				sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), senderEmail);
			}
			if (sender == null) {
				sender = new Sender();
			}

			map.put("sender", sender);
			map.put("action", action);

			return "default/account/sender_add";
		} catch (Exception e) {
			logger.error("(Sender:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String orgSenderEmail = null;
		
		try {
		    _throw();
			CSRF.validate(request);
			
			orgSenderEmail = Https.getStr(request, "orgSenderEmail", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}", "发件人邮箱");
			String senderEmail = Https.getStr(request, "senderEmail", R.CLEAN, R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}", "发件人邮箱");
			String senderName = Https.getStr(request, "senderName", R.CLEAN, R.LENGTH, "{1,20}", "发件人昵称");
			
			Asserts.notUnique(senderService.unique(UserSession.getCorpId(), senderEmail, orgSenderEmail), "发件人邮箱");

			Sender sender = null;
			if (StringUtils.isNotBlank(orgSenderEmail)) {
				sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), orgSenderEmail);
				Asserts.isNull(sender, "发件人");
				if (!orgSenderEmail.equals(senderEmail)) {
					throw new Errors("禁止修改发件人邮箱");
				}
			} else {
			    if (!UserSession.hasAnyPermission(new String[] {Permissions.BSN.getName()})) {
                    Integer senderCount = Property.getInt(Config.SENDER_COUNT);
					if (!senderService.max(UserSession.getCorpId(), UserSession.getUserId(), senderCount)) {
						throw new Errors("发件人邮箱不能多于 " + senderCount + " 个");
					}
				}
				sender = new Sender();
				sender.setCorpId(UserSession.getCorpId());
				sender.setUserId(UserSession.getUserId());
				sender.setSenderEmail(senderEmail);
				sender.setRequired(Status.DISABLED);
				sender.setRandCode(Randoms.getRandomString(32));
				sender.setStatus(Status.DISABLED);
			}

			sender.setSenderName(senderName);
			senderService.save(sender, orgSenderEmail);
			
			long sum = senderService.countResult(UserSession.getCorpId(), UserSession.getUserId(), null);
			if (StringUtils.isBlank(orgSenderEmail)) {
				validateNotify(sender);
				cache.put(Fqn.SENDER, UserSession.getUserId() + "_" + sender.getSenderEmail(), "lock");
			}
			
			mb.put("userId", UserSession.getUserId());
			mb.put("senderEmail", sender.getSenderEmail());
			mb.put("senderName", senderName);
			mb.put("required", sender.getRequired());
			mb.put("status", sender.getStatus());
			mb.put("sum", sum);
			Date now = new Date();
			mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
			mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
			Views.ok(mb, (StringUtils.isNotBlank(orgSenderEmail) ? "修改发件人成功" : "新建发件人成功（验证发件人邮件已发送，请注意查收）"));
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (StringUtils.isNotBlank(orgSenderEmail) ? "修改" : "新建") + "发件人失败");
			logger.error("(Sender:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
		    _throw();
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");
			Sender sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), checkedIds);
			Asserts.isNull(sender, "发件人");
			map.put("checkedName", sender.getSenderEmail());
			
			Views.map(map, "del", "", "删除", "发件人", "checkedIds", checkedIds);

			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Sender:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
		    _throw();
			CSRF.validate(request);
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");

            if (taskService.lockSender(UserSession.getCorpId(), UserSession.getUserId(), checkedIds)){
                throw new Errors("所选的收件人已关联投递任务，禁止删除");
			}
            
            senderService.delete(UserSession.getUserId(), checkedIds);
            
            long sum = senderService.countResult(UserSession.getCorpId(), UserSession.getUserId(), null);
			mb.put("sum", sum);
			
            Views.ok(mb, "删除发件人成功");
		} catch (Errors e) {
            Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除发件人失败");
			logger.error("(Sender:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String required(HttpServletRequest request, ModelMap map) {
		try {
		    _throw();
			CSRF.generate(request);
			
			String operate = "设为";
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");
			Sender sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), checkedIds);
			Asserts.isNull(sender, "发件人");
			if (sender.getRequired().equals(Value.T)) {
				operate = "取消";
			}
			map.put("checkedName", sender.getSenderEmail());
			
			Views.map(map, "required", "", operate, "默认", "checkedIds", checkedIds);

			return "default/account/sender_form";
		} catch (Exception e) {
			logger.error("(Sender:required) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void required(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		String operate = null;
		try {
		    _throw();
			CSRF.validate(request);
			
			operate = "设为";
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");
			Sender sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), checkedIds);
			Asserts.isNull(sender, "发件人");
			if (sender.getRequired().equals(Value.T)) {
				operate = "取消";
				sender.setRequired(Value.F);
			} else {
				senderService.batchUpdate(UserSession.getCorpId(), UserSession.getUserId());
				sender.setRequired(Value.T);
			}
			senderService.save(sender, sender.getSenderEmail());
			
			Views.ok(mb, "发件人" + operate + "默认成功");
			mb.put("checkedIds", checkedIds);
		} catch (Exception e) {
			Views.error(mb, "发件人" + operate + "默认失败");
			logger.error("(Sender:required) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String validate(HttpServletRequest request, ModelMap map) {
		try {
		    _throw();
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");
			Sender sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), checkedIds);
			Asserts.isNull(sender, "发件人");
			map.put("checkedName", sender.getSenderEmail());
			
			Views.map(map, "validate", "", "重发", "验证邮件", "checkedIds", checkedIds);

			return "default/account/sender_form";
		} catch (Exception e) {
			logger.error("(Sender:validate) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void validate(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
		    _throw();
			CSRF.validate(request);
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REQUIRED, R.MAIL);
			Asserts.isNull(checkedIds, "发件人");
			Sender sender = senderService.get(UserSession.getCorpId(), UserSession.getUserId(), checkedIds);
			Asserts.isNull(sender, "发件人");

			if (sender.getStatus().equals(Status.ENABLED)) {
				throw new Errors("禁止重发验证邮件");
			}
			
			String lock = (String) cache.get(Fqn.SENDER, UserSession.getUserId() + "_" + sender.getSenderEmail());
			if (StringUtils.isNotBlank(lock) && lock.equals("lock")) {
				int seconds = cache.getSeconds(Fqn.SENDER, UserSession.getUserId() + "_" + sender.getSenderEmail());
				throw new Errors("邮件发送太频繁，请您在" + seconds + "秒后再发送！");
			}
			
			sender.setRandCode(Randoms.getRandomString(32));
			
			senderService.save(sender, sender.getSenderEmail());
			validateNotify(sender);
			
			cache.put(Fqn.SENDER, UserSession.getUserId() + "_" + sender.getSenderEmail(), "lock");
			
			Views.ok(mb, "重发验证邮件成功");
			mb.put("checkedIds", checkedIds);
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
			logger.error("(Sender:validate) error: ", e);
		} catch (Exception e) {
			Views.error(mb, "重发验证邮件失败");
			logger.error("(Sender:validate) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	private void _throw() {
        if (!UserSession.senderValidate()) { throw new Errors("禁止验证发件人"); }
    }
	
	private void validateNotify(Sender sender) {
		if (producer != null) {
			try {
				Map<String, Object> map = Maps.newHashMap();
				map.put(Mail.TEMPLATE_FILE_NAME, "sender.vm");
				map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
				map.put(Mail.TO, sender.getSenderEmail());
				map.put(Mail.SUBJECT, Property.getStr(Config.SENDER));
				map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
				map.put(Mail.USER_NAME, sender.getUserId());
				map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(sender.getSenderEmail().getBytes()));
				map.put(Mail.RAND_CODE, sender.getRandCode());
				producer.sendTopic(map);
			} catch (Exception e) {
				throw new Errors("(sender:validateNotify) error: ", e);
			}
		}
	}
}

