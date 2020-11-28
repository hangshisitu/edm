package cn.edm.app.logger;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ActiveLogger {

    private static final Logger logger = LoggerFactory.getLogger(ActiveLogger.class);

    public static void ok(String tagId, String email, DateTime now) {
        logger.info("(Active:ok) " + tagId + "\t" + email + "\t" + now.getMillis());
    }
}
