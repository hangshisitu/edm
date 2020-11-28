package cn.edm.util;

import java.util.Comparator;

import cn.edm.vo.PropertyVo;

public class PropertyStatComparator implements Comparator<PropertyVo>{

	@Override
	public int compare(PropertyVo pv1, PropertyVo pv2) {
		if(pv1.getNum()>pv2.getNum()) {
			return -1;
		}else if(pv1.getNum()<pv2.getNum()) {
			return 1;
		}else {
			return 0;
		}
	}


}
