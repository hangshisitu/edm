package test.edm.utils;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Attribute;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Entities;
import org.jsoup.select.Elements;
import org.junit.Test;

import cn.edm.utils.Asserts;
import cn.edm.utils.helper.Modulars;

public class ModularsTest {

    public void t() {
        Entities.EscapeMode.base.getMap().clear(); // JSoup hack
//        System.out.println(Modulars.filter("",
//                "/WEB-INF/static/template/259/201403/20140318161346_215.txt",
//                false, 1, 1, 1, 1, "", ""));
    }

    @Test
    public void v() throws IOException {
        String[] tags = {
                "a",
                "b", "blockquote", "br",
                "caption", "center", "cite", "code", "col", "colgroup",
                "dd", "div", "dl", "dt",
                "em",
                "font",
                "h1", "h2", "h3", "h4", "h5", "h6", "hr",
                "i", "img",
                "li",
                "meta",
                "ol",
                "p", "pre",
                "q",
                "small", "span", "strike", "strong", "sub", "sup",
                "table", "tbody", "td", "tfoot", "th", "thead", "title", "tr",
                "u", "ul"
        };

        Map<String, String[]> attrs = new HashMap<String, String[]>();
        
        attrs.put("a", new String[] {"href", "title", "target"});
        attrs.put("img", new String[] {"align", "border", "height", "src", "title", "width"});
        attrs.put("meta", new String[] {"http-equiv", "content"});
        attrs.put("table", new String[] {"align", "bgcolor", "border", "cellpadding", "cellspacing", "width"});
        attrs.put("tbody", new String[] {"align", "valign"});
        attrs.put("td", new String[] {"align", "bgcolor", "colspan", "height", "rowspan", "valign", "width"});
        attrs.put("th", new String[] {"align", "bgcolor", "colspan", "height", "rowspan", "valign", "width"});
        attrs.put("tfoot", new String[] {"align", "valign"});
        attrs.put(":all", new String[] {"class", "id", "name", "style"});
        
        Document doc = Jsoup.parse(new File("E:/03.workspace/EDM2/test/test/edm/utils/test.html"), "UTF-8");
        
        iter(doc.children(), tags, attrs);
    }
    
    private void iter(Elements elements, String[] tags, Map<String, String[]> attrs) {
        for (Element e : elements) {
            if (!Asserts.hasAny(e.tagName(), tags)) {
                System.out.println("tag: " + e.tagName());
            }
            for (Attribute attr : e.attributes()) {
                if (Asserts.hasAny(attr.getKey(), attrs.get(e.tagName()))
                        || Asserts.hasAny(attr.getKey(), attrs.get(":all"))) {
                    continue;
                }
                System.out.println("tag: " + e.tagName() + ", attr: " + attr.getKey());
            }
            iter(e.children(), tags, attrs);
        }
    }
}
