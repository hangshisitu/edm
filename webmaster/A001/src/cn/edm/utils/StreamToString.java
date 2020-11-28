package cn.edm.utils;

import javax.servlet.http.HttpServletRequest;

public class StreamToString {
	
	public static String getStringByUTF8(String object){
		try{
		   if(object != null && object !="" ){
			   byte source [] = object.getBytes("iso8859-1");//得到form提交的原始数据(默认采用iso8859-1编码)
			   object = new String (source,"UTF-8");//解决乱码
			   return object;
		   }	
		   return null;
		}catch(Exception e){
		   e.getMessage();	
		   return null;
		}
		
	}
	
	
	public static String getStringByUTF8(HttpServletRequest request,String object){
		try{
			
		   object =(String)request.getParameter("search_"+object);
		   if(object != null && object !="" ){
			   byte source [] = object.getBytes("iso8859-1");//得到form提交的原始数据(默认采用iso8859-1编码)
			   object = new String (source,"UTF-8");//解决乱码
			   return object;
		   }	
		   return null;
		}catch(Exception e){
		   e.getMessage();	
		   return null;
		}
		
	}	
}