package cn.edm.util;

import java.io.IOException;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletResponse;

public class IOUitl {
	
	public static ServletOutputStream getOut(HttpServletResponse response, String fileName) throws IOException {
		response.setCharacterEncoding("utf-8");
		response.setContentType("application/x-msdownload");
		response.setHeader("Content-Disposition", "attachment;fileName=" + fileName);
		return response.getOutputStream();
	}
}
