package cn.edm.util;

import java.util.Comparator;

import cn.edm.domain.Url;

public class UrlClickComparator implements Comparator<Url>{

	@Override
	public int compare(Url o1, Url o2) {
		if(o1.getClickCount()>o2.getClickCount()) {
			return -1;
		}else if(o1.getClickCount()<o2.getClickCount()) {
			return 1;
		}else {
			return 0;
		}
	} 

	

}
