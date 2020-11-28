package test.edm.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;

import cn.edm.utils.web.Webs;

public class Meta {
    public static void main(String[] args) {
        BufferedReader reader = null;
        BufferedWriter writer = null;
        try {
            String path = Webs.webRoot() + "/views/common/meta.html";
            
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
            long mills = new DateTime().getMillis();
            StringBuffer sbff = new StringBuffer();
            String line = null;
            while ((line = reader.readLine()) != null) {
                if (StringUtils.startsWith(line, "<#assign version = "))
                    sbff.append("<#assign version = \"" + mills + "\" />").append("\n");
                else
                    sbff.append(line).append("\n");
            }
            
            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(path), "UTF-8"));
            writer.write(sbff.toString());
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeQuietly(writer);
            IOUtils.closeQuietly(reader);
        }
    }
}
