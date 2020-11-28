package cn.edm.utils.execute;

import cn.edm.consts.Config;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Values;

public class Hashs {

	public static int BKDR(String str) {
		int seed = 31;
		int hash = 0;
		int i = 0;
		while (i < str.length()) {
			hash = hash * seed + str.charAt(i);
			i++;
		}

		return hash & 0x7FFFFFFF;
	}
	
	public static int splitCount(int inCount, int exCount) {
		double repeatSize = Values.get(Property.getInt(Config.REPEAT_SIZE), 1);
		int count = (int) Math.round((double) (inCount + exCount) / repeatSize);
		return count > 0 ? count : 1;
	}
}
