package cn.edm.util;

import java.util.HashMap;

import cn.edm.vo.DeliveryLogVo;

	@SuppressWarnings("rawtypes")
	public class LogHashMap extends HashMap{
		private static final long serialVersionUID = -8651511832767249036L;
		
		@SuppressWarnings("unchecked")
		@Override
		public Object put(Object key, Object log) {
			 DeliveryLogVo vo = (DeliveryLogVo) log;
			 if(!this.containsKey(key)) 
				 return super.put(key, vo);   
			 DeliveryLogVo old = (DeliveryLogVo)super.get(key);
			 old.getLogList().addAll(vo.getLogList());
	         return super.put(key,old);   
		}
    }


