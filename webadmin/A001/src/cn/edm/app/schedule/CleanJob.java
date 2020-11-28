package cn.edm.app.schedule;

import java.io.File;
import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;

import cn.edm.consts.Fqn;
import cn.edm.consts.Status;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Category;
import cn.edm.model.Cnd;
import cn.edm.model.Selection;
import cn.edm.model.Step;
import cn.edm.model.Tag;
import cn.edm.modules.cache.Ehcache;
import cn.edm.service.CategoryService;
import cn.edm.service.CndService;
import cn.edm.service.SelectionService;
import cn.edm.service.TagService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.execute.R;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

/**
 * 1: 若 (R not in task |(R in task & task.status=3)) & R not in cache, clear R.
 * 2: 若 export not in selection, clear export.  
 */
public class CleanJob {

    private static final Logger logger = LoggerFactory.getLogger(CleanJob.class);

    @Autowired
    private CndService cndService;
    @Autowired
    private Ehcache ehcache;
    @Autowired
    private SelectionService selectionService;
    @Autowired
    private TagService tagService;
    @Autowired
    private CategoryService categoryService;

    public void execute() {
        try {
            r();
            export();
            tagTemp();
        } catch (Exception e) {
            logger.error("(CleanJob:execute) error: ", e);
        }
    }
    
    @SuppressWarnings("unchecked")
    public void r() {
        String path = Webs.rootPath() + PathMap.R.getPath();
        File r = new File(path);
        
        if (!r.isDirectory()) {
            return;
        }
        
        if (r.listFiles() != null) {
            for (File owner : r.listFiles()) {
                File files = new File(path + "/" + owner.getName());
                if (!files.isDirectory()) {
                    continue;
                }
                if ("export".equals(owner.getName())) {
                	continue;
                }
                
                if (files.listFiles() != null) {
                    List<String> fileIds = Lists.newArrayList();
                    for (File file : files.listFiles()) {
                        fileIds.add(file.getName());
                    }
                    
                    Map<String, Cnd> cndMap = Maps.newHashMap();
                    if (!Asserts.empty(fileIds)) {
                        List<Cnd> cndList = cndService.find(fileIds);
                        if (!Asserts.empty(cndList)) {
                            for (Cnd cnd : cndList)
                                cndMap.put(cnd.getFileId(), cnd);
                        }
                    }
                        
                    Map<String, Step> stepMap = Maps.newHashMap();
                    List<Step> stepList = (List<Step>) ehcache.getValues(Fqn.R);
                    if (!Asserts.empty(stepList)) {
                        for (Step step : stepList)
                            stepMap.put(step.getFileId(), step);
                    }
                    
                    for (String fileId : fileIds) {
                        try {
                            boolean remove = false;
                            if (cndMap.containsKey(fileId)) {
                                Cnd cnd = cndMap.get(fileId);
                                if (cnd != null) {
                                    if (cnd.getStatus().equals(Status.DELETED))
                                        remove = true;
                                }
                            } else {
                                remove = true;
                            }
                            if (!stepMap.containsKey(fileId) && remove) {
                                R.delete(owner.getName(), fileId);
                                logger.info("(CleanJob:R) fileId: " + fileId);
                            }
                        } catch (Exception e) {
                            logger.error("(CleanJob:R) error: " + e);
                        }
                    }
                }
            }
        }
    }

    public void export() {
        String path = Webs.rootPath() + PathMap.EXECUTE.getPath() + "export";
        
        File export = new File(path);
        if (!export.isDirectory())
            return;
        
        if (export.listFiles() != null) {
            for (File owner : export.listFiles()) {
                File files = new File(path + "/" + owner.getName());
                if (!files.isDirectory())
                    continue;

                selectionService.delete(owner.getName(), 7);

                if (files.listFiles() != null) {
                    List<String> fileIds = Lists.newArrayList();
                    for (File file : files.listFiles())
                        fileIds.add(file.getName());
                    
                    Map<String, Integer> map = Maps.newHashMap();
                    if (!Asserts.empty(fileIds)) {
                        List<Selection> selectionList = selectionService.find(fileIds);
                        if (!Asserts.empty(selectionList))
                            for (Selection selection : selectionList)
                                map.put(selection.getFileId(), selection.getSelectionId());
                    }
                    
                    for (String fileId : fileIds) {
                        try {
                            if (map.containsKey(fileId))
                                continue;
                            String filePath = PathMap.EXECUTE.getPath()  + "export/" + owner.getName() + "/" + fileId;
                            Files.delete(Webs.rootPath(), filePath, PathMap.EXECUTE.getPath());
                            
                            logger.info("(CleanJob:export) fileId: " + fileId);
                        } catch (Exception e) {
                            logger.error("(CleanJob:export) error: " + e);
                        }
                    }
                }
            }
        }
    }
    /**
     * 删除上传的临时文件标签以及删除对应的上传的文件
     */
    public void tagTemp() {
    	String path = Webs.rootPath() + PathMap.R.getPath() + "export";
    	
    	File r = new File(path);
    	if (!r.isDirectory())
    		return;
    	
    	if (r.listFiles() != null) {
    		for (File owner : r.listFiles()) {
    			String userId = owner.getName();
    			File files = new File(path + "/" + userId);
    			if (!files.isDirectory())
    				continue;
    			
    			Category category = categoryService.get(null, userId, CategoryMap.FILE.getName(), Category.TAG);
    			if(category == null) 
    				continue;
    			
    			int categoryId = category.getCategoryId();
    			List<Tag> tagList = tagService.find(userId, categoryId, 7);
    			String tagIdStr = "";
    			String tagDescStr = "";
    			for(Tag tag : tagList) {
    				tagIdStr += tag.getTagId() + ",";
    				tagDescStr += tag.getTagDesc() + ",";
    			}
    			if("".equals(tagIdStr)){
    				continue;
    			}
    			
    			tagService.delete(userId, Converts._toIntegers(tagIdStr));
    			
    			String[] dirNames = Converts._toStrings(tagDescStr);
    			if (dirNames != null) {
    				for (String dirName : dirNames) {
    					try {
    						String filePath = PathMap.R.getPath()  + "export/" + userId + "/" + dirName;
    						Files.delete(Webs.rootPath(), filePath, PathMap.R.getPath());
    						
    						logger.info("(CleanJob:export) fileId: " + dirName);
    					} catch (Exception e) {
    						logger.error("(CleanJob:export) error: " + e);
    					}
    				}
    			}
    		}
    	}
    }
}
