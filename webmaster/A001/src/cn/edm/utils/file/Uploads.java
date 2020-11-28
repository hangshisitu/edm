package cn.edm.utils.file;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.constants.Config;
import cn.edm.constants.Suffix;
import cn.edm.exception.Errors;
import cn.edm.modules.utils.Props;

public class Uploads {
	
	public static void maxSize(String template) {
		try {
			long contentSize = IOUtils.toInputStream(template).available();
			long maxSize = Props.getInt(Config.TEMPLATE_SIZE);
			if (contentSize > maxSize) {
				throw new Errors("模板大小不能大于" + Files.unit(maxSize));
			}
		} catch (IOException e) {
			throw new Errors("模板大小获取失败");
		}
	}

	public static void maxSize(MultipartFile file) {
		if (file.isEmpty()) {
			throw new Errors("文件内容不能为空值");
		}
		long maxSize = Props.getInt(Config.UPLOAD_SIZE);
		if (file.getSize() > maxSize) {
			throw new Errors("文件大小不能大于" + Files.unit(maxSize));
		}
	}
	
	public static String upload(int corpId, String charset, MultipartFile file, String savePath, String filePath) {
		try {
			Files.valid(savePath + filePath);
			
			DateTime now = new DateTime();
			String dir = Files.dir(corpId, now);
			String name = Files.name(now, "." + StringUtils.substringAfterLast(file.getOriginalFilename(), "."));
			
			Files.make(savePath + filePath + dir);
			
			String output = savePath + filePath + dir + name;
			String suff = Files.suffix(name);
			if (suff.equals(Suffix.TXT)) {
				Writers.txt(charset, "UTF-8", file.getInputStream(), new FileOutputStream(output));
			} else if (suff.equals(Suffix.CSV)) {
				Writers.csv(charset, "UTF-8", file.getInputStream(), new FileOutputStream(output));
			} else if (suff.equals(Suffix.XLS) || suff.equals(Suffix.XLSX)) {
				Writers.excel(suff, file.getInputStream(), new FileOutputStream(output));
			} else {
				throw new Errors("文件后缀不是合法值");
			}
			
			return filePath + dir + name;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			throw new Errors("文件上传失败", e);
		}
	}
	
	public static String upload(int corpId, String content, String savePath, String filePath) {
		try {
			Files.valid(savePath + filePath);
			
			DateTime now = new DateTime();
			String dir = Files.dir(corpId, now);
			String name = Files.name(now, ".txt");
			
			Files.make(savePath + filePath + dir);
			
			PrintWriter writer = new PrintWriter(new OutputStreamWriter(new FileOutputStream(savePath + filePath + dir + name), "UTF-8"));
			writer.print(content);
			writer.flush();
			writer.close();
			return filePath + dir + name;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			throw new Errors("文件上传失败", e);
		} 
	}
}
