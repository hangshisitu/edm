package cn.edm.util;

import java.util.HashMap;

	@SuppressWarnings("rawtypes")
	public class PropMap extends HashMap{
		private static final long serialVersionUID = -8651511832767249036L;
		
		@SuppressWarnings("unchecked")
		@Override
		public Object put(Object key, Object value) {
			 if(!this.containsKey(key))  {
				 return super.put(key, value);   
			 }else {
				 Integer num =  (Integer) super.get(key);
				 num = num +(Integer)value;
				 return super.put(key,num);  
			 }
				
	         
		}
    }


