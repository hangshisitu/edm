package cn.edm.web.facade;

import java.util.Random;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Config;
import cn.edm.consts.Value;
import cn.edm.modules.utils.Property;

public class Resources {
    
    /**
     * 获取发送设置随机码.
     */
    public static int rnd(String senders, String helos, String skips) {
        int a = StringUtils.isNotBlank(senders) ? StringUtils.split(senders, ",").length : 0;
        int b = StringUtils.isNotBlank(helos) ? StringUtils.split(helos, ",").length : 0;
        int c = StringUtils.isNotBlank(skips) ? StringUtils.split(skips, ",").length : 0;

        int max = Math.max(Math.max(a, b), c);

        Random random = new Random();
        return random.nextInt(max);
    }
    
    /**
     * 获取发送设置.
     */
    public static String get(String list, int random) {
        if (StringUtils.isBlank(list)) return "";
        String[] tokens = StringUtils.splitPreserveAllTokens(list, ",");
        int number = (random) % tokens.length;
        return StringUtils.trim(tokens[number]);
    }
    
    /**
     * 打开链接.
     */
    public static String open(String skip) {
        skip = StringUtils.trim(skip);
        if (StringUtils.isNotBlank(skip)) return skip;
        return Property.getStr(Config.DEFAULT_SKIP);
    }

    /**
     * 点击链接.
     */
    public static String click(String skip) {
        skip = StringUtils.trim(skip);
        if (StringUtils.isNotBlank(skip)) return skip;
        return Value.S;
    }
}
