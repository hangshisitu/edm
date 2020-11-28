package cn.edm.utils.web;

import org.apache.commons.lang.StringUtils;

public class XSS {
    
    public String filter(String text) {
        if (StringUtils.isBlank(text)) {
            return "";
        }

        text = text.replace("&", "&amp;");
        text = text.replace("<", "&lt;");
        text = text.replace(">", "&gt;");
        text = text.replace("\"", "&quot;");
        text = text.replace("\'", "&#x27;");
        text = text.replace("/", "&#x2F");
        
        return text;
    }
    
    public static String clean(String text) {
        StringBuilder sbff = new StringBuilder();
        char[] ch = text.toCharArray();
        for (int i = 0; i < ch.length; i++) {
            char c = ch[i];
            if (!Clean.index(c) && !Clean.extension(c)) {
                sbff.append(c);
            }
        }

        return sbff.toString();
    }
}
