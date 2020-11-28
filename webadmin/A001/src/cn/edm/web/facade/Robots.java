package cn.edm.web.facade;

import java.util.List;

import org.apache.commons.lang.StringUtils;

import cn.edm.model.Robot;
import cn.edm.utils.Asserts;

public class Robots {

    /**
     * 获取网络发件人集合.
     */
    public static String get(List<Robot> robots) {
        StringBuffer sbff = new StringBuffer();
        if (!Asserts.empty(robots)) {
            for (Robot robot : robots) {
                sbff.append(robot.getEmail()).append(",");
            }
        }
        return StringUtils.removeEnd(sbff.toString(), ",");
    }
}
