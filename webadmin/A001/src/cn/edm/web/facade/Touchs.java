package cn.edm.web.facade;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Adapter;
import cn.edm.model.Touch;
import cn.edm.modules.orm.MapBean;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.web.Webs;

public class Touchs {

    private static final Logger logger = LoggerFactory.getLogger(Touchs.class);
    
    public static void create(List<Touch> touchList,
            String personal) {
        for (Touch touch : touchList) {
            PrintWriter writer = null;
            try {
                String id = Tasks.id(touch.getCorpId(), touch.getTaskId(), touch.getTemplateId());
                Files.make(Webs.rootPath() + PathMap.TOUCH.getPath() + id);
                String filePath = id + "/template.txt";
                writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + PathMap.TOUCH.getPath() + filePath), "UTF-8")));
                
                MapBean phoneAttrs = new MapBean();
                Adapter adapter = new Adapter();
                adapter.setPhoneAttrs(touch.getPhoneAttrs());
                Templates.attrs(adapter, phoneAttrs);
                
                Delivery.node(writer, "PERSONAL", personal);
                Delivery.node(writer, "PHONE_PERSONAL", phoneAttrs.getString("personal"));
                Delivery.node(writer, "JOIN_PHONE", String.valueOf(touch.getJoinPhone()));
                Delivery.node(writer, "JOIN_SMS", String.valueOf(touch.getJoinSms()));
                Delivery.MODULA_TEXT(writer, Files.get(Webs.rootPath(), touch.getSmsPath()));
                Delivery.MODULA_QVGA(writer, Files.get(Webs.rootPath(), touch.getPhonePath()));
                Delivery.MODULA(writer, Files.get(Webs.rootPath(), touch.getFilePath()));
                
                writer.flush();
            } catch (Exception e) {
                logger.error("(Touchs.create) error: ", e);
                throw new Errors("触发模板创建失败");
            } finally {
                if (writer != null) {
                    writer.close();
                }
            }
        }
    }
}
