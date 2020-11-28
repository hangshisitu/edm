package cn.edm.util;

import java.util.Comparator;

import cn.edm.vo.DomainStat;

public class DomainStatComparator implements Comparator<DomainStat>{

	@Override
	public int compare(DomainStat o1, DomainStat o2) {
		if(o1.getShare()>o2.getShare()) {
			return -1;
		}else if(o1.getShare()<o2.getShare()) {
			return 1;
		}else {
			return 0;
		}
	} 

	

}
