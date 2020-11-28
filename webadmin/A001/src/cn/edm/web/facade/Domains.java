package cn.edm.web.facade;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.BeanUtils;

import cn.edm.model.Domain;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.utils.Converts;

import com.google.common.collect.Maps;

public class Domains {
    
    /**
     * 抽取other.
     */
    public static void fetchOther(List<Domain> domainList) {
        Domain other = null;
        for (Domain d : domainList) {
            if (StringUtils.equals(d.getEmailDomain(), "other"))
                other = d;
        }
        if (other != null) {
            domainList.remove(other);
            domainList.add(other);
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
        }

        String ignore = "corpId,taskId,templateId,planId,campaignId,campaignName,taskName,status,deliveryTime,touchCount,templateCount";
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
            }
            if (d == null)
                d = new Domain();
            
            String[] copy = Converts._toStrings(ignore);
            BeanUtils.copyProperties(d, r, copy);
        }
    }
    
    public static void copyUrlsToDomain(List<Setting> settingList, List<Domain> domainList) {
    	if(settingList!=null && settingList.size()>0 && domainList!=null && domainList.size()>0){
    		for(Domain d : domainList) {
    			for(Setting s : settingList) {
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
    
}
