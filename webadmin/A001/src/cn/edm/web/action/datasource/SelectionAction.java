package cn.edm.web.action.datasource;

import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Cnds;
import cn.edm.consts.Config;
import cn.edm.consts.Options;
import cn.edm.consts.Status;
import cn.edm.consts.Suffix;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.SelectionMap;
import cn.edm.consts.mapper.TypeMap;
import cn.edm.model.Category;
import cn.edm.model.Filter;
import cn.edm.model.Prop;
import cn.edm.model.Selection;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.Property;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.modules.utils.web.Servlets;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.file.Downloads;
import cn.edm.utils.file.Files;
import cn.edm.utils.file.Lines;
import cn.edm.utils.file.Readers;
import cn.edm.utils.file.Uploads;
import cn.edm.utils.helper.Concurrencys;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Categorys;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Recipients;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/datasource/selection/**")
public class SelectionAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(SelectionAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
		    String token = CSRF.generate(request);
		    CSRF.generate(request, "/datasource/selection/download", token);
		    
			Page<Selection> page = new Page<Selection>();
			Pages.page(request, page);
			
			MapBean mb = new MapBean();
			page = selectionService.search(page, mb, UserSession.getCorpId(), null);

			map.put("page", page);
			map.put("mb", mb);

			return "default/datasource/selection_page";
		} catch (Exception e) {
			logger.error("(Selection:page) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(value = "import", method = RequestMethod.GET)
    public String imports(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/selection/counter", token);
            CSRF.generate(request, "/datasource/selection/upload", token);
            
            Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
            String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^file|tag$");
            map.put("action", action);
            if (tagId != null) {
                Tag tag = tagService.get(UserSession.getCorpId(), null, tagId);
                Asserts.isNull(tag, "标签");
                map.put("tagId", tagId);
                map.put("tag", tag);
            }
            Category category = categoryService.get(UserSession.getCorpId(), null, CategoryMap.FILE.getName(), Category.TAG);
            Asserts.isNull(category, "临时文件类别");
            
            List<Category> categories = categoryService.find(null, UserSession.getCorpId(), Category.TAG, CategoryMap.FORM.getName(), Cnds.NOT_EQ);
            List<Category> categoryList = Lists.newArrayList();
            if (categories != null) {
                for (Category c : categories) {
                    if (!category.getCategoryId().equals(c.getCategoryId())) {
                        categoryList.add(c);
                    }
                }
            }
            
            List<Tag> tagList = tagService.find(UserSession.getCorpId(), CategoryMap.FORM.getName(), Cnds.NOT_EQ);
            Map<String, List<Tag>> groupMap = Categorys.groupMap(categoryList, tagList);

            List<Prop> propList = propService.find(UserSession.getCorpId());
            Props.mapper(propList);
            
            map.put("groupMap", groupMap);
            map.put("propList", propList);
            map.put("categoryList", categoryList);
            map.put("types", TypeMap.values());
            map.put("category", category);
            
            return "default/datasource/selection_import";
        } catch (Exception e) {
            logger.error("(Selection:imports) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.POST)
    public void upload(HttpServletRequest request, HttpServletResponse response, 
            @RequestParam(required = false, value = "file") MultipartFile upload) {
        MapBean mb = new MapBean();
        try {
            CSRF.auth(request);
            
            Files.valid(upload, new MapBean("file", Suffix.FILES));
            Uploads.maxSize(upload);
            
            String[] GB2312 = Readers.lines(upload, "GB2312", 2);
            String[] UTF8 = Readers.lines(upload, "UTF-8", 2);
            
            mb.put("GB2312", GB2312);
            mb.put("UTF8", UTF8);
            mb.put("emailCount", Lines.count(upload, "UTF-8"));
            
            Views.ok(mb, "上传文件成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "上传文件失败");
            logger.error("(Selection:upload) error: ", e);
        }

        ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
    }
    
    @RequestMapping(value = "import", method = RequestMethod.POST)
    public void imports(HttpServletRequest request, HttpServletResponse response,
            @RequestParam(required = false, value = "file") MultipartFile upload) {
        MapBean mb = new MapBean();
        try {
            CSRF.auth(request);
            
            String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^tag|file$");
            mb.put("action", action);
            
            if("tag".equals(action)){
            	importTag(request, upload, mb);
            }else{
            	importFile(request, upload, mb);
            }
            Views.ok(mb, "正在导入收件人");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "收件人导入失败");
            logger.error("(Selection:imports) error: ", e);
        }
        
        ObjectMappers.renderJson(response, Servlets.TEXT_TYPE, mb);
    }
    
    private void importTag(HttpServletRequest request, MultipartFile upload, MapBean mb) {
    	 Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
         Asserts.isNull(tagId, "标签");
         Tag tag = tagService.get(UserSession.getCorpId(), null, tagId);
         Asserts.isNull(tag, "标签");
         if (tag.getCategoryName().equals(CategoryMap.FORM.getName())) {
             throw new Errors("收件人的标签类别不能为表单收集类别");
         }
         
         String charset = Https.getStr(request, "charset", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^GB2312|UTF-8$:只能为GB2312或UTF-8", "编码");
         
         Files.valid(upload, new MapBean("file", Suffix.FILES));
         Uploads.maxSize(upload);
         
         Prop email = propService.get(UserSession.getCorpId(), null, PropMap.EMAIL.getId());
         Integer[] propIds = Recipients.propIds(request, upload, charset, email);
         importSync.execute(tag.getCorpId(), tag.getUserId(), upload, charset, propIds, tagId);
    }
    
    private void importFile(HttpServletRequest request, MultipartFile upload, MapBean mb) {
    	String charset = Https.getStr(request, "charset", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^GB2312|UTF-8$:只能为GB2312或UTF-8", "编码");
    	Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
    	Tag tag = null;
        if (tagId != null) {
            tag = tagService.get(UserSession.getCorpId(), null, tagId);
        }
		
		Files.valid(upload, new MapBean("file", Suffix.FILES));
		Uploads.maxSize(upload);
		
		Prop email = propService.get(UserSession.getCorpId(), null, PropMap.EMAIL.getId());
		Integer[] propIds = Recipients.propIds(request, upload, charset, email);
		
		Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER);
        Asserts.isNull(categoryId, "临时文件类别");
		String tagName = Https.getStr(request, "tagName", R.CLEAN, R.LENGTH, "{1,60}", "标签名称");
		Asserts.isNull(tagName, "标签名称");
		if(tag == null) Asserts.notUnique(tagService.unique(UserSession.getCorpId(), tagName, ""), "标签");
		
        if (tag == null) {
            tag = new Tag();
            tag.setCategoryId(categoryId);
            tag.setCategoryName(CategoryMap.FILE.getName());
            tag.setCorpId(UserSession.getCorpId());
            tag.setUserId(UserSession.getUserId());
            tag.setDbId(Category.TAG);
            String now = new DateTime().toString("yyyyMMddHHmmss");
            String fileId = "EXPORT_" + now;
            tag.setTagDesc(fileId);
            tag.setEmailCount(Value.I);
        }
        tag.setTagName(tagName);
        tagService.save(tag);
        
		importSync.executeTemp(tag.getCorpId(), tag.getUserId(), upload, charset, propIds, tag);
    		
    }
    
    @RequestMapping(value = "export", method = RequestMethod.GET)
    public String exports(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            String token = CSRF.generate(request);
            CSRF.generate(request, "/datasource/selection/counter", token);
            CSRF.generate(request, "/datasource/selection/download", token);
            
            List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TAG, CategoryMap.FILE.getName(), Cnds.NOT_EQ);
            List<Tag> tagList = tagService.find(UserSession.getCorpId());
            Map<String, List<Tag>> groupMap = Categorys.groupMap(categoryList, tagList);
            
            List<Filter> filterList = filterService.find(UserSession.getCorpId());
            settings(filterList);

            map.put("categoryList", categoryList);
            map.put("groupMap", groupMap);
            map.put("filterList", filterList);
            
            return "default/datasource/selection_export";
        } catch (Exception e) {
            logger.error("(Selection:exports) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "export", method = RequestMethod.POST)
    public void exports(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);
            
            String includeTagIds = Https.getStr(request, "includeTagIds", R.CLEAN, R.REQUIRED, R.REGEX, "regex:^[0-9,]+$:不是合法值", "包含标签");
            String excludeTagIds = Https.getStr(request, "excludeTagIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "排除标签");
            Integer filterId = Https.getInt(request, "filterId", R.CLEAN, R.INTEGER, "过滤器");
            
            String activeCnd = Https.getStr(request, "activeCnd", R.CLEAN, R.REQUIRED, "regex:^-1|0|1week|1month|2month|3month$");
            String inCnd = Https.getStr(request, "inCnd", R.CLEAN, R.REQUIRED, "regex:^or|and$");
            Integer type = Https.getInt(request, "type", R.CLEAN, R.INTEGER, R.REQUIRED, R.REGEX, "regex:^0|1|2$");
            Integer emailCount = Https.getInt(request, "emailCount", R.CLEAN, R.INTEGER, R.REQUIRED);
            
            String includes = Https.getStr(request, "includes", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,512}", "包含标签");
            String excludes = Https.getStr(request, "excludes", R.CLEAN, R.LENGTH, "{1,512}", "排除标签");
            String filters = Https.getStr(request, "filter", R.CLEAN, R.LENGTH, "{1,255}", "排除标签");
            
            activeCnd = Values.get(activeCnd, "0");
            inCnd = Values.get(inCnd, "or");
            type = Values.get(type, 0);

            Integer tagId = null;
            if (SelectionMap.option(type).equals(Options.INPUT)) {
                tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
                Asserts.isNull(tagId, "标签");
                Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
                Asserts.isNull(tag, "标签");
                excludeTagIds = excludeTagIds + "," + tagId;
                if (StringUtils.equals(tag.getCategoryName(), CategoryMap.FORM.getName())) {
                    throw new Errors("标签类别不能为表单收集类别");
                }
            }
            Integer[] inTagIds = Converts._toIntegers(includeTagIds);
            Asserts.isEmpty(inTagIds, "包含标签");
            Integer[] exTagIds = Converts._toIntegers(excludeTagIds);

            if (filterId != null) {
                Filter filter = filterService.get(UserSession.getCorpId(), UserSession.getUserId(), filterId);
                Asserts.isNull(filter, "过滤器");
            }

            int selectionCount = Property.getInt(Config.SELECTION_COUNT) ;
            if (selectionService.processingCount(UserSession.getCorpId(), UserSession.getUserId()) > (selectionCount - 1)) {
                throw new Errors("您最多可同时进行" + selectionCount + "条数据筛选，请稍后再处理。");
            }
            
            Selection selection = new Selection();
            selection.setCorpId(UserSession.getCorpId());
            selection.setUserId(UserSession.getUserId());
            selection.setIncludeIds(Converts.toString(inTagIds));
            selection.setActiveCnd(activeCnd);
            selection.setInCnd(inCnd);
            selection.setIncludes(Values.get(includes, null));
            selection.setExcludeIds(Converts.toString(exTagIds));
            selection.setExcludes(Values.get(excludes, null));
            selection.setFilterId(filterId);
            selection.setFilter(Values.get(filters, null));
            selection.setEmailCount(emailCount);
            selection.setType(type);
            selection.setTagId(tagId);
            selection.setStatus(Status.DISABLED);
            selectionService.save(selection);
            Files.createSelection(selection.getSelectionId());
            
            Views.ok(mb, "正在筛选收件人");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "收件人筛选失败");
            logger.error("(Selection:exports) error: ", e);
        }
        
        ObjectMappers.renderJson(response, mb);
    }

    @RequestMapping(method = RequestMethod.GET)
    public String download(HttpServletRequest request, HttpServletResponse response) {
        try {
            CSRF.auth(request);
            Integer selectionId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
            Asserts.isNull(selectionId, "筛选");
            Selection selection = selectionService.get(selectionId);
            Asserts.isNull(selection, "筛选");
            Asserts.isNull(selection.getFileId(), "筛选文件");
            
            String path = PathMap.EXECUTE.getPath() + "export/" + UserSession.getUserId() + "/" + selection.getFileId() + "/reduce.txt";
            Downloads.download(response, Webs.rootPath(), path, "导出.csv");

            return null;
        } catch (Exception e) {
            logger.error("(Selection:download) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.POST)
    public void counter(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);
            Counter counter = Concurrencys.getCounter(cache, UserSession.getUserId());
            if (counter != null) {
                mb.put("counter", counter);
            }
            Views.ok(mb, "执行计数器成功");
        } catch (Exception e) {
            Views.error(mb, "执行计数器失败");
            logger.error("(Selection:counter) error: ", e);
        }
        
        ObjectMappers.renderJson(response, mb);
    }
    
    private void settings(List<Filter> filterList) {
        if (!Asserts.empty(filterList)) {
            for (Filter filter : filterList) {
                String settings = Props.settings(filter.getProps());
                filter.setProps(filter.getFilterName() + "（" + settings + "）");
            }
        }
    }
}

