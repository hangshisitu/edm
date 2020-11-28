package cn.edm.utils.execute;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Map;

import org.apache.commons.lang.StringUtils;

import cn.edm.constant.mapper.PathMap;
import cn.edm.exception.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Maps;
import com.mongodb.BasicDBList;

public class R {

	public static Map<String, String> query(String path, BasicDBList emailList) {
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
			
			Map<String, String> queryMap = Maps.newLinkedHashMap();
			
			String line = null;

			while ((line = reader.readLine()) != null) {
				if (!StringUtils.contains(line, ",")) {
					continue;
				}
				
				String email = StringUtils.trim(StringUtils.substringBefore(line, ","));
				email = StringUtils.lowerCase(email);
				if (!Imports.isEmail(email)) {
					continue;
				}
				
				if (!emailList.contains(email)) {
					continue;
				}
				
				queryMap.put(email, StringUtils.substringAfter(line, ","));
			}
			
			return queryMap;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		}  catch (Exception e) {
			throw new Errors("不可能抛出的异常", e);
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
	/**
	 * 删除R临时文件.
	 */
	public static void delete(String userId, String fileId) {
		if (StringUtils.isBlank(fileId)) {
			return;
		}

		String filePath = PathMap.R.getPath() + userId + "/" + fileId;
		Files.delete(Webs.root(), filePath, PathMap.R.getPath());
	}
}
