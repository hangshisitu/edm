package cn.edm.utils;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.BeanUtils;

import cn.edm.domain.Domain;
import cn.edm.domain.Result;
import cn.edm.domain.TouchSetting;

import com.google.common.collect.Maps;

public class ResultDataOption {

	/**
	 * 复制Result值
	 * 参数results为没投递效果的任务
	 * 从Result查询数据，复制Result的投递效果的统计数据到results对应属性中
	 * @param results
	 */
	public static void copyResultList(List<Domain> sourceResults, List<Domain> targetResults) {
		String ignore = "corpId, taskId, templateId, campaignId, campaignName, taskName, status, deliveryTime,corpPath";
		for(Domain newR : targetResults) {
			for(Domain oldR : sourceResults) {
				if(newR.getPlanId().equals(oldR.getPlanId())){
					String[] copy = Converts._toStrings(ignore);
		            BeanUtils.copyProperties(newR, oldR, copy);
		            break;
				}
			}
		}
	}
	/**
	 * 对结果数据进行统计
	 * @param rt
	 * @param rg
	 * @return
	 */
	public static Domain statResultTotal(Domain rt, Domain rg) {
		Domain r = new Domain();
		r.setAfterNoneCount(rt.getAfterNoneCount() + rg.getAfterNoneCount());
		r.setBeforeNoneCount(rt.getBeforeNoneCount() + rg.getBeforeNoneCount());
		r.setClickCount(rt.getClickCount() + rg.getClickCount());
		r.setClickUserCount(rt.getClickUserCount() + rg.getClickUserCount());
		r.setDnsFailCount(rt.getDnsFailCount() + rg.getDnsFailCount());
		r.setForwardCount(rt.getForwardCount() + rg.getForwardCount());
		r.setHardBounceCount(rt.getHardBounceCount() + rg.getHardBounceCount());
		r.setReachCount(rt.getReachCount() + rg.getReachCount());
		r.setReadCount(rt.getReadCount() + rg.getReadCount());
		r.setReadUserCount(rt.getReadUserCount() + rg.getReadUserCount());
		r.setSentCount(rt.getSentCount() + rg.getSentCount());
		r.setSoftBounceCount(rt.getSoftBounceCount() + rg.getSoftBounceCount());
		r.setSpamGarbageCount(rt.getSpamGarbageCount() + rg.getSpamGarbageCount());
		r.setUnsubscribeCount(rt.getUnsubscribeCount() + rg.getUnsubscribeCount());

		return r;
	}
	/**
	 * 把TemplateSetting的touchUrls解析到domain中的urlList中
	 * @param settingList
	 * @param domainList
	 */
    public static void copyUrlsToDomain(List<TouchSetting> settingList, List<Domain> domainList) {
    	if(settingList != null){
    		for(Domain d : domainList) {
    			for(TouchSetting s : settingList) {
    				if(d.getTemplateId().equals(s.getTemplateId()) && d.getCorpId().equals(s.getCorpId())){
    					String urlStr = s.getTouchUrls();
    					List<String> urlList = Converts.toStrings(urlStr);
    					d.setUrlList(urlList);
    					break;
    				}
    			}
    		}
		}
    }
    /**
     * 复制属性值: domain >> result.
     */
    public static void copy(String group, List<Domain> domainList, List<Result> resultList) {
        Map<String, Domain> map = Maps.newHashMap();
        for (Domain d : domainList) {
            if (group.equals("campaign"))
                map.put(String.valueOf(d.getCampaignId()), d);
            else if (group.equals("task"))
                map.put(String.valueOf(d.getTaskId()), d);
            else if (group.equals("triger"))
                map.put(String.valueOf(d.getCorpId() + ":" +  d.getTaskId()), d);
            else if (group.equals("plan"))
                map.put(String.valueOf(d.getPlanId()), d);
            else if (group.equals("touch"))
            	map.put(String.valueOf(d.getTemplateId()), d);
            else if (group.equals("customer")) 
            	map.put(String.valueOf(d.getCorpId() + ":" +  d.getTaskId()), d);
        }
        
        String ignore = "corpId, taskId, userId, taskType, templateId, campaignId, campaignName, taskName, subject, status, "
        			  + "deliveryTime, successRate, openRate, clickRate, urlList, emailDomain, touchCount, labelStatus, "
        			  + "createTime, isCanImport,companyName,corpPath";
        for (Result r : resultList) {
            Domain d = null;
            if (group.equals("campaign")) {
            	d = map.get(String.valueOf(r.getCampaignId()));
            } else if (group.equals("task")) {
                d = map.get(String.valueOf(r.getTaskId()));
            } else if (group.equals("triger")) {
                d = map.get(String.valueOf(r.getCorpId() + ":" + r.getTaskId()));
            } else if (group.equals("plan")) {
            	d = map.get(String.valueOf(r.getPlanId()));
            	ignore = StringUtils.remove(ignore, ",touchCount");
            } else if (group.equals("touch")) {
            	d = map.get(String.valueOf(r.getTemplateId()));
            } else if (group.equals("customer")) {
            	d = map.get(String.valueOf(r.getCorpId() + ":" + r.getTaskId()));
            }
//            if (d == null)
//                d = new Domain();
            if(d != null){
            	String[] copy = Converts._toStrings(ignore);
            	BeanUtils.copyProperties(d, r, copy);
            }
        }
    }
    
}
