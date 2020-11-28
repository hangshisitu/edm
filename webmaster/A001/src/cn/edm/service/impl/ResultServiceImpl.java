package cn.edm.service.impl;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.joda.time.DateTime;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Status;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Domain;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Url;
import cn.edm.persistence.ResultMapper;
import cn.edm.service.DomainService;
import cn.edm.service.PlanService;
import cn.edm.service.ResultService;
import cn.edm.service.TaskService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.DateUtil;
import cn.edm.util.Pagination;
import cn.edm.util.PropertiesUtil;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;
import cn.edm.vo.Log;

import com.google.common.collect.Maps;

/**
 * @author Luxh
 *
 */
@Service
public class ResultServiceImpl implements ResultService{
	
	@Autowired
	private ResultMapper resultMapper;
	@Autowired
	private PlanService planService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private DomainService domainService;
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public Pagination<Result> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectResultList(paramsMap);
		long recordCount = resultMapper.selectResultCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}
	
	/**
	 * 复制Result值
	 * 从Result查询数据，复制Result的投递效果的统计数据到results对应属性中
	 * @param results
	 */
	private void copyResultList(List<Result> results, Map<String,Object> realParamsMap, int currentPage, int pageSize) {
		// 周期任务
		String pIds = "";
		String tIds = "";
		for(int i=0,len=results.size(); i<len; i++) {
			if(results.get(i).getPlanId()!=null && !"".equals(results.get(i).getPlanId())){
				pIds += results.get(i).getPlanId() + ",";
			}
			if(results.get(i).getTaskId()!=null && !"".equals(results.get(i).getTaskId())){
				tIds += results.get(i).getTaskId() + ",";
			}
		}
		if(pIds!=null && !"".equals(pIds)){
			pIds = pIds.substring(0, pIds.length()-1);
		}
		Pagination<Result> pagination = this.getPlanResultSumList(realParamsMap, Converts._toIntegers(pIds), "", 0, 1000000);
		List<Result> rs = pagination.getRecordList();
		String ignore = "corpId, taskId, templateId, campaignId, campaignName, taskName, status, deliveryTime";
		for(Result newR : rs) {
			if(newR.getPlanId()==null || "".equals(newR.getPlanId())){
				continue;
			}
			for(Result oldR : results) {
				if(oldR.getPlanId()==null || "".equals(oldR.getPlanId())){
					continue;
				}
				if(newR.getPlanId().equals(oldR.getPlanId())){
					String[] copy = Converts._toStrings(ignore);
		            BeanUtils.copyProperties(newR, oldR, copy);
		            break;
				}
			}
		}
		// 单项任务
		Map<String, Object> singleParamMap = Maps.newHashMap();
		singleParamMap.put("corpId", realParamsMap.get("corpId"));
		singleParamMap.put("planCnd", "isNull");
		singleParamMap.put("taskIds", Converts._toIntegers(tIds));

		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("taskIds", ConvertUtils.str2Int(tIds));
		List<Domain> domainList = domainService.getDomainSum(paramsMap);
		ResultDataOption.copy("task", domainList, results);
		
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public void writeCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		List<Result> resultList = this.getViewDeliveryTasksByCorpId(paramsMap, 0, 0);//resultMapper.selectViewDeliveryTasksByCorpId(paramsMap);
		String[] title = new String[] {"任务名称", "任务类型", "邮件主题", "账号", "公司名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数", "点击总次数", "退订总数", "转发总数", "无效人数", "任务状态", "时间"};
		csvWriter.writeNext(title);
		if(resultList != null && resultList.size()>0) {
			String[] content = new String[title.length];
			int len = resultList.size();
			Result r = null;
			for(int i=0;i<len;i++) {
				r = resultList.get(i);
				content[0] = "\t"+r.getTaskName();
				String taskType = "单项任务";
				if(r.getTaskType() == 1){
					taskType = "测试任务";
				}else if(r.getTaskType()==3 || r.getTaskType()==2){
					taskType = "周期任务";
				}else if(r.getTaskType() == 4){
					taskType = "API群发任务";
				}
				content[1] = "\t"+taskType;
				content[2] = "\t"+r.getSubject();
				content[3] = r.getUserId();
				content[4] = "\t"+r.getCompanyName();
				content[5] = getValue(r.getSentCount(), "0");
				content[6] = getValue(r.getReachCount(), "0");
				content[7] = getValue(r.getSoftBounceCount()+r.getHardBounceCount(), "0");
				content[8] = getValue(r.getReadUserCount(), "0");
				content[9] = getValue(r.getReadCount(), "0");
				content[10] = getValue(r.getClickUserCount(), "0");
				content[11] = getValue(r.getClickCount(), "0");
				content[12] = getValue(r.getUnsubscribeCount(), "0");
				content[13] = getValue(r.getForwardCount(), "0");
				content[14] = getValue(r.getAfterNoneCount(), "0");
				String taskStatus = "";
				if(22==r.getStatus()) {
					taskStatus = "待发送";
				}else if(23==r.getStatus()) {
					taskStatus = "队列中";
				}else if(24==r.getStatus()) {
					taskStatus = "发送中";
				}else if(25==r.getStatus()) {
					taskStatus = "已暂停";
				}else if(26==r.getStatus()) {
					taskStatus = "已取消";
				}else if(27==r.getStatus()) {
					taskStatus = "已完成";
				}else if(11==r.getStatus()) {
					taskStatus = "待批示";
				}else if(12==r.getStatus()) {
					taskStatus = "批示不通过";
				}else if(13==r.getStatus()) {
					taskStatus = "待审核";
				}else if(14==r.getStatus()) {
					taskStatus = "审核不通过";
				}else if(21==r.getStatus()) {
					taskStatus = "测试发送";
				}else if(0==r.getStatus()) {
					taskStatus = "草稿";
				}else {
					taskStatus = "未知";
				}
				content[15] = taskStatus;
				String time = "";
				if(r.getCreateTime() != null) {
					time += new DateTime(r.getCreateTime()).toString("yyyy-MM-dd HH:mm")+ "[创建时间]";
				}
				if(r.getBeginSendTime() != null) {
					time += new DateTime(r.getBeginSendTime()).toString("yyyy-MM-dd HH:mm")+ "[开始发送]";
				}
				if(r.getEndSendTime() != null) {
					time += new DateTime(r.getEndSendTime()).toString("yyyy-MM-dd HH:mm")+ "[结束发送]";
				}
				
				content[16] = time;
				csvWriter.writeNext(content);
				
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}
	
	private String getValue(Integer value,String defaultValue) {
		return value==null ? defaultValue : String.valueOf(value);
	}
	
	private String getValue(Double value,String defaultValue) {
		return value==null ? defaultValue : String.valueOf(value);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public List<Result> getAll(Map<String, Object> paramsMap) {
		return resultMapper.selectAll(paramsMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public Result getByTaskId(Integer taskId) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		Map<String, Object> paramsMap1 = Maps.newHashMap();
		Task task = taskService.getByTaskId(taskId);
		// 周期任务
		if(task!=null && task.getPlanId()!=null && "None".equals(task.getPlanReferer())){
			List<Task> tasks = taskService.getPlanTasksByPlanId(paramsMap, task.getPlanId());
			String planIdStr = "";
			for(Task t : tasks) {
				planIdStr += t.getPlanId() + ",";
			}
			Integer[] planIds = ConvertUtils.str2Int(planIdStr);
			paramsMap1.put("planIds", planIds);
			paramsMap1.put("status", Status.task("processing|completed"));
			Result result = resultMapper.selectPlanResultSum(paramsMap1);
			return result;
		}else{
			paramsMap1.put("taskId", taskId);
			List<Result> resultList = resultMapper.selectAll(paramsMap1);
			if(resultList != null && resultList.size()>0) {
				return resultList.get(0);
			}
		}
		return null;
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Result getByDomainStat(Map<String, Object> paramsMap) {
		return resultMapper.selectByDomainStat(paramsMap);
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Result getApiTriggerSum(Map<String, Object> paramsMap) {
		return resultMapper.selectApiTriggerSum(paramsMap);
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Pagination<Result> getApiTriggerPagination(
			Map<String, Object> paramsMap, String orderBy, int currentPage,
			int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectApiTriggerByUserList(paramsMap);
		long recordCount = resultMapper.selectApiTriggerByUserCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public void writeCsvApi(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		List<Result> resultList = resultMapper.selectApiByUserList(paramsMap);
		String[] title = new String[] { "客户账号", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数","点击总次数","退订总数","转发总数","无效人数" };
		csvWriter.writeNext(title);
		if(resultList != null&&resultList.size()>0) {
			String[] content = new String[title.length];
			int len = resultList.size();
			Result r = null;
			for(int i=0;i<len;i++) {
				r = resultList.get(i);
				content[0] = r.getUserId();
				content[1] = getValue(r.getSentCount(), "0");
				content[2] = getValue(r.getReachCount(), "0");
				content[3] = getValue(r.getSoftBounceCount()+r.getHardBounceCount(), "0");
				content[4] = getValue(r.getReadUserCount(), "0");
				content[5] = getValue(r.getReadCount(), "0");
				content[6] = getValue(r.getClickUserCount(), "0");
				content[7] = getValue(r.getClickCount(), "0");
				content[8] = getValue(r.getUnsubscribeCount(), "0");
				content[9] = getValue(r.getForwardCount(), "0");
				content[10] = getValue(r.getAfterNoneCount(), "0");
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public List<Result> getApiByUserList(Map<String, Object> paramsMap) {
		return resultMapper.selectApiByUserList(paramsMap);
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Pagination<Result> getPaginationGroupByTask(
			Map<String, Object> paramsMap, String orderBy, int currentPage,
			int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectApiByTaskList(paramsMap);
		long recordCount = resultMapper.selectApiByTaskCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}
	
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public void writeCsvApiGroupByTask(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		List<Result> resultList = resultMapper.selectAllApi(paramsMap);
//		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
//		List<Result> resultList = resultMapper.selectApiGroupsByTaskList(paramsMap);
		domainCopy(resultList);
		
		String[] title = new String[] {"任务名称", "邮件主题", "账号", "发送总数", "成功总数", "弹回总数", "打开人数", "打开次数", "点击人数", "点击次数", "退订总数", "转发总数", "无效人数", "时间 "};
		csvWriter.writeNext(title);
		if(resultList != null && resultList.size()>0) {
			String[] content = new String[title.length];
			int len = resultList.size();
			Result r = null;
			for(int i=0;i<len;i++) {
				r = resultList.get(i);
				content[0] = "\t" + r.getTaskName();
				content[1] = "\t" + r.getSubject();
				content[2] = "\t" + r.getUserId();
				
				content[3] = getValue(r.getSentCount(), "0");
				content[4] = getValue(r.getReachCount(), "0");
				content[5] = getValue(r.getSoftBounceCount()+r.getHardBounceCount(), "0");
				content[6] = getValue(r.getReadUserCount(), "0");
				content[7] = getValue(r.getReadCount(), "0");
				content[8] = getValue(r.getClickUserCount(), "0");
				content[9] = getValue(r.getClickCount(), "0");
				content[10] = getValue(r.getUnsubscribeCount(), "0");
				content[11] = getValue(r.getForwardCount(), "0");
				content[12] = getValue(r.getAfterNoneCount(), "0");
				
				String time = "";
				if(r.getCreateTime() != null) {
					time += new DateTime(r.getCreateTime()).toString("yyyy-MM-dd HH:mm");
				}
				
				content[13] = time + "[触发时间]";
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public List<Result> getAllApi(Map<String, Object> paramsMap) {
		return resultMapper.selectAllApi(paramsMap);
	}

	@Override
	public List<Result> getIsCanImportTask(List<Result> results) {
		String daysTemp = PropertiesUtil.get(ConfigKeys.PROPERTY_DAYS);
		
		/**
		 * days为app.properties的变量名property.days
		 */
		int days = 15;
		if(StringUtils.isNotEmpty(daysTemp)) {
			days = Integer.parseInt(daysTemp.trim());
		}
		if(results != null&&results.size()>0) {
			for(Result r:results) {
				DateTime createTime = new DateTime(r.getCreateTime());
				if(createTime.plusDays(days).isBeforeNow()) {
					r.setIsCanImport("Y");
				}else {
					r.setIsCanImport("N");
				}
			}
 		}
		
		return results;
	}

	@Override
	public void writeCsvOverview(CSVWriter csvWriter, Result result) {
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = { "发送总数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数","点击次数","退订人数","转发人数","无效人数","成功率","打开率","点击率"};
			csvWriter.writeNext(title2);
			String[] content2 = new String[title2.length];
			content2[0] = getValue(result.getSentCount(), "0");
			content2[1] = getValue(result.getReachCount(), "0");
			
			content2[2] = getValue(result.getSoftBounceCount()+result.getHardBounceCount(), "0");
			content2[3] = getValue(result.getReadUserCount(), "0");
			content2[4] = getValue(result.getReadCount(), "0");
			content2[5] = getValue(result.getClickUserCount(), "0");
			content2[6] = getValue(result.getClickCount(), "0");
			content2[7] = getValue(result.getUnsubscribeCount(), "0");
			content2[8] = getValue(result.getForwardCount(), "0");
			content2[9] = getValue(result.getAfterNoneCount(), "0");
			
			
			if(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()!=0) {
				content2[10] = getValue(result.getReachCount()*100.00/(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()), "0")+"%";
			}else {
				content2[10] = "0.00%";
			}
			
			if(result.getReachCount()!=0) {
				content2[11] = getValue(result.getReadUserCount()*100.00/result.getReachCount(), "0")+"%";
			}else {
				content2[11] = "0.00%";
			}
			
			if(result.getReadUserCount()!=0) {
				content2[12] = getValue(result.getClickUserCount()*100.00/result.getReadUserCount(), "0")+"%";
			}else {
				content2[12] = "0.00%";
			}
			csvWriter.writeNext(content2);
		}
	}
	
	private String getStatus(Integer status) {
		String taskStatus = "";
		if(22==status) {
			taskStatus = "待发送";
		}else if(23==status) {
			taskStatus = "队列中";
		}else if(24==status) {
			taskStatus = "发送中";
		}else if(25==status) {
			taskStatus = "已暂停";
		}else if(26==status) {
			taskStatus = "已取消";
		}else if(27==status) {
			taskStatus = "已完成";
		}else if(11==status) {
			taskStatus = "待批示";
		}else if(12==status) {
			taskStatus = "批示不通过";
		}else if(13==status) {
			taskStatus = "待审核";
		}else if(14==status) {
			taskStatus = "审核不通过";
		}else if(21==status) {
			taskStatus = "测试发送";
		}else if(0==status) {
			taskStatus = "草稿";
		}else {
			taskStatus = "未知";
		}
		return taskStatus;
	}

	@Override
	public void writeCsvSend(CSVWriter csvWriter, Result result, Domain domain) {
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = { "弹回详情", "弹回数", "占比"};
			csvWriter.writeNext(title2);
			
			String[] email = new String[title2.length];
			email[0] = "邮箱无效";
			email[1] = getValue(domain.getAfterNoneCount(), "0");
			if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
				email[2] = getValue(domain.getAfterNoneCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()), "0")+"%";;
			}else {
				email[2] = "0.00%";
			}
			csvWriter.writeNext(email);
			
			String[] dns = new String[title2.length];
			dns[0] = "域名无效";
			dns[1] = getValue(domain.getDnsFailCount(), "0");
			if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
				dns[2] = getValue(domain.getDnsFailCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()), "0")+"%";;
			}else {
				dns[2] = "0.00%";
			}
			csvWriter.writeNext(dns);
			
			
			String[] global = new String[title2.length];
			global[0] = "判定垃圾";
			global[1] = getValue(domain.getSpamGarbageCount(), "0");
			if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
				global[2] = getValue(domain.getSpamGarbageCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()), "0")+"%";;
			}else {
				global[2] = "0.00%";
			}
			csvWriter.writeNext(global);
			
			String[] other = new String[title2.length];
			other[0] = "其他";
			other[1] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount(), "0");
			if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
				other[2] = getValue((domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount())*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()), "0")+"%";;
			}else {
				other[2] = "0.00%";
			}
			csvWriter.writeNext(other);
		}
	}

	@Override
	public void writeCsvClick(CSVWriter csvWriter, Result result,
			List<Url> urlList, Integer totalClick) {
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = { "链接地址", "点击次数", "占比"};
			csvWriter.writeNext(title2);
			if(urlList==null||urlList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(int i=0;i<urlList.size();i++) {
					Url u = urlList.get(i);
					content2[0] = u.getUrl();
					content2[1] = getValue(u.getClickCount(), "0");
					if(totalClick!=0) {
						content2[2] =  getValue(u.getClickCount()*100.00/totalClick, "0")+"%";
					}else {
						content2[2] = "0.00%";
					}
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
		}
		
	}

	@Override
	public void writeCsvDomain(CSVWriter csvWriter, Result result,
			List<Domain> domainList) {
		
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = { "域名", "发送数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			if(domainList==null||domainList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(Domain d:domainList) {
					
					content2[0] = d.getEmailDomain();
					content2[1] = getValue(d.getSentCount(), "0");
					
					content2[2] = getValue(d.getReachCount(), "0");
					content2[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(), "0");
					content2[4] = getValue(d.getReadUserCount(), "0");
					content2[5] = getValue(d.getReadCount(), "0");
					content2[6] = getValue(d.getClickUserCount(), "0");
					content2[7] = getValue(d.getClickCount(), "0");
					content2[8] = getValue(d.getUnsubscribeCount(), "0");
					content2[9] = getValue(d.getForwardCount(), "0");
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
			
			
			
			
		}
	}

	@Override
	public void writeCsvRegion(CSVWriter csvWriter, Result result,
			List<Region> provinceRegionList, List<Region> cityRegionList) {
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = {"地区","打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			if(provinceRegionList==null||provinceRegionList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(Region pr:provinceRegionList) {
					content2[0] = pr.getProvinceName();
					content2[1] = getValue(pr.getReadUserCount(), "0");
					content2[2] = getValue(pr.getReadCount(), "0");
					content2[3] = getValue(pr.getClickUserCount(), "0");
					content2[4] = getValue(pr.getClickCount(), "0");
					content2[5] = getValue(pr.getUnsubscribeCount(), "0");
					content2[6] = getValue(pr.getForwardCount(), "0");
					csvWriter.writeNext(content2);
					if(cityRegionList!=null) {
						for(Region cr:cityRegionList){
							if(cr.getProvinceName().equals(pr.getProvinceName())) {
								content2[0] = "　　"+cr.getCityName();
								content2[1] = getValue(cr.getReadUserCount(), "0");
								content2[2] = getValue(cr.getReadCount(), "0");
								content2[3] = getValue(cr.getClickUserCount(), "0");
								content2[4] = getValue(cr.getClickCount(), "0");
								content2[5] = getValue(cr.getUnsubscribeCount(), "0");
								content2[6] = getValue(cr.getForwardCount(), "0");
								csvWriter.writeNext(content2);
							}
						}
					}
				}
				content2 = null;
			}
		}
	}

	@Override
	public void writeCsvOpen(CSVWriter csvWriter, Result result, List<Log> openList) {
		if(result==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			String[] title2 = {"邮件地址","所属地区", "打开时间"};
			csvWriter.writeNext(title2);
			if(openList==null||openList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(Log log:openList) {
					content2[0] = log.getEmail();
					content2[1] = log.getRegion();
					content2[2] = log.getTime();
					csvWriter.writeNext(content2);
				}
			}
		}
	}

	@Override
	public Pagination<Result> getPlanResultSumList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("planIds", planIds);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("status", Status.task("processing|completed"));
		List<Result> recordList = resultMapper.selectPlanResultSumList(paramsMap);
		long recordCount = resultMapper.selectPlanTaskCount(paramsMap);
		Pagination<Result> pagination = new Pagination<Result>(currentPage, pageSize, recordCount, recordList);
		return pagination;
	}
	
	@Override
	public List<Result> getViewDeliveryTasksByCorpId(Map<String, Object> paramsMap, int currentPage, int pageSize) {
		List<Result> resultList = resultMapper.selectViewDeliveryTasksByCorpId(paramsMap);
		pageSize = pageSize==0 ? 1000000 : pageSize;
		copyResultList(resultList, paramsMap, currentPage,  pageSize);
		return resultList;
	}

	@Override
	public Result getApiGroupsSum(Map<String, Object> paramsMap) {
		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		return resultMapper.selectApiGroupsSum(paramsMap);
	}

	@Override
	public Pagination<Result> getApiGroupsByUserPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		
		List<Result> recordList = resultMapper.selectApiGroupsByUserList(paramsMap);
		long recordCount = resultMapper.selectApiGroupsByUserCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public Pagination<Result> getApiGroupsByTaskPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		
		List<Result> recordList = resultMapper.selectApiGroupsByTaskList(paramsMap);
		long recordCount = resultMapper.selectApiGroupsByTaskCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public Pagination<Result> getAllApiTriggerPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectAllApi(paramsMap);
		long recordCount = resultMapper.selectAllApiCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public Result getTouchResult(Integer taskId, Integer templateId) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("taskId", taskId);
		paramsMap.put("templateId", templateId);
		return resultMapper.selectTouchResult(paramsMap);
	}
	
	private void domainCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getCorpId()).append(",");
		}
		List<Domain> domainList = domainService.getApiTriggerSumList(ConvertUtils.str2Int(sbff.toString()));
		ResultDataOption.copy("triger", domainList, sourceResults);
	}

}
