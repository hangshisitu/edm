package cn.edm.jms;

import java.util.Map;

import javax.jms.MapMessage;
import javax.jms.Message;
import javax.jms.MessageListener;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;


import cn.edm.constant.MailKeys;

import com.google.common.collect.Maps;

/**
 * 消息的异步被动接收者.
 * 
 * 使用Spring的MessageListenerContainer侦听消息并调用本Listener进行处理.
 */
public class NotifyMessageListener implements MessageListener {

	private static Logger logger = LoggerFactory.getLogger(NotifyMessageListener.class);

	@Autowired(required = false)
	private MimeMailService mimeMailService;
	
	/**
	 * MessageListener回调函数.
	 */
	@Override
	public void onMessage(Message message) {
		try {
			MapMessage mapMessage = (MapMessage) message;
			String templateFileName = mapMessage.getString(MailKeys.TEMPLATE_FILE_NAME);
			String from = mapMessage.getString(MailKeys.FROM);
			String to = mapMessage.getString(MailKeys.TO);
			String subject = mapMessage.getString(MailKeys.SUBJECT);
			String mtarget = mapMessage.getString(MailKeys.MTARGET);
			String userName = mapMessage.getString(MailKeys.USER_NAME);
			String email = mapMessage.getString(MailKeys.EMAIL);
			String randCode = mapMessage.getString(MailKeys.RAND_CODE);
			
			// 打印消息详情
			logger.info("FROM: " + from + ", TO: " + to + ", SUBJECT: " + subject);

			// 发送邮件
			if (mimeMailService != null) {
				Map<String, Object> map = Maps.newHashMap();
				map.put(MailKeys.TEMPLATE_FILE_NAME, templateFileName);
				map.put(MailKeys.FROM, from);
				map.put(MailKeys.TO, to);
				map.put(MailKeys.SUBJECT, subject);
				map.put(MailKeys.MTARGET, mtarget);
				map.put(MailKeys.USER_NAME, userName);
				map.put(MailKeys.EMAIL, email);
				map.put(MailKeys.RAND_CODE, randCode);
				mimeMailService.sendNotificationMail(map);
			}
		} catch (Exception e) {
			logger.error("处理消息时发生异常.", e);
		}
	}
}
