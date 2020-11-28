package cn.edm.utils.helper;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Date;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.utils.Values;

public class Delivery {
    
    private static final Logger logger = LoggerFactory.getLogger(Delivery.class);

	public static void ID(PrintWriter writer, String corpId, String taskId, String templateId) {
		writer.println("<$ID>");
		writer.println(corpId + "." + taskId + "." + templateId);
		writer.println("</$ID>");
	}
	
	public static void TIME(PrintWriter writer, Date jobTime) {
		writer.println("<$TIME>");
		writer.println(jobTime != null ? new DateTime(jobTime).toString("yyyy-MM-dd HH:mm:ss") : "");
		writer.println("</$TIME>");
	}

	public static void AD(PrintWriter writer, String ad) {
		writer.println("<$AD>");
		writer.println(Values.get(ad, "false"));
		writer.println("</$AD>");
	}

	public static void SMS(PrintWriter writer, String sms) {
		writer.println("<$SMS>");
		writer.println(Values.get(sms, "0"));
		writer.println("</$SMS>");
	}

	public static void SUBJECT(PrintWriter writer, String subject) {
		writer.println("<$SUBJECT>");
		writer.println(Values.get(subject));
		writer.println("</$SUBJECT>");
	}

	public static void SENDER(PrintWriter writer, String senderEmail) {
		writer.println("<$SENDER>");
		writer.println(Values.get(senderEmail));
		writer.println("</$SENDER>");
	}

	public static void NICKNAME(PrintWriter writer, String senderName) {
		writer.println("<$NICKNAME>");
		writer.println(Values.get(senderName));
		writer.println("</$NICKNAME>");
	}
	
	public static void REPLIER(PrintWriter writer, String replier) {
		writer.println("<$REPLIER>");
		writer.println(Values.get(replier));
		writer.println("</$REPLIER>");
	}

	public static void HELO(PrintWriter writer, String helo) {
		writer.println("<$HELO>");
		writer.println(Values.get(helo));
		writer.println("</$HELO>");
	}

	public static void ROBOT(PrintWriter writer, String robotEmail) {
		writer.println("<$ROBOT>");
		writer.println(Values.get(robotEmail));
		writer.println("</$ROBOT>");
	}

	public static void RAND(PrintWriter writer, String rand) {
		writer.println("<$RAND>");
		writer.println(StringUtils.isNotBlank(rand) && StringUtils.equals(rand, "true") ? "true" : "false");
		writer.println("</$RAND>");
	}

	public static void PIPE(PrintWriter writer, String pipe) {
		writer.println("<$PIPE>");
		writer.println(Values.get(pipe));
		writer.println("</$PIPE>");
	}

	public static void CHART(PrintWriter writer, String chart) {
		writer.println("<$CHART>");
		writer.println(Values.get(chart));
		writer.println("</$CHART>");
	}

    public static void MODULA_TEXT(PrintWriter writer, String content) {
        writer.println("<$MODULA_TEXT>");
        writer.println(Values.get(content));
        writer.println("</$MODULA_TEXT>");
    }

    public static void MODULA_QVGA(PrintWriter writer, String content) {
        writer.println("<$MODULA_QVGA>");
        writer.println(Values.get(content));
        writer.println("</$MODULA_QVGA>");
    }

    public static void MODULA(PrintWriter writer, String content) {
        writer.println("<$MODULA>");
        writer.println(Values.get(content));
        writer.println("</$MODULA>");
    }

	public static void MAILLIST_BEGIN(PrintWriter writer, Map<Integer, String> propMap) {
		writer.println("<$MAILLIST>");
		StringBuilder sbff = new StringBuilder();
		sbff.append("MAIL_BOX,")
			.append("RECEIVER,")
			.append("EMAIL,")
			.append("EMAIL_PRE");
		
		for (int i = 0; i < propMap.size(); i++) {
			sbff.append(",").append(propMap.get(i));
		}

		writer.println(sbff.toString());
	}

	public static void CONTENT(PrintWriter writer, String receiver, Map<Integer, String> propMap, Map<String, String> parameterMap, String email, String props) {
		StringBuilder sbff = new StringBuilder();
		String mailbox = Values.get(email);
		String mailbox_pre = StringUtils.substringBefore(mailbox, "@");
		
		for (String prop : StringUtils.splitPreserveAllTokens(props, ",")) {
			String k = StringUtils.substringBefore(prop, "=");
			String v = StringUtils.substringAfter(prop, "=");
			if (StringUtils.isBlank(k)) {
				continue;
			}
			parameterMap.put(k, v);
		}
		
		// 默认邮箱地址
		parameterMap.put(PropMap.EMAIL.getId(), mailbox);

		String mailbox_receiver = Value.S;
		
		if (StringUtils.isNotBlank(receiver)) {
			if (receiver.equals("email")) {
				mailbox_receiver = mailbox;
			} else if (receiver.equals("email_pre")) {
				mailbox_receiver = mailbox_pre;
			} else if (receiver.equals("true_name")) {
				mailbox_receiver = Values.get(parameterMap.get(PropMap.TRUE_NAME.getId()));
			} else if (receiver.equals("nick_name")) {
				mailbox_receiver = Values.get(parameterMap.get(PropMap.NICK_NAME.getId()));
			}
		}
		
		sbff.append(mailbox).append(",")
			.append(mailbox_receiver).append(",")
			.append(mailbox).append(",")
			.append(mailbox_pre);
		
		for (int i = 0; i < propMap.size(); i++) {
			String prop = parameterMap.get(propMap.get(i));
			sbff.append(",").append(Values.get(prop));
		}

		writer.println(sbff.toString());
	}

	public static void MAILLIST_END(PrintWriter writer) {
		writer.println("</$MAILLIST>");
	}
	
    public static void node(PrintWriter writer, String tag, String content) {
        writer.println("<$" + tag + ">");
        writer.println(Values.get(content));
        writer.println("</$" + tag + ">");
    }
    
    public static String node(String path, String tag) {
        String content = null;
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));
            boolean isTag = false;
            boolean hasTag = false;
            String line = null;
            
            StringBuffer sbff = new StringBuffer();
            
            while ((line = reader.readLine()) != null) {
                if (StringUtils.equals(StringUtils.trim(line), "<$" + tag + ">")) {
                    isTag = true;
                    hasTag = true;
                    continue;
                }
                if (StringUtils.equals(StringUtils.trim(line), "</$" + tag + ">")) {
                    isTag = false;
                    break;
                }
                if (isTag) {
                    sbff.append(line);
                }
            }
            
            if (hasTag) {
                content = StringUtils.trim(sbff.toString());
            }
        } catch (Exception e) {
            logger.error("(Delivery:line) error: ", e);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                }
            }
        }
        return content;
    }
}
