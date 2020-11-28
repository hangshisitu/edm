package cn.edm.web.facade;

import java.util.List;

import cn.edm.consts.Value;
import cn.edm.model.Corp;
import cn.edm.utils.Converts;

import com.google.common.collect.Lists;

public class Corps {

    /**
     * 获取当前机构ID与下一级机构ID.
     */
    public static Integer[] combine(Integer corpId, List<Corp> corpList) {
        List<Integer> corpIds = Lists.newArrayList();
        for (Corp corp : corpList) {
            corpIds.add(corp.getCorpId());
        }
        corpIds.add(corpId);
        return Converts._toIntegers(corpIds);
    }
    
    /**
     * 顶级机构.
     */
    public static Corp getRoot(List<Corp> corpList, Corp currentCorp) {
        if (currentCorp.getParentId().equals(Value.I)) {
            return currentCorp;
        }
        for (Corp corp : corpList) {
            if (corp.getCorpId().equals(currentCorp.getParentId())) {
                if (corp.getParentId().equals(0)) {
                    return corp;
                } else {
                    return getRoot(corpList, corp);
                }
            }
        }
        return null;
    }
    
    public static Integer getLevel(List<Corp> corpList, Corp currentCorp, Integer level) {
        level += 1;
        if (currentCorp.getParentId().equals(Value.I)) {
            return level;
        }
        for (Corp corp : corpList) {
            if (corp.getCorpId().equals(currentCorp.getParentId())) {
                if (corp.getParentId().equals(0)) {
                    return level + 1;
                } else {
                    return getLevel(corpList, corp, level);
                }
            }
        }
        return level;
    }
}
