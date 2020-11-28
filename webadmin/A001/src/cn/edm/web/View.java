package cn.edm.web;

import java.io.File;
import java.util.Date;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;

import cn.edm.app.auth.Auth;
import cn.edm.consts.mapper.BrowserMap;
import cn.edm.consts.mapper.FormMap;
import cn.edm.consts.mapper.LangMap;
import cn.edm.consts.mapper.OsMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.SelectionMap;
import cn.edm.consts.mapper.StatusMap;
import cn.edm.consts.mapper.WeekMap;
import cn.edm.model.Menu;
import cn.edm.modules.utils.encoder.Md5;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Values;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Audits;

public class View {
    
    /**
     * LICENSE权限.
     */
    public boolean auth(String link) {
        if (!Auth.isAuth())
            return true;
//        String robot = Auth.ROBOT;
//        String code = Auth.MAP.get(UrlMap.CODE.getAction());
//        return Auth.link(link, robot, code);
        return true;
    }
    
    /**
     * 取值.
     */
    public String value(String value, String defaultValue) {
        if (StringUtils.isNotBlank(value))
            return value;
        return defaultValue;
    }
    
    /**
     * 浏览器.
     */
    public String browser(int id) {
        return BrowserMap.getName(id);
    }

    /**
     * 语言.
     */
    public String lang(int id) {
        return LangMap.getName(id);
    }
    
    /**
     * 系统.
     */
    public String os(int id) {
        return OsMap.getName(id);
    }
    
    /**
     * 收件人昵称.
     */
    public String receiver(String receiver) {
        if (StringUtils.isBlank(receiver)) {
            return "邮箱";
        }
        if (receiver.equals("email_pre")) {
            return "邮箱前缀";
        } else if (receiver.equals("true_name")) {
            return "姓名";
        } else if (receiver.equals("nick_name")) {
            return "昵称";
        } else {
            return "邮箱";
        }
    }

    /**
     * 周期cron.
     */
    public String cron(String cron) {
        String cronStr = null;
        if (StringUtils.isNotBlank(cron)) {
            String[] crons = StringUtils.splitPreserveAllTokens(cron, " ");
            if (Asserts.empty(crons) || crons.length < 5) {
                return null;
            }

            String minute = crons[0];
            String hour = crons[1];
            String day = crons[2];
            String week = crons[3];
            String month = crons[4];

            // month
            if (!day.equals("*")) {
                cronStr = "每月";
                if (day.equals("-1")) cronStr += "最后一天";
                else cronStr += day + "日";
            }
            // week
            if (!week.equals("*")) {
                cronStr = "每周";
                if (week.equals("1,2,3,4,5")) cronStr += "工作日（周一至周五）";
                else if (week.equals("6,7")) cronStr += "休假（周六至周日）";
                else if (StringUtils.splitPreserveAllTokens(week, ",").length == 1)
                    cronStr += "周" + WeekMap.week(week);
            }
            // day
            if (month.equals("*") && week.equals("*") && day.equals("*")) {
                cronStr = "每日";
            }

            if (StringUtils.isNotBlank(cronStr)) {
                if (hour.length() == 1) hour = "0" + hour;
                if (minute.length() == 1) minute = "0" + minute;
                cronStr += hour + ":" + minute;                
            }
        }
        
        return cronStr;
    }
    
    /**
     * 属性.
     */
    public String prop_name(String propName) {
        return Values.get(PropMap.getName(propName), propName);
    }
    
    /**
     * 筛选存储类型.
     */
    public String selection_type(int type) {
        return SelectionMap.getName(type);
    }
    
    /**
     * 表单类别.
     */
    public String form_type(int type) {
        return FormMap.getName(type);
    }
    
    /**
     * 表单路径.
     */
    public boolean form_path(String path) {
        if (StringUtils.isBlank(path))
            return false;
        return new File(Webs.rootPath() + path).exists();
    }
    
    /**
     * 活跃.
     */
    public String active_cnd(String cnd) {
        if (cnd.equals("-1")) {
            return "不活跃";
        } else if (cnd.equals("1")) {
            return "活跃";
        } else if (cnd.equals("1week")) {
            return "一周以前活跃";
        } else if (cnd.equals("1month")) {
            return "一个月以前活跃";
        } else if (cnd.equals("2month")) {
            return "二个月以前活跃";
        } else if (cnd.equals("3month")) {
            return "三个月以前活跃";
        } else {
            return "全部";
        }
    }
    
