package cn.edm.web.handler;

import java.io.IOException;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.Authentication;
import org.springframework.security.web.authentication.AuthenticationSuccessHandler;

import cn.edm.app.security.Securitys;
import cn.edm.consts.Const;
import cn.edm.consts.Sessions;
import cn.edm.consts.Value;
import cn.edm.model.Corp;
import cn.edm.model.Cos;
import cn.edm.model.History;
import cn.edm.model.Ip;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.HistoryService;
import cn.edm.service.IpService;
import cn.edm.utils.DateUtil;
import cn.edm.utils.web.Cookies;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;

public class LoginSuccessHandler implements AuthenticationSuccessHandler {

	private static final Logger logger = LoggerFactory.getLogger(LoginSuccessHandler.class);
	
	@Autowired
	private HistoryService historyService;
	@Autowired
	private IpService ipService;
	@Autowired
	private CorpService corpService;
	@Autowired
	private CosService  cosService;
	
	@Override
	public void onAuthenticationSuccess(HttpServletRequest request, HttpServletResponse response, Authentication authentication)
			throws IOException, ServletException {
		MapBean mb = new MapBean();
		try {
			String rememberMe = Https.getStr(request, "remember_me", R.CLEAN, R.REGEX, "regex:^true|false$"); 
			if (StringUtils.isNotBlank(rememberMe) && StringUtils.equals(rememberMe, "true")) {
				response.addCookie(Cookies.add(Cookies.USER_ID, Encodes.encodeBase64(UserSession.getUserId().getBytes())));
			} else {
				response.addCookie(Cookies.delete(Cookies.USER_ID));
			}
			
			response.addCookie(Cookies.add(Cookies.PAGE_SIZE, String.valueOf(Const.PAGE_SIZE)));

			request.getSession().removeAttribute(Sessions.KAPTCHA_COUNT);
			
			String userIp = Securitys.getCurrentUserIp();
			Ip ip = ipService.get(userIp);
			String region = (ip != null ? ip.getRegion() : Value.S);
			
			historyService.save(UserSession.getCorpId(), UserSession.getUserId(), history(Webs.browser(request), userIp, region));
			
			int corpId = UserSession.getCorpId();
	        Corp corp = corpService.get(corpId);
	        Cos  cos  = cosService.get(corp.getCosId());
	        Integer type = cos.getType();
	        int deadLine =0;
	        int effect =0;
	        String message ="登录成功";
	        if( type == 0 && cos.getEndTime() != null && cos.getStartTime() != null){
	        	deadLine = DateUtil.compareMills(cos.getEndTime(),new Date());
	        	effect = DateUtil.compareMills(cos.getStartTime(),new Date());
	        }
	        //测试账号- 如有提醒选项,倒数七天内登录弹框提示
	        if(type == 0){    
	        	if(cos.getRemind() != null && cos.getRemind() != "" && cos.getRemind().equals("1")){ 
	        	    if(deadLine > 0 && deadLine < 7*24*60*60){
	        		    logger.info("[ 试用套餐 ] : 试用套餐将要到期 - 请联系相关人员续费套餐!");
	            	    message ="[ 试用套餐 ] : 试用套餐将要到期 - 请联系相关人员续费套餐!";
	            	    mb.put("assign","AROUND");
	        	    }
	        	    if(deadLine <= 0 ){
	        	    	logger.info("[ 试用套餐 ] : 试用套餐已经到期 - 请联系相关人员续费套餐!");
	            	    message ="[ 试用套餐 ] : 试用套餐已经到期 - 请联系相关人员续费套餐!";
	            	    mb.put("assign","ALREADY");
	        	    }
	        	    if(effect > 0 ){
	        	    	logger.info("[ 试用套餐 ] : 试用套餐还未激活 - 还不能发邮件,请耐心等待!");
	            	    message ="[ 试用套餐 ] : 试用套餐还未激活 - 还不能发邮件,请耐心等待!";
	            	    mb.put("assign","NOTNOW");
	        	    }	        	   
        		} 
	        	Views.ok(mb,message);
        	}else{
        		Views.ok(mb,"登录成功");
        	}
		} catch (Exception e) {
			Views.error(mb, "登录超时");
			logger.error("(Success:on) error: ", e);
		}
		
		mb.put("count", Value.I);
		
		ObjectMappers.renderJson(response, mb);
	}

	private History history(String client, String ip, String region) {
		History history = new History();
		history.setCorpId(UserSession.getCorpId());
		history.setUserId(UserSession.getUserId());
		history.setLoginIp(ip);
		history.setRegion(region);
		history.setClient(client);
		history.setResult(Value.T);
		return history;
	}
}
