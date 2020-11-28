package test.edm.utils;

import org.apache.commons.lang.StringUtils;
import org.junit.Test;
import org.xbill.DNS.ARecord;
import org.xbill.DNS.Lookup;
import org.xbill.DNS.MXRecord;
import org.xbill.DNS.Record;
import org.xbill.DNS.TXTRecord;
import org.xbill.DNS.Type;

import cn.edm.utils.except.Errors;

public class DNS {

	public void mx() {
		try {
			Record[] records = new Lookup("gmail.com", Type.MX).run();
			isEmpty(records, "发件人邮箱地址无效");
			for (int i = 0; i < records.length; i++) {
				MXRecord mx = (MXRecord) records[i];
				System.out.println("mx[" + i + "]: " + mx.getTarget() + " " + mx.getPriority());
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void a() {
		try {
			Record[] records = new Lookup("gmail.com", Type.A).run();
			isEmpty(records, "发件人邮箱地址无效");
			for (int i = 0; i < records.length; i++) {
				ARecord a = (ARecord) records[i];
				System.out.println("a[" + i + "]: " + a.getAddress().getHostAddress());
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Test
	public void txt() {
		txt("yeah.net");
	}

	private void txt(String name) {
		try {
			Record[] records = new Lookup(name, Type.TXT).run();
			for (int i = 0; i < records.length; i++) {
				TXTRecord txt = (TXTRecord) records[i];
				System.out.println("txt[" + i + "]: " + txt.getStrings().toString());
				for (String str : StringUtils.split(txt.getStrings().toString(), " ")) {
					if (StringUtils.startsWith((String) str, "include:") || StringUtils.startsWith((String) str, "redirect:")) {
						txt(StringUtils.substringAfter(str, ":"));
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void isEmpty(Record[] records, String message) {
		if (records == null || records.length == 0) {
			throw new Errors(message);
		}
	}
}
