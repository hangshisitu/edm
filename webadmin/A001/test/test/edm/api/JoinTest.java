package test.edm.api;

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
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.test.context.ContextConfiguration;

import cn.edm.consts.mapper.PathMap;
import cn.edm.modules.test.CtxTestCase;
import cn.edm.service.CndService;
import cn.edm.service.CorpService;
import cn.edm.service.SettingService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.service.UserService;
import cn.edm.utils.web.Webs;

@ContextConfiguration(locations = { "/app-ctx.xml" })
public class JoinTest extends CtxTestCase {

    @Autowired
    private TaskService taskService;
    @Autowired
    private TemplateService templateService;
    @Autowired
    private CndService cndService;
    @Autowired
    private SettingService settingService;
    @Autowired
    private CorpService corpService;
    @Autowired
    private UserService userService;
    
    public void join() {
        HttpClient client = null;
        BufferedReader reader = null;
        PrintWriter writer = null;
        try {
            String path = "/temp/result/1.1157.107.txt.test";
            String name = StringUtils.substring(path, StringUtils.lastIndexOf(path, "/") + 1);
            client = new DefaultHttpClient();
            HttpGet get = new HttpGet("http://192.168.18.156" + path);
            HttpResponse response = client.execute(get);
            HttpEntity entity = response.getEntity();
            if (entity != null) {
                reader = new BufferedReader(new InputStreamReader(entity.getContent(), "UTF-8"));
                writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.webRoot() + PathMap.JOIN_API.getPath() + name))));
                String line = null;
                while ((line = reader.readLine()) != null) {
                    writer.write(line);
                    writer.write("\n");
                }
                writer.flush();
            }
            get.abort();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeQuietly(reader);
            IOUtils.closeQuietly(writer);
            client.getConnectionManager().shutdown();
        }
    }
}
