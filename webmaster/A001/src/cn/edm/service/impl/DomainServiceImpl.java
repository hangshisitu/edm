package cn.edm.service.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

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
import cn.edm.domain.Campaign;
import cn.edm.domain.Corp;
import cn.edm.domain.CustomerReport;
import cn.edm.domain.Domain;
import cn.edm.domain.Plan;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Touch;
import cn.edm.domain.TouchSetting;
import cn.edm.domain.Url;
import cn.edm.persistence.CorpMapper;
import cn.edm.persistence.DomainMapper;
import cn.edm.persistence.ResultMapper;
import cn.edm.service.CorpService;
import cn.edm.service.DomainService;
import cn.edm.service.PlanService;
import cn.edm.service.ResultService;
import cn.edm.service.TaskService;
import cn.edm.service.TouchService;
import cn.edm.service.TouchSettingService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.DateUtil;
import cn.edm.util.Pagination;
import cn.edm.util.PropertiesUtil;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.ResultDataOption;
import cn.edm.vo.Log;
import cn.edm.vo.Send;

import com.google.common.collect.Maps;

@Service
public class DomainServiceImpl implements DomainService{
	
	@Autowired
	private DomainMapper domainMapper;
	@Autowired
	private ResultMapper resultMapper;
	@Autowired
	private PlanService planService;
	@Autowired
	private TaskService taskService;
	@Autowired
	private ResultService resultService;
	@Autowired
	private TouchSettingService settingService;
	@Autowired
	private TouchService touchService;	
	@Autowired
	private CorpMapper corpMapper;
	@Autowired
	private CorpService corpService;
	
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public Domain getSum(Map<String, Object> paramsMap) {		
		return domainMapper.selectSum(paramsMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Domain> getSumList(Map<String, Object> paramsMap) {
		return domainMapper.selectSumList(paramsMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public Pagination<Domain> getPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		
		//所有一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("parentId", 0);			
		CustomerReport report = getCustomerReportList(topCorpMap, paramsMap);
	
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage,pageSize,report.getRecordCount(),report.getRecordList());
		return pagination;
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<Domain> getResultPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Domain> recordList = domainMapper.selectResultList(paramsMap);
		long recordCount = domainMapper.selectResultCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public void writeCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
//		List<Domain> domainList = domainMapper.selectUserStatList(paramsMap);
		//指定的一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("corpIds", paramsMap.get("corpIds"));
		CustomerReport report = getCustomerReportList(topCorpMap, paramsMap);
		List<Domain> recordList = report.getRecordList();
		
		String[] title = new String[] {"机构名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数","点击总次数","退订总数","转发总数","无效人数"};
		csvWriter.writeNext(title);
		if(recordList != null && recordList.size()>0) {
			String[] content = new String[title.length];
			int len = recordList.size();
			Domain d = null;
			for(int i=0;i<len;i++) {
				d = recordList.get(i);
				content[0] = d.getCompanyName();
				content[1] = getValue(d.getSentCount(),"0");
				content[2] = getValue(d.getReachCount(),"0");
				content[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(),"0");
				content[4] = getValue(d.getReadUserCount(),"0");
				content[5] = getValue(d.getReadCount(),"0");
				content[6] = getValue(d.getClickUserCount(),"0");
				content[7] = getValue(d.getClickCount(),"0");
				content[8] = getValue(d.getUnsubscribeCount(),"0");
				content[9] = getValue(d.getForwardCount(),"0");
				content[10] = getValue(d.getAfterNoneCount(),"0");
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
	}
	
	private String getValue(Integer value, String defaultValue) {
		return value==null ? defaultValue:String.valueOf(value);
	}
	
	private String getValue(Double value,String defaultValue) {
		return value==null ? defaultValue : String.valueOf(value);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public List<Domain> getDomainList(Map<String, Object> paramsMap) {		
		return domainMapper.selectUserStatList(paramsMap);
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public List<Domain> getByTaskIdAndGroupByDomain(Map<String, Object> paramsMap) {
		return domainMapper.selectByTaskIdAndGroupByDomain(paramsMap);
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Pagination<Domain> getPaginationByDomain(
			Map<String, Object> paramsMap, String orderBy, int currentPage,
			int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Domain> recordList = domainMapper.selectByTaskIdAndGroupByDomain(paramsMap);
		long recordCount = domainMapper.selectByTaskIdAndGroupByDomainCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage,pageSize,recordCount,recordList);
		return pagination;
	}

	@Override
	//@Cacheable(cacheName="reportCache")
	public Domain getApiSum(Map<String, Object> paramsMap) {
		return domainMapper.selectApiSum(paramsMap);
	}

	@Override
	public void writeHisCsv(CSVWriter csvWriter, Map<String, Domain> allMap) {
		String[] title = new String[] { "域名", "发送总数","成功总数", "弹回总数","无效人数", "打开人数", "打开次数", "点击人数","点击次数","退订总数","转发总数","成功率","打开率","点击率" };
		csvWriter.writeNext(title);
		csvWriter.writeNext(getContent(allMap.get("domain139")==null?new Domain():allMap.get("domain139"),"139邮箱"));
		csvWriter.writeNext(getContent(allMap.get("domain163")==null?new Domain():allMap.get("domain163"),"网易邮箱"));
		csvWriter.writeNext(getContent(allMap.get("domainGmail")==null?new Domain():allMap.get("domainGmail"),"Gmail邮箱"));
		csvWriter.writeNext(getContent(allMap.get("domainQQ")==null?new Domain():allMap.get("domainQQ"),"QQ邮箱"));
		csvWriter.writeNext(getContent(allMap.get("domainSina")==null?new Domain():allMap.get("domainSina"),"新浪邮箱"));
		csvWriter.writeNext(getContent(allMap.get("domainSohu")==null?new Domain():allMap.get("domainSohu"),"搜狐邮箱"));
		csvWriter.writeNext(getContent(allMap.get("ohterDomain")==null?new Domain():allMap.get("ohterDomain"),"其他"));
		csvWriter.writeNext(getContent(allMap.get("domain")==null?new Domain():allMap.get("domain"),"合计"));
	}
	
	private String[] getContent(Domain d,String domainName) {
		String[] content = new String[14];
		content[0] = domainName;
		content[1] = getValue(d.getSentCount(),"0");
		content[2] = getValue(d.getReachCount(),"0");
		content[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(),"0");
		content[4] = getValue(d.getAfterNoneCount(),"0");
		content[5] = getValue(d.getReadUserCount(),"0");
		content[6] = getValue(d.getReadCount(),"0");
		content[7] = getValue(d.getClickUserCount(),"0");
		content[8] = getValue(d.getClickCount(),"0");
		content[9] = getValue(d.getUnsubscribeCount(),"0");
		content[10] = getValue(d.getForwardCount(),"0");
		content[11] = d.getSuccessRate();
		content[12] = d.getOpenRate();
		content[13] = d.getClickRate();
		return content;
	}

	@Override
	public Domain getSendBackSum(Map<String, Object> paramsMap) {
		return domainMapper.selectSendBackSum(paramsMap);
	}

	@Override
	public Domain getApiGroupsSum(Map<String, Object> paramsMap) {
		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		return domainMapper.selectApiGroupsSum(paramsMap);
	}

	@Override
	public Pagination<Domain> getAllApiTriggerPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Domain> recordList = domainMapper.selectAllApi(paramsMap);
		long recordCount = domainMapper.selectAllApiCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public List<Domain> getApiByUserList(Map<String, Object> paramsMap) {
		return domainMapper.selectApiByUserList(paramsMap);
	}

	@Override
	public Pagination<Domain> getApiGroupsByTaskPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
		
		List<Domain> recordList = domainMapper.selectApiGroupsByTaskList(paramsMap);
		long recordCount = resultMapper.selectApiGroupsByTaskCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public List<Domain> getIsCanImportTask(List<Domain> domains) {
		String daysTemp = PropertiesUtil.get(ConfigKeys.PROPERTY_DAYS);
		
		/**
		 * days为app.properties的变量名property.days
		 */
		int days = 15;
		if(StringUtils.isNotEmpty(daysTemp)) {
			days = Integer.parseInt(daysTemp.trim());
		}
		if(domains != null&&domains.size()>0) {
			for(Domain d : domains) {
				DateTime createTime = new DateTime(d.getCreateTime());
				if(createTime.plusDays(days).isBeforeNow()) {
					d.setIsCanImport("Y");
				}else {
					d.setIsCanImport("N");
				}
			}
 		}
		
		return domains;
	}

	@Override
	public void writeCsvApi(CSVWriter csvWriter, Map<String, Object> paramsMap) {		
		//指定的一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("corpIds", paramsMap.get("corpIds"));
		CustomerReport report = getCustomerReportAPIList(topCorpMap, paramsMap);
		List<Domain> recordList = report.getRecordList();	
		
		String[] title = new String[] { "机构名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数","点击总次数","退订总数","转发总数","无效人数" };
		csvWriter.writeNext(title);
		if(recordList!=null && recordList.size()>0) {
			String[] content = new String[title.length];
			int len = recordList.size();
			Domain d = null;
			for(int i=0; i<len; i++) {
				d = recordList.get(i);
				content[0] = d.getCompanyName();
				content[1] = getValue(d.getSentCount(), "0");
				content[2] = getValue(d.getReachCount(), "0");
				content[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(), "0");
				content[4] = getValue(d.getReadUserCount(), "0");
				content[5] = getValue(d.getReadCount(), "0");
				content[6] = getValue(d.getClickUserCount(), "0");
				content[7] = getValue(d.getClickCount(), "0");
				content[8] = getValue(d.getUnsubscribeCount(), "0");
				content[9] = getValue(d.getForwardCount(), "0");
				content[10] = getValue(d.getAfterNoneCount(), "0");
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}

	@Override
	public Pagination<Domain> getPaginationByCustomer(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_PARENT_PLAN};
		paramsMap.put("types", types);
		
		Map<String, Object> pMap = Maps.newHashMap();
		pMap.putAll(paramsMap);
		List<Domain> results = this.getViewDeliveryTasksByCorpId(paramsMap);
		long recordCount = domainMapper.selectViewDeliveryTaskCount(pMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, results);
		return pagination;
	}

	@Override
	public Pagination<Domain> getApiTriggerPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);		
		//所有一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("parentId", 0);	
		CustomerReport report = getCustomerReportAPIList(topCorpMap, paramsMap);		
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage,pageSize,report.getRecordCount(),report.getRecordList());
		
		return pagination;
	}

	@Override
	public Pagination<Domain> getPaginationGroupByTask(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Domain> recordList = domainMapper.selectApiByTaskList(paramsMap);
		long recordCount = domainMapper.selectApiByTaskCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	public Pagination<Domain> getApiGroupsByUserPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);		
		//所有一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("parentId", 0);			
		CustomerReport report = getCustomerReportApiGroupList(topCorpMap, paramsMap);		
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, report.getRecordCount(), report.getRecordList());
		
		return pagination;
	}
	
	private List<Domain> getViewDeliveryTasksByCorpId(Map<String, Object> paramsMap) {
		List<Domain> domainList = domainMapper.selectViewDeliveryTasksByCorpId(paramsMap);
		copyResultList(domainList, paramsMap);
		return domainList;
	}
	
	@Override
	public Pagination<Domain> getPlanResultSumList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("planIds", planIds);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("status", Status.task("processing|completed"));
		paramsMap.put("type", TaskTypeConstant.TASK_SON_PLAN);
		List<Domain> recordList = domainMapper.selectPlanResultSumList(paramsMap);
		long recordCount = domainMapper.selectPlanTaskCount(paramsMap);
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		return pagination;
	}
	
	/**
	 * 复制Result值
	 * 从Result查询数据，复制Result的投递效果的统计数据到results对应属性中
	 * @param domains
	 */
	private void copyResultList(List<Domain> domains, Map<String,Object> realParamsMap) {
		// 周期任务
		String pIds = "";
		String tIds = "";
		for(int i=0,len=domains.size(); i<len; i++) {
			if(domains.get(i).getPlanId()!=null && !"".equals(domains.get(i).getPlanId())){
				pIds += domains.get(i).getPlanId() + ",";
			}
			if(domains.get(i).getTaskId()!=null && !"".equals(domains.get(i).getTaskId())){
				tIds += domains.get(i).getTaskId() + ",";
			}
		}
		if(pIds!=null && !"".equals(pIds)){
			pIds = pIds.substring(0, pIds.length()-1);
		}
		Map<String, Object> planMap = Maps.newHashMap();
		planMap.put("planIds", Converts._toIntegers(pIds));
		List<Domain> ds = domainMapper.selectPlanResultSumList(planMap);
		String ignore = "corpId, taskId, templateId, campaignId, campaignName, taskName, status, deliveryTime, touchCount,corpPath";
		for(Domain newD : ds) {
			if(newD.getPlanId()==null || "".equals(newD.getPlanId())){
				continue;
			}
			for(Domain oldD : domains) {
				if(oldD.getPlanId()==null || "".equals(oldD.getPlanId())){
					continue;
				}
				if(newD.getPlanId().equals(oldD.getPlanId())){
					String[] copy = Converts._toStrings(ignore);
		            BeanUtils.copyProperties(newD, oldD, copy);
		            break;
				}
			}
		}
		// 单项任务
		Map<String, Object> singleParamMap = Maps.newHashMap();
		singleParamMap.put("corpId", realParamsMap.get("corpId"));
		singleParamMap.put("planCnd", "isNull");
		singleParamMap.put("taskIds", Converts._toIntegers(tIds));
		
		List<Domain> ds1 = domainMapper.selectDomainSum(singleParamMap);
		for(Domain r : domains) {
			for(Domain newD : ds1){
				if(newD.getTaskId().equals(r.getTaskId())){
					String[] copy = Converts._toStrings(ignore);
					BeanUtils.copyProperties(newD, r, copy);
					break;
				}
			}
		}
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public void writeCsvApiGroupByTask(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		List<Domain> domainList = domainMapper.selectAllApi(paramsMap);
		String[] title = new String[] {"任务名称", "邮件主题", "账号", "发送总数", "成功总数", "弹回总数", "打开人数", "打开次数", "点击人数", "点击次数", "退订总数", "转发总数", "无效人数", "时间 "};
		csvWriter.writeNext(title);
		if(domainList != null && domainList.size()>0) {
			String[] content = new String[title.length];
			int len = domainList.size();
			Domain d = null;
			for(int i=0;i<len;i++) {
				d = domainList.get(i);
				content[0] = "\t" + d.getTaskName();
				content[1] = "\t" + d.getSubject();
				content[2] = "\t" + d.getUserId();
				
				content[3] = getValue(d.getSentCount(), "0");
				content[4] = getValue(d.getReachCount(), "0");
				content[5] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(), "0");
				content[6] = getValue(d.getReadUserCount(), "0");
				content[7] = getValue(d.getReadCount(), "0");
				content[8] = getValue(d.getClickUserCount(), "0");
				content[9] = getValue(d.getClickCount(), "0");
				content[10] = getValue(d.getUnsubscribeCount(), "0");
				content[11] = getValue(d.getForwardCount(), "0");
				content[12] = getValue(d.getAfterNoneCount(), "0");
				
				String time = "";
				if(d.getCreateTime() != null) {
					time += new DateTime(d.getCreateTime()).toString("yyyy-MM-dd HH:mm");
				}
				
				content[13] = time + "[触发时间]";
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}

	@Override
	public List<Domain> getAllApi(Map<String, Object> paramsMap) {
		return domainMapper.selectAllApi(paramsMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public Domain getByTaskId(Integer taskId) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		Map<String, Object> paramsMap1 = Maps.newHashMap();
		Task task = taskService.getByTaskId(taskId);
		// 周期任务
		if(task.getPlanId()!=null && "None".equals(task.getPlanReferer())){
			List<Task> tasks = taskService.getPlanTasksByPlanId(paramsMap, task.getPlanId());
			String planIdStr = "";
			for(Task t : tasks) {
				planIdStr += t.getPlanId() + ",";
			}
			Integer[] planIds = ConvertUtils.str2Int(planIdStr);
			paramsMap1.put("planIds", planIds);
			paramsMap1.put("status", Status.task("processing|completed"));
			Domain domain = domainMapper.selectPlanResultSum(paramsMap1);
			return domain;
		}else{
			paramsMap1.put("taskId", taskId);
			List<Domain> domainList = domainMapper.selectAll(paramsMap1);
			if(domainList != null && domainList.size()>0) {
				return domainList.get(0);
			}
		}
		return null;
	}
	
	@Override
	public void writeCsvApiHis(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		// API触发类
		Domain dt = this.getApiTriggerSum(paramsMap);
		// API群发类
		Domain dg = this.getApiGroupsSum(paramsMap);
		if(dt == null){
			dt = new Domain();
		}
		if(dg == null){
			dg = new Domain();
		}
		// 触发任务与群发任务合计
		Domain resultTotal = ResultDataOption.statResultTotal(dt, dg);
		
		String[] title = new String[] { "API类型", "发送总数", "成功总数", "弹回总数", "无效人数", "打开人数", "打开次数", "点击人数", "点击次数", "退订总数", "转发总数", "成功率", "打开率", "点击率" };
		csvWriter.writeNext(title);
		
		String[] content1 = new String[title.length];
		content1[0] = "触发任务";
		content1[1] = getValue(dt.getSentCount(), "0");
		content1[2] = getValue(dt.getReachCount(), "0");
		content1[3] = getValue(dt.getSoftBounceCount() + dt.getHardBounceCount(), "0");
		content1[4] = getValue(dt.getAfterNoneCount(), "0");
		content1[5] = getValue(dt.getReadUserCount(), "0");
		content1[6] = getValue(dt.getReadCount(), "0");
		content1[7] = getValue(dt.getClickUserCount(), "0");
		content1[8] = getValue(dt.getClickCount(), "0");
		content1[9] = getValue(dt.getUnsubscribeCount(), "0");
		content1[10] = getValue(dt.getForwardCount(), "0");
		
		content1[11] = getValue(dt.getResultReach(), "0.00") + "%";
		content1[12] = getValue(dt.getResultRead(), "0.00") + "%";
		content1[13] = getValue(dt.getResultClick(), "0.00") + "%";
		csvWriter.writeNext(content1);
		
		String[] content2 = new String[title.length];
		content2[0] = "群发任务";
		content2[1] = getValue(dg.getSentCount(), "0");
		content2[2] = getValue(dg.getReachCount(), "0");
		content2[3] = getValue(dg.getSoftBounceCount() + dg.getHardBounceCount(), "0");
		content2[4] = getValue(dg.getAfterNoneCount(), "0");
		content2[5] = getValue(dg.getReadUserCount(), "0");
		content2[6] = getValue(dg.getReadCount(), "0");
		content2[7] = getValue(dg.getClickUserCount(), "0");
		content2[8] = getValue(dg.getClickCount(), "0");
		content2[9] = getValue(dg.getUnsubscribeCount(), "0");
		content2[10] = getValue(dg.getForwardCount(), "0");
		
		content2[11] = getValue(dg.getResultReach(), "0.00") + "%";
		content2[12] = getValue(dg.getResultRead(), "0.00") + "%";
		content2[13] = getValue(dg.getResultClick(), "0.00") + "%";
		csvWriter.writeNext(content2);
		
		String[] content3 = new String[title.length];
		content3[0] = "合计";
		content3[1] = getValue(resultTotal.getSentCount(), "0");
		content3[2] = getValue(resultTotal.getReachCount(), "0");
		content3[3] = getValue(resultTotal.getSoftBounceCount() + resultTotal.getHardBounceCount(), "0");
		content3[4] = getValue(resultTotal.getAfterNoneCount(), "0");
		content3[5] = getValue(resultTotal.getReadUserCount(), "0");
		content3[6] = getValue(resultTotal.getReadCount(), "0");
		content3[7] = getValue(resultTotal.getClickUserCount(), "0");
		content3[8] = getValue(resultTotal.getClickCount(), "0");
		content3[9] = getValue(resultTotal.getUnsubscribeCount(), "0");
		content3[10] = getValue(resultTotal.getForwardCount(), "0");
		
		content3[11] = getValue(resultTotal.getResultReach(), "0.00") + "%";
		content3[12] = getValue(resultTotal.getResultRead(), "0.00") + "%";
		content3[13] = getValue(resultTotal.getResultClick(), "0.00") + "%";
		csvWriter.writeNext(content3);
		
	}
	@Override
	//@Cacheable(cacheName="reportCache")
	public Domain getApiTriggerSum(Map<String, Object> paramsMap) {
		List<Domain> domainList = domainMapper.selectApiTriggerSum(paramsMap);
		return (domainList!=null && domainList.size()>0) ? domainList.get(0) : null;
	}

	@Override
	public Domain getPlanResultByPlanId(Integer planId) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("status", Status.task("processing|completed"));
		paramsMap.put("planId", planId);
		List<Domain> resultList = domainMapper.selectPlanResultSumList(paramsMap);
		if(resultList != null && resultList.size()>0) {
			return resultList.get(0);
		} 
		return null;
	}

	@Override
	public Pagination<Domain> getResultList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize) {
		paramsMap.clear();
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("planIds", planIds);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("status", Status.task("processing|completed"));
		List<Domain> recordList = domainMapper.selectResultHisList(paramsMap);
		long recordCount = domainMapper.selectPlanTaskCount(paramsMap);
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public void writePlanCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		paramsMap.put("status", Status.task("processing|completed"));
		List<Domain> domainList = domainMapper.selectPlanResultSumList(paramsMap);
		// 周期任务信息
		String[] title = { "任务名称", "邮件主题", "周期时间", "投递次数", "有效期" };
		Integer planId = (Integer) paramsMap.get("planId");
		Task task = taskService.getPlanTaskByPlanId(paramsMap, planId, TaskTypeConstant.TASK_PARENT_PLAN);
		Plan plan = planService.getPlanById(paramsMap, planId);
		plan.setTranslateCron(plan.getCron());
		// 投递次数
		int taskCount = taskService.getTaskCount(paramsMap, planId);
		String begin = DateUtil.getDateByForamt(plan.getBeginTime(), "yyyy-MM-dd HH:mm");
		String end = plan.getEndTime() != null ? DateUtil.getDateByForamt(plan.getEndTime(), "yyyy-MM-dd HH:mm") : "现在";
		csvWriter.writeNext(title);
		String[] content1 = new String[]{ task.getTaskName(), task.getSubject(), plan.getTranslateCron(), taskCount+"", begin + " 至 " + end};
		csvWriter.writeNext(content1);
		csvWriter.writeNext(new String[] {});
		if(domainList != null && domainList.size()>0) {
			Domain domain = domainList.get(0);
			// 概览
			csvWriter.writeNext(new String[] { "#概览#" });
			String[] title2 = { "发送总数", "成功数", "弹回数", "打开人数","打开次数","点击人数","点击次数","成功率", "打开率", "点击率"};
			csvWriter.writeNext(title2);
			String[] content2 = new String[title2.length];
			content2[0] = getValue(domain.getSentCount(), "0");
			content2[1] = getValue(domain.getReachCount(), "0");
			
			content2[2] = getValue(domain.getSoftBounceCount() + domain.getHardBounceCount(), "0");
			content2[3] = getValue(domain.getReadUserCount(), "0");
			content2[4] = getValue(domain.getReadCount(), "0");
			content2[5] = getValue(domain.getClickUserCount(), "0");
			content2[6] = getValue(domain.getClickCount(), "0");
			if(domain.getSentCount() - domain.getAfterNoneCount()!=0) {
				content2[7] = domain.getResultReach() + "%";
			}else {
				content2[7] = "0.00%";
			}
			
			if(domain.getReachCount()!=0) {
				content2[8] = domain.getResultRead() + "%";
			}else {
				content2[8] = "0.00%";
			}
			
			if(domain.getReadUserCount()!=0) {
				content2[9] = domain.getResultClick() + "%";
			}else {
				content2[9] = "0.00%";
			}
			csvWriter.writeNext(content2);
			
			csvWriter.writeNext(new String[]{});
			
			// 投递历史
			csvWriter.writeNext(new String[] { "#投递历史#" });
			List<Domain> hisResults = domainMapper.selectResultHisList(paramsMap);
			String[] title3 = { "投递时间", "发送状态", "发送总数", "成功数", "弹回数", "打开人数","打开次数", "点击人数","点击次数", "成功率", "打开率", "点击率" };
			csvWriter.writeNext(title3);
			String[] content3 = new String[title3.length];
			for(Domain hisResult : hisResults) {
				content3[0] = hisResult.getDeliveryTime()!=null ? DateUtil.getDateByForamt(hisResult.getDeliveryTime(), "yyyy-MM-dd HH:mm") : "";
				content3[1] = getStatus(hisResult.getStatus());
				content3[2] = getValue(hisResult.getSentCount(), "0");
				content3[3] = getValue(hisResult.getReachCount(), "0");
				
				content3[4] = getValue(hisResult.getSoftBounceCount() + hisResult.getHardBounceCount(), "0");
				content3[5] = getValue(hisResult.getReadUserCount(), "0");
				content3[6] = getValue(hisResult.getReadCount(), "0");
				content3[7] = getValue(hisResult.getClickUserCount(), "0");
				content3[8] = getValue(hisResult.getClickCount(), "0");
				if(hisResult.getSentCount()-hisResult.getBeforeNoneCount()-hisResult.getAfterNoneCount()!=0) {
					content3[9] = getValue(hisResult.getReachCount()*100.00/(hisResult.getSentCount()-hisResult.getBeforeNoneCount()-hisResult.getAfterNoneCount()), "0")+"%";
				}else {
					content3[9] = "0.00%";
				}
				
				if(hisResult.getReachCount()!=0) {
					content3[10] = getValue(hisResult.getReadUserCount()*100.00/hisResult.getReachCount(), "0")+"%";
				}else {
					content3[10] = "0.00%";
				}
				
				if(hisResult.getReadUserCount()!=0) {
					content3[11] = getValue(hisResult.getClickUserCount()*100.00/hisResult.getReadUserCount(), "0")+"%";
				}else {
					content3[11] = "0.00%";
				}
				csvWriter.writeNext(content3);
				
			}
			csvWriter.writeNext(new String[]{});
			// 触发
			List<Task> taskList = taskService.getTouchTask(planId, "plan");
	        String taskIdStr = "";
	        for(Task t : taskList) {
	        	taskIdStr += t.getTaskId() + ",";
	        }
	        Integer[] taskIds = Converts._toIntegers(taskIdStr);
	        Map<String, Object> pMap = Maps.newHashMap();
	        List<TouchSetting> settingList = settingService.getSettings(pMap, task.getTemplateId(), taskIds);
	    	List<Touch> touchList = touchService.getByParentIds(taskIds);
	    	String touchIdStr = ""; 
	    	for(int i=0,len=touchList.size(); i<len; i++) {
	    		Touch t = touchList.get(i);
	    		touchIdStr += t.getTaskId() + ",";
	    	}
	    	Integer[] touchIds = Converts._toIntegers(touchIdStr);
	    	if(touchIds!=null && touchIds.length>0) {
	    		csvWriter.writeNext(new String[] { "#触发#" });
	    		paramsMap.clear();
	    		paramsMap.put("taskIds", touchIds);
	    		List<Domain> touchDomainList = this.getTouchList(paramsMap);
	    		for(Domain d : touchDomainList) {
	    			Domain d2 = this.getPlanResultSum(paramsMap, new Integer[]{ planId });
	    			Integer parentReadUserCount = 0;
	            	if(d2 != null){
	            		parentReadUserCount = d2.getReadUserCount();
	            	}
	            	d.setParentReadUserCount(parentReadUserCount);
	    		}
	    		ResultDataOption.copyUrlsToDomain(settingList, touchDomainList);
	    		
	    		for(Domain d : touchDomainList) {
	    			String touchUrls = "";
					List<String> urls = d.getUrlList();
					if(urls != null){
						for(String url : urls) {
							touchUrls += url + "；";
						}
					}
					csvWriter.writeNext(new String[]{ "触点链接", touchUrls });
					
					String[] title4 = {"模板名称", "触发总数", "成功数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数", "触发率", "成功率", "打开率", "点击率"};
		    		csvWriter.writeNext(title4);
		    		String[] content4 = new String[title4.length];
		    		
	    			content4[0] = d.getTemplateName();
	    			content4[1] = getValue(d.getTouchCount(), "0");
	    			content4[2] = getValue(d.getReachCount(), "0");
	    			content4[3] = getValue(d.getReadUserCount(), "0");
	    			content4[4] = getValue(d.getReadCount(), "0");
	    			content4[5] = getValue(d.getClickUserCount(), "0");
	    			content4[6] = getValue(d.getClickCount(), "0");
	    			content4[7] = getValue(d.getUnsubscribeCount(), "0");
	    			content4[8] = getValue(d.getForwardCount(), "0");
	    			
	    			content4[9] = getValue(d.getTouchRate(), "0.00") + "%";
	    			content4[10] = getValue(d.getResultReach(), "0.00") + "%";
	    			content4[11] = getValue(d.getResultRead(), "0.00") + "%";
	    			content4[12] = getValue(d.getResultClick(), "0.00") + "%";
	    			csvWriter.writeNext(content4);
	    			csvWriter.writeNext(new String[]{"",""});
	    		}
	    	}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
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
	public Pagination<Domain> getPaginationPlan(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		paramsMap.put("type", TaskTypeConstant.TASK_PARENT_PLAN);
		List<Domain> recordList = domainMapper.selectPlanTaskList(paramsMap);
		long recordCount = domainMapper.selectPlanTaskCount(paramsMap);
		
		Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, recordCount, recordList);
		
		return pagination;
	}

	@Override
	public Domain getPlanResultSum(Map<String, Object> paramsMap, Integer[] planIds) {
		paramsMap.put("planIds", planIds);
		paramsMap.put("status", Status.task("processing|completed"));
		Domain domain = domainMapper.selectPlanResultSum(paramsMap);
		return domain;
	}

	@Override
	public void writePlanCollectCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		paramsMap.put("status", Status.task("processing|completed"));
		List<Domain> domains = domainMapper.selectPlanResultSumList(paramsMap);
		if(domains!=null && domains.size()>0){
			String[] title = { "任务名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数", "点击总次数", "退订总数", "转发总数", "无效人数", "成功率", "打开率", "点击率"};
			int[] contentTotal = new int[title.length + 1];
			csvWriter.writeNext(title);
			for(Domain result : domains) {
				String[] content = new String[title.length];
				content[0] = result.getTaskName();
				content[1] = getValue(result.getSentCount(), "0");
				content[2] = getValue(result.getReachCount(), "0");
				content[3] = getValue(result.getSoftBounceCount() + result.getHardBounceCount(), "0");
				content[4] = getValue(result.getReadUserCount(), "0");
				content[5] = getValue(result.getReadCount(), "0");
				content[6] = getValue(result.getClickUserCount(), "0");
				content[7] = getValue(result.getClickCount(), "0");
				content[8] = getValue(result.getUnsubscribeCount(), "0");
				content[9] = getValue(result.getForwardCount(), "0");
				content[10] = getValue(result.getAfterNoneCount(), "0");
				
				if(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()!=0) {
					content[11] = getValue(result.getReachCount()*100.00/(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()), "0")+"%";
				}else {
					content[11] = "0.00%";
				}
				
				if(result.getReachCount()!=0) {
					content[12] = getValue(result.getReadUserCount()*100.00/result.getReachCount(), "0")+"%";
				}else {
					content[12] = "0.00%";
				}
				
				if(result.getReadUserCount()!=0) {
					content[13] = getValue(result.getClickUserCount()*100.00/result.getReadUserCount(), "0")+"%";
				}else {
					content[13] = "0.00%";
				}
				csvWriter.writeNext(content);
				/*
				 * 合计
				 */
				contentTotal[1]	+= Integer.valueOf(content[1]);
				contentTotal[2] += Integer.valueOf(content[2]);
				contentTotal[3] += Integer.valueOf(content[3]);
				contentTotal[4] += Integer.valueOf(content[4]);
				contentTotal[5] += Integer.valueOf(content[5]);
				contentTotal[6] += Integer.valueOf(content[6]);
				contentTotal[7] += Integer.valueOf(content[7]);
				contentTotal[8] += Integer.valueOf(content[8]);
				contentTotal[9] += Integer.valueOf(content[9]);
				contentTotal[10] += Integer.valueOf(content[10]);
				
				contentTotal[14] += result.getBeforeNoneCount()==null ? 0 : result.getBeforeNoneCount();
				
			}
			String[] contentTotalStr = new String[title.length];
			contentTotalStr[0] = "合计";
			if(contentTotal[1]-contentTotal[14]-contentTotal[10]!=0) {
				contentTotalStr[11] = getValue(contentTotal[2]*100.00/(contentTotal[1]-contentTotal[14]-contentTotal[10]), "0") + "%";
			}else {
				contentTotalStr[11] = "0.00%";
			}
			
			if(contentTotal[2] != 0) {
				contentTotalStr[12] = getValue(contentTotal[4]*100.00 / contentTotal[2], "0") + "%";
			}else {
				contentTotalStr[12] = "0.00%";
			}
			
			if(contentTotal[4] != 0) {
				contentTotalStr[13] = getValue(contentTotal[6]*100.00/ contentTotal[4], "0") + "%";
			}else {
				contentTotalStr[13] = "0.00%";
			}
			/**
			 * 复制数组contentTotal的值到contentTotalStr
			 * len=contentTotalStr.length-3 为后面统计百份比不复制
			 */
			for(int i=1,len=contentTotalStr.length-3; i<len; i++) {
				contentTotalStr[i] = getValue(contentTotal[i], "0");
			}
			csvWriter.writeNext(contentTotalStr);
			
		}else{
			csvWriter.writeNext(new String[]{"暂无数据"});
		}
		
	}
	
	@Override
	public List<Domain> getDomainSumByTask(Integer[] taskIds) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("taskIds", taskIds);
		return domainMapper.selectDomainSumByTask(paramsMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public void writeDomainCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		List<Domain> domainList = this.getViewDeliveryTasksByCorpId(paramsMap);//resultMapper.selectViewDeliveryTasksByCorpId(paramsMap);
		String[] title = new String[] {"任务名称", "任务类型", "邮件主题", "账号", "公司名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数", "点击总次数", "退订总数", "转发总数", "无效人数", "触发计划", "任务状态", "时间"};
		csvWriter.writeNext(title);
		if(domainList != null && domainList.size()>0) {
			String[] content = new String[title.length];
			int len = domainList.size();
			Domain d = null;
			for(int i=0;i<len;i++) {
				d = domainList.get(i);
				content[0] = "\t"+d.getTaskName();
				String taskType = "单项任务";
				if(d.getTaskType() == 1){
					taskType = "测试任务";
				}else if(d.getTaskType()==3 || d.getTaskType()==2){
					taskType = "周期任务";
				}else if(d.getTaskType() == 4){
					taskType = "API群发任务";
				}
				content[1] = "\t"+taskType;
				content[2] = "\t"+d.getSubject();
				content[3] = d.getUserId();
				content[4] = "\t"+d.getCompanyName();
				content[5] = getValue(d.getSentCount(), "0");
				content[6] = getValue(d.getReachCount(), "0");
				content[7] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(), "0");
				content[8] = getValue(d.getReadUserCount(), "0");
				content[9] = getValue(d.getReadCount(), "0");
				content[10] = getValue(d.getClickUserCount(), "0");
				content[11] = getValue(d.getClickCount(), "0");
				content[12] = getValue(d.getUnsubscribeCount(), "0");
				content[13] = getValue(d.getForwardCount(), "0");
				content[14] = getValue(d.getAfterNoneCount(), "0");
				Integer touchCount = d.getTouchCount();
				if(touchCount == 0){
					content[15] = "无";
				}else{
					content[15] = "有";
				}
				String taskStatus = "";
				if(22==d.getStatus()) {
					taskStatus = "待发送";
				}else if(23==d.getStatus()) {
					taskStatus = "队列中";
				}else if(24==d.getStatus()) {
					taskStatus = "发送中";
				}else if(25==d.getStatus()) {
					taskStatus = "已暂停";
				}else if(26==d.getStatus()) {
					taskStatus = "已取消";
				}else if(27==d.getStatus()) {
					taskStatus = "已完成";
				}else if(11==d.getStatus()) {
					taskStatus = "待批示";
				}else if(12==d.getStatus()) {
					taskStatus = "批示不通过";
				}else if(13==d.getStatus()) {
					taskStatus = "待审核";
				}else if(14==d.getStatus()) {
					taskStatus = "审核不通过";
				}else if(21==d.getStatus()) {
					taskStatus = "测试发送";
				}else if(0==d.getStatus()) {
					taskStatus = "草稿";
				}else {
					taskStatus = "未知";
				}
				content[16] = taskStatus;
				String time = "";
				Map<String, Object> paramsMap1 = Maps.newHashMap();
				paramsMap1.put("taskId", d.getTaskId());
				List<Result> rs = resultMapper.selectResultList(paramsMap1);
				Result r = null;
				if(rs!=null && rs.size()>0){
					r = rs.get(0);
					if(d.getCreateTime() != null) {
						time += new DateTime(d.getCreateTime()).toString("yyyy-MM-dd HH:mm")+ "[创建时间]";
					}
					if(r.getBeginSendTime() != null) {
						time += new DateTime(r.getBeginSendTime()).toString("yyyy-MM-dd HH:mm")+ "[开始发送]";
					}
					if(r.getEndSendTime() != null) {
						time += new DateTime(r.getEndSendTime()).toString("yyyy-MM-dd HH:mm")+ "[结束发送]";
					}
				}
				
				content[17] = time;
				csvWriter.writeNext(content);
				
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
		
	}
	
	@Override
	public void writeCsvOverview(CSVWriter csvWriter, Domain domain) {
		if(domain==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			Result result = resultService.getByTaskId(domain.getTaskId());
			String begin = DateUtil.getDateByForamt(result.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			String end = result.getEndSendTime()!=null?DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm"):"现在";
			String[] content = {result.getTaskName(),result.getSubject(),getStatus(result.getStatus()),begin+"至"+end};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{"",""});
			
			String[] title2 = { "发送总数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数","点击次数","退订人数","转发人数","无效人数","成功率","打开率","点击率"};
			csvWriter.writeNext(title2);
			String[] content2 = new String[title2.length];
			content2[0] = getValue(domain.getSentCount(), "0");
			content2[1] = getValue(domain.getReachCount(), "0");
			
			content2[2] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount(), "0");
			content2[3] = getValue(domain.getReadUserCount(), "0");
			content2[4] = getValue(domain.getReadCount(), "0");
			content2[5] = getValue(domain.getClickUserCount(), "0");
			content2[6] = getValue(domain.getClickCount(), "0");
			content2[7] = getValue(domain.getUnsubscribeCount(), "0");
			content2[8] = getValue(domain.getForwardCount(), "0");
			content2[9] = getValue(domain.getAfterNoneCount(), "0");
			
			
			if(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount()!=0) {
				content2[10] = getValue(domain.getReachCount()*100.00/(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount()), "0")+"%";
			}else {
				content2[10] = "0.00%";
			}
			
			if(domain.getReachCount()!=0) {
				content2[11] = getValue(domain.getReadUserCount()*100.00/domain.getReachCount(), "0")+"%";
			}else {
				content2[11] = "0.00%";
			}
			
			if(domain.getReadUserCount()!=0) {
				content2[12] = getValue(domain.getClickUserCount()*100.00/domain.getReadUserCount(), "0")+"%";
			}else {
				content2[12] = "0.00%";
			}
			csvWriter.writeNext(content2);
		}
	}

	@Override
	public Domain getTouchSum(Map<String, Object> paramsMap) {
		return  domainMapper.selectTouchSum(paramsMap);
	}
	
	@Override
	public void writeCsvOpen(CSVWriter csvWriter, Domain domain, List<Log> openList) {
		if(domain==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(domain.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			Result r = resultService.getByTaskId(domain.getTaskId());
			String end = r.getEndSendTime()!=null ? DateUtil.getDateByForamt(r.getEndSendTime(), "yyyy-MM-dd HH:mm") : "现在";
			String[] content = {domain.getTaskName(),domain.getSubject(),getStatus(domain.getStatus()),begin+"至"+end};
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
	public void writeCsvClick(CSVWriter csvWriter, Domain domain, List<Url> urlList, Integer totalClick) {
		if(domain==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(domain.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			Result r = resultService.getByTaskId(domain.getTaskId());
			String end = r.getEndSendTime()!=null ? DateUtil.getDateByForamt(r.getEndSendTime(), "yyyy-MM-dd HH:mm") : "现在";
			String[] content = {domain.getTaskName(),domain.getSubject(),getStatus(domain.getStatus()),begin+"至"+end};
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
	public void writeCsvRegion(CSVWriter csvWriter, Domain domain, List<Region> provinceRegionList, List<Region> cityRegionList) {
		if(domain==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"任务","邮件主题","当前状态","投递时间"};
			csvWriter.writeNext(title);
			String begin = DateUtil.getDateByForamt(domain.getDeliveryTime(), "yyyy-MM-dd HH:mm");
			Result r = resultService.getByTaskId(domain.getTaskId());
			String end = r.getEndSendTime()!=null ? DateUtil.getDateByForamt(r.getEndSendTime(), "yyyy-MM-dd HH:mm") : "现在";
			String[] content = {domain.getTaskName(),domain.getSubject(),getStatus(domain.getStatus()),begin+"至"+end};
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
	public void writeCsvTouch(CSVWriter csvWriter, List<Domain> domainList, Task task, Plan plan, Result result, String type) {
		
		if(domainList==null || domainList.size()<0) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String begin = "";
			String end = "";
			if("plan".equals(type)){
				if(plan != null){
					begin = DateUtil.getDateByForamt(plan.getBeginTime(), "yyyy-MM-dd HH:mm");
					end = plan.getEndTime()!=null ? DateUtil.getDateByForamt(plan.getEndTime(), "yyyy-MM-dd HH:mm") : "现在";
				}
				String header = "任务名称,邮件主题,周期时间,已投递次数,有效期";
				csvWriter.writeNext(Converts._toStrings(header));
		        if (plan==null) return;
		        String name = task.getTaskName();
		        String subject = task.getSubject();
		        plan.setTranslateCron(plan.getCron());
		        String cron = plan.getTranslateCron();
		        String taskCount = String.valueOf(plan.getTaskCount());
		        csvWriter.writeNext(new String[] { name, subject, cron, taskCount, begin + "至" + end });
			}else{
				if(result != null){
					begin = DateUtil.getDateByForamt(task.getDeliveryTime(), "yyyy-MM-dd HH:mm");
					end = result.getEndSendTime()!=null ? DateUtil.getDateByForamt(result.getEndSendTime(), "yyyy-MM-dd HH:mm") : "现在";
				}
				String header = "任务名称,邮件主题,当前状态,投递时间";
				csvWriter.writeNext(Converts._toStrings(header));
		        if (task == null) return;
		        String name = task.getTaskName();
		        String subject = task.getSubject();
		        String status = getStatus(task.getStatus());
		        
		        csvWriter.writeNext(new String[] { name, subject, status, begin + "至" + end });
			}
			csvWriter.writeNext(new String[]{"",""});
			for(Domain d : domainList) {
				String touchUrls = "";
				List<String> urls = d.getUrlList();
				if(urls != null){
					for(String url : urls) {
						touchUrls += url + "；";
					}
				}
				csvWriter.writeNext(new String[]{ "触点链接", touchUrls });
					
				String[] title2 = {"模板名称", "触发总数", "成功数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数", "触发率", "成功率", "打开率", "点击率"};
				csvWriter.writeNext(title2);
				
				String[] content2 = new String[title2.length];
				content2[0] = d.getTemplateName();
				content2[1] = getValue(d.getTouchCount(), "0");
				content2[2] = getValue(d.getReachCount(), "0");
				content2[3] = getValue(d.getReadUserCount(), "0");
				content2[4] = getValue(d.getReadCount(), "0");
				content2[5] = getValue(d.getClickUserCount(), "0");
				content2[6] = getValue(d.getClickCount(), "0");
				content2[7] = getValue(d.getUnsubscribeCount(), "0");
				content2[8] = getValue(d.getForwardCount(), "0");
				
				content2[9] = getValue(d.getTouchRate(), "0.00") + "%";
				content2[10] = getValue(d.getResultReach(), "0.00") + "%";
				content2[11] = getValue(d.getResultRead(), "0.00") + "%";
				content2[12] = getValue(d.getResultClick(), "0.00") + "%";
				csvWriter.writeNext(content2);
				csvWriter.writeNext(new String[]{"",""});
			}
			
		}
		
	}
	/**
	 * 导出触发－－收件人详情
	 */
	@Override
	public void writeCsvRecipients(CSVWriter csvWriter, List<Send> sendList, List<Log> openList, List<Log> clickList) {
		String header = "收件人,发送,打开,点击";
    	csvWriter.writeNext(Converts._toStrings(header));

    	String[] contents = new String[4];
    	int totalSend = 0;
    	int totalOpen = 0;
    	int totalClick = 0;
    	if(!Asserts.empty(sendList)){
    		for(int i=0,len=sendList.size(); i<len; i++) {
    			String email = sendList.get(i).getEmail();
    			if(StringUtils.isBlank(email)) {
    				continue;
    			}
    			// 发送
    			contents[0] = email;
    			contents[1] = "1";
    			totalSend++;
    			// 打开
    			boolean iso = false;
    			for(Log o : openList) {
    				if(email.equals(o.getEmail())){
    					contents[2] = "1";
    					totalOpen++;
    					iso = true;
    					continue;
    				}
    			}
    			if(!iso) contents[2] = "0";
    			// 点击
    			boolean isc = false;
    			for(Log c : clickList) {
    				if(email.equals(c.getEmail())){
    					contents[3] = "1";
    					totalClick++;
    					isc = true;
    					continue;
    				}
    			}
    			if(!isc) contents[3] = "0";
    			csvWriter.writeNext(contents);
    		}
    		csvWriter.writeNext(new String[] { "合计", totalSend+"", totalOpen+"", totalClick+""});
    	}
		
	}

	@Override
	public List<Domain> getTouchList(Map<String, Object> paramsMap) {
		return domainMapper.selectTouchList(paramsMap);
	}

	@Override
	public List<Domain> getDomainSum(Map<String, Object> paramsMap) {
		return domainMapper.selectDomainSum(paramsMap);
	}

	@Override
	public List<Domain> getCampaignTouchList(Map<String, Object> paramsMap) {
		return domainMapper.selectCampaignTouchList(paramsMap);
	}

	@Override
	public void writeCsvCampaignTouch(CSVWriter csvWriter, Map<String, List<Domain>> domainMap, Integer[] taskIds, Campaign campaign) {
		String header = "活动名称,活动描述,投递次数,更新时间";
    	csvWriter.writeNext(Converts._toStrings(header));
    	csvWriter.writeNext(new String[] { campaign.getCampaignName(), campaign.getCampaignDesc(), campaign.getTaskCount()+"", campaign.getLatestTime() });
    	csvWriter.writeNext(new String[]{"",""});
		if(!Asserts.empty(domainMap)) {
			Iterator<Entry<String, List<Domain>>> it = domainMap.entrySet().iterator();
	    	while(it.hasNext()) {
	    		Map.Entry<String, List<Domain>> entry = (Entry<String, List<Domain>>) it.next();
	    		String taskName = entry.getKey();
	    		List<Domain> domainList = entry.getValue();
	    		for(Integer taskId : taskIds) {
	    			String endStr = "_" + taskId;
	    			if(taskName.contains(endStr)){
	    				taskName = taskName.substring(0, taskName.lastIndexOf(endStr));
	    			}
	    		}
	    		csvWriter.writeNext(new String[]{"任务来源", taskName});
	    		String[] title2 = {"模板名称", "触发总数", "成功数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数", "触发率", "成功率", "打开率", "点击率"};
	    		csvWriter.writeNext(title2);
	    		for(Domain d : domainList) {
	    			
	    			String[] content2 = new String[title2.length];
	    			content2[0] = d.getTemplateName();
	    			content2[1] = getValue(d.getTouchCount(), "0");
	    			content2[2] = getValue(d.getReachCount(), "0");
	    			content2[3] = getValue(d.getReadUserCount(), "0");
	    			content2[4] = getValue(d.getReadCount(), "0");
	    			content2[5] = getValue(d.getClickUserCount(), "0");
	    			content2[6] = getValue(d.getClickCount(), "0");
	    			content2[7] = getValue(d.getUnsubscribeCount(), "0");
	    			content2[8] = getValue(d.getForwardCount(), "0");
	    			
	    			content2[9] = getValue(d.getTouchRate(), "0.00") + "%";
	    			content2[10] = getValue(d.getResultReach(), "0.00") + "%";
	    			content2[11] = getValue(d.getResultRead(), "0.00") + "%";
	    			content2[12] = getValue(d.getResultClick(), "0.00") + "%";
	    			csvWriter.writeNext(content2);
	    		}
	    		csvWriter.writeNext(new String[]{"",""});
	    	}
		}else{
			csvWriter.writeNext(new String[]{ "暂无数据" });
		}
	}

	@Override
	public Domain getByDomainStat(Map<String, Object> paramsMap) {
		return domainMapper.selectByDomainStat(paramsMap);
	}

	@Override
	public List<Domain> getApiTriggerSumList(Integer[] corpIds) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("corpIds", corpIds);
		return domainMapper.selectApiTriggerSumList(paramsMap);
	}

	@Override
	public List<Domain> getByDomainSumByCorp(Integer[] taskIds) {
		Map<String, Object> paramsMap = Maps.newHashMap();
		paramsMap.put("taskIds", taskIds);
		return domainMapper.selectByDomainSumByCorp(paramsMap);
	}

	@Override
	public List<Domain> getAll(Map<String, Object> paramsMap) {
		return domainMapper.selectAll(paramsMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	//@Cacheable(cacheName="reportCache")
	public void writeApiGroupCsv(CSVWriter csvWriter, Map<String, Object> paramsMap) {
		//指定的一级机构
		Map<String,Object> topCorpMap = new HashMap<String, Object>();
		topCorpMap.put("corpIds", paramsMap.get("corpIds"));
		CustomerReport report = getCustomerReportApiGroupList(topCorpMap, paramsMap);
		List<Domain> recordList = report.getRecordList();
		
		String[] title = new String[] {"机构名称", "发送总数", "成功总数", "弹回总数", "打开总人数", "打开总次数", "点击总人数","点击总次数","退订总数","转发总数","无效人数"};
		csvWriter.writeNext(title);
		if(recordList != null && recordList.size()>0) {
			String[] content = new String[title.length];
			int len = recordList.size();
			Domain d = null;
			for(int i=0;i<len;i++) {
				d = recordList.get(i);
				content[0] = d.getCompanyName();
				content[1] = getValue(d.getSentCount(),"0");
				content[2] = getValue(d.getReachCount(),"0");
				content[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(),"0");
				content[4] = getValue(d.getReadUserCount(),"0");
				content[5] = getValue(d.getReadCount(),"0");
				content[6] = getValue(d.getClickUserCount(),"0");
				content[7] = getValue(d.getClickCount(),"0");
				content[8] = getValue(d.getUnsubscribeCount(),"0");
				content[9] = getValue(d.getForwardCount(),"0");
				content[10] = getValue(d.getAfterNoneCount(),"0");
				csvWriter.writeNext(content);
			}
		}else {
			csvWriter.writeNext(new String[]{"未找到记录"});
		}
	}
	
	
	public CustomerReport getCustomerReportList(Map<String, Object> topCorpMap,Map<String, Object> paramsMap){		
        List<Domain> recordList = new ArrayList<Domain>();		
		Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN};
		paramsMap.put("types", types);
			
		List<Corp> corpList = corpService.getCorpByTopLevel(topCorpMap);
		//key:一级机构corp_id,value:一级机构统计下所有子机构的数据
		Map<Integer, List<Domain>> map = new HashMap<Integer, List<Domain>>();
		
		//先检查根据一级机构统计客户发送--投递发送类，哪些一级机构有数据，用于按一级机构统计
		for(Corp corp : corpList){
			paramsMap.put("corpIds", corp.getBranchCorp());//一级机构下属所有子机构													
			List<Domain> DomainList = domainMapper.selectDomainResultList(paramsMap);
			if(DomainList ==null || DomainList.size() == 0){
				continue;
			}else{
				map.put(corp.getCorpId(), DomainList);				
			}							
		}
		long recordCount = 0;
		if(!map.isEmpty()){
			paramsMap.put("corpIds", map.keySet());
			corpList = corpService.getCorpByTopLevel(paramsMap);//重新查询有数据的一级机构，用于前台分页
			recordCount = corpMapper.getCorpCount(paramsMap);
			
			for(Corp corp : corpList){
				Domain domain = new Domain();
				domain.setCorpId(corp.getCorpId());
				domain.setCompanyName(corp.getCompany());
				
				List<Domain> DomainList = map.get(corp.getCorpId());
				//所有下属机构的发送统计数相加
				for(Domain domainDetail : DomainList){
					domain.setSentCount(domain.getSentCount() + domainDetail.getSentCount());
					domain.setReachCount(domain.getReachCount() + domainDetail.getReachCount());
					domain.setReadCount(domain.getReadCount() + domainDetail.getReadCount());
					domain.setReadUserCount(domain.getReadUserCount() + domainDetail.getReadUserCount());
					domain.setClickCount(domain.getClickCount() + domainDetail.getClickCount());
					domain.setClickUserCount(domain.getClickUserCount() + domainDetail.getClickUserCount());
					domain.setUnsubscribeCount(domain.getUnsubscribeCount() + domainDetail.getUnsubscribeCount());
					domain.setForwardCount(domain.getForwardCount() + domainDetail.getForwardCount());
					domain.setSoftBounceCount(domain.getSoftBounceCount() + domainDetail.getSoftBounceCount());
					domain.setHardBounceCount(domain.getHardBounceCount() + domainDetail.getHardBounceCount());
					domain.setBeforeNoneCount(domain.getBeforeNoneCount() + domainDetail.getBeforeNoneCount());
					domain.setAfterNoneCount(domain.getAfterNoneCount() + domainDetail.getAfterNoneCount());
					domain.setDnsFailCount(domain.getDnsFailCount() + domainDetail.getDnsFailCount());
					domain.setSpamGarbageCount(domain.getSpamGarbageCount() + domainDetail.getSpamGarbageCount());
				}
				recordList.add(domain);				
			}
		}
		CustomerReport report = new CustomerReport();
		report.setRecordCount(recordCount);
		report.setRecordList(recordList);
		
		return report;
	}
	
	
	private CustomerReport getCustomerReportAPIList(Map<String, Object> topCorpMap,Map<String, Object> paramsMap){		
        List<Domain> recordList = new ArrayList<Domain>();		
			
		List<Corp> corpList = corpService.getCorpByTopLevel(topCorpMap);
		//key:一级机构corp_id,value:一级机构统计下所有子机构的数据
		Map<Integer, List<Domain>> map = new HashMap<Integer, List<Domain>>();
		
		//先检查根据一级机构统计客户发送--投递发送类，哪些一级机构有数据，用于按一级机构统计
		for(Corp corp : corpList){
			paramsMap.put("corpIds", corp.getBranchCorp());//一级机构下属所有子机构				
			List<Domain> DomainList = domainMapper.selectApiTriggerByUserList(paramsMap);
			if(DomainList ==null || DomainList.size() == 0){
				continue;
			}else{
				map.put(corp.getCorpId(), DomainList);				
			}							
		}
		long recordCount = 0;
		if(!map.isEmpty()){
			paramsMap.put("corpIds", map.keySet());
			corpList = corpService.getCorpByTopLevel(paramsMap);//重新查询有数据的一级机构，用于前台分页
			recordCount = corpMapper.getCorpCount(paramsMap);
			
			for(Corp corp : corpList){
				Domain domain = new Domain();
				domain.setCorpId(corp.getCorpId());
				domain.setCompanyName(corp.getCompany());
				
				List<Domain> DomainList = map.get(corp.getCorpId());
				//所有下属机构的发送统计数相加
				for(Domain domainDetail : DomainList){
					domain.setSentCount(domain.getSentCount() + domainDetail.getSentCount());
					domain.setReachCount(domain.getReachCount() + domainDetail.getReachCount());
					domain.setReadCount(domain.getReadCount() + domainDetail.getReadCount());
					domain.setReadUserCount(domain.getReadUserCount() + domainDetail.getReadUserCount());
					domain.setClickCount(domain.getClickCount() + domainDetail.getClickCount());
					domain.setClickUserCount(domain.getClickUserCount() + domainDetail.getClickUserCount());
					domain.setUnsubscribeCount(domain.getUnsubscribeCount() + domainDetail.getUnsubscribeCount());
					domain.setForwardCount(domain.getForwardCount() + domainDetail.getForwardCount());
					domain.setSoftBounceCount(domain.getSoftBounceCount() + domainDetail.getSoftBounceCount());
					domain.setHardBounceCount(domain.getHardBounceCount() + domainDetail.getHardBounceCount());
					domain.setBeforeNoneCount(domain.getBeforeNoneCount() + domainDetail.getBeforeNoneCount());
					domain.setAfterNoneCount(domain.getAfterNoneCount() + domainDetail.getAfterNoneCount());
					domain.setDnsFailCount(domain.getDnsFailCount() + domainDetail.getDnsFailCount());
					domain.setSpamGarbageCount(domain.getSpamGarbageCount() + domainDetail.getSpamGarbageCount());
				}
				recordList.add(domain);				
			}
		}
		CustomerReport report = new CustomerReport();
		report.setRecordCount(recordCount);
		report.setRecordList(recordList);
		
		return report;
	}
	
	
	private CustomerReport getCustomerReportApiGroupList(Map<String, Object> topCorpMap,Map<String, Object> paramsMap){		
        List<Domain> recordList = new ArrayList<Domain>();		
        paramsMap.put("type", TaskTypeConstant.TASK_API_GROUPS);
			
		List<Corp> corpList = corpService.getCorpByTopLevel(topCorpMap);
		//key:一级机构corp_id,value:一级机构统计下所有子机构的数据
		Map<Integer, List<Domain>> map = new HashMap<Integer, List<Domain>>();
		
		//先检查根据一级机构统计客户发送--投递发送类，哪些一级机构有数据，用于按一级机构统计
		for(Corp corp : corpList){
			paramsMap.put("corpIds", corp.getBranchCorp());//一级机构下属所有子机构													
			List<Domain> DomainList = domainMapper.selectApiGroupsByUserList(paramsMap);
			if(DomainList ==null || DomainList.size() == 0){
				continue;
			}else{
				map.put(corp.getCorpId(), DomainList);				
			}							
		}
		long recordCount = 0;
		if(!map.isEmpty()){
			paramsMap.put("corpIds", map.keySet());
			corpList = corpService.getCorpByTopLevel(paramsMap);//重新查询有数据的一级机构，用于前台分页
			recordCount = corpMapper.getCorpCount(paramsMap);
			
			for(Corp corp : corpList){
				Domain domain = new Domain();
				domain.setCorpId(corp.getCorpId());
				domain.setCompanyName(corp.getCompany());
				
				List<Domain> DomainList = map.get(corp.getCorpId());
				//所有下属机构的发送统计数相加
				for(Domain domainDetail : DomainList){
					domain.setSentCount(domain.getSentCount() + domainDetail.getSentCount());
					domain.setReachCount(domain.getReachCount() + domainDetail.getReachCount());
					domain.setReadCount(domain.getReadCount() + domainDetail.getReadCount());
					domain.setReadUserCount(domain.getReadUserCount() + domainDetail.getReadUserCount());
					domain.setClickCount(domain.getClickCount() + domainDetail.getClickCount());
					domain.setClickUserCount(domain.getClickUserCount() + domainDetail.getClickUserCount());
					domain.setUnsubscribeCount(domain.getUnsubscribeCount() + domainDetail.getUnsubscribeCount());
					domain.setForwardCount(domain.getForwardCount() + domainDetail.getForwardCount());
					domain.setSoftBounceCount(domain.getSoftBounceCount() + domainDetail.getSoftBounceCount());
					domain.setHardBounceCount(domain.getHardBounceCount() + domainDetail.getHardBounceCount());
					domain.setBeforeNoneCount(domain.getBeforeNoneCount() + domainDetail.getBeforeNoneCount());
					domain.setAfterNoneCount(domain.getAfterNoneCount() + domainDetail.getAfterNoneCount());
					domain.setDnsFailCount(domain.getDnsFailCount() + domainDetail.getDnsFailCount());
					domain.setSpamGarbageCount(domain.getSpamGarbageCount() + domainDetail.getSpamGarbageCount());
				}
				recordList.add(domain);				
			}
		}
		CustomerReport report = new CustomerReport();
		report.setRecordCount(recordCount);
		report.setRecordList(recordList);
		
		return report;
	}

}
