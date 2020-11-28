package cn.edm.util;

import java.util.Comparator;
import java.util.Date;

import org.joda.time.DateTime;

import cn.edm.domain.Result;

public class ResultSortComparator implements Comparator<Result>{
	
	private String flag;
	
	public ResultSortComparator(String flag){
		this.flag = flag;
	}

	@Override
	public int compare(Result o1, Result o2) {
		
		if("deliveryTime".equals(flag)) {
			Date d1 = o1.getDeliveryTime();
			Date d2 = o2.getDeliveryTime();
			DateTime dt1 = new DateTime(d1);
			DateTime dt2 = new DateTime(d2);
			if(dt1.isAfter(dt2)) {
				return -1;
			}else if(dt1.isBefore(dt2)) {
				return 1;
			}else {
				return 0;
			}
		}else if("sentCount".equals(flag)){
			
			if(o1.getSentCount()>o2.getSentCount()) {
				return -1;
			}else if(o1.getSentCount()<o2.getSentCount()) {
				return 1;
			}else {
				return 0;
			}
		}else if("reachCount".equals(flag)) {
			double f1 = o1.getSuccessRate();
			double f2 = o2.getSuccessRate();
			if(f1>f2) {
				return -1;
			}else if(f1<f2) {
				return 1;
			}else {
				return 0;
			}
		}else if("readUserCount".equals(flag)){
			double f1 = o1.getOpenRate();
			double f2 = o2.getOpenRate();
			if(f1>f2) {
				return -1;
			}else if(f1<f2) {
				return 1;
			}else {
				return 0;
			}
		}else {
			double f1 = o1.getClickRate();
			double f2 = o2.getClickRate();
			if(f1>f2) {
				return -1;
			}else if(f1<f2) {
				return 1;
			}else {
				return 0;
			}
		}
		
		
	} 

	

}
