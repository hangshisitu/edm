package test.edm.api;

import java.io.File;
import java.io.IOException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Entities;

import cn.edm.modules.utils.Property;
import cn.edm.utils.web.Webs;
import cn.edm.utils.web.XSS;


public class Safe4Test {

    public static void main(String[] args) throws IOException {
        Property.load(Webs.webRoot() + "/WEB-INF/classes/app.properties");
        
        Entities.EscapeMode.base.getMap().clear(); // JSoup hack
        
        Document doc = Jsoup.parse(new File(Webs.webRoot() + "/WEB-INF/static/template/259/201403/20140318154121_442.txt"), "utf-8");
        doc = XSS.safety(doc);
        
        System.out.println(doc.toString());
        
    }
}
