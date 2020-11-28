package cn.edm.web.api;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.mapper.PathMap;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Webs;

public class Joins {

    private static Logger logger = LoggerFactory.getLogger(Joins.class);
    
    public String copy(String url, String path) {
        HttpClient client = null;
        BufferedReader reader = null;
        PrintWriter writer = null;
        try {
            String name = StringUtils.substring(path, StringUtils.lastIndexOf(path, "/") + 1);
            client = new DefaultHttpClient();
            HttpGet get = new HttpGet(url + path);
            HttpResponse response = client.execute(get);
            HttpEntity entity = response.getEntity();
            if (entity != null) {
                reader = new BufferedReader(new InputStreamReader(entity.getContent(), "UTF-8"));
                writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + PathMap.JOIN_API.getPath() + name))));
                String line = null;
                while ((line = reader.readLine()) != null) {
                    writer.write(line);
                    writer.write("\n");
                }
                writer.flush();
            }
            get.abort();
            return name;
        } catch (Exception e) {
            logger.warn("(Joins:copy) warn: " + e);
            throw new Errors("复制API文件失败");
        } finally {
            IOUtils.closeQuietly(reader);
            IOUtils.closeQuietly(writer);
            client.getConnectionManager().shutdown();
        }
    }
}
