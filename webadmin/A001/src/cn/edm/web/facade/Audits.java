package cn.edm.web.facade;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Id;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.utils.Asserts;

public class Audits {
    
    /**
     * 新增审核路径.
     */
    public static String add(String auditPath, int corpId) {
        String result = null;
        if (StringUtils.isNotBlank(auditPath)) {
            result = auditPath;
            result += corpId + "/";
        } else {
            result = "/" + corpId + "/";
        }
        
        return result;
    }
    
    /**
     * 移除审核路径.
     */
    public static String remove(String auditPath, int corpId) {
        String result = Value.S;
        if (StringUtils.isNotBlank(auditPath)) {
            result = StringUtils.replace(auditPath, "/" + corpId + "/", "/");
            if (StringUtils.equals(result, "/")) {
                result = Value.S;
            }
        }
        return result;
    }
    
    /**
     * 任务是否需要审核.
     */
    public static boolean moderate(String auditPath) {
        boolean audit = false;
        if (StringUtils.isNotBlank(auditPath) && StringUtils.equals(auditPath, "/" + Id.CORP + "/"))
        	audit = true;
        return audit;
    }
    
    /**
     * 任务是否需要审核.
     */
    public static boolean moderate(Integer moderate) {
    	return moderate != null && moderate.equals(Value.T);
    }
    
    /**
     * 获取任务的发送状态. sendCode(schedule:定时, current:实时, plan:周期)
     */
    public static int moderate(String sendCode, String auditPath) {
        int level = StringUtils.countMatches(auditPath, "/");
        if (level == 0) {
            return Asserts.hasAny(sendCode, new String[] { "schedule", "plan" }) ? Status.WAITED : Status.QUEUEING;       
//        } else if (level == 2 && auditPath.equals("/" + Id.CORP + "/"))
//            return Status.REVIEWING;
//        else
//            return Status.PROOFING;
        } else {
            return Status.REVIEWING;
        }
    }
}
