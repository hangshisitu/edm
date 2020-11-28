package cn.edm.app.schedule;

import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;

import cn.edm.app.jms.NotifyMessageProducer;
import cn.edm.consts.Config;
import cn.edm.consts.Mail;
import cn.edm.consts.Status;
import cn.edm.model.Cos;
import cn.edm.model.User;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.UserService;
import cn.edm.utils.DateUtil;
import cn.edm.utils.except.Errors;

import com.google.common.collect.Maps;
/**
 * @author xiaobo
 * 每分钟执行一次,检查试用账户的套餐是否到期，到期了给其发送邮件通知。
 */
public class RemindJob {

    private static final Logger logger = LoggerFactory.getLogger(RemindJob.class);
    
    private static Map<String,String> hadSendMap =new HashMap<String, String>();
    
    private NotifyMessageProducer producer;
	
    @Autowired
    private UserService userService;
    @Autowired
    private CorpService corpService;
    @Autowired
    private CosService  cosService;
    
	@Autowired(required = false)
	public void setProducer(NotifyMessageProducer producer) {
		this.producer = producer;
	}

	public void execute() {
		try {
			scan();
		} catch (Exception e) {
			logger.error("(remindJob:execute) error: ", e);
		}
	}

	private void scan() {
		List<Cos> coses = cosService.getCosByStatus(Status.TESTED,"1",Status.REMIND);
		if(coses != null && coses.size() > 0){	
			for(Cos cos:coses){
		        int deadLine = DateUtil.compareMills(cos.getEndTime(),new Date());
		        if(deadLine >60 || deadLine < -60){
		            continue;
		        }else{		
		        	cos.setStatus("0");	
		        	cosService.save(cos);
		    		logger.info("[ remindJob ] : scanMap {"+""+"}");
		            List<Integer> corpIds = corpService.getCorpId(cos.getCosId());
		            if(corpIds !=null && corpIds.size()>0){
		            	List<User> expireUsers = userService.get(corpIds);
		            	if(expireUsers != null && expireUsers.size()>0){
		            		for(User user:expireUsers){ 
		            			 if(hadSendMap.containsKey(user.getUserId())){
		            				 logger.info("[ 试用套餐到期提醒 ] : "+user.getUserId()+" 已提醒，跳过!");
		            				 continue ;
		            			 }else{
		            				 expireNotify(user);
		                    	     hadSendMap.put(user.getUserId(),user.getEmail()+"");
		                    	     System.out.println("size :" + hadSendMap.size());
		                    	     logger.info("[ 试用套餐到期提醒 ] : "+user.getUserId()+"账号试用套餐已经到期 - 相关提醒邮件已经发出!");
		            			 }                    	     
	                        }
		            	}  	
		            }       			
			    }	
		    }	
	    }
	}
	
	
	private void expireNotify(User user) {
		if (producer != null) {
			try {
				Map<String, Object> map = Maps.newHashMap();
				map.put(Mail.TEMPLATE_FILE_NAME, "remind.vm");
				map.put(Mail.FROM, Property.getStr(Config.SMTP_FROM));
				map.put(Mail.TO, user.getEmail());
				map.put(Mail.SUBJECT, Property.getStr(Config.REMIND));
				map.put(Mail.APP_URL, Property.getStr(Config.APP_URL));
				map.put(Mail.USER_NAME, user.getUserId());
				map.put(Mail.EMAIL, Encodes.encodeBase64URLSafe(user.getEmail().getBytes()));
				map.put(Mail.RAND_CODE, user.getRandCode());
				producer.sendTopic(map);
			} catch (Exception e) {
				throw new Errors("(Api:remindNotify) error: ", e);
			}
		}
	}
    
}