package cn.edm.web.action.datasource;

import java.util.Date;
import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Suffix;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.File;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.service.FileService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Downloads;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Lines;
import cn.edm.utils.file.Readers;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;

@Controller
@RequestMapping("/datasource/file/**")
public class FileAction {

	private static final Logger logger = LoggerFactory.getLogger(FileAction.class);
	
	@Autowired
	private FileService fileService;

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<File> page = new Page<File>();
			Pages.page(request, page);

			String fileName = Https.getStr(request, "fileName", R.CLEAN, R.LENGTH, "{1,20}");
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			
			MapBean mb = new MapBean();
			page = fileService.search(page, mb, UserSession.getCorpId(), UserSession.getUserId(), fileName, beginTime, endTime);

			map.put("page", page);
			map.put("mb", mb);

			return "default/datasource/file_page";
		} catch (Exception e) {
			logger.error("(File:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/datasource/file/preview", token);
			
			String path = "default/datasource/file_add";
			
			Integer fileId = Https.getInt(request, "fileId", R.CLEAN, R.INTEGER);

			File file = null;
			if (fileId != null) {
				file = fileService.get(UserSession.getCorpId(), UserSession.getUserId(), fileId);
			}
			if (file == null) {
				file = new File();
				path = "default/datasource/file_upload";
			}

			map.put("file", file);
			
			return path;
		} catch (Exception e) {
			logger.error("(File:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void preview(HttpServletRequest request, HttpServletResponse response, 
			@RequestParam(required = false, value = "file") MultipartFile upload) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Files.valid(upload, new MapBean("file", Suffix.FILES));
			Uploads.maxSize(upload);
			
			String[] GB2312 = Readers.lines(upload, "GB2312", 2);
			String[] UTF8 = Readers.lines(upload, "UTF-8", 2);
			
			mb.put("GB2312", GB2312);
			mb.put("UTF8", UTF8);
			
			Views.ok(mb, "预览文件成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "预览文件失败");
			logger.error("(File:preview) error: ", e);
		}

		ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response, 
			@RequestParam(required = false, value = "file") MultipartFile upload) {
		MapBean mb = new MapBean();
		Integer fileId = null;
		String filePath = null;
		try {
			CSRF.validate(request);
			
			fileId = Https.getInt(request, "fileId", R.CLEAN, R.INTEGER);
			
			String charset = null;
			String fileName = null;
			String suff = null;
			
			String orgFileName = Https.getStr(request, "orgFileName", R.CLEAN, R.LENGTH, "{1,20}", "文件名称");
			String fileDesc = Https.getStr(request, "fileDesc", R.CLEAN, R.LENGTH, "{1,60}", "文件描述");
			
			File file = null;
			if (fileId != null) {
				fileName = Https.getStr(request, "fileName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "文件名称");
				file = fileService.get(UserSession.getCorpId(), UserSession.getUserId(), fileId);
				Asserts.isNull(file, "文件");
				suff = file.getSuff();
			} else {
				charset = Https.getStr(request, "charset", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^GB2312|UTF-8$:只能为GB2312或UTF-8", "编码");
				fileName = Files.prefix(upload.getOriginalFilename());
				suff = Files.suffix(upload.getOriginalFilename());
			}
			
			Asserts.notUnique(fileService.unique(UserSession.getCorpId(), UserSession.getUserId(), fileName, orgFileName, suff), "文件");

			if (fileId == null) {
				file = new File();
				
				Files.valid(upload, new MapBean("file", Suffix.FILES));
				Uploads.maxSize(upload);
				
				file.setCorpId(UserSession.getCorpId());
				file.setUserId(UserSession.getUserId());
				file.setFilePath(Uploads.upload(UserSession.getCorpId(), charset, upload, Webs.rootPath(), PathMap.FILE.getPath()));
				file.setFileSize((int) upload.getSize());
				file.setSuff(suff);
				file.setEmailCount(Lines.count(upload, "UTF-8"));
				file.setStatus(Value.F);
				
				filePath = file.getFilePath();
			}

			file.setFileName(fileName);
			file.setFileDesc(fileDesc);
			
			fileService.save(file);

			mb.put("fileId", file.getFileId());
			mb.put("fileName", file.getFileName());
			mb.put("fileDesc", file.getFileDesc());
			mb.put("size", file.getSize());
			mb.put("suff", file.getSuff());
			mb.put("status", file.getStatus());
			Date now = new Date();
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
            
			Views.ok(mb, (fileId != null ? "修改" : "上传") + "文件成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (fileId != null ? "修改" : "上传") + "文件失败");
			if (fileId == null) {
				Files.delete(Webs.rootPath(), filePath, PathMap.FILE.getPath());
			}
			logger.error("(File:add) error: ", e);
		}

		if (fileId != null) {
			ObjectMappers.renderJson(response, mb);
		} else {
			ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
		}
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			int[] fileIds = Converts._toInts(checkedIds);

			if (fileIds != null && fileIds.length == 1) {
				File file = fileService.get(UserSession.getCorpId(), UserSession.getUserId(), fileIds[0]);
				if (file != null) {
					map.put("checkedName", file.getFileName());
				}
			}
			
			Views.map(map, "del", "", "删除", "所选文件", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(File:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
			CSRF.validate(request);
			
			checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			Integer[] fileIds = Converts._toIntegers(checkedIds);
			
			if (!Asserts.empty(fileIds)) {
				List<File> files = fileService.find(UserSession.getCorpId(), UserSession.getUserId(), fileIds);
				fileService.delete(UserSession.getCorpId(), UserSession.getUserId(), fileIds);
				
				StringBuffer sbff = new StringBuffer();
				for (File file : files) {
					sbff.append(file.getFileId()).append(",");
					Files.delete(Webs.rootPath(), file.getFilePath(), PathMap.FILE.getPath());
				}
				
				checkedIds = sbff.toString();
			}
			
			Views.ok(mb, "删除文件成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除文件失败");
			logger.error("(File:del) error: ", e);
		}
		
		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String download(HttpServletRequest request, HttpServletResponse response) {
		try {
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			List<Integer> fileIds = Converts.toIntegers(checkedIds);
			
			Asserts.isEmpty(fileIds, "文件");
			
			if (fileIds.size() == 1) {
				File file = fileService.get(UserSession.getCorpId(), UserSession.getUserId(), fileIds.get(Value.I));
				Asserts.isNull(file, "文件");
				
				Asserts.notExists(Webs.rootPath(), file.getFilePath(), Value.S);
				
				String name = file.getFileName() + "." + file.getSuff();
				Downloads.download(response, Webs.rootPath(), file.getFilePath(), name);
			}
			
			return null;
		} catch (Exception e) {
			logger.error("(File:download) error: ", e);
			return Views._404();
		}
	}
}

