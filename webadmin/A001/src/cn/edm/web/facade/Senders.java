package cn.edm.web.facade;

import java.util.List;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Value;
import cn.edm.model.Sender;
import cn.edm.model.Task;
import cn.edm.utils.Asserts;

public class Senders {
    
    /**
     * 获取默认发件人.
     */
    public static Sender getRequired(List<Sender> senderList) {
        if (!Asserts.empty(senderList)) {
            for (Sender sender : senderList) {
                if (sender.getRequired().equals(Value.T)) {
                    return sender;
                }
            }
        }

        return null;
    }
    
    /**
     * 复制属性值: sender >> task.
     */
    public static void copy(Sender sender, Task task) {
        if (sender != null) {
            if (StringUtils.isBlank(task.getSenderEmail())) {
                task.setSenderEmail(sender.getSenderEmail());
            }
            if (StringUtils.isBlank(task.getSenderName())) {
                task.setSenderName(sender.getSenderName());
            }
        }
    }
}
