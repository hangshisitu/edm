package cn.edm.app.schedule;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.task.TaskExecutor;

import cn.edm.consts.Fqn;
import cn.edm.consts.Quartz;
import cn.edm.consts.Queue;
import cn.edm.consts.Status;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.SelectionMap;
import cn.edm.model.Selection;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.service.ExportService;
import cn.edm.service.SelectionService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.file.Files;
import cn.edm.utils.helper.Concurrencys;
import cn.edm.utils.web.Webs;

import com.google.common.collect.Maps;

public class SelectionJob {
    
    private static final Logger logger = LoggerFactory.getLogger(SelectionJob.class);

    @Autowired
    private Ehcache ehcache;
    @Autowired
    private ExportService exportService;
    @Autowired
    private SelectionService selectionService;
    
    public void execute() {
        try {
            scan();
            queue();
        } catch (Exception e) {
            logger.error("(SelectionJob:execute) error: ", e);
        }
    }

    @SuppressWarnings("unchecked")
    private void scan() {
        Map<Integer, Integer> scanMap = (Map<Integer, Integer>) ehcache.get(Fqn.SELECTION, Queue.SELECTION);
        if (scanMap == null) {
            scanMap = Maps.newLinkedHashMap();
            ehcache.put(Fqn.SELECTION, Queue.SELECTION, scanMap);
        }
        
        String path = Webs.webRoot() + PathMap.SCAN_SELECTION.getPath();
		List<Integer> selectionIds = Files.scan(path);
		if(selectionIds.size() > 0) {
			for (Integer selectionId : selectionService.scan(selectionIds)) {
				synchronized (selectionId) {
					scanMap.put(selectionId, Quartz.WAITING);
				}
			}
		}
        
        logger.info("(SelectionJob:scan) scanMap: " + scanMap);
    }
    
    @SuppressWarnings("unchecked")
    private void queue() {
        final Map<Integer, Integer> scanMap = (Map<Integer, Integer>) ehcache.get(Fqn.SELECTION, Queue.SELECTION);
        if (Asserts.empty(scanMap)) {
            return;
        }
        
        TaskExecutor executor = CtxHolder.getBean("export");
        
        // 执行<Selection>筛选, 并发上限为10.
        int max = 0;
        for (Entry<Integer, Integer> entry : scanMap.entrySet()) {
            Integer selectionId = entry.getKey();
            synchronized (selectionId) {
                if (scanMap.containsKey(selectionId)) {
                    if (scanMap.get(selectionId).equals(Quartz.PROCESSING)) {
                        max++;
                    }
                }
            }
        }
        if (max > Quartz.MAX_POOL - 1) {
            return;
        }

        int i = 1;
        for (Entry<Integer, Integer> entry : scanMap.entrySet()) {
            final Integer selectionId = entry.getKey();
            synchronized (selectionId) {
                if (scanMap.containsKey(selectionId)
                        && Asserts.hasAny(scanMap.get(selectionId), new Integer[] { Quartz.PROCESSING })) {
                    continue;
                }
            }
            
            executor.execute(new Runnable() {
                @Override
                public void run() {
                    Selection selection = null;
                    Counter counter = null;
                    try {
                        synchronized (selectionId) {
                            selection = selectionService.get(selectionId);
                            Asserts.isNull(selection, "筛选");
                            if (!selection.getStatus().equals(Status.DISABLED)) throw new Errors("禁止获取非等待的筛选队列");
                            scanMap.put(selectionId, Quartz.PROCESSING);
                        }
                        
                        counter = Concurrencys.getCounter(ehcache, true, selection.getUserId());

                        String now = new DateTime().toString("yyyyMMddHHmmss");
                        
                        String option = SelectionMap.option(selection.getType());
                        int corpId = selection.getCorpId();
                        String userId = selection.getUserId();
                        String fileId = SelectionMap.getName(selection.getType()) + "." + now;
                        
                        Integer[] includeIds = Converts._toIntegers(selection.getIncludeIds());
                        Integer[] excludeIds = Converts._toIntegers(selection.getExcludeIds());
                        String activeCnd = selection.getActiveCnd();
                        String inCnd = selection.getInCnd();
                        Integer filterId = selection.getFilterId();
                        Integer tagId = selection.getTagId();
                        
                        selection.setFileId(fileId);
                        
                        exportService.execute(option, corpId, userId, PathMap.EXECUTE, "export/" + userId + "/" + fileId, includeIds, excludeIds, activeCnd, inCnd, filterId, tagId, counter);
                        
                        synchronized (selectionId) {
                            selection.setEmailCount(counter.getEnd());
                            selection.setStatus(Status.ENABLED);
                            selectionService.save(selection);
                            scanMap.remove(selectionId);
                            // 扫描入库成功后，删除扫描文件
                            deleteFile(selectionId);
                        }
                    } catch (Errors e) {
                        if (selection != null) {
                            synchronized (selectionId) {
                                selection.setEmailCount(Value.I);
                                selection.setStatus(Status.DISABLED);
                                selectionService.save(selection);
                                // 扫描入库成功后，删除扫描文件
//                                deleteFile(selectionId);
                            }
                        }
                        scanMap.remove(selectionId);
                        throw new Errors("(SelectionJob:queue) selectionId: " + selectionId + ", error: ", e);
                    } catch (ServiceErrors e) {
                        scanMap.remove(selectionId);
                        throw new Errors("(SelectionJob:queue) selectionId: " + selectionId + ", error: ", e);
                    } catch (Exception e) {
                        scanMap.remove(selectionId);
                        throw new Errors("(SelectionJob:queue) selectionId: " + selectionId + ", error: ", e);
                    } finally {
                        if (counter != null)
                            counter.setCode(Counter.COMPLETED);
                    }
                }
            });
            
            if (i > (Quartz.MAX_POOL - max)) {
                break;
            }
            i++;
        }
    }
    
	private void deleteFile(Integer selectionId) {
		String taskidPath = Webs.webRoot() + PathMap.SCAN_SELECTION.getPath() + selectionId + ".txt";
		Files.delete(taskidPath);
	}
	
}