    /**
     * 筛选集合.
     */
    public String in_cnd(String cnd) {
        if (StringUtils.isNotBlank(cnd) && cnd.equals("and")) {
            return "交集";
        } else {
            return "并集";
        }
    }
    
    /**
     * 任务状态.
     */
    public String task_status(int id) {
        return StatusMap.getName(id);
    }
    
    /**
     * 审核状态.
     */
    public String task_moderate(Integer moderate) {
        return Audits.moderate(moderate) ? "需要" : "不需要";
    }
    /**
     * 审核状态.
     */
    public String task_moderate(String auditPath) {
    	return Audits.moderate(auditPath) ? "需要" : "不需要";
    }
    
    /**
     * 广告.
     */
    public String task_ad(String ad) {
        if (StringUtils.isNotBlank(ad) && ad.equals("true")) {
            return "（AD）";
        } else {
            return "";
        }
    }
    
    /**
     * 网络发件人.
     */
    public String task_robot(String robot) {
        if (StringUtils.isBlank(robot) || robot.equals("-1")) {
            return "随机";
        } else {
            return robot;
        }
    }
    
    /**
     * 加入随机数.
     */
    public String task_rnd(String rand) {
        if (StringUtils.isNotBlank(rand) && rand.equals("true")) {
            return "（加入随机数）";
        }
        return "";
    }
    
    /**
     * 发送方式.
     */
    public String task_send_code(String sendCode, Date date) {
        if (StringUtils.isBlank(sendCode)) return "";
        if (sendCode.equals("current")) {
            return "实时发送";
        } else if (sendCode.equals("schedule")) {
            return "定时发送&nbsp;" + new DateTime(date).toString(Calendars.DATE_TIME);
        } else {
            return "周期发送";
        }
    }
    
    /**
     * 成功数.
     */
    public float result_reach(Integer sent, Integer reach, Integer none) {
        int sum = sent - none;
        return sum == 0 ? 0 : (float) reach * 100 / sum;
    }
    
    /**
     * 打开百分比.
     */
    public float result_read(Integer reach, Integer read) {
        return reach == 0 ? 0 : (float) read * 100 / reach;
    }
    
    /**
     * 点击百分比.
     */
    public float result_click(Integer read, Integer click) {
        return read == 0 ? 0 : (float) click * 100 / read;
    }
    /**
     * 触发率百分比. </br>
     * 该触点链接的点击人数/主模板任务的打开人数
     */
    public float result_touch(Integer touchCount, Integer readUserCount) {
    	return readUserCount == 0 ? 0 : (float) touchCount * 100 / readUserCount;
    }
    /**
     * 模板编号
     * @param userId
     * @param templateId
     * @return
     */
    public String templateId(String userId, Integer templateId) {
        return new Md5().encode(userId + templateId);
    }
    /**
     * 验证各菜单对应的操作功能权限 </br>
     * 菜单功能增删查改权限，0=全部功能，（1,2,3,4,5,6）=查询、增加、修改、删除、监管、导出
     * @param url
     * 			菜单URL
     * @param funcAuthNum
     * 			 菜单功能增删查改权限,查询=1、增加=2、修改=3、删除=4、监管=5、导出＝6、
     * @return
     */
    public boolean funcAuth(String url, Integer funcAuthNum) {
    	if(StringUtils.isBlank(url) || funcAuthNum==null) return false;
    	List<Menu> menuList = UserSession.getUser().getMenus();
    	if(menuList==null || menuList.size()==0) return false;
    	for(Menu m : menuList) {
    		String menuUrl = m.getUrl();
    		if(url.equals(menuUrl)){
    			Integer[] funcAuths = m.getFuncAuths();
    			// 对 0 进行特别验证，如果是 0 即表示所有权限，无论funcAuthNum值为多少都为true
    			if(funcAuths!=null && funcAuths.length==1 && funcAuths[0]==0) {
					return true;
    			}
    			if(Asserts.hasAny(funcAuthNum, funcAuths)){
    				return true;
    			}else{
    				return false;
    			}
    		}
    	}
    	return false;
    }
    
}
