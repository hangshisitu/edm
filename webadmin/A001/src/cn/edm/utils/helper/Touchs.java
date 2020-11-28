package cn.edm.utils.helper;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;

import org.apache.commons.io.FileUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Resource;
import cn.edm.model.Robot;
import cn.edm.model.Setting;
import cn.edm.model.Touch;
import cn.edm.utils.Values;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Resources;
import cn.edm.web.facade.Robots;

import com.google.common.collect.Maps;

public class Touchs {

private static Logger logger = LoggerFactory.getLogger(Forwards.class);
    
    public static String write(String ids, String fid, Touch touch, Setting setting,
            List<Robot> robots, Resource resource, Map<Integer, String> propMap, String email) {
        String path = PathMap.TOUCH.getPath() + ids + "/";
        
        boolean completed = false;
        PrintWriter writer = null;
        try {
            Files.make(Webs.rootPath() + path);
            
            if (Files.exists(Webs.rootPath() + path + fid)) {
                completed = true;
                return path + fid;
            }
            
            DateTime start = new DateTime();

            String template = Webs.rootPath() + path + "/template.txt";
            boolean base64 = Values.get(Delivery.node(template, "PERSONAL")).equals("false");
            boolean phoneBase64 = Values.get(Delivery.node(template, "PHONE_PERSONAL")).equals("false");
            
            boolean joinPhone = Values.get(Delivery.node(template, "JOIN_PHONE")).equals("1");
            boolean joinSms = Values.get(Delivery.node(template, "JOIN_SMS")).equals("1");
            
            String templateContent = Values.get(Delivery.node(template, "MODULA"));
            String phoneConent = Values.get(Delivery.node(template, "MODULA_QVGA"));
            String smsContent = Values.get(Delivery.node(template, "MODULA_TEXT"));
            
            int rnd = Resources.rnd(resource.getSenderList(), resource.getHeloList(), resource.getSkipList());
            String helo = Resources.get(resource.getHeloList(), rnd);
            String skip = Resources.get(resource.getSkipList(), rnd);
            
            String SENDER = null;
            String ROBOT = null;
            String RAND = null;

            String robot = StringUtils.substringBefore(Robots.get(robots), ",");
            
            if (StringUtils.isBlank(robot))
                ROBOT = StringUtils.substringBefore(resource.getSenderList(), ",");
            else {
                if (StringUtils.isNotBlank(setting.getRobot()))
                    ROBOT = StringUtils.substringBefore(setting.getRobot(), ",");
                else
                    ROBOT = robot;
            }
            if (StringUtils.isNotBlank(touch.getSenderEmail()))
                SENDER = touch.getSenderEmail();
            else
                SENDER = ROBOT;
            RAND = setting.getRand();
            
            writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + path + fid), "UTF-8")));
            Delivery.ID(writer, String.valueOf(touch.getCorpId()), String.valueOf(touch.getTaskId()), String.valueOf(touch.getTemplateId()));
            Delivery.TIME(writer, null);
            Delivery.AD(writer, setting.getAd());
            Delivery.SMS(writer, setting.getSms());
            Delivery.SUBJECT(writer, touch.getSubject());
            Delivery.NICKNAME(writer, touch.getSenderName());
            Delivery.SENDER(writer, SENDER);
            
            Delivery.PIPE(writer, resource.getResourceId());
            Delivery.HELO(writer, helo);
            Delivery.ROBOT(writer, ROBOT);
            Delivery.RAND(writer, RAND);
            
            Delivery.MAILLIST_BEGIN(writer, propMap);
            Map<String, String> parameterMap = Maps.newHashMap();
            Delivery.CONTENT(writer, touch.getReceiver(), propMap, parameterMap, email, Value.S);
            Delivery.MAILLIST_END(writer);
            Delivery.CHART(writer, "");
            
            if (joinSms) {
                Delivery.MODULA_TEXT(writer, smsContent);
            }
            if (joinPhone) {
                Delivery.MODULA_QVGA(writer, Modulars.filterContent(touch.getCorpId(), touch.getTaskId(), touch.getTemplateId(),
                        skip, phoneConent, phoneBase64, 
                        null,
                        setting.getUnsubscribeId(), "", "1"));
            }
            
            Delivery.MODULA(writer, Modulars.filterContent(touch.getCorpId(), touch.getTaskId(), touch.getTemplateId(), 
                    skip, templateContent, base64, 
                    null,
                    setting.getUnsubscribeId(), "", "1"));
            
            writer.flush();
            
            FileUtils.copyFile(new File(Webs.rootPath() + path + fid), new File(Webs.webRoot() + path + fid));
            
            DateTime end = new DateTime();
            long seconds = Seconds.secondsBetween(start, end).getSeconds();
            
            logger.info("(Touchs:write) fid: " + fid + ", seconds: " + seconds + ", counter: 1");
            
            completed = true;
            
            return path + fid;
        } catch (Exception e) {
            throw new ServiceErrors("(Touchs:write) error:", e);
        } finally {
            if (writer != null) {
                writer.close();
            }
            if (!completed) {
                if (Files.exists(Webs.rootPath() + path + fid) && StringUtils.endsWith(path + fid, ".txt")) {
                    Files.delete(Webs.rootPath(), path + fid, PathMap.TOUCH.getPath());
                }
            }
        }
    }
}
