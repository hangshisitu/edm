package cn.edm.jms;

import java.util.Map;

import javax.mail.MessagingException;
import javax.mail.internet.MimeMessage;

import org.apache.commons.lang.StringUtils;
import org.apache.velocity.app.VelocityEngine;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.mail.javamail.JavaMailSenderImpl;
import org.springframework.mail.javamail.MimeMessageHelper;
import org.xbill.DNS.Lookup;
import org.xbill.DNS.Record;
import org.xbill.DNS.Type;


import cn.edm.constant.MailKeys;
import cn.edm.util.SpringContextHolder;
import cn.edm.util.VelocityUtils;

/**
 * MIME邮件服务类.
 * 
 * 由Velocity引擎生成的的html格式邮件, 并带有附件.
 */
public class MimeMailService {

	private static final String DEFAULT_ENCODING = "utf-8";

	private static Logger logger = LoggerFactory.getLogger(MimeMailService.class);

	private VelocityEngine velocityEngine;

	/**
	 * 发送MIME格式的用户保单通知邮件.
	 */
	public void sendNotificationMail(Map<String, Object> map) {
		try {
			JavaMailSenderImpl mailSenderImpl = SpringContextHolder.getBean("mailSender");
			mailSenderImpl.setHost(host((String) map.get(MailKeys.TO)));
			JavaMailSender mailSender = mailSenderImpl;
			MimeMessage msg = mailSender.createMimeMessage();
			MimeMessageHelper helper = new MimeMessageHelper(msg, false, DEFAULT_ENCODING);

			helper.setFrom((String) map.get(MailKeys.FROM));
			helper.setTo((String) map.get(MailKeys.TO));
			helper.setSubject((String) map.get(MailKeys.SUBJECT));
			helper.setText(generateContent(map), true);
			
			mailSender.send(msg);
			logger.info("HTML版邮件已发送至" + (String) map.get(MailKeys.TO));
		} catch (MessagingException e) {
			logger.error("构造邮件失败", e);
		} catch (Exception e) {
			logger.error("发送邮件失败", e);
		}
	}

	private String host(String to) {
		try {
			String domain = StringUtils.substringAfter(to, "@");
			Lookup lookup = new Lookup(domain, Type.MX);
			lookup.run();
			if (lookup.getResult() == Lookup.SUCCESSFUL) {
				Record[] result = lookup.getAnswers();
				String host = result[0].getAdditionalName().toString();
				return host;
			}
		} catch (Exception e) {
			logger.error("获取MX失败", e);
		}
		return null;
	}
	
	/**
	 * 使用Velocity生成html格式内容.
	 */
	private String generateContent(Map<String, Object> map) throws MessagingException {
		String templateFileName = (String) map.get(MailKeys.TEMPLATE_FILE_NAME);
		return VelocityUtils.renderFile(templateFileName, velocityEngine, DEFAULT_ENCODING, map);
	}

	public void setVelocityEngine(VelocityEngine velocityEngine) {
		this.velocityEngine = velocityEngine;
	}
}
