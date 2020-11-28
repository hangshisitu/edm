package cn.edm.utils.helper;

import org.apache.commons.lang.StringUtils;

import cn.edm.consts.Value;

public class Onlines {

	public static final String filter(String content, String EMAIL) {
		if (StringUtils.isBlank(content)) {
			return "";
		}
		
		String mailbox = StringUtils.isNotBlank(EMAIL) ? EMAIL : Value.S;
		
		content = StringUtils.replace(content, "$iedm_to$", mailbox);
		content = StringUtils.replace(content, "$iedm_to2$", mailbox);
		content = StringUtils.replace(content, "$MAIL_BOX$", mailbox);
		
		return content;
	}
}
