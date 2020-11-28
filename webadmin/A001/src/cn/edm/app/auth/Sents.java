package cn.edm.app.auth;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.MyCode;
import cn.edm.consts.Status;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Webs;

public class Sents {

	private static final Object SENT = new Object();
	
	public static void plus(Integer status, int holdCount, int emailCount) {
		Integer[] STATUS = { 
				Status.PROOFING, Status.REVIEWING, Status.TEST, Status.WAITED,
				Status.QUEUEING, Status.PROCESSING, Status.PAUSED, Status.CANCELLED, 
				Status.COMPLETED
		};
		
		if (!Auth.isAuth() || !Asserts.hasAny(Values.get(status, Status.DRAFTED), STATUS)) {
			return;
		}

		synchronized (SENT) {
			Long sent = get() - holdCount + emailCount;
			if (sent > Auth.SENDS) {
				throw new Errors("已发送量不能大于" + Auth.SENDS);
			}

			set(sent);
		}
	}
	
	public static void minus(int emailCount) {
		if (!Auth.isAuth()) {
			return;
		}
		
		synchronized (SENT) {
			Long sent = get() - emailCount;
			if (sent < 0) {
				throw new Errors("已发送量不能小于0");
			}

			set(sent);
		}
	}
	
	private static Long get() {
		BufferedReader reader = null;
		try {
			reader = new BufferedReader(new InputStreamReader(new FileInputStream(SENT()), "UTF-8"));
			String text = reader.readLine();
			if (StringUtils.isBlank(text) || text.length() != 128) {
				throw new Errors("已发送量不是合法值");
			}
			String myCode = MyCode.get(StringUtils.substring(text, 32, 96));
			Long sent = Long.valueOf((String) Validator.validate(myCode, R.CLEAN, R.REQUIRED, R.LONG, "SENT"));
			return sent;
		} catch (Errors e) {
			throw new Errors(e.getMessage(), e);
		} catch (Exception e) {
			throw new Errors("获取已发送量失败", e);
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
	}

	private static void set(Long sent) {
		BufferedWriter writer = null;
		try {
			writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(SENT()), "UTF-8"));
			String myCode = (String) Validator.validate(String.valueOf(sent), R.CLEAN, R.REQUIRED, R.LONG, "SENT");
			writer.write(MyCode.generate(myCode, 8, 24));
			writer.flush();
		} catch (Exception e) {
			throw new Errors("设置已发送量失败", e);
		} finally {
			try {
				if (writer != null) {
					writer.close();
				}
			} catch (IOException e) {
			}
		}
	}

	private static String SENT() {
		return Webs.rootPath() + "/WEB-INF/classes/cn/edm/app/auth/SENT";
	}
}
