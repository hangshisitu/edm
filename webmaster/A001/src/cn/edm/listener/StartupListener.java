package cn.edm.listener;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.jsoup.nodes.Entities;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.constant.Config;
import cn.edm.modules.utils.Props;
import cn.edm.util.Mongos;

public class StartupListener implements ServletContextListener {

    private static final Logger logger = LoggerFactory.getLogger(StartupListener.class);

    @Override
    public void contextInitialized(ServletContextEvent event) {
        try {
            String appPath = event.getServletContext().getRealPath("");

            String appFile = appPath + "/WEB-INF/classes/app.properties";
            Props.load(appFile);
            
            String host = Props.getStr(Config.MONGO_HOST);
            Integer port = Props.getInt(Config.MONGO_PORT);
            Integer poolSize = Props.getInt(Config.MONGO_POOL_SIZE);
            Mongos.getInstance().init(host, port, poolSize);
            
            Entities.EscapeMode.base.getMap().clear(); // JSoup hack
            
            logger.info("App domain: {}", Props.getStr(Config.APP_URL));
            logger.info("[ISP start]","开始获取相关isp备案信息...");
            
            logger.info("[ISP finished]","初始化isp备案信息完成.");
        } catch (Exception e) {
            logger.error(e.getMessage(), e);
        }
    }
      
    @Override
    public void contextDestroyed(ServletContextEvent event) {
    }
}
