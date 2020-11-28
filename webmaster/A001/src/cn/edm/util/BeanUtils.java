package cn.edm.util;

import javax.servlet.ServletContext;

import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

public class BeanUtils {

	
	public static Object getBean(String bean,ServletContext sc){
    	
   	 ServletContext application;     
        WebApplicationContext wac;     
        application = sc;     
        wac = WebApplicationContextUtils.getWebApplicationContext(application);//获取spring的context     
        return wac.getBean(bean);
        
   }
	
}
