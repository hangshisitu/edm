package cn.edm.controller;

import java.io.InputStream;
import java.io.PrintWriter;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.multipart.commons.CommonsMultipartFile;

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
import cn.edm.modules.utils.Props;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.service.CategoryService;
import cn.edm.service.EmailService;
import cn.edm.service.ImportService;
import cn.edm.service.PropService;
import cn.edm.service.TagService;
import cn.edm.utils.Asserts;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.facade.Recipients;

@Controller
@RequestMapping("/")
public class ApiAction {

    private static final Logger logger = LoggerFactory.getLogger(ApiAction.class);
    
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

    @RequestMapping(value = "refuse", method = RequestMethod.POST)
    public void refuse(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            String ip = Webs.ip(request);

            String REFUSE_API = Props.getStr("refuse.api");

            if (StringUtils.isNotBlank(REFUSE_API)) {
                if (StringUtils.isNotBlank(ip)) {
                    if (!ip.equals(REFUSE_API)) {
                        throw new Errors("[" + REFUSE_API + "] 此IP(" + ip + ")不允许");
                    }
                }
            }
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name("REFUSE"));
            Asserts.isNull(tag, "标签");
            
            String charset = "utf-8";
            
            InputStream input = request.getInputStream();

            String content = IOUtils.toString(input);
            
            String path = Webs.getRealPath(request) + "/WEB-INF/static/refuse/recipient.txt"; 
            PrintWriter out = new PrintWriter(path);
            
            out.write(content);
            out.flush();
            out.close();
            
            MultipartFile upload = new CommonsMultipartFile(Files.create(path, "recipient.txt"));
            
            Files.valid(upload, new MapBean("file", Suffix.FILES));
            Uploads.maxSize(upload);
            
            Prop email = propService.get(Id.ROBOT_CORP, Id.ROBOT_USER, PropMap.EMAIL.getId());
            Integer[] propIds = Recipients.propIds(request, upload, charset, email);
  
            String fileId = "import/" + Id.ROBOT_USER;
            Integer[] excludeIds = { tag.getTagId() };
            
            Counter counter = new Counter();
            importService.execute(Options.INPUT, Id.ROBOT_CORP, Id.ROBOT_USER, PathMap.EXECUTE, fileId, upload, charset, null, propIds, excludeIds, tag.getTagId(), counter);
        } catch (Exception e) {
            logger.error("(Api:refuse)" + e);
            Views.error(mb, "error");
        } finally {
        }
        
        ObjectMappers.renderText(response, "ok");
    }
}
