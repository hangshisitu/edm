package cn.edm.service;

import java.util.List;
import java.util.Map;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.domain.Browser;
import cn.edm.domain.Campaign;
import cn.edm.domain.Domain;
import cn.edm.domain.Hour;
import cn.edm.domain.Lang;
import cn.edm.domain.Os;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Url;
import cn.edm.util.Pagination;

public interface CampaignService {
	
	/**
	 * 分页查询
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Result> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 根据活动ID查询
	 * @param campaignId
	 * @return
	 */
	Campaign getById(Integer campaignId);
	
	/**
	 * 获取某个活动统计信息
	 * @param campaignId
	 * @return
	 */
//	Result getCampaignSum(Integer campaignId);
	
	/**
	 * 根据id查询活动任务数
	 * @param campaignId
	 * @return
	 */
	long getTaskNumById(Integer campaignId);
	
	/**
	 * 根据id查询活动任务的最新投递时间
	 * @param campaignId
	 * @return
	 */
	String getLatestDeliveryTimeById(Integer campaignId);
	
	/**
	 * 查询活动任务弹回数据
	 * @param campaignId
	 * @return
	 */
	Domain getCampaignSendBackSum(Integer campaignId);
	
	
	/**
	 * 查询活动时间点统计
	 * @param campaignId
	 * @return 
	 */
	List<Hour> getCampaignHour(Integer campaignId);
	
	/**
	 * 查询活动点击统计
	 * @param campaignId
	 * @return
	 */
	List<Url> getCampaignClick(Integer campaignId);
	
	/**
	 * 查询活动点击总数
	 * @param campaignId
	 * @return
	 */
	Integer getCampaignClickSum(Integer campaignId);
	
	/**
	 * 活动域名发送统计
	 * @param campaignId
	 * @return
	 */
	List<Domain> getCampaignDomainSum(Integer campaignId);
	
	/**
	 * 活动地域统计省份
	 * @param campaignId
	 * @return
	 */
	List<Region> getCampaignProvince(Integer campaignId);
	
	/**
	 * 活动地域统计城市
	 * @param campaignId
	 * @return
	 */
	List<Region> getCampaignCity(Integer campaignId);
	
	/**
	 * 导出csv
	 */
	void writeCsv(CSVWriter csvWriter,Integer campaignId,String flag);
	
	void writeDeliveryCsv(CSVWriter csvWriter,Integer campaignId,String flag,String orderby);
	
	/**
	 * 查询某个活动下的任务统计
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<Result> getCampaignResult(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 排序
	 * @param results
	 * @return
	 */
	List<Result> sortResultList(List<Result> results,String flag);
	
	/**
	 * 获取活动最近几周的数据
	 * @param paramsMap
	 * @param week
	 * @return
	 */
	List<Result> getCampaignLatestWeek(Map<String,Object> paramsMap,String week);
	
	/**
	 * 任务历时是否足够对比
	 * @param campaignId
	 * @return
	 */
	boolean isEnoughPeriod(Integer campaignId,String week);
	
	Domain getCampaignRange(Map<String,Object> paramsMap);
	
	List<Domain> getResultListByRange(Map<String,Object> paramsMap,String mode,String recents,String extra,String checkeds);

	void writeCompareCsv(CSVWriter csvWriter, Integer campaignId, String flag,
			String mode, String recents, String checkeds, String extra);
	
	List<Browser> getBrowsers(Integer campaignId);
	
	List<Lang> getLangs(Integer campaignId);
	
	List<Os> getOss(Integer campaignId);
	
	Map<String,Browser> calcBrowserRate(List<Browser> browserList);
	
	Map<String,Lang> calcLangRate(List<Lang> langList);
	
	Map<String,Os> calcOsRate(List<Os> osList);
	
	Map<String,Integer> getLastTime(Integer campaignId);
	/**
	 * 获取某个活动统计信息
	 * @param campaignId
	 * @return
	 */
	public Domain getCampaignSum(Integer campaignId);
	
	public List<Domain> getCampaignSumList(Integer[] campaignIds, String groupbyType);
	
}
