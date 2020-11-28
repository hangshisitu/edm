package cn.edm.consts;

import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;

import cn.edm.modules.utils.web.Randoms;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public enum MyCode {
	
	_1(1, "l5ss", "fkjv", "nczy", "kqjd", "wpls", "wwom", "hvma", "si9s", "2lyz", "rwdc", "ssqo", "icqq"),
	_2(2, "fq2u", "geau", "jvzs", "bqbu", "q27l", "cr3y", "71fl", "gth7", "go2g", "sffh", "zlny", "uk4w"),
	_3(3, "nf9v", "zesq", "rjf9", "z9zt", "umvb", "y652", "728k", "sz5d", "9rsp", "abnl", "ujwu", "pwk5"),
	_4(4, "bsnv", "uwfd", "a5tq", "bx0s", "z0kn", "ulud", "kxcr", "xood", "lsch", "706k", "yapf", "tmdn"),
	_5(5, "iv3d", "wyte", "cpli", "nnuh", "v0zu", "gtls", "rutz", "k4oo", "o8az", "mwuk", "sode", "yag0"),
	_6(6, "toy0", "5uuc", "9gse", "vrge", "nfol", "zbd3", "nekp", "gb3b", "fz8q", "f0ft", "mchj", "eygy"),
	_7(7, "a3i2", "x58f", "itlc", "u7vt", "xr7t", "a43p", "lgiu", "muir", "fhui", "vhks", "pdzr", "bb8z"),
	_8(8, "fmer", "olqi", "tkq0", "elgp", "q7ho", "tw3y", "mhkw", "eami", "dmt6", "ahch", "wnqx", "lwix"),
	_9(9, "eo3t", "f341", "wfpa", "tq1q", "0ps8", "gsnd", "rxjx", "m94l", "ryuy", "dua8", "madf", "hzpc"),
	_0(0, "bz6p", "jmnh", "kuul", "3qyq", "0ssi", "y8wo", "7pnb", "n6la", "r4p8", "khip", "4rgm", "nhh2");

	private final Integer id;
	private final String[] codes;

	private MyCode(Integer id, String... codes) {
		this.id = id;
		this.codes = codes;
	}

	public Integer getId() {
		return id;
	}

	public String[] getCodes() {
		return codes;
	}
	
	public static String get(String content) {
		if (StringUtils.isBlank(content) || content.length() < 4) {
			return null;
		}

		int start = 0;
		int end = 4;
		StringBuffer sbff = new StringBuffer();
		for (int i = 0; i < content.length(); i++) {
			String code = StringUtils.substring(content, start, end);
			boolean next = false;
			for (MyCode myCode : MyCode.values()) {
				for (String value : myCode.getCodes()) {
					if (code.equals(value)) {
						sbff.append(myCode.getId());
						next = true;
					}
				}
			}
			if (!next) {
				break;
			}
			
			start = start + 4;
			end = end + 4;
		}

		return sbff.toString();
	}

	public static String generate(String content, int prev, int suff) {
		if (StringUtils.isBlank(content)) {
			content = Value.S;
		}
		content = StringUtils.trim(content);
		
		int len = content.length();
		Map<Integer, List<String>> codeMap = Maps.newHashMap();
		for (int i = 0; i < len; i++) {
			Integer id = Integer.valueOf(String.valueOf(content.charAt(i)));
			if (codeMap.containsKey(id)) {
				continue;
			}
			codeMap.put(id, codes(id));
		}

		StringBuffer sbff = new StringBuffer();
		
		// prev
		if (prev > 0) {
			for (int i = 0; i < prev; i++) {
				sbff.append(unique());
			}
		}
		
		for (int i = 0; i < len; i++) {
			Integer id = Integer.valueOf(String.valueOf(content.charAt(i)));
			List<String> codes = codeMap.get(id);
			for (String code : codes) {
				if (StringUtils.contains(sbff.toString(), code)) {
					continue;
				}
				sbff.append(code);
				break;
			}
		}
		
		// suff
		int last = suff - len;
		
		if (last > 0) {
			for (int i = 0; i < last; i++) {
				sbff.append(unique());
			}
		}

		return sbff.toString();
	}
	
	private static final List<String> codes(Integer id) {
		List<String> codes = null;
		for (MyCode value : MyCode.values()) {
			if (value.getId().equals(id)) {
				codes = Lists.newArrayList();
				for (String code : value.getCodes()) {
					codes.add(code);
				}
				break;
			}
		}
		Collections.shuffle(codes);

		return codes;
	}

	private static String unique() {
		String rnd = StringUtils.lowerCase(Randoms.getRandomString(4));
		for (MyCode value : MyCode.values()) {
			for (String code : value.getCodes()) {
				if (code.equals(rnd)) {
					return unique();
				}
			}
		}

		return rnd;
	}
	
	public static void main(String[] args) {
		System.out.println(generate("1234567890", 8, 24));
		System.out.println(get("l5ssgo2gabnllschv0zu5uucxr7ttw3ymadf3qyq"));
	}
}
