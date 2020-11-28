package cn.edm.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class FileManagerUtils {

	private static final Logger logger = LoggerFactory.getLogger(FileManagerUtils.class);
	
	public static String getContent(String savePath, String filePath) {
		String content = "";
		InputStream in = null;
		try {
			File file = new File(savePath + filePath);
			if (file.exists()) {
				in = new FileInputStream(file);
				content = IOUtils.toString(in, "UTF-8");
				content = parseContent(content);
			}
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		} finally {
			try {
				if(in != null) 
					in.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return content;
	}
	
	public static String getContent(File file) {
		String content = "";
		InputStream in = null;
		try {
			if (file.exists()) {
				in = new FileInputStream(file);
				content = IOUtils.toString(in, "UTF-8");
				content = parseContent(content);
			}
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		} finally {
			try {
				if(in != null) 
					in.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return content;
	}
	
	public static String parseContent(String content) {
		Document doc = Jsoup.parse(content);
		Elements links = doc.getElementsByTag("a"); 
		for (Element link : links) { 
			  String linkHref = link.attr("href"); 
			  String linkText = link.text(); 
			  
			  if(StringUtils.endsWith(linkHref, "/forward")&&"转发给好友".equals(linkText.trim())) {
				  link.attr("href", "javascript:alert('此功能需在邮件中体验，模板无法提供真实效果。')");
				  link.removeAttr("target");
			  }else if(StringUtils.endsWith(linkHref, "/unsubscribe")&&"退订".equals(linkText.trim())) {
				  link.attr("href", "javascript:alert('此功能需在邮件中体验，模板无法提供真实效果。')");
				  link.removeAttr("target");
			  }else if(StringUtils.endsWith(linkHref, "/complain")&&"投诉".equals(linkText.trim())){
				  link.attr("href", "javascript:alert('此功能需在邮件中体验，模板无法提供真实效果。')");
				  link.removeAttr("target");
			  }
	    } 
		return doc.html();
	}
	
	public static String node(String path, String tag) {
        String content = null;
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
            boolean isTag = false;
            String line = null;
            
            StringBuffer sbff = new StringBuffer();
            
            while ((line = reader.readLine()) != null) {
                if (StringUtils.equals(StringUtils.trim(line), "<$" + tag + ">")) {
                    isTag = true;
                    continue;
                }
                if (StringUtils.equals(StringUtils.trim(line), "</$" + tag + ">")) {
                    isTag = false;
                    break;
                }
                if (isTag) {
                    sbff.append(line);
                }
            }
            
            content = StringUtils.trim(sbff.toString());
        } catch (Exception e) {
            logger.error("(Delivery:line) error: ", e);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                }
            }
        }
        return content;
    }
}
