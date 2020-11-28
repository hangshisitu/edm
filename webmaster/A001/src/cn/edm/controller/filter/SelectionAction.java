package cn.edm.controller.filter;

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

import cn.edm.constants.Id;
import cn.edm.constants.Options;
import cn.edm.constants.Suffix;
import cn.edm.constants.mapper.CategoryMap;
import cn.edm.constants.mapper.PathMap;
import cn.edm.constants.mapper.PropMap;
import cn.edm.domain.Prop;
import cn.edm.domain.Tag;
import cn.edm.exception.Errors;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.service.CategoryService;
import cn.edm.service.EmailService;
import cn.edm.service.ImportService;
import cn.edm.service.PropService;
import cn.edm.service.TagService;
import cn.edm.util.Values;
import cn.edm.utils.Asserts;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.facade.Recipients;


@Controller
@RequestMapping("/filter/selection")
public class SelectionAction {

	private static final Logger logger = LoggerFactory.getLogger(SelectionAction.class);

    @Autowired
    private CategoryService categoryService;
    @Autowired
    private Ehcache ehcache;
    @Autowired
    private EmailService emailService;
    @Autowired
    private ImportService importService;
    @Autowired
    private PropService propService;
    @Autowired
    private TagService tagService;

	@RequestMapping(value = "import", method = RequestMethod.GET)
    public String imports(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            CSRF.generate(request);
            
            String type = Https.getStr(request, "type", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            type = Values.get(type, "red");
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(type.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            map.put("type", type);
            
            return "filter/selection_import";
        } catch (Exception e) {
            logger.error("(Selection:imports) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "import", method = RequestMethod.POST)
    public void imports(HttpServletRequest request, HttpServletResponse response,
            @RequestParam(required = false, value = "file") MultipartFile upload) {
        MapBean mb = new MapBean();
        try {
            CSRF.auth(request);
            
            String type = Https.getStr(request, "type", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            type = Values.get(type, "red");
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(type.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            String charset = Https.getStr(request, "charset", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^GB2312|UTF-8$:只能为GB2312或UTF-8", "编码");
            
            String desc = Https.getStr(request, "desc", R.CLEAN, R.LENGTH, "{1,60}", "描述");
            
            Files.valid(upload, new MapBean("file", Suffix.FILES));
            Uploads.maxSize(upload);
            
            Prop email = propService.get(Id.ROBOT_CORP, Id.ROBOT_USER, PropMap.EMAIL.getId());
            Integer[] propIds = Recipients.propIds(request, upload, charset, email);
  
            String fileId = "import/" + Id.ROBOT_USER;
            Integer[] excludeIds = { tag.getTagId() };
            
            Counter counter = new Counter();
            importService.execute(Options.INPUT, Id.ROBOT_CORP, Id.ROBOT_USER, PathMap.EXECUTE, fileId, upload, charset, desc, propIds, excludeIds, tag.getTagId(), counter);
            
            mb.put("counter", counter);
            
            Views.ok(mb, "正在导入收件人");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
            logger.error("(Selection:imports) error: ", e);
        } catch (Exception e) {
            Views.error(mb, "收件人导入失败");
            logger.error("(Selection:imports) error: ", e);
        }
        
        ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
    }
}

