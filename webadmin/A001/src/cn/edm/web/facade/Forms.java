package cn.edm.web.facade;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Map;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.Config;
import cn.edm.consts.Status;
import cn.edm.modules.utils.Property;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Maps;

public class Forms {

    private static final Logger logger = LoggerFactory.getLogger(Forms.class);
    
    /**
     * 表单流程是否已完成.
     */
    public static boolean ok(int confirm, int prompt,
            String inputPath, String submitPath, String confirmPath, String promptPath) {
        if (!new File(Webs.rootPath() + inputPath).exists()) return false;
        if (!new File(Webs.rootPath() + submitPath).exists()) return false;
        if (confirm == Status.ENABLED && prompt == Status.ENABLED) {
            if (!new File(Webs.rootPath() + confirmPath).exists()) return false;
            if (!new File(Webs.rootPath() + promptPath).exists()) return false;
        } else if (confirm == Status.ENABLED && prompt == Status.DISABLED) {
            if (!new File(Webs.rootPath() + confirmPath).exists()) return false;
        } else if (confirm == Status.DISABLED && prompt == Status.ENABLED) {
            if (!new File(Webs.rootPath() + promptPath).exists()) return false;
        }
        return true;
    }
    
    /**
     * 从/logs/form.log或/logs/form/*.log获取收件人.
     */
    public static Map<String, String> recipient(String email) {
        Map<String, String> map = Maps.newHashMap();
        String path = Property.getStr(Config.FORM_LOGGER);
        DateTime before = new DateTime().plusMonths(-1);
        scan(map, email, path + "/" + before.toString("yyyyMM") + ".log");
        scan(map, email, path + ".log");
        return map;
    }
    /**
     * 扫描log.
     */
    private static void scan(Map<String, String> map, String email, String path) {
        BufferedReader reader = null;
        try {
            if (!Files.exists(path))
                return;
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
            String[] nextLine = null;
            String line = null;
            while ((line = reader.readLine()) != null) {
                nextLine = StringUtils.splitPreserveAllTokens(StringUtils.removeStart(line, "(Form:ok) "), "\t");
                if (nextLine.length != 2)
                    continue;
                if (StringUtils.equalsIgnoreCase(email, nextLine[0])) {
                    map.put("email", nextLine[0]);
                    map.put("parameter_list", nextLine[1]);
                }
            }
        } catch (Exception e) {
            logger.error("(Forms:scan) error: ", e);
        } finally {
            IOUtils.closeQuietly(reader);
        }
    }
}
