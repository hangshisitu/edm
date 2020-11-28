package cn.edm.util;

import java.util.Comparator;

import cn.edm.vo.QuantityVo;

public class QuantityVoComparator implements Comparator<QuantityVo>{

	@Override
	public int compare(QuantityVo o1, QuantityVo o2) {
		if(o1.getOrder()<o2.getOrder()) {
			return -1;
		}else if(o1.getOrder()>o2.getOrder()) {
			return 1;
		}else {
			return 0;
		}
	} 

	

}
