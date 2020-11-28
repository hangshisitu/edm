package cn.edm.app.schedule;

import java.io.File;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;

import cn.edm.app.auth.Auth;
import cn.edm.consts.HttpCode;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Corp;
import cn.edm.model.Prop;
import cn.edm.model.Resource;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Touch;
import cn.edm.modules.utils.Encodes;
import cn.edm.service.CorpService;
import cn.edm.service.PropService;
import cn.edm.service.ResourceService;
import cn.edm.service.RobotService;
import cn.edm.service.SettingService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.service.TouchService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Delivery;
import cn.edm.utils.helper.Touchs;
import cn.edm.utils.web.Webs;
import cn.edm.web.api.Processings;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Tasks;

public class TouchJob {
    
    private static final Logger logger = LoggerFactory.getLogger(TouchJob.class);

    private static boolean setup = false;
    
    @Autowired
    private CorpService corpService;
    @Autowired
    private PropService propService;
    @Autowired
    private ResourceService resourceService;
    @Autowired
    private RobotService robotService;
    @Autowired
    private SettingService settingService;
    @Autowired
    private TaskService taskService;
    @Autowired
    private TemplateService templateService;
    @Autowired
    private TouchService touchService;
    
    public void execute() {
        if (!Auth.isSetup()) {
            return;
        }
        
        try {
            if (setup) {
                return;
            }
            setup = true;
            
            scan();
        } catch (Exception e) {
            logger.error("(TouchJob:execute) error: ", e);
        } finally {
            setup = false;
        }
    }
    
    private void scan() {
        DateTime now = new DateTime();
        
        File root = new File(Webs.webRoot() + PathMap.TOUCH.getPath());
        if (root.listFiles() == null) {
            return;
        }
        
        for (File current : root.listFiles()) {
            if (current.listFiles() == null) {
                continue;
            }

            String ids = current.getName();
            int touchCount = 0;
            
            for (File file : current.listFiles()) {
                String fileName = file.getName();
                
                if (fileName.equals("template.txt")) {
                    continue;
                }
                
                if (!fileName.startsWith("~")) {
                    touchCount++;
                    continue;
                }
                
                if (StringUtils.startsWith(fileName, "~")) fileName = StringUtils.removeStart(fileName, "~");
                if (StringUtils.endsWith(fileName, ".txt")) fileName = StringUtils.removeEnd(fileName, ".txt");
                
                String afterMinutes = StringUtils.substringBefore(fileName, "_");
                String email64 = StringUtils.substringBefore(fileName.substring(afterMinutes.length() + 1), "_");
                String email = new String(Encodes.decodeBase64(email64));
                
                if (afterMinutes.compareTo(now.toString("yyyyMMddHHmm")) > 0) {
                    continue;
                }

                boolean completed = false; 
                try {
                    String code = Processings.touching(write(fileName, ids, email));
                    if (StringUtils.isNotBlank(code) && StringUtils.equals(code, HttpCode._200)) {
                        logger.info("(TouchJob:scan) code: " + code + ", sid: " + ids + ", status: ok");
                    } else {
                        throw new Errors("(TouchJob:scan) code is " + code);
                    }
                    
                    touchCount++;
                    
                    completed = true;
                } catch (Exception e) {
                    logger.error("(TouchJob:scan) error: ", e);
                } finally {
                    if (completed) {
                        String touchPath = PathMap.TOUCH.getPath() + ids + "/" + "~" + fileName + ".txt";
                        if (Files.exists(Webs.webRoot() + touchPath) && StringUtils.endsWith(touchPath, ".txt")) {
                            Files.delete(Webs.webRoot(), touchPath, PathMap.TOUCH.getPath());
                        }
                    }
                }
            }
            
            try {
                Integer[] arr = Tasks.id(ids);
                Touch touch = touchService.get(arr[0], arr[1], arr[2]);
                Asserts.isNull(touch, "触发计划");
                Integer oldTouchCount = Values.get(touch.getTouchCount(), 0);
                if(touchCount > oldTouchCount) {
                	touch.setTouchCount(touchCount);
                	touchService.save(touch);
                }
            } catch (Exception e) {
                logger.error("(TouchJob:scan) sid: " + ids + ", error: ", e);
            }
        }
    }
    
    private String write(String fileName, String ids, String email) {
        Integer[] arr = Tasks.id(ids);
        int corpId = arr[0];
        int taskId = arr[1];
        int templateId = arr[2];
        
        Touch touch = touchService.get(corpId, taskId, templateId);
        Asserts.isNull(touch, "触发计划");
        Task task = taskService.get(touch.getParentId());
        Asserts.isNull(task, "任务");
        Setting setting = settingService.get(task.getTaskId());
        Asserts.isNull(setting, "发送设置");
        Corp corp = corpService.get(corpId);
        Asserts.isNull(corp, "企业");
        List<Prop> propList = propService.find(touch.getCorpId());
        Asserts.isEmpty(propList, "属性");
        
        String path = PathMap.TOUCH.getPath() + ids + "/";
        String template = Webs.rootPath() + path + "/template.txt";
        String subject = Values.get(touch.getSubject());
        String content = Values.get(Delivery.node(template, "MODULA"));
        Map<Integer, String> propMap = Props.parameters(subject, content, propList);
        
        Resource resource = resourceService.get(task.getResourceId());
        Asserts.isNull(resource, "通道");
        
        String fid = fileName + ".txt";
        return Touchs.write(ids, fid, touch, setting, robotService.find(corpId), resource, propMap, email);
    }
}
