package cn.edm.constants.mapper;

public enum WeekMap {

    _0(0, "本周"),
    _1(1, "上一周"),
    _2(2, "上两周"),
    _3(3, "上三周");

    private final int week;
    private final String desc;

    private WeekMap(int week, String desc) {
        this.week = week;
        this.desc = desc;
    }

    public int getWeek() {
        return week;
    }

    public String getDesc() {
        return desc;
    }

    public static final String getWeek(int week) {
        for (WeekMap mapper : WeekMap.values()) {
            if (mapper.getWeek() == week) {
                return mapper.getDesc();
            }
        }
        return null;
    }
    
    public static String week(String n) {
        if (n.equals("1")) return "一";
        else if (n.equals("2")) return "二";
        else if (n.equals("3")) return "三";
        else if (n.equals("4")) return "四";
        else if (n.equals("5")) return "五";
        else if (n.equals("6")) return "六";
        else if (n.equals("7")) return "七";
        else return null;
    }
}
