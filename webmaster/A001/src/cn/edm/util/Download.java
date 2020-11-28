package cn.edm.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.OutputStream;

import javax.servlet.http.HttpServletResponse;

import cn.edm.constant.Suff;
import cn.edm.exception.Errors;
import cn.edm.modules.utils.Encodes;

public class Download {
	public static void download(HttpServletResponse response, String savePath, String filePath, String fileName) {
		Webs.setHeader(response, Encodes.urlEncode(fileName));

		File file = new File(savePath + filePath);
		if (!file.exists()) {
			throw new Errors("文件不存在");
		}

		try {
			OutputStream output = response.getOutputStream();
			String suff = Managers.suff(fileName);
			
			if (suff.equals(Suff.TXT)) {
				Writers.txt("UTF-8", "GBK", new FileInputStream(file), output);
			} else if (suff.equals(Suff.CSV)) {
				Writers.csv("UTF-8", "GBK", new FileInputStream(file), output);
			} else if (suff.equals(Suff.XLS) || suff.equals(Suff.XLSX)) {
				//Writers.excel(suff, new FileInputStream(file), output);
			} else {
				throw new Errors("文件后缀不是合法值");
			}
			
			output.flush();
			output.close();
		} catch (Exception e) {
			throw new Errors("文件下载失败", e);
		}
	}
	
	public static void downloadFile(HttpServletResponse response,File file) {
		if (!file.exists()) {
			throw new Errors("文件不存在");
		}
		Webs.setHeader(response, Encodes.urlEncode(file.getName()));
		try {
			OutputStream output = response.getOutputStream();
			String suff = Managers.suff(file.getName());
			
			if (suff.equals(Suff.TXT)) {
				Writers.txt("GBK", "GBK", new FileInputStream(file), output);
			} else if (suff.equals(Suff.CSV)) {
				Writers.csv("GBK", "GBK", new FileInputStream(file), output);
			} else if (suff.equals(Suff.XLS) || suff.equals(Suff.XLSX)) {
				//Writers.excel(suff, new FileInputStream(file), output);
			} else {
				throw new Errors("文件后缀不是合法值");
			}
			
			output.flush();
			output.close();
		} catch (Exception e) {
			throw new Errors("文件下载失败", e);
		}
	}
}
