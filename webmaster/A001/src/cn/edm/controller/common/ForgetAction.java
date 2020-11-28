package cn.edm.controller.common;

import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;


import cn.edm.constant.ConfigKeys;
import cn.edm.constant.MailKeys;
import cn.edm.domain.User;
import cn.edm.jms.NotifyMessageProducer;
import cn.edm.service.UserService;
import cn.edm.util.EncodeUtils;
import cn.edm.util.Https;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.RandomUtils;

import com.google.common.collect.Maps;

@Controller
public class ForgetAction {

	private static final Logger logger = LoggerFactory.getLogger(ForgetAction.class);

	private NotifyMessageProducer notifyProducer;
	
	@Autowired
	private UserService userService;
	
	@RequestMapping("/forget")
	public String forget() {
		return "common/forget";
	}

	@RequestMapping(value = "/forget", method = RequestMethod.POST)
	public String forget(HttpServletRequest request, ModelMap map) {
		try {
			String userId = Https.getString(request, "userId");
			valid(userId);
			

			User user = userService.getByUserId(userId);
			//valid(user);
			if (user == null) {
				map.put("message", "用户名不存在");
			}else if(StringUtils.isBlank(user.getEmail())){
				map.put("message", "注册用户没有绑定的邮箱，请联系M.target客服。");
			}else {
				user.setRandCode(RandomUtils.getRandomString(22));
				userService.updateUser(user, false);
				sendForgetMessage(user);
				success(map, user.getEmail());
			}
			return "common/success";
		}catch (Exception e) {
			logger.error(e.getMessage(), e);
			error(map, e.getMessage());
		}
		
		return "action/error";
	}
	
	private void valid(String userId) {
		if (StringUtils.isBlank(userId)) {
			throw new RuntimeException("请输入用户名。");
		}
	}
	
	/*private void valid(User user) {
		if (user == null) {
			throw new Errors("用户名不存在。");
		}
		if (StringUtils.isBlank(user.getEmail())) {
			throw new Errors("注册用户没有绑定的邮箱，请联系M.target客服。");
		}
	}*/
	
	private void success(ModelMap map, String email) {
		map.put("pageTitle", "M.target做中国最优秀的邮件营销平台");
		map.put("website", PropertiesUtil.get(ConfigKeys.WEBSITE_HTTP));
		map.put("status", "忘记密码");
		map.put("title", "邮件已发送");
		map.put("message", "我们已将重置密码邮件发送到您账号绑定的邮箱<b>" + email + "</b>中，请您查收邮件并根据邮件内容重置密码。");
	}
	
	private void error(ModelMap map, String message) {
		map.put("status", "邮件发送失败");
		map.put("message", message);
	}
	
	private void sendForgetMessage(User user) {
		if (notifyProducer != null) {
			try {
				Map<String, Object> map = Maps.newHashMap();
				map.put(MailKeys.TEMPLATE_FILE_NAME, "forget.vm");
				map.put(MailKeys.FROM, PropertiesUtil.get(ConfigKeys.SMTP_FROM));
				map.put(MailKeys.TO, user.getEmail());
				map.put(MailKeys.SUBJECT, PropertiesUtil.get(ConfigKeys.FORGET));
				map.put(MailKeys.MTARGET, PropertiesUtil.get(ConfigKeys.WEBADMIN_HTTP));
				map.put(MailKeys.USER_NAME, user.getUserId());
				map.put(MailKeys.EMAIL, EncodeUtils.encodeBase64URLSafe(user.getEmail().getBytes()));
				map.put(MailKeys.RAND_CODE, user.getRandCode());
				notifyProducer.sendTopic(map);
			} catch (Exception e) {
				throw new RuntimeException("邮件发送出现异常。");
			}
		}
	}
	
	@Autowired(required = false)
	public void setNotifyProducer(NotifyMessageProducer notifyProducer) {
		this.notifyProducer = notifyProducer;
	}
}