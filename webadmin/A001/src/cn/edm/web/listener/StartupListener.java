package cn.edm.web.listener;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

import org.jsoup.nodes.Entities;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.Config;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Mongos;

public class StartupListener implements ServletContextListener {

	private static final Logger logger = LoggerFactory.getLogger(StartupListener.class);

	@Override
	public void contextInitialized(ServletContextEvent event) {
		try {
			String appPath = event.getServletContext().getRealPath("");

			String appFile = appPath + "/WEB-INF/classes/app.properties";
			String i18nFile = appPath + "/WEB-INF/classes/i18n.properties";
			Property.load(appFile, i18nFile);
			
			String host = Property.getStr(Config.MONGO_HOST);
			Integer port = Property.getInt(Config.MONGO_PORT);
			Integer poolSize = Property.getInt(Config.MONGO_POOL_SIZE);
			Mongos.getInstance().init(host, port, poolSize);
			
			Entities.EscapeMode.base.getMap().clear(); // JSoup hack
			
			logger.info("App domain: {}", Property.getStr(Config.APP_URL));
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		}
	}

	@Override
	public void contextDestroyed(ServletContextEvent event) {
	}
}
