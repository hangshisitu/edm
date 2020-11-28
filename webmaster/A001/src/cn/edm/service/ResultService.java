package cn.edm.service;

import java.util.List;
import java.util.Map;

import au.com.bytecode.opencsv.CSVWriter;

import cn.edm.domain.Domain;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Url;
import cn.edm.util.Pagination;
import cn.edm.vo.Log;

/**
 * @author Luxh
 *
 */
public interface ResultService {
	
	/**
	 * 分页查询任务统计记录
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Result> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	/**
	 * 分页查询客户发送统计
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
//	Pagination<Result> getPaginationByCustomer(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 把内容写到csv文件
	 * @param csvWriter
	 * @param paramsMap
	 */
	void writeCsv(CSVWriter csvWriter,Map<String,Object> paramsMap);
	
	/**
	 * 查询任务统计
	 * @param paramsMap
	 * @return
	 */
	List<Result> getAll(Map<String,Object> paramsMap);
	
	/**
	 * 根据任务id查询
	 * @param paramsMap
	 * @return
	 */
	Result getByTaskId(Integer taskId);
	
	/**
	 * 根据发送域名统计
	 * @param paramsMap
	 * @return
	 */
	Result getByDomainStat(Map<String,Object> paramsMap);
	
	/**
	 * API触发统计
	 * @param paramsMap
	 * @return
	 */
	Result getApiTriggerSum(Map<String,Object> paramsMap);
	
	/**
	 * 根据客户统计API触发
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Result> getApiTriggerPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	/**
	 * 把内容写到csv文件(客户统计api触发类)
	 * @param csvWriter
	 * @param paramsMap
	 */
	void writeCsvApi(CSVWriter csvWriter,Map<String,Object> paramsMap);
	
	List<Result> getApiByUserList(Map<String,Object> paramsMap);
	
	Pagination<Result> getPaginationGroupByTask(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	void writeCsvApiGroupByTask(CSVWriter csvWriter, Map<String, Object> paramsMap);
	
	List<Result> getAllApi(Map<String,Object> paramsMap);
	/**
	 * 为isCanImport属性赋值 Y/N </br>
	 * days 天之内可以进行导入操作  </br>
	 * Y为可导入</br>
	 * N为否导入</br>
	 * 
	 * @param results
	 * @return Result列表results
	 */
	List<Result> getIsCanImportTask(List<Result> results);

	void writeCsvOverview(CSVWriter csvWriter, Result result);

	void writeCsvSend(CSVWriter csvWriter, Result result, Domain domain);

	void writeCsvClick(CSVWriter csvWriter, Result result, List<Url> urlList, Integer totalClick);

	void writeCsvDomain(CSVWriter csvWriter, Result result, List<Domain> domainList);

	void writeCsvRegion(CSVWriter csvWriter, Result result, List<Region> provinceRegionList, List<Region> cityRegionList);

	void writeCsvOpen(CSVWriter csvWriter, Result result, List<Log> openList);
	/**
	 * 分页周期任务统计结果列表(没投递效果)
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
//	public Pagination<Result> getPaginationPlan(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 分页周期任务统计结果列表(有投递效果)
	 * @param paramsMap
	 * @param planIds
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
//	public Pagination<Result> getResultList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize);
	/**
	 * 分页周期任务统计结果列表(有投递效果的总和)
	 * @param paramsMap
	 * @param planIds
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Result> getPlanResultSumList(Map<String, Object> paramsMap, Integer[] planIds, String orderBy, int currentPage, int pageSize);
	/**
	 * 通过任务ID获得周期任务统计信息
	 * @param planId
	 * @return
	 */
//	public Result getPlanResultByPlanId(Integer planId);
	/**
	 * 周期任务投递统计汇总
	 * @param paramsMap
	 * @param planIds
	 * @return
	 */
//	public Result getPlanResultSum(Map<String, Object> paramsMap, Integer[] planIds);
	/**
	 * 导出周期任务统计到csv
	 * @param csvWriter
	 * @param paramsMap
	 */
//	public void writePlanCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 导出周期任务统计汇总到csv
	 * @param csvWriter
	 * @param paramsMap
	 */
//	public void writePlanCollectCsv(CSVWriter csvWriter, Map<String, Object> paramsMap);
	/**
	 * 客户发送统计
	 * @param paramsMap
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public List<Result> getViewDeliveryTasksByCorpId(Map<String, Object> paramsMap, int currentPage, int pageSize);
	/**
	 * API群发类统计
	 * @param paramsMap
	 * @return
	 */
	public Result getApiGroupsSum(Map<String,Object> paramsMap);
	/**
	 * 获取用户API群发任务统计分页列表
	 * @param paramsMap
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Result> getApiGroupsByUserPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 客户API群发任务分页列表
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	public Pagination<Result> getApiGroupsByTaskPagination(Map<String, Object> paramsMap, String orderBy, int currentPage, int pageSize);
	/**
	 * 查询所有API触发类任务 列表
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Result> getAllApiTriggerPagination(Map<String,Object> paramsMap, String orderBy, int currentPage, int pageSize);
//	/**
//	 * API历史投递统计 导出
//	 * @param csvWriter
//	 * @param paramsMap
//	 */
//	public void writeCsvApiHis(CSVWriter csvWriter, Map<String, Object> paramsMap);
	
	public Result getTouchResult(Integer taskId, Integer templateId);
	
}
