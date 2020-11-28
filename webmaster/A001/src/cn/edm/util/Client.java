package cn.edm.util;

public class Client {
	public static String browser(int id) {
        String browser = "其他";
        switch (id) {
            case 1: browser = "Opera"; break;
            case 2: browser = "QQ"; break;
            case 3: browser = "Chrome"; break;
            case 4: browser = "UC"; break;
            case 5: browser = "Safari"; break;
            case 6: browser = "Firefox"; break;
            case 7: browser = "360"; break;
            case 8: browser = "IE"; break;
        }
        return browser;
    }
    
    public static String lang(int id) {
        String lang = "其他";
        switch (id) {
            case 1: lang = "简体中文"; break;
            case 2: lang = "繁体中文"; break;
            case 3: lang = "英语"; break;
            case 4: lang = "法语"; break;
            case 5: lang = "德语"; break;
            case 6: lang = "日语"; break;
            case 7: lang = "韩语"; break;
            case 8: lang = "西班牙语"; break;
            case 9: lang = "瑞典语"; break;
        }
        return lang;
    }
    
    public  static String os(int id) {
        String os = "其他";
        switch (id) {
            case 1: os = "Android"; break;
            case 2: os = "iPhone"; break;
            case 3: os = "MacOS"; break;
            case 4: os = "Symbian"; break;
            case 5: os = "iPad"; break;
            case 6: os = "Windows"; break;
            case 7: os = "Linux"; break;
            case 8: os = "Windows Phone"; break;
        }
        return os;
    }

}
