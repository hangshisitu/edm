package cn.edm.web.facade;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Value;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validates;

public class Plans {

    public static String cron(HttpServletRequest request, Integer required) {
        String planType = Https.getStr(request, "planType", R.CLEAN, required, R.REGEX, "regex:^day|week|month$", "周期");
        planType = Values.get(planType, Value.S);
        String cron = Value.S;
        if (StringUtils.equals(planType, "day")) {
            String day_HHmm = Https.getStr(request, "day_HHmm", required, "时间点");
            if (!Validates.formatter(day_HHmm, "HH:mm")) throw new Errors("时间点不是合法值");
            String[] arr = day_HHmm.split(":");
            if (arr != null && arr.length == 2)
                cron = Integer.valueOf(arr[1]) + " " + Integer.valueOf(arr[0]) + " * * *";     
        } else if (StringUtils.equals(planType, "week")) {
            String week_dd = Https.getStr(request, "week_dd", R.CLEAN, required, "星期");
            String week_HHmm = Https.getStr(request, "week_HHmm", required, "时间点");
            if (!Validates.formatter(week_HHmm, "HH:mm")) throw new Errors("时间点不是合法值");
            String days = Value.S;
            if (StringUtils.equals(week_dd, "work_day")) days = "1,2,3,4,5";
            else if (StringUtils.equals(week_dd, "holiday")) days = "6,7";
            else days = week_dd;
            String[] arr = week_HHmm.split(":");
            if (arr != null && arr.length == 2)
                cron = Integer.valueOf(arr[1]) + " " + Integer.valueOf(arr[0]) + " * " + days + " *";
        } else if (StringUtils.equals(planType, "month")) {
            String month_dd = Https.getStr(request, "month_dd", R.CLEAN, required, "日期");
            String month_HHmm = Https.getStr(request, "month_HHmm", required, "时间点");
            if (!Validates.formatter(month_HHmm, "HH:mm")) throw new Errors("时间点不是合法值");
            String days = Value.S;
            if (StringUtils.equals(month_dd, "last_day")) days = "-1";
            else days = month_dd;
            String[] arr = month_HHmm.split(":");
            if (arr != null && arr.length == 2)
                cron = Integer.valueOf(arr[1]) + " " + Integer.valueOf(arr[0]) + " " + days + " * " + "*";
        }
        
        return cron;
    }
}
