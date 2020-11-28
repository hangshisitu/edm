package cn.edm.service;

import java.util.List;
import java.util.Map;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.domain.Campaign;
import cn.edm.domain.CustomerReport;
import cn.edm.domain.Domain;
import cn.edm.domain.Plan;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Url;
import cn.edm.util.Pagination;
import cn.edm.vo.Log;
import cn.edm.vo.Send;

public interface DomainService {
	
	/**
	 * 获取总览统计
	 * @param paramsMap
	 * @return
	 */
	Domain getSum(Map<String, Object> paramsMap);
	
	/**
	 * 获取总览统计
	 * @param paramsMap
	 * @return
	 */
	List<Domain> getSumList(Map<String, Object> paramsMap);
	/**
	 * 分页查询客户任务统计
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Domain> getPagination(Map<String, Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 把内容写到csv文件
	 * @param csvWriter
	 * @param paramsMap
	 */
	void writeCsv(CSVWriter csvWriter,Map<String, Object> paramsMap);
	
	/**
	 * 查询统计信息
	 * @param paramsMap
	 * @return
	 */
	List<Domain> getDomainList(Map<String, Object> paramsMap);
	
	/**
	 *  根据任务id按域名分组查询
	 * @param paramsMap
	 * @return
	 */
	List<Domain> getByTaskIdAndGroupByDomain(Map<String, Object> paramsMap);
	
	/**
	 * 按域名分页查询
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Domain> getPaginationByDomain(Map<String, Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	
	Domain getApiSum(Map<String, Object> paramsMap);
	
	
	void writeHisCsv(CSVWriter csvWriter,Map<String,Domain> allMap);
	
	Domain getSendBackSum(Map<String, Object> paramsMap);
	/**
	 * API群发类统计
	 * @param paramsMap
	 * @return
	 */
	public Domain getApiGroupsSum(Map<String, Object> paramsMap);
	///////////////////////////////////////////////////////////////////////////////////
	// 修改如下
	///////////////////////////////////////////////////////////////////////////////////
	/**
	 * 分页查询客户发送统计
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getPaginationByCustomer(Map<String, Object> paramsMap,String orderBy,int currentPage,int pageSize);
	/**
	 * 根据客户统计API触发
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getApiTriggerPagination(Map<String, Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	public Pagination<Domain> getPaginationGroupByTask(Map<String, Object> paramsMap,String orderBy,int currentPage,int pageSize);
	/**
	 * 获取用户API群发任务统计分页列表
	 * @param paramsMap
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getApiGroupsByUserPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	
	/**
	 * 查询所有API触发类任务 列表
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getAllApiTriggerPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	
	public List<Domain> getApiByUserList(Map<String, Object> paramsMap);
	/**
	 * 客户API群发任务分页列表
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getApiGroupsByTaskPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 为isCanImport属性赋值 Y/N </br>
	 * days 天之内可以进行导入操作  </br>
	 * Y为可导入</br>
	 * N为否导入</br>
	 * 
	 * @param results
	 * @return Result列表results
	 */
	public List<Domain> getIsCanImportTask(List<Domain> results);
	
	/**
	 * 把内容写到csv文件(客户统计api触发类)
	 * @param csvWriter
	 * @param paramsMap
	 */
	public void writeCsvApi(CSVWriter csvWriter,Map<String, Object> paramsMap);
	/**
	 * 分页周期任务统计结果列表(有投递效果的总和)
	 * @param paramsMap
	 * @param planIds
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getPlanResultSumList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize);
	
	public void writeCsvApiGroupByTask(CSVWriter csvWriter, Map<String, Object> paramsMap);
	
	public List<Domain> getAllApi(Map<String, Object> paramsMap);
	/**
	 * 根据任务id查询
	 * @param paramsMap
	 * @return
	 */
	public Domain getByTaskId(Integer taskId);
	/**
	 * API触发统计
	 * @param paramsMap
	 * @return
	 */
	public Domain getApiTriggerSum(Map<String, Object> paramsMap);
	/**
	 * API触发统计
	 * @param corpIds
	 * @return
	 */
	public List<Domain> getApiTriggerSumList(Integer[] corpIds);
	/**
	 * API历史投递统计 导出
	 * @param csvWriter
	 * @param paramsMap
	 */
	public void writeCsvApiHis(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 通过任务ID获得周期任务统计信息
	 * @param planId
	 * @return
	 */
	public Domain getPlanResultByPlanId(Integer planId);
	/**
	 * 分页周期任务统计结果列表(有投递效果)
	 * @param paramsMap
	 * @param planIds
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getResultList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize);
	/**
	 * 导出周期任务统计到csv
	 * @param csvWriter
	 * @param paramsMap
	 */
	public void writePlanCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 分页周期任务统计结果列表(没投递效果)
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getPaginationPlan(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 周期任务投递统计汇总
	 * @param paramsMap
	 * @param planIds
	 * @return
	 */
	public Domain getPlanResultSum(Map<String, Object> paramsMap, Integer[] planIds);
	/**
	 * 导出周期任务统计汇总到csv
	 * @param csvWriter
	 * @param paramsMap
	 */
	public void writePlanCollectCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 分页查询任务发送统计
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Domain> getResultPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 查询任务统计
	 * @param paramsMap
	 * @return
	 */
	public List<Domain> getAll(Map<String, Object> paramsMap);
	/**
	 * 导出
	 * @param csvWriter
	 * @param paramsMap
	 */
	public void writeDomainCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 导出概览
	 * @param csvWriter
	 * @param result
	 */
	public void writeCsvOverview(CSVWriter csvWriter, Domain domain);
	/**
	 * 触发任务发送合计统计
	 * @param paramsMap
	 * @return
	 */
	public Domain getTouchSum(Map<String, Object> paramsMap);
	
	public void writeCsvOpen(CSVWriter csvWriter, Domain domain, List<Log> openList);
	
	public void writeCsvClick(CSVWriter csvWriter, Domain domain, List<Url> urlList, Integer totalClick);

	public void writeCsvRegion(CSVWriter csvWriter, Domain domain, List<Region> provinceRegionList, List<Region> cityRegionList);
	
	public void writeCsvTouch(CSVWriter csvWriter, List<Domain> domainList, Task task, Plan plan, Result result, String type);
	
	public void writeCsvRecipients(CSVWriter csvWriter, List<Send> sendList, List<Log> openList, List<Log> clickList);
	
	public List<Domain> getTouchList(Map<String, Object> paramsMap);
	
	public List<Domain> getCampaignTouchList(Map<String, Object> paramsMap);
	
	public List<Domain> getDomainSum(Map<String, Object> paramsMap);
	
	public void writeCsvCampaignTouch(CSVWriter csvWriter, Map<String, List<Domain>> domainMap, Integer[] taskIds, Campaign campaign);
	
	public Domain getByDomainStat(Map<String, Object> paramsMap);
	
	public List<Domain> getByDomainSumByCorp(Integer[] taskIds);
	
	public List<Domain> getDomainSumByTask(Integer[] taskIds);
	
	public void writeApiGroupCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	
	public CustomerReport getCustomerReportList(Map<String, Object> topCorpMap,Map<String, Object> paramsMap);
	
}
