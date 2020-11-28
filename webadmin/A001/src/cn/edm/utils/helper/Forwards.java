package cn.edm.utils.helper;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.joda.time.Seconds;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.consts.Config;
import cn.edm.consts.Id;
import cn.edm.consts.Status;
import cn.edm.consts.Sync;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Resource;
import cn.edm.model.Robot;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.Property;
import cn.edm.utils.Asserts;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Resources;
import cn.edm.web.facade.Robots;
import cn.edm.web.facade.Templates;

import com.google.common.collect.Maps;

public class Forwards {

	private static Logger logger = LoggerFactory.getLogger(Forwards.class);
	
	public static String write(Task task, Setting setting, Template template, Map<String, Integer> touchMap, List<Robot> robots, Resource resource, Map<Integer, String> propMap, String[] emailList) {
		DateTime now = new DateTime();
		
		String path = PathMap.TASK.getPath() + now.toString("yyyyMM") + "/";
		Files.make(Webs.rootPath() + path);

		File current = new File(Webs.rootPath() + path);
		String count = count(now, current);
		String SID = Id.CORP + "." + count + "." + task.getTemplateId() + ".txt";
		
		PrintWriter writer = null;
		try {
			DateTime start = new DateTime();

			MapBean attrs = new MapBean();
			Templates.attrs(template, attrs);
	        boolean base64 = attrs.get("personal").equals("false");
			String label_ids = (String) Validator.validate(setting.getLabelIds(), R.CLEAN, R.REGEX, "regex:^[0-9,]+$:");
			
			int rnd = Resources.rnd(resource.getSenderList(), resource.getHeloList(), resource.getSkipList());
			String helo = Resources.get(resource.getHeloList(), rnd);
			String skip = Resources.get(resource.getSkipList(), rnd);
			
			String SENDER = null;
			String ROBOT = null;
			String RAND = null;

			String robot = StringUtils.replace(Robots.get(robots), ",", ";");
			if (StringUtils.isBlank(robot) || Asserts.hasAny(task.getStatus(), new Integer[] { Status.TEST }))
				ROBOT = StringUtils.replace(resource.getSenderList(), ",", ";");
			else {
				if (StringUtils.isNotBlank(setting.getRobot()))
					ROBOT = StringUtils.replace(setting.getRobot(), ",", ";");
				else
					ROBOT = robot;
			}
			if (StringUtils.isNotBlank(task.getSenderEmail()))
				SENDER = task.getSenderEmail();
			else
				SENDER = ROBOT;
			RAND = setting.getRand();
			
			writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(Webs.rootPath() + path + SID), "UTF-8")));
			Delivery.ID(writer, String.valueOf(Id.CORP), count, String.valueOf(task.getTemplateId()));
			Delivery.TIME(writer, task.getJobTime());
			Delivery.AD(writer, setting.getAd());
			Delivery.SMS(writer, setting.getSms());
			Delivery.SUBJECT(writer, task.getSubject());
			Delivery.NICKNAME(writer, task.getSenderName());
			Delivery.SENDER(writer, SENDER);
			
			Delivery.PIPE(writer, resource.getResourceId());
			Delivery.HELO(writer, helo);
			Delivery.ROBOT(writer, ROBOT);
			Delivery.RAND(writer, RAND);
			
			Delivery.MAILLIST_BEGIN(writer, propMap);
			Map<String, String> parameterMap = Maps.newHashMap();
			int emailCount = 0;
			for (String email : emailList) {
				Delivery.CONTENT(writer, task.getReceiver(), propMap, parameterMap, email, Value.S);
				emailCount++;
			}
            String test = "1";
            if (emailCount > Property.getInt(Config.PIPE_SIZE)) {
                test = "0";
            }
			Delivery.MAILLIST_END(writer);
			Delivery.CHART(writer, Charts.cost(Webs.rootPath(), template.getFilePath()));
            Delivery.MODULA(writer, Modulars.filterPath(task.getCorpId(), task.getTaskId(), task.getTemplateId(), 
                    skip, template.getFilePath(), base64, 
                    touchMap,
                    setting.getUnsubscribeId(), label_ids, test));
            
			writer.flush();
			
			DateTime end = new DateTime();
			long seconds = Seconds.secondsBetween(start, end).getSeconds();
			
			logger.info("(Forwards:write) SID: " + SID + ", seconds: " + seconds + ", counter: " + emailList.length);
			
			return path + SID;
		} catch (Exception e) {
			throw new ServiceErrors("(Forwards:write) error:", e);
		} finally {
			if (writer != null) {
				writer.close();
			}
		}
	}

	private static String count(DateTime now, File path) {
		synchronized (Sync.FORWARD) {
			int max = 0;
			if (path.listFiles() != null) {
				for (File file : path.listFiles()) {
					String fileName = file.getName();
					if (StringUtils.startsWith(fileName, Id.CORP + ".")) {
						fileName = StringUtils.removeStart(fileName, Id.CORP + ".99" + now.toString("yyyyMM"));
						int i = Integer.valueOf(StringUtils.substringBefore(fileName, "."));
						max = Math.max(i, max);
					}
				}
			}

			return "99" + now.toString("yyyyMM") + (max + 1);
		}
	}
}
