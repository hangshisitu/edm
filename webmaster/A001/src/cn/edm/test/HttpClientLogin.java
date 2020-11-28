package cn.edm.test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.apache.commons.httpclient.Cookie;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.NameValuePair;
import org.apache.commons.httpclient.cookie.CookiePolicy;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;


public class HttpClientLogin {
 
	private static String doctext ="";
	
	
    public static void getSinaISPRecord(String url){
        //登陆 Url
/*        String loginUrl = "http://chengxin.mail.sina.com.cn";*/
        //需登陆后访问的 Url
        String dataUrl = "http://chengxin.mail.sina.com.cn/use/comp_reputation_manage.php";
        HttpClient httpClient = new HttpClient();
        //模拟登陆，按实际服务器端要求选用 Post 或 Get 请求方式
        PostMethod postMethod = new PostMethod(url); 
        //设置登陆时要求的信息，一般就用户名和密码，验证码自己处理了
        NameValuePair[] data = {
                new NameValuePair("mail", "caixunyewu@sina.cn"),
                new NameValuePair("pwd", "Richinfo666")
/*                new NameValuePair("code", "anyany")*/
        };
        postMethod.setRequestBody(data);
 
        try {
            //设置 HttpClient 接收 Cookie,用与浏览器一样的策略
            httpClient.getParams().setCookiePolicy(CookiePolicy.BROWSER_COMPATIBILITY);
            httpClient.executeMethod(postMethod);
 
            //获得登陆后的 Cookie
            Cookie[] cookies=httpClient.getState().getCookies();
            String tmpcookies= "";
            for(Cookie c:cookies){
                tmpcookies += c.toString()+";";
            } 
            //进行登陆后的操作
            GetMethod getMethod = new GetMethod(dataUrl);
            //每次访问需授权的网址时需带上前面的 cookie 作为通行证
            getMethod.setRequestHeader("cookie",tmpcookies); 
            //你还可以通过 PostMethod/GetMethod 设置更多的请求后数据
            //例如，referer 从哪里来的，UA 像搜索引擎都会表名自己是谁，无良搜索引擎除外
            postMethod.setRequestHeader("Referer", "http://unmi.cc");
            postMethod.setRequestHeader("User-Agent","Unmi Spot");
            httpClient.executeMethod(getMethod);
            //打印出返回数据，检验一下是否成功
            /*doctext = getMethod.getResponseBodyAsString();*/ //返回内容太大导致警报
            BufferedReader reader = new BufferedReader(new InputStreamReader(getMethod.getResponseBodyAsStream()));  
            StringBuffer stringBuffer = new StringBuffer();  
            String str = "";  
            while((str = reader.readLine())!=null){  
                stringBuffer.append(str);  
            }  
            String ts = stringBuffer.toString();
            doctext = ts;
/*            System.out.println(text);*/
        } catch (Exception e) {
            e.printStackTrace();
        }
   
    }
    
    /*
	 * 获取指定HTML 文档制定的body
	 */
	private static void blogBody(String docText) throws IOException{
		//直接从字符串中输入HTML文档
		Document doc =Jsoup.parse(doctext);
/*		Iterator i = maps.entrySet().iterator();
		while (i.hasNext()) {
			Entry entry = (java.util.Map.Entry)i.next();
			Elements listDiv = doc.getElementsByAttributeValue(entry.getKey().toString(),entry.getValue().toString());//企业信誉管理
		}*/
/*		Elements listDiv = doc.getElementsByAttributeValue("class","signUp");//企业信誉管理
		System.out.println(listDiv.html());*/
		Elements listDiv1 = doc.getElementsByAttributeValue("class","hnTop");//信誉等级值
		for(Element e:listDiv1){
			;
			System.out.println("当前信誉等级  : "+e.getElementsByTag("b").html());
			
		}
	}
    
        public static void main(String[] args) throws IOException {
        	String loginUrl = "http://chengxin.mail.sina.com.cn";
        	getSinaISPRecord(loginUrl);
			blogBody("");
		}
    
    
}