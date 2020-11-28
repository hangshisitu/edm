package cn.edm.web.facade;

import java.util.List;

import org.apache.commons.lang.StringUtils;

import cn.edm.model.Region;

import com.google.common.collect.Lists;

public class Regions {

    /**
     * 合并.
     */
    public static List<Region> combine(List<Region> provinceList, List<Region> cityList, String blank) {
        List<Region> regionList = Lists.newArrayList();
        for (Region province : provinceList) {
            province.setRegionName(province.getProvinceName());
            regionList.add(province);
            Region other = null;
            boolean hasCity = false;
            for (Region city : cityList) {
                if (city.getProvinceName().equals(province.getProvinceName())) {
                    if (StringUtils.isNotBlank(city.getCityName())) {
                        city.setRegionName(blank + city.getCityName());
                        regionList.add(city);
                        hasCity = true;
                    } else {
                        city.setRegionName(blank + "其他");
                        other = city;
                    }
                }
            }
            if (hasCity && other != null) {
                regionList.add(other);
            }
        }
        
        return regionList;
    }
}
