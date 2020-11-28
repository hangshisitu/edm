package cn.edm.web.facade;

import org.apache.commons.lang.StringUtils;

import cn.edm.modules.utils.Reflections;
import cn.edm.utils.Values;

public class Counts {
    
    private static final String FIELDS = "sent,reach,softBounce,hardBounce,readUser,read,clickUser,click,unsubscribe,forward,beforeNone,afterNone,dnsFail,spamGarbage,touch";
                                 // 映射:  0    1      2          3          4        5    6         7     8           9       10         11        12      13          14

    public static int[] fields(Object obj) {
        String[] fields = StringUtils.split(FIELDS, ",");
        int[] counts = new int[fields.length];
        int i = 0;
        for (String field : fields) {
            Integer count = 0;
            try {
                count = (Integer) Reflections.getFieldValue(obj, field + "Count");
            } catch (IllegalArgumentException e) { }
            counts[i] = Values.get(count);
            i++;
        }
        return counts;
    }
    
    public static String[] counts(int[] counts) {
        String[] arr = new String[counts.length];
        for (int i = 0; i < counts.length; i++) {
            arr[i] = Values.get(counts[i], "0");
        }
        return arr;
    }
    
    public static float[] percents(int[] counts) {
        float a = counts[0] - counts[11] == 0 ? 0 : (float) counts[1] * 100 / (counts[0] - counts[11]); // 成功率
        float b = counts[1] == 0 ? 0 : (float) counts[4] * 100 / counts[1]; // 打开率
        float c = counts[4] == 0 ? 0 : (float) counts[6] * 100 / counts[4]; // 点击率
        float d = counts[2] == 0 ? 0 : (float) counts[11] * 100 / (counts[2] + counts[3]); // 邮箱无效
        float e = counts[2] == 0 ? 0 : (float) counts[12] * 100 / (counts[2] + counts[3]); // 域名无效
        float f = counts[2] == 0 ? 0 : (float) counts[13] * 100 / (counts[2] + counts[3]); // 判定垃圾
        float g = counts[2] == 0 ? 0 : (float) ((counts[2] + counts[3]) - (counts[11] + counts[12] + counts[13])) * 100 / (counts[2] + counts[3]); // 其他
        return new float[] { a, b, c, d, e, f, g };
    }
    
    public static String[] percents(float[] percents) {
        String[] arr = new String[percents.length];
        for (int i = 0; i < percents.length; i++) {
            arr[i] = Values.get(percents[i], "0");
        }
        return arr;
    }

    public static void plus(Object source, Object target) {
        count(source, target, true);
    }
    
    public static void minus(Object source, Object target) {
        count(source, target, false);
    }
    
    private static void count(Object source, Object target, boolean t) {
        String[] fields = StringUtils.split(FIELDS, ",");
        for (String field : fields) {
            try {
                Integer a = (Integer) Reflections.getFieldValue(source, field + "Count");
                Integer b = (Integer) Reflections.getFieldValue(target, field + "Count");
                Integer c = t ? Values.get(b) + Values.get(a) : Values.get(b) - Values.get(a);
                Reflections.setFieldValue(target, field + "Count", c);
            } catch (IllegalArgumentException e) {
            }
        }
    }
}
