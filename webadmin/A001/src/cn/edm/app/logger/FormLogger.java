package cn.edm.app.logger;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class FormLogger {
    
    private static final Logger logger = LoggerFactory.getLogger(FormLogger.class);

    public static void ok(String email, String parameters) {
        logger.info("(Form:ok) " + email + "\t" + parameters);
    }
}
