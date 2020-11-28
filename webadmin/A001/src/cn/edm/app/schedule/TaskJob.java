package cn.edm.app.schedule;

import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.task.TaskExecutor;

import cn.edm.app.auth.Auth;
import cn.edm.consts.Fqn;
import cn.edm.consts.HttpCode;
import cn.edm.consts.Quartz;
import cn.edm.consts.Queue;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Cnd;
import cn.edm.model.Corp;
import cn.edm.model.Prop;
import cn.edm.model.Resource;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Template;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.service.CndService;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.PropService;
import cn.edm.service.ResourceService;
import cn.edm.service.SettingService;
import cn.edm.service.TaskService;
import cn.edm.service.TemplateService;
import cn.edm.service.WriterService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.NotifyListener;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.R;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.Webs;
import cn.edm.web.api.Processings;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Props;
import cn.edm.web.facade.Tasks;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public class TaskJob {

	private static final Logger logger = LoggerFactory.getLogger(TaskJob.class);

	@Autowired
	private CndService cndService;
	@Autowired
	private CorpService corpService;
	@Autowired
	private CosService cosService;
	@Autowired
	private Ehcache ehcache;
	@Autowired
	private PropService propService;
	@Autowired
	private ResourceService resourceService;
	@Autowired
	private SettingService settingService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private TemplateService templateService;
	@Autowired
	private WriterService writerService;
	
	public void execute() {
		if (!Auth.isSetup()) {
			return;
		}
		
		try {
			scan();
			queue();
		} catch (Exception e) {
			logger.error("(TaskJob:execute) error: ", e);
		}
	}

	@SuppressWarnings("unchecked")
	private void scan() {
		Map<Integer, Integer> scanMap = (Map<Integer, Integer>) ehcache.get(Fqn.TASK, Queue.TASK);
		if (scanMap == null) {
			scanMap = Maps.newHashMap();
			ehcache.put(Fqn.TASK, Queue.TASK, scanMap);
		}
		// 扫描文件
		String path = Webs.webRoot() + PathMap.SCAN_TASK.getPath();
		List<Integer> fileIdList = Files.scan(path);
		List<Integer> scanTaskidList = Lists.newArrayList();
		if(fileIdList.size() > 0){
			Integer[] taskIds = Converts._toIntegers(fileIdList);
			List<Integer> jobTaskIds = taskService.exclude(taskIds);
			scanTaskidList = taskService.scan(Converts._toIntegers(jobTaskIds));
			// 扫描出来的ID taskIds 与 查询出来的 tids 对比，如果没有结果的元素则对其以ID为名的文件进行删除
			for(Integer tid : fileIdList) {
				boolean isDel = true;
				for(Integer jtid : jobTaskIds) {
					if(tid.equals(jtid)) {
						isDel = false;
						break;
					}
				}
				// 对不符合扫描的任务并且不是周期计划的任务进行删除基础扫描文件
				if(isDel) {
					deleteFile(tid);
				}
			}
			for (Integer taskId : scanTaskidList) {
				synchronized (taskId) {
					if (scanMap.containsKey(taskId)) {
						continue;
					}
					scanMap.put(taskId, Quartz.WAITING);
				}
			}
			logger.info("(TaskJob:scan) scanMap: " + scanMap);
		}
	}
	
	@SuppressWarnings("unchecked")
	private void queue() {
		final Map<Integer, Integer> scanMap = (Map<Integer, Integer>) ehcache.get(Fqn.TASK, Queue.TASK);
		if (Asserts.empty(scanMap)) {
			return;
		}
		
		TaskExecutor executor = CtxHolder.getBean("scan");
		
		// 执行<DeliveryEngine>投递的任务, 并发上限为10.
		int max = 0;
		for (Entry<Integer, Integer> entry : scanMap.entrySet()) {
			Integer taskId = entry.getKey();
			synchronized (taskId) {
				if (scanMap.containsKey(taskId)) {
					if (scanMap.get(taskId).equals(Quartz.PROCESSING)) {
						max++;
					}
					if (Asserts.hasAny(scanMap.get(taskId), new Integer[] { Quartz.PAUSED, Quartz.CANCELLED })) {
						scanMap.remove(taskId);
					}
				}
			}
		}
		if (max > Quartz.MAX_POOL - 1) {
			return;
		}

		int i = 1;
		for (Entry<Integer, Integer> entry : scanMap.entrySet()) {
			final Integer taskId = entry.getKey();
			synchronized (taskId) {
				if (scanMap.containsKey(taskId)
						&& Asserts.hasAny(scanMap.get(taskId), new Integer[] { Quartz.PROCESSING, Quartz.PAUSED, Quartz.CANCELLED })) {
					continue;
				}
			}
			
			executor.execute(new Runnable() {
				@Override
				public void run() {
					Task task = null;
					Setting setting = null;
					Cnd cnd = null;
					
					String sid = null;
					
					try {
						synchronized (taskId) {
						    task = taskService.get(taskId);
	                        Asserts.isNull(task, "任务");
	                        
	                        sid = Tasks.id(task.getCorpId(), task.getTaskId(), task.getTemplateId());
	                        
						    if (scanMap.containsKey(taskId)
			                        && Asserts.hasAny(scanMap.get(taskId), new Integer[] { Quartz.PROCESSING, Quartz.PAUSED, Quartz.CANCELLED })) {
						        return;
			                }
						    
							listener(scanMap, taskId);
							scanMap.put(taskId, Quartz.PROCESSING);
						}
						
						if (task.getEmailCount() < 1) {
							throw new Errors("收件人数不能少于1");
						}

						cnd = cndService.get(taskId);
						Asserts.isNull(cnd, "收件人设置");
						setting = settingService.get(taskId);
						Asserts.isNull(setting, "发送设置");
						
						Corp corp = corpService.get(task.getCorpId());
						Asserts.isNull(corp, "企业");
						
						String path = null;
						if (Asserts.hasAny(task.getType(), new Integer[] {Type.FORMAL, Type.COLL})) {
						    Template template = templateService.get(task.getTemplateId());
	                        Asserts.isNull(template, "模板");
	                        Asserts.notExists(Webs.rootPath(), template.getFilePath(), "模板");
	                        
						    Resource resource = resourceService.get(task.getResourceId());
	                        Asserts.isNull(resource, "通道");
	                        
	                        List<Prop> propList = propService.find(task.getCorpId());
	                        Asserts.isEmpty(propList, "属性");
	                        
	                        DateTime jobTime = jobTime(task);
	                        
	                        List<Corp> corpList = corpService.find();
	                        Corp currentCorp = corpService.get(corp.getCorpId());
	                        Corp root = Corps.getRoot(corpList, currentCorp);
	                        List<Integer> corpIdList = Lists.newArrayList();
	                        corpService.cross(corpList, root.getCorpId(), corpIdList);
	                        corpIdList.add(root.getCorpId()); //算发送量的时候需要加上顶级父机构的发送量
	                        Integer[] corpIds = Converts._toIntegers(corpIdList);
	                        cosService.balance(corpIds, root.getCosId(), Tasks.holdCount(task.getStatus(), task.getEmailCount()), task.getEmailCount(), jobTime);
	                        
	                        String subject = Values.get(task.getSubject());
	                        String content = Values.get(Files.get(Webs.rootPath(), template.getFilePath()));
	                        Map<Integer, String> propMap = Props.parameters(subject, content, propList);
						    
						    path = writerService.write(task, template, resource, propMap);
						} else if (task.getType().equals(Type.JOIN)) {
						    path = PathMap.JOIN_API.getPath() + cnd.getFileId();
						}
						
						Integer scan = taskService.scan(taskId);
						if (scan != null) {
							synchronized (taskId) {
								listener(scanMap, taskId);
								String code = Processings.processing(path);
								if (StringUtils.isNotBlank(code) && StringUtils.equals(code, HttpCode._200)) {
									Date now = new Date();
									task.setStatus(Status.PROCESSING);
									task.setDeliveryTime(now);
									taskService.save(task);
									scanMap.remove(taskId);
								} else {
									scanMap.put(taskId, Quartz.WAITING);
								}
								logger.info("(TaskJob:queue) code: " + code + ", sid: " + sid + ", status: " + task.getStatus());
							}
						} else {
							scanMap.remove(taskId);
							// 扫描入库成功后，删除扫描文件
							deleteFile(taskId);
						}
					} catch (NotifyListener e) {
						logger.info("(TaskJob:queue) sid: " + sid + ", notify: " + e.getMessage());
					} catch (ServiceErrors e) {
						if (task != null) {
							task.setStatus(setting.getSendCode().equals("schedule") ? Status.WAITED : Status.QUEUEING);
							taskService.save(task);
						}
						scanMap.remove(taskId);
			            logger.error("(TaskJob:queue) sid: " + sid + ", error: ", e);
						throw new Errors("(TaskJob:queue) sid: " + sid + ", error: ", e);
					} catch (Exception e) {
						if (task != null) {
							task.setStatus(Status.DRAFTED);
							taskService.saveAndMinusSent(task, task.getEmailCount());
							// 变成草稿后，删除扫描文件
							deleteFile(taskId);
						}
						
						scanMap.remove(taskId);
						logger.error("(TaskJob:queue) sid: " + sid + ", error: ", e);
						throw new Errors("(TaskJob:queue) sid: " + sid + ", error: ", e);
					} finally {
						if (task != null && Asserts.hasAny(task.getType(), new Integer[] {Type.FORMAL, Type.COLL})) {
							boolean isProcessing = task.getStatus().equals(Status.PROCESSING);
							boolean isDelivery = (task.getDeliveryTime() != null);
							if (isProcessing && isDelivery) {
								R.delete(task.getUserId(), cnd.getFileId());
							}
						}
					}
				}
			});
			
			if (i > (Quartz.MAX_POOL - max)) {
				break;
			}
			i++;
		}
	}
	
	private void listener(Map<Integer, Integer> scanMap, Integer taskId) {
		if (scanMap.containsKey(taskId)) {
			if (scanMap.get(taskId).equals(Quartz.PAUSED)) {
				throw new NotifyListener("is paused");
			}
			if (scanMap.get(taskId).equals(Quartz.CANCELLED)) {
				throw new NotifyListener("is cancelled");
			}
		}
	}
	
	private DateTime jobTime(Task task) {
		DateTime jobTime = null;
		if (task.getJobTime() != null) {
			jobTime = new DateTime(task.getJobTime());
		} else {
			jobTime = new DateTime(task.getModifyTime());
		}
		return jobTime;
	}
	
	private void deleteFile(Integer taskId) {
		String taskidPath = Webs.webRoot() + PathMap.SCAN_TASK.getPath() + taskId + ".txt";
		Files.delete(taskidPath);
	}
	
}
