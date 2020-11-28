package cn.edm.web.action.datasource;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Tag;
import cn.edm.service.ExportService;
import cn.edm.service.TagService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.file.Downloads;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;

@Controller("DatasourceExport")
@RequestMapping("/datasource/export/**")
public class ExportAction {

	private static final Logger logger = LoggerFactory.getLogger(ExportAction.class);
	
	@Autowired
	private ExportService exportService;
	@Autowired
	private TagService tagService;
	
	@RequestMapping(method = RequestMethod.GET)
	public String recipient(HttpServletRequest request, HttpServletResponse response) {
		try {
			CSRF.auth(request);
			
			String activeCnd = Https.getStr(request, "activeCnd", R.CLEAN, R.REGEX, "regex:^1|-1$");
			activeCnd = Values.get(activeCnd, "1");
			
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			Asserts.isNull(tag, "标签");

			String fileId = "export/" + UserSession.getUserId();
			Integer[] inTagIds = { tagId };
			
			exportService.execute("output", UserSession.getCorpId(), UserSession.getUserId(), PathMap.EXECUTE, fileId, inTagIds, null, activeCnd, "or", null, tagId, null);
			
			String path = PathMap.EXECUTE.getPath() + "export/" + UserSession.getUserId() + "/reduce.txt";
			Downloads.download(response, Webs.rootPath(), path, "导出.csv");
			
			return null;
		} catch (Exception e) {
			logger.error("(Export:recipient) error: ", e);
			return Views._404();
		}
	}
}
