package cn.edm.util;

import java.io.File;
import java.io.FileInputStream;
import java.util.Arrays;
import java.util.Random;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.exception.Errors;
import cn.edm.modules.orm.MapBean;

public class Managers {

	public static void valid(String path) {
		File file = new File(path);
		if (!file.isDirectory()) {
			throw new Errors("文件路径不存在");
		}
		if (!file.canWrite()) {
			throw new Errors("文件路径没有写权限");
		}
	}
	
	public static void valid(MultipartFile file, MapBean suffMap) {
		if (file == null) {
			throw new Errors("文件不存在");
		}
		if (file.isEmpty()) {
			throw new Errors("文件内容为空");
		}
		if (!suffMap(file.getOriginalFilename(), suffMap)) {
			throw new Errors("文件后缀有误");
		}
	}
	
	public static boolean exists(String path) {
		if (new File(path).exists()) {
			return true;
		}
		return false;
	}

	private static boolean suffMap(String path, MapBean suffMap) {
		String suff = StringUtils.lowerCase(StringUtils.substringAfterLast(path, "."));
		if (!Arrays.<String> asList(suffMap.getString("file").split(",")).contains(suff)) {
			return false;
		}
		return true;
	}

	public static String get(String webRoot, String path) {
		try {
			File file = new File(webRoot + path);

			if (file.exists()) {
				FileInputStream fis = new FileInputStream(file);
				String content = IOUtils.toString(fis, "utf-8");
				fis.close();
				return content;
			}
		} catch (Exception e) {
			throw new Errors("文件不存在", e);
		}
		return null;
	}

	public static void delete(String webRoot, String filePath, String validate) {
		if (StringUtils.isNotBlank(filePath) && StringUtils.startsWith(filePath, validate)) {
			delete(webRoot + filePath);
		}
	}
	
	private static void delete(String path) {
		try {
			if (StringUtils.isNotBlank(path)) {
				File file = new File(path);
				if (file.exists()) {
					FileUtils.forceDelete(file);
				}
			}
		} catch (Exception e) {
			throw new Errors("文件删除失败", e);
		}
	}

	public static void make(String path) {
		File file = new File(path);
		if (!file.exists()) {
			file.mkdirs();
		}
	}

	public static String dir(int corpId, DateTime date) {
		return corpId + "/" + date.toString("yyyyMM") + "/";
	}
	
	public static String name(DateTime date, String suff) {
		return date.toString("yyyyMMddHHmmss") + "_" + new Random().nextInt(1000) + suff;
	}
	
	public static String pref(String path) {
		if (StringUtils.isNotBlank(path)) {
			return StringUtils.substringBeforeLast(path, ".");
		}
		return null;
	}
	
	public static String suff(String path) {
		if (StringUtils.isNotBlank(path)) {
			return StringUtils.lowerCase(StringUtils.substringAfterLast(path, "."));
		}
		return null;
	}

	
}
