package cn.edm.web.facade;

import java.util.List;

import cn.edm.domain.Tag;
import cn.edm.utils.Asserts;

public class Tags {

    public static int count(List<Tag> sources, List<Tag> targets) {
        int count = 0;
        if (Asserts.empty(sources)) return count;
        for (Tag tag : sources) {
            if (tag.getDbId() > 0) {
                targets.add(tag);
                count += tag.getEmailCount();
            }
        }
        return count;
    }
}
