package cn.edm.utils.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigDecimal;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Random;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.constant.PathCons;
import cn.edm.constants.Const;
import cn.edm.constants.mapper.PathMap;
import cn.edm.exception.Errors;
import cn.edm.modules.orm.MapBean;

public class Files {

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

	public static void delete(String webRoot, String filePath, String root) {
		if (StringUtils.isNotBlank(filePath) && StringUtils.startsWith(filePath, root)) {
			delete(webRoot + filePath);
		}
	}
	
	public static void delete(String path) {
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
	
	public static String name(DateTime date, String suffix) {
		return date.toString("yyyyMMddHHmmss") + "_" + new Random().nextInt(1000) + suffix;
	}
	
	public static String prefix(String path) {
		if (StringUtils.isNotBlank(path)) {
			return StringUtils.substringBeforeLast(path, ".");
		}
		return null;
	}
	
	public static String suffix(String path) {
		if (StringUtils.isNotBlank(path)) {
			return StringUtils.lowerCase(StringUtils.substringAfterLast(path, "."));
		}
		return null;
	}

	public static String unit(double value) {
		double size = 0;
		String unit = null;

		double b = bd(value, Const.B);
		if ((b >= 0) && (b < Const.KB)) {
			size = b;
			unit = " 字节";
		} else if ((b >= Const.KB) && (b < Const.MB)) {
			size = bd(value, Const.KB);
			unit = " Kb";
		} else if ((b >= Const.MB) && (b < Const.GB)) {
			size = bd(value, Const.MB);
			unit = " Mb";
		} else if ((b >= Const.GB) && (b < Const.TB)) {
			size = bd(value, Const.GB);
			unit = " Gb";
		}
		
		return size + unit;
	}
	
	private static double bd(double value, int unit) {
		BigDecimal bd = new BigDecimal(value / unit);
		return bd.setScale(2, BigDecimal.ROUND_HALF_UP).doubleValue();
	}
	
	public static void delete(int splitCount, String webRoot, String filePath, PathMap path) {
		if (splitCount == 0) {
			return;
		}
		for (int i = 0; i < splitCount; i++) {
			String inTagPath = filePath + "/in.map." + (i + 1) + ".txt";
			if (Files.exists(webRoot + inTagPath)) {
				delete(webRoot, inTagPath, path.getPath());
			}
			String exTagPath = filePath + "/ex.map." + (i + 1) + ".txt";
			if (Files.exists(webRoot + exTagPath)) {
				delete(webRoot, exTagPath, path.getPath());
			}
		}
	}
	
    public static FileItem create(String filePath, String fileName) {
        FileItem item = new DiskFileItemFactory().createItem(null, null, false, fileName);
        InputStream input = null;
        OutputStream output = null;
        try {
            input = new FileInputStream(new File(filePath));
            output = item.getOutputStream();
            IOUtils.copy(input, output);
        } catch (IOException e) {
        } finally {
            try {
                if (output != null) output.close();
                if (input != null) input.close();
            } catch (IOException e) { }
        }
        return item;
    }
    /**
     * 遍历dirPath目录，找出前缀不为"~"并且是最新修改时间的任务文件
     * @param dirPath
     * @return
     */
    public static File getNewestFile(String dirPath) {
    	File dir = new File(dirPath);
    	File newestFile = null;
    	if(dir.exists()){
    		File[] files = dir.listFiles();
			if(files!=null && files.length>0){
				Arrays.sort(files, new Files.CompratorByLastModified());
				for(int i=0,len=files.length; i<len; i++) {
	    			File f = files[i];
	    			String fileName = f.getName();
	    			if(!StringUtils.startsWith(fileName, "~")) {
	    				newestFile = f;
	    				break;
	    			}
	    		}
			}
    		
    	}
    	return newestFile;
    }
    /**
	 * 进行文件排序时间，按最新时间到最旧时间排序
	 */
	private static class CompratorByLastModified implements Comparator<File> {
		public int compare(File f1, File f2) {
			long diff = f1.lastModified() - f2.lastModified();
			if (diff > 0)
				return -1;
			else if (diff == 0)
				return 0;
			else
				return 1;
		}

	}
	
	/**
     * 创建文件
     * @param path
     * @return
     */
    public static boolean createFile(String path) {
    	File file = new File(path);
        boolean b = false;
        try {
        	b = file.createNewFile();
		} catch (IOException e) {
			e.printStackTrace();
		}  
        return b;
    }
	
    /**
	 * 创建以任务ID为名的文件，扫描时扫描该名的文件
	 * @param taskId
	 */
	public static boolean createTask(Integer taskId) {
		 String dir = PathCons.WEBADMIN_PATH + PathCons.SCAN_TASK;
         if(!Files.exists(dir)) Files.make(dir);
         String fileName = taskId + ".txt";
         String filePath = dir + fileName;
         return createFile(filePath);
	}
	
	/**
	 * 创建以周期ID为名的文件，扫描时扫描该名的文件
	 * @param planId
	 */
	public static boolean createPlan(Integer planId) {
		String dir = PathCons.WEBADMIN_PATH + PathCons.SCAN_PLAN;
		if(!Files.exists(dir)) Files.make(dir);
		String fileName = planId + ".txt";
		String filePath = dir + fileName;
		return createFile(filePath);
	}
	
}
