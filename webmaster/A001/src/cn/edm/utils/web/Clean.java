package cn.edm.utils.web;


public class Clean {

	private static char[] chars = { 
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, // 000 - 015 0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 016 - 031 1
		1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, // 032 - 047 2
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 048 - 063 3
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 064 - 079 4
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, // 080 - 095 5
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 096 - 111 6
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0  // 112 - 127 7
	};
	
	public static boolean clean(String str) {
		boolean clean = true;
		char[] ch = str.toCharArray();
		for (int i = 0; i < ch.length; i++) {
			char c = ch[i];
			if (!index(c) && !extension(c)) {
				clean = false;
			}
		}
		return clean;
	}
	
	public static void main(String[] args) {
        
	    System.out.println((int)')');
    }

	public static boolean index(char c) {
		int idx = c;
		return (idx < 128 && chars[idx] != 0);
	}
	
	public static boolean extension(char c) {
		return c > 127;
	}
}
