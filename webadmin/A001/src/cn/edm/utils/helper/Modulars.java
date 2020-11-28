package cn.edm.utils.helper;

import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import cn.edm.consts.Config;
import cn.edm.consts.Id;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;
import cn.edm.web.facade.Resources;
import cn.edm.web.facade.Tasks;

public class Modulars {

    public static void filter(Document doc) {
        Elements imgs = doc.select("img");
        for (Element img : imgs) {
            String src = img.attr("src");
            if (StringUtils.startsWith(src, "/static"))
                img.attr("src", Property.getStr(Config.APP_URL) + src);
        }
    }
    
    public static String filterContent(int corpId, int taskId, int templateId,
            String skip, String content, boolean base64,
            Map<String, Integer> touchMap,
            Integer unsubscribeId, String labelIds, String test) {
        Document doc = Jsoup.parse(content);
        return filter(corpId, taskId, templateId, skip, doc, base64, touchMap, unsubscribeId, labelIds, test);
    }
    
    public static String filterPath(int corpId, int taskId, int templateId,
            String skip, String path, boolean base64,
            Map<String, Integer> touchMap,
            Integer unsubscribeId, String labelIds, String test) {
        Document doc = Jsoup.parse(Files.get(Webs.rootPath(), path));
        return filter(corpId, taskId, templateId, skip, doc, base64, touchMap, unsubscribeId, labelIds, test);
    }
	
	private static String filter(int corpId, int taskId, int templateId,
	        String skip, Document doc, boolean base64,
	        Map<String, Integer> touchMap, // { touchUrl, templateId }
	        Integer unsubscribeId, String labelIds, String test) {
		String open = Resources.open(skip);
		String click = Resources.click(skip);
		String appUrl = Property.getStr(Config.APP_URL);
        String[] appUrls = Converts._toStrings(Property.getStr(Config.APP_URLS));

        String uid = Values.get(unsubscribeId, "");
        String la = Values.get(labelIds);
        String t = Values.get(test, "0");
        
        String[] hrefs = { "unsubscribe", "complain", "forward", "online" };
        if (StringUtils.isNotBlank(click)) {
            Elements links = doc.select("[href]");
            Elements images = doc.select("[src]");
            for (Element link : links) {
                String href = link.attr("abs:href");
                boolean isStartsWithAppUrl = false;
                for(String url : appUrls) {
                	if(StringUtils.startsWith(href, url)){
                		isStartsWithAppUrl = true;
                		break;
                	}
                }
                if (isStartsWithAppUrl && Asserts.containsAny(href, hrefs)) {
					click(link, base64, href, "unsubscribe", appUrl, click, corpId, taskId, templateId, "&f=u", "&uid=" + uid, "", "");
					click(link, base64, href, "complain", appUrl, click, corpId, taskId, templateId, "&f=c", "&uid=" + uid, "", "");
					click(link, base64, href, "forward", appUrl, click, corpId, taskId, templateId, "&f=f", "", "", "");
					online(link, href, appUrl, corpId, taskId, templateId);
				} else {
                    if (!Asserts.empty(touchMap) && touchMap.containsKey(href)) {
                        String sid = Tasks.id(corpId, Id.START_TOUCH_ID + taskId, touchMap.get(href));
                        click(link, base64, href, ":href", appUrl, click, corpId, taskId, templateId, "&f=t&sid=" + sid, "", "&la=" + la, "&t=" + t);
                    } else {
                        click(link, base64, href, ":href", appUrl, click, corpId, taskId, templateId, "", "", "&la=" + la, "&t=" + t);
                    }
				}
			}
			for (Element link : images) {
				String src = link.attr("abs:src");
				click(link, base64, src, ":src", appUrl, click, corpId, taskId, templateId, "", "", "&la=" + la, "&t=" + t);
			}
		}
        if (StringUtils.isNotBlank(open)) {
		    open(doc, open, corpId, taskId, templateId, la, t);
		}
		
		Charts.filter(doc);
		
		String content = XSS.safety(doc).toString();
		
        content = StringUtils.replace(content, "#$CHART#", "<$CHART>");
        content = StringUtils.replace(content, "#/$CHART#", "</$CHART>");
        content = StringUtils.replace(content, "#$BASE64#", "<$BASE64>");
        content = StringUtils.replace(content, "#/$BASE64#", "</$BASE64>");

		return content;
	}
	
	private static void open(Document doc, String open, int corpId, int taskId, int templateId, String la, String t) {
        if (StringUtils.isNotBlank(open)) {
            doc.body().prepend("<img src=\"http://" + open + "/" + Property.getStr(Config.DEFAULT_OPEN) + "?u=http://" + open + "/logo.gif&c=" + corpId + "&ta=" + taskId + "&te=" + templateId + "&la=" + la + "&e=$iedm2_to$&t=" + t + "\" width=\"0\" height=\"0\" style=\"display:none\"/>");
        }
    }

	private static void click(Element link, boolean base64, String href, String action, String appUrl, String click, int corpId, int taskId, int templateId, String f, String uid, String la, String t) {
	    String u = null;
        if (StringUtils.endsWith(href, action))
            u = Encodes.encodeBase64((appUrl + "/" + action).getBytes());
        else if (StringUtils.equals(action, ":href") || StringUtils.equals(action, ":src"))
            if (base64) {
            	if(href.contains("form/input")){
            		u = "#$BASE64#" + href + "#/$BASE64#";
            	}else{
            		u = Encodes.encodeBase64(href.getBytes());
            	}
            } else {
            	u = "#$BASE64#" + href + "#/$BASE64#";
            }
        else
            return;
        StringBuffer sbff = new StringBuffer();
        sbff.append("http://").append(click).append("/").append(Property.getStr(Config.DEFAULT_CLICK)).append("?")
            .append("u=").append(u)
            .append(f)
            .append("&c=").append(corpId)
            .append("&ta=").append(taskId)
            .append("&te=").append(templateId)
            .append(uid)
            .append(la)
            .append(t)
            .append("&e=$iedm2_to$");
        link.attr("href", sbff.toString());
	}

	private static void online(Element link, String href, String appUrl, int corpId, int taskId, int templateId) {
        if (StringUtils.endsWith(href, "online"))
            link.attr("href", appUrl + "/online?SID=" + Tasks.id(corpId, taskId, templateId) + "&EMAIL=$iedm_to$");
	}
}
