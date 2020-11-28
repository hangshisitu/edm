package cn.edm.web.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.edm.app.logger.ActiveLogger;
import cn.edm.consts.HttpCode;
import cn.edm.consts.Status;
import cn.edm.model.Setting;
import cn.edm.modules.utils.Encodes;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.service.SettingService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Validator;
import cn.edm.web.facade.Tasks;

public class ActiveServlet extends HttpServlet {

	private static final long serialVersionUID = -7571871724391389446L;

	private static final Logger logger = LoggerFactory.getLogger(ActiveServlet.class);
	
	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		String id = null;
		String email = null;
		
		try {
			id = Https.getStr(request, "id", R.CLEAN, R.REQUIRED, R.LENGTH, "{5,65}", "任务");
			email = Https.getStr(request, "e", R.REQUIRED, "邮箱");

			email = new String(Encodes.decodeBase64(email));
			email = (String) Validator.validate(email, R.CLEAN, R.REQUIRED, R.MAIL, "{1,64}", "邮箱");
			Integer[] sid = Tasks.id(id);
			Asserts.notEquals(sid, 3, "[企业ID.任务ID.模板ID]");
			Asserts.isNull(email, "邮箱");
			
			SettingService settingService = CtxHolder.getBean(SettingService.class);
			Integer[] STATUS = { Status.PROCESSING, Status.PAUSED, Status.CANCELLED, Status.COMPLETED };
			Setting setting = settingService.get(sid[0], sid[1], sid[2]);
			Asserts.isNull(setting, "任务设置");
            Asserts.notHasAny(setting.getStatus(), STATUS, "非投递状态的任务");
			
			String[] tagIds = Converts._toStrings(setting.getTagIds());
			Asserts.isEmpty(tagIds, "标签");
			
			DateTime now = new DateTime();
			
			for (String tagId : tagIds) {
				ActiveLogger.ok(tagId, email, now);
			}
		} catch (Errors e) {
			logger.warn("(Active) id: " + id + ", e: " + email + ", warn: " + e.getMessage());
		} catch (Exception e) {
			logger.error("(Active) error: ", e);
		} finally {
			ObjectMappers.renderText(response, HttpCode._200);
		}
	}

	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}
}
