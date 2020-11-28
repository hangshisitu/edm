package cn.edm.util;

import java.io.PrintWriter;

import javax.servlet.http.HttpServletResponse;

import com.google.gson.Gson;

public class JsonUtil {
	
	public static void out(HttpServletResponse response,Object obj){
		response.setContentType("application/json;charset=utf-8");
        response.setHeader("pragma", "no-cache");
        response.setHeader("cache-control", "no-cache");
        PrintWriter out =null;
        try {
        	out = response.getWriter();
        	Gson gson = new Gson();
        	String result = gson.toJson(obj);
        	System.out.println("result is "+result);
        	out.write(result);
        	out.flush();
        }catch(Exception e) {
        	e.printStackTrace();
        }finally {
        	 if(out != null) {
                 out.close();
        	 }
        }

	}
}
