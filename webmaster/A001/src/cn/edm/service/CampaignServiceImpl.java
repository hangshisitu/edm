package cn.edm.service;

import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.joda.time.DateTime;
import org.joda.time.format.DateTimeFormat;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.Status;
import cn.edm.constant.TaskTypeConstant;
import cn.edm.domain.Browser;
import cn.edm.domain.Campaign;
import cn.edm.domain.Domain;
import cn.edm.domain.Hour;
import cn.edm.domain.Lang;
import cn.edm.domain.Os;
import cn.edm.domain.Region;
import cn.edm.domain.Result;
import cn.edm.domain.Task;
import cn.edm.domain.Touch;
import cn.edm.domain.Url;
import cn.edm.exception.Errors;
import cn.edm.persistence.BrowserMapper;
import cn.edm.persistence.CampaignMapper;
import cn.edm.persistence.DomainMapper;
import cn.edm.persistence.HourMapper;
import cn.edm.persistence.LangMapper;
import cn.edm.persistence.OsMapper;
import cn.edm.persistence.RegionMapper;
import cn.edm.persistence.ResultMapper;
import cn.edm.persistence.TaskMapper;
import cn.edm.persistence.UrlMapper;
import cn.edm.util.Asserts;
import cn.edm.util.Calendars;
import cn.edm.util.Client;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.Converts;
import cn.edm.util.Loggers;
import cn.edm.util.Pagination;
import cn.edm.util.Params;
import cn.edm.util.ResultSortComparator;
import cn.edm.util.Validates;
import cn.edm.utils.ResultDataOption;
import cn.edm.vo.Log;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Service
public class CampaignServiceImpl implements CampaignService{
	
	@Autowired
	private ResultMapper resultMapper;
	
	@Autowired
	private CampaignMapper campaignMapper;
	
	@Autowired
	private DomainMapper domainMapper;
	
	@Autowired
	private HourMapper hourMapper;
	
	@Autowired
	private UrlMapper urlMapper;
	
	@Autowired
	private RegionMapper regionMapper;
	
	@Autowired
	private TaskMapper taskMapper;
	
	@Autowired
	private BrowserMapper browserMapper;
	
	@Autowired
	private LangMapper langMapper;
	
	@Autowired
	private OsMapper osMapper;
	@Autowired
	private TaskService taskService;
	@Autowired
	private TouchService touchService;
	@Autowired
	private TouchSettingService settingService;
	@Autowired
	private DomainService domainService;

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<Result> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectCampaignList(paramsMap);
		long recordCount = resultMapper.selectCampaignCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Campaign getById(Integer campaignId) {
		return campaignMapper.selectById(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Domain getCampaignSum(Integer campaignId) {
		Map<String, Object> pMap = Maps.newHashMap();
//		pMap.put("type", TaskTypeConstant.TASK_SINGLE);
		pMap.put("types", new Integer[] {TaskTypeConstant.TASK_SINGLE,TaskTypeConstant.TASK_API_GROUPS});
		pMap.put("statuses", Status.task("completed"));
		pMap.put("campaignId", campaignId);
		return domainMapper.selectCampaignSum(pMap);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Domain> getCampaignSumList(Integer[] campaignIds, String groupbyType) {
		Map<String, Object> pMap = Maps.newHashMap();
//		pMap.put("type", TaskTypeConstant.TASK_SINGLE);
		pMap.put("types", new Integer[] {TaskTypeConstant.TASK_SINGLE,TaskTypeConstant.TASK_API_GROUPS});
		pMap.put("statuses", Status.task("completed"));
		pMap.put("campaignIds", campaignIds);
		pMap.put("groupbyType", groupbyType);
		return domainMapper.selectCampaignSumList(pMap);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public long getTaskNumById(Integer campaignId) {
		return campaignMapper.selectTaskNumById(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public String getLatestDeliveryTimeById(Integer campaignId) {
		return campaignMapper.selectLatestDeliveryTime(campaignId);
	}
	
	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Domain getCampaignSendBackSum(Integer campaignId) {
		return domainMapper.selectCampaignSendBackSum(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Hour> getCampaignHour(Integer campaignId) {
		return hourMapper.selectCampaignHour(campaignId);
	}

	@Override
	public List<Url> getCampaignClick(Integer campaignId) {
		return urlMapper.selectCampaignClick(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Integer getCampaignClickSum(Integer campaignId) {
		return urlMapper.selectCampaignSum(campaignId);
	}

	@Override
	public List<Domain> getCampaignDomainSum(Integer campaignId) {
		return domainMapper.selectCampaignDomainSum(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Region> getCampaignProvince(Integer campaignId) {
		return regionMapper.selectCampaignProvince(campaignId);
	}

	@Override
	public List<Region> getCampaignCity(Integer campaignId) {
		return regionMapper.selectCampaignCity(campaignId);
	}

	@Override
	public void writeCsv(CSVWriter csvWriter, Integer campaignId, String flag) {
		if("overview".equals(flag)) {
			overviewCsv(csvWriter, campaignId);
		}else if("send".equals(flag)) {
			sendCsv(csvWriter, campaignId);
		}else if("open".equals(flag)) {
			openCsv(csvWriter, campaignId);
		}else if("click".equals(flag)) {
			clickCsv(csvWriter, campaignId);
		}else if("domain".equals(flag)) {
			domainCsv(csvWriter, campaignId);
		}else if("region".equals(flag)) {
			regionCsv(csvWriter, campaignId);
		}else if("delivery".equals(flag)) {
			//deliveryCsv(csvWriter, campaignId);
		}else if("client".equals(flag)) {
			clientCsv(csvWriter, campaignId);
		}else if("touch".equals(flag)) {
			touchCsv(csvWriter, campaignId);
		}else if("all".equals(flag)) {
			sendAll(csvWriter, campaignId);
		}
	}
	
	/**
	 * 概览
	 * @param csvWriter
	 * @param campaignId
	 */
	private void overviewCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		Domain domain = getCampaignSum(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			if(latestTime==null) {
				latestTime="-";
			}
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			String[] title2 = { "发送总数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数","点击次数","退订人数","转发人数","无效人数","成功率","打开率","点击率"};
			csvWriter.writeNext(title2);
			String[] content2 = new String[title2.length];
			
			if(domain!=null) {
				content2[0] = getValue(domain.getSentCount(),"0");
				content2[1] = getValue(domain.getReachCount(),"0");
				
				content2[2] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount(),"0");
				content2[3] = getValue(domain.getReadUserCount(),"0");
				content2[4] = getValue(domain.getReadCount(),"0");
				content2[5] = getValue(domain.getClickUserCount(),"0");
				content2[6] = getValue(domain.getClickCount(),"0");
				content2[7] = getValue(domain.getUnsubscribeCount(),"0");
				content2[8] = getValue(domain.getForwardCount(),"0");
				content2[9] = getValue(domain.getAfterNoneCount(),"0");
				
				if(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount()!=0) {
					content2[10] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getReachCount()*100.00/(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount())), 2),"0")+"%";
				}else {
					content2[10] = "\t"+"0.00%";
				}
				
				if(domain.getReachCount()!=0) {
					content2[11] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getReadUserCount()*100.00/domain.getReachCount()),2),"0")+"%";
				}else {
					content2[11] = "\t"+"0.00%";
				}
				
				if(domain.getReadUserCount()!=0) {
					content2[12] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getClickUserCount()*100.00/domain.getReadUserCount()),2),"0")+"%";
				}else {
					content2[12] = "\t"+"0.00%";
				}
				
				
				
				csvWriter.writeNext(content2);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
			
		}
	}
	
	/**
	 * 导出所有
	 * @param csvWriter
	 * @param campaignId
	 */
	private void sendAll(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(latestTime==null) {
			latestTime = "-";
		}
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			String[] blank = new String[]{""};
			csvWriter.writeNext(blank);
			//总览信息
			csvWriter.writeNext(new String[]{"概览"});
			writeContent(csvWriter,campaignId,"overview");
			csvWriter.writeNext(blank);
			//发送
			csvWriter.writeNext(new String[]{"发送"});
			writeContent(csvWriter,campaignId,"send");
			csvWriter.writeNext(blank);
			//打开
			csvWriter.writeNext(new String[]{"打开"});
			writeContent(csvWriter,campaignId,"open");
			csvWriter.writeNext(blank);
			//点击
			csvWriter.writeNext(new String[]{"点击"});
			writeContent(csvWriter,campaignId,"click");
			csvWriter.writeNext(blank);
			//域名
			csvWriter.writeNext(new String[]{"域名"});
			writeContent(csvWriter,campaignId,"domain");
			csvWriter.writeNext(blank);
			
			//地域
			csvWriter.writeNext(new String[]{"地域"});
			writeContent(csvWriter,campaignId,"region");
			csvWriter.writeNext(blank);
			
			//终端
			csvWriter.writeNext(new String[]{"终端"});
			writeContent(csvWriter,campaignId,"client");
			csvWriter.writeNext(blank);
			
			//投递历史
			csvWriter.writeNext(new String[]{"投递历史"});
			writeContent(csvWriter,campaignId,"delivery");
			csvWriter.writeNext(blank);
			
			//对比
			csvWriter.writeNext(new String[]{"对比"});
			csvWriter.writeNext(new String[]{"最近两周"});
			comapreCsv(csvWriter, campaignId, "", "week", "2", "", "");
			csvWriter.writeNext(blank);
			csvWriter.writeNext(new String[]{"最近两月"});
			comapreCsv(csvWriter, campaignId, "", "month", "2", "", "");
			csvWriter.writeNext(blank);
			csvWriter.writeNext(new String[]{"最近两季度"});
			comapreCsv(csvWriter, campaignId, "", "season", "2", "", "");
			csvWriter.writeNext(blank);
			
			// 触发
			csvWriter.writeNext(new String[]{"触发"});
			writeContent(csvWriter, campaignId, "touch");
			csvWriter.writeNext(blank);
		}
	}
	
	/**
	 * 发送
	 * @param csvWriter
	 * @param campaignId
	 */
	private void sendCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		Domain domain = getCampaignSendBackSum(campaignId);
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			if(latestTime==null) {
				latestTime = "-";
			}
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			String[] title2 = { "弹回详情", "弹回数", "占比"};
			csvWriter.writeNext(title2);
			
			if(domain!=null) {
				String[] email = new String[title2.length];
				email[0] = "邮箱无效";
				email[1] = getValue(domain.getAfterNoneCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					email[2] = getValue(domain.getAfterNoneCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					email[2] = "0.00%";
				}
				csvWriter.writeNext(email);
				
				String[] dns = new String[title2.length];
				dns[0] = "域名无效";
				dns[1] = getValue(domain.getDnsFailCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					dns[2] = getValue(domain.getDnsFailCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					dns[2] = "0.00%";
				}
				csvWriter.writeNext(dns);
				
				
				String[] global = new String[title2.length];
				global[0] = "判定垃圾";
				global[1] = getValue(domain.getDnsFailCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					global[2] = getValue(domain.getSpamGarbageCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					global[2] = "0.00%";
				}
				csvWriter.writeNext(global);
				
				String[] other = new String[title2.length];
				other[0] = "其他";
				other[1] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					other[2] = getValue((domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount())*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					other[2] = "0.00%";
				}
				csvWriter.writeNext(other);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
			
		}
	}
	
	
	/**
	 * 打开
	 * @param csvWriter
	 * @param campaignId
	 */
	private void openCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(latestTime==null) {
			latestTime = "-";
		}
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			
			String[] title2 = {"邮件地址","所属地区", "打开时间"};
			csvWriter.writeNext(title2);
			
			List<Log> openList = Lists.newArrayList();
			List<Task> tasks = taskMapper.selectTasksByCampaignId(campaignId);
			if(tasks!=null&&tasks.size()>0) {
				String[] sids = new String[tasks.size()];
				String[] times = new String[tasks.size()];
				for(int i=0;i<tasks.size();i++) {
					//打开的详细列表
					Task task = tasks.get(i);
					String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
					String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
					sids[i] = sid;
					times[i] = time;
				}
				openList = Loggers.openCampaign(sids, times,"head", "utf-8");
			}
			
			
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
	
	
	/**
	 * 点击
	 * @param csvWriter
	 * @param campaignId
	 */
	private void clickCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(latestTime==null) {
			latestTime = "-";
		}
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			
			String[] title2 = { "链接地址", "点击次数", "占比"};
			csvWriter.writeNext(title2);
			List<Url> urlList = getCampaignClick(campaignId);
			Integer totalClick = getCampaignClickSum(campaignId);
			if(urlList==null||urlList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(int i=0;i<urlList.size();i++) {
					Url u = urlList.get(i);
					content2[0] = u.getUrl();
					content2[1] = getValue(u.getClickCount(),"0");
					if(totalClick!=0) {
						content2[2] =  getValue(u.getClickCount()*100.00/totalClick,"0")+"%";
					}else {
						content2[2] = "0.00%";
					}
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
		}
	}
	
	/**
	 * 域名
	 * @param csvWriter
	 * @param campaignId
	 */
	private void domainCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(latestTime==null) {
			latestTime = "-";
		}
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			
			String[] title2 = { "域名", "发送数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			
			List<Domain> domainList = getCampaignDomainSum(campaignId);
			
			if(domainList==null||domainList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(Domain d:domainList) {
					
					content2[0] = d.getEmailDomain();
					content2[1] = getValue(d.getSentCount(),"0");
					
					content2[2] = getValue(d.getReachCount(),"0");
					content2[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(),"0");
					content2[4] = getValue(d.getReadUserCount(),"0");
					content2[5] = getValue(d.getReadCount(),"0");
					content2[6] = getValue(d.getClickUserCount(),"0");
					content2[7] = getValue(d.getClickCount(),"0");
					content2[8] = getValue(d.getUnsubscribeCount(),"0");
					content2[9] = getValue(d.getForwardCount(),"0");
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
		}
	}
	
	
	/**
	 * 地域
	 * @param csvWriter
	 * @param campaignId
	 */
	private void regionCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		String latestTime = getLatestDeliveryTimeById(campaignId);
		if(latestTime==null) {
			latestTime = "-";
		}
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","更新时间"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),"\t"+latestTime};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			
			String[] title2 = {"地区","打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			
			List<Region> provinceRegionList = getCampaignProvince(campaignId);
			List<Region> cityRegionList = getCampaignCity(campaignId);
			
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
							if(null==cr.getProvinceName()) {
								continue;
							}
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
	
	/**
	 * 终端
	 * @param csvWriter
	 * @param campaignId
	 */
	private void clientCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum)};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			
			String[] titleBrowser = {"浏览器","打开次数", "占比"};
			csvWriter.writeNext(titleBrowser);
			
			List<Browser> browserList = getBrowsers(campaignId);
			if(browserList !=null &&browserList.size()>0) {
				Map<String,Browser> browserMap = calcBrowserRate(browserList);
				String[] contentBrowser = new String[titleBrowser.length];
				for(int i=1;i<=8;i++) {
					contentBrowser[0] = "\t"+Client.browser(i);
					contentBrowser[1] = getValue(browserMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentBrowser[2] = CommonUtil.getFormatNumber(browserMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentBrowser);
				}
				contentBrowser[0] = Client.browser(browserMap.get("0").getBrowser());
				contentBrowser[1] = getValue(browserMap.get("0").getOpenCount(), "0");
				contentBrowser[2] = CommonUtil.getFormatNumber(browserMap.get("0").getOpenRate());
				csvWriter.writeNext(contentBrowser);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});	
			}
				
			csvWriter.writeNext(new String[]{""});
			String[] titleOs = {"操作系统","打开次数", "占比"};
			csvWriter.writeNext(titleOs);
			
			List<Os> osList = getOss(campaignId);
			if(osList !=null &&osList.size()>0) {
				Map<String,Os> osMap = calcOsRate(osList);
				String[] contentOs = new String[titleOs.length];
				for(int i=1;i<=8;i++) {
					contentOs[0] = Client.os(i);
					contentOs[1] = getValue(osMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentOs[2] = CommonUtil.getFormatNumber(osMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentOs);
				}
				contentOs[0] = Client.os(osMap.get("0").getOs());
				contentOs[1] = getValue(osMap.get("0").getOpenCount(), "0");
				contentOs[2] = CommonUtil.getFormatNumber(osMap.get("0").getOpenRate());
				csvWriter.writeNext(contentOs);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			csvWriter.writeNext(new String[]{""});
			
			
			String[] titleLang = {"语言环境","打开次数", "占比"};
			csvWriter.writeNext(titleLang);
			List<Lang> langList = getLangs(campaignId);
			if(osList !=null &&osList.size()>0) {
				Map<String,Lang> langMap = calcLangRate(langList);
				String[] contentLang = new String[titleLang.length];
				for(int i=1;i<=9;i++) {
					contentLang[0] = Client.lang(i);
					contentLang[1] = getValue(langMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentLang[2] = CommonUtil.getFormatNumber(langMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentLang);
				}
				contentLang[0] = Client.os(langMap.get("0").getLang());
				contentLang[1] = getValue(langMap.get("0").getOpenCount(), "0");
				contentLang[2] = CommonUtil.getFormatNumber(langMap.get("0").getOpenRate());
				csvWriter.writeNext(contentLang);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
		}
	}
	/**
	 * 投递历史
	 * @param csvWriter
	 * @param campaignId
	 */
	private void deliveryCsv(CSVWriter csvWriter,Integer campaignId,String orderby) {
		Campaign campaign = getById(campaignId);
		//Result result = getCampaignSum(campaignId);
		long taskNum = getTaskNumById(campaignId);
		//String latestTime = getLatestDeliveryTimeById(campaignId);
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数"};
			csvWriter.writeNext(title);
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum)};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			String[] title2 = { "任务名称","发送总数", "成功数", "弹回数", "打开人数", "点击人数","退订人数","转发人数","无效人数","成功率","打开率","点击率","投递时间"};
			csvWriter.writeNext(title2);
			
			List<Result> results = resultMapper.selectCampaignResult(campaignId);
			domainCopy(results);
			if(results!=null&&results.size()>0) {
				for(Result r:results) {
					double success = 0.00d;
					if((r.getSentCount()-r.getAfterNoneCount())!=0) {
						success = (r.getReachCount()*100.00)/(r.getSentCount()-r.getAfterNoneCount());
					}
					r.setSuccessRate(CommonUtil.getFormatNumber4(success,2));
					
					double open = 0.00d;
					if(r.getReachCount()!=0) {
						open = (r.getReadUserCount()*100.00)/r.getReachCount();
					}
					r.setOpenRate(CommonUtil.getFormatNumber4(open,2));
					double click = 0.00d;
					if(r.getReadUserCount()!=0) {
						click = (r.getClickUserCount()*100.00)/r.getReadUserCount();
					}
					r.setClickRate(CommonUtil.getFormatNumber4(click,2));
				}
			}
			Collections.sort(results, new ResultSortComparator(orderby));
			if(results!=null&&results.size()>0) {
				for(Result result:results) {
					if(result!=null) {
						String[] content2 = new String[title2.length];
						content2[0] = result.getTaskName();
						content2[1] = getValue(result.getSentCount(),"0");
						content2[2] = getValue(result.getReachCount(),"0");
						content2[3] = getValue(result.getSoftBounceCount()+result.getHardBounceCount(),"0");
						content2[4] = getValue(result.getReadUserCount(),"0");
						content2[5] = getValue(result.getClickUserCount(),"0");
						content2[6] = getValue(result.getUnsubscribeCount(),"0");
						content2[7] = getValue(result.getForwardCount(),"0");
						content2[8] = getValue(result.getAfterNoneCount(),"0");
						if(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()!=0) {
							
							//content2[9] = "\t"+getValue(result.getReachCount()*100.00/(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()),"0")+"%";
							content2[9] = "\t"+result.getSuccessRate()+"%";
						}else {
							content2[9] = "\t"+"0.00%";
						}
						
						if(result.getReachCount()!=0) {
							//content2[10] = "\t"+getValue(result.getReadUserCount()*100.00/result.getReachCount(),"0")+"%";
							content2[10] = "\t"+result.getOpenRate()+"%";
						}else {
							content2[10] = "\t"+"0.00%";
						}
						
						if(result.getReadUserCount()!=0) {
							//content2[11] = "\t"+getValue(result.getClickUserCount()*100.00/result.getReadUserCount(),"0")+"%";
							content2[11] = "\t"+result.getClickRate()+"%";
						}else {
							content2[11] = "\t"+"0.00%";
						}
						
						
						String time = "";
						if(result.getDeliveryTime() != null) {
							time = new DateTime(result.getDeliveryTime()).toString("yyyy-MM-dd HH:mm:ss");
							
						}
						content2[12] = "\t"+time;
						csvWriter.writeNext(content2);
					}else {
						csvWriter.writeNext(new String[]{"暂无数据"});
					}
					
				}
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
		}
	}
	/**
	 * 对比
	 * @param csvWriter
	 * @param campaignId
	 */
	private void comapreCsv(CSVWriter csvWriter,Integer campaignId,String flag,
			String mode, String recents, String checkeds, String extra) {
		Campaign campaign = getById(campaignId);
		long taskNum = getTaskNumById(campaignId);
		if(campaign==null) {
			csvWriter.writeNext(new String[]{"暂无数据"});
		}else {
			String[] title = {"活动名称","活动描述","投递次数","活动历时"};
			csvWriter.writeNext(title);
			Map<String,Integer> lastMap = getLastTime(campaignId);
			String last = "-";
			
			int time = 0;
			if("week".equals(mode)) {
				time = lastMap.get("week");
				last = time + "周";
			}else if("month".equals(mode)) {
				time = lastMap.get("month");
				last = time + "个月";
			}else if("season".equals(mode)) {
				time = lastMap.get("season");
				last = time + "个季度";
			}
			
			
			String[] content = {campaign.getCampaignName(),campaign.getCampaignDesc(),String.valueOf(taskNum),last};
			csvWriter.writeNext(content);
			csvWriter.writeNext(new String[]{""});
			String[] title2 = { "对比项","发送总数", "成功数", "打开人数", "点击人数","退订人数","无效人数","成功率","打开率","点击率"};
			csvWriter.writeNext(title2);
			if(time<2) {
				csvWriter.writeNext(new String[]{"暂无可对比数据"});
			}else{
				Map<String,Object> paramsMap = Maps.newHashMap();
				paramsMap.put("campaignId", campaignId);
				List<Domain> domains = this.getResultListByRange(paramsMap, mode, recents, extra, checkeds);
				if(domains!=null && domains.size()>0) {
					for(Domain d:domains) {
						double success = 0.00d;
						if((d.getSentCount()-d.getAfterNoneCount())!=0) {
							success = (d.getReachCount()*100.00)/(d.getSentCount()-d.getAfterNoneCount());
						}
						d.setSuccessRate(CommonUtil.getFormatNumber4(success,2) + "");
						
						double open = 0.00d;
						if(d.getReachCount()!=0) {
							open = (d.getReadUserCount()*100.00)/d.getReachCount();
						}
						d.setOpenRate(CommonUtil.getFormatNumber4(open,2) + "");
						
						double click = 0.00d;
						if(d.getReadUserCount()!=0) {
							click = (d.getClickUserCount()*100.00)/d.getReadUserCount();
						}
						d.setClickRate(CommonUtil.getFormatNumber4(click,2) + "");
					}
					for(Domain domain : domains) {
						if(domain!=null) {
							String[] content2 = new String[title2.length];
							content2[0] = "\t"+domain.getType();
							content2[1] = getValue(domain.getSentCount(),"0");
							content2[2] = getValue(domain.getReachCount(),"0");
							content2[3] = getValue(domain.getReadUserCount(),"0");
							content2[4] = getValue(domain.getClickUserCount(),"0");
							content2[5] = getValue(domain.getUnsubscribeCount(),"0");
							content2[6] = getValue(domain.getAfterNoneCount(),"0");
							
							if(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount()!=0) {
								//content2[7] = getValue(result.getReachCount()*100.00/(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()),"0")+"%";
								content2[7] = "\t"+domain.getSuccessRate()+"%";
							}else {
								content2[7] = "0.00%";
							}
							
							if(domain.getReachCount()!=0) {
								//content2[8] = getValue(result.getReadUserCount()*100.00/result.getReachCount(),"0")+"%";
								content2[8] = "\t"+domain.getOpenRate()+"%";
							}else {
								content2[8] = "0.00%";
							}
							
							if(domain.getReadUserCount()!=0) {
								//content2[9] = getValue(result.getClickUserCount()*100.00/result.getReadUserCount(),"0")+"%";
								content2[9] = "\t"+domain.getClickRate()+"%";
							}else {
								content2[9] = "0.00%";
							}
							
							csvWriter.writeNext(content2);
						}else {
							csvWriter.writeNext(new String[]{"暂无数据"});
						}
						
					}
				}else {
					csvWriter.writeNext(new String[]{"暂无数据"});
				}
			}
			
		}
	}

	private String getValue(Double value,String defaultValue) {
		return value==null?defaultValue:String.valueOf(value);
	}
	
	private String getValue(Integer value,String defaultValue) {
		return value==null?defaultValue:String.valueOf(value);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Pagination<Result> getCampaignResult(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);
		
		List<Result> recordList = resultMapper.selectCampaignResultList(paramsMap);
		long recordCount = resultMapper.selectCampaignResultCount(paramsMap);
		
		Pagination<Result> pagination = new Pagination<Result>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	public List<Result> sortResultList(List<Result> results,String flag) {
		List<Result> resultList = Lists.newArrayList();
		if(results!=null&&results.size()>0) {
			for(Result r:results) {
				double success = 0.00d;
				if((r.getSentCount()-r.getAfterNoneCount())!=0) {
					success = (r.getReachCount()*100.00)/(r.getSentCount()-r.getAfterNoneCount());
				}
				r.setSuccessRate(CommonUtil.getFormatNumber4(success,2));
				
				double open = 0.00d;
				if(r.getReachCount()!=0) {
					open = (r.getReadUserCount()*100.00)/r.getReachCount();
				}
				r.setOpenRate(CommonUtil.getFormatNumber4(open,2));
				
				double click = 0.00d;
				if(r.getReadUserCount()!=0) {
					click = (r.getClickUserCount()*100.00)/r.getReadUserCount();
				}
				r.setClickRate(CommonUtil.getFormatNumber4(click,2));
				resultList.add(r);
			}
		}
		Collections.sort(resultList,new ResultSortComparator(flag));
		return resultList;
	}

	@Override
	public void writeDeliveryCsv(CSVWriter csvWriter, Integer campaignId,
			String flag, String orderby) {
		deliveryCsv(csvWriter, campaignId, orderby);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Result> getCampaignLatestWeek(Map<String, Object> paramsMap,
			String week) {
		List<Result> results = null;
		if("2".equals(week)) {
			results = resultMapper.selectCampaignRangeTwoWeek(paramsMap);
		}else if("3".equals(week)) {
			results = resultMapper.selectCampaignRangeThreeWeek(paramsMap);
		}else if("4".equals(week)){
			results = resultMapper.selectCampaignRangeFourWeek(paramsMap);
		}
		return results;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public boolean isEnoughPeriod(Integer campaignId, String week) {
		boolean flag = false;
		Integer period = taskMapper.selectTaskPeriod(campaignId);
		
		Integer n = Integer.parseInt(week);
		if(period!=null&&period>((n-1)*7)) {
			flag = true;
		}
		return flag;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Domain getCampaignRange(Map<String, Object> paramsMap) {
		return domainMapper.selectCampaignRange(paramsMap);
	}

	@Override
	public List<Domain> getResultListByRange(Map<String, Object> paramsMap,
			String mode, String recents, String extra, String checkeds) {
		List<Domain> domainList = Lists.newArrayList();
		
		DateTime now = new DateTime();
		
		if("week".equals(mode)) {
			 DateTime startOfWeek = now.dayOfWeek().withMinimumValue().millisOfDay().withMinimumValue();
             DateTime endOfWeek = now.dayOfWeek().withMaximumValue().millisOfDay().withMaximumValue();
             int range = Integer.valueOf(recents);
             for (int i = 0; i < range; i++) {
                 String beginTime = startOfWeek.plusWeeks(-i).toString("yyyy-MM-dd HH:mm:ss");
                 String endTime = endOfWeek.plusWeeks(-i).toString("yyyy-MM-dd HH:mm:ss");
                 paramsMap.put("from", beginTime);
     			 paramsMap.put("to", endTime);
     			 Domain d = getCampaignRange(paramsMap);
    			 d.setType(getType(mode,i));
    			 domainList.add(d);
             }
		}else if("month".equals(mode)) {
			 if("other".equals(extra)) {
				  String[] months = Converts._toStrings(Converts.repeat(checkeds));
				  if (Asserts.empty(months) || months.length < 2 || months.length > 4) {
                      throw new Errors("请选择2个到4个不同的月份进行对比！");
                  }
                  
                  for (String e : months) {
                      if (!Validates.formatter(e, "yyyyMM")) {
                          continue;
                      }
                      DateTime month = DateTimeFormat.forPattern("yyyyMM").parseDateTime(e);  
                      String beginTime = month.dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue().toString("yyyy-MM-dd HH:mm:ss");
                      String endTime = month.dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue().toString("yyyy-MM-dd HH:mm:ss");
                      paramsMap.put("from", beginTime);
 	     			  paramsMap.put("to", endTime);
 	     			  Domain d = getCampaignRange(paramsMap);
 	    			  d.setType(month.toString("yyyy年MM月"));
 	    			  domainList.add(d);
                  }
                  
			 }else {
				 DateTime startOfMonth = now.dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue();
	             DateTime endOfMonth = now.dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue();
	             int range = Integer.valueOf(recents);
	             for (int i = 0; i < range; i++) {
	                 String beginTime = startOfMonth.plusMonths(-i).toString("yyyy-MM-dd HH:mm:ss");
	                 String endTime = endOfMonth.plusMonths(-i).toString("yyyy-MM-dd HH:mm:ss");
	                 paramsMap.put("from", beginTime);
	     			 paramsMap.put("to", endTime);
	     			 Domain d = getCampaignRange(paramsMap);
	    			 d.setType(getType(mode,i));
	    			 domainList.add(d);
	             }
			 }
			
		}else if("season".equals(mode)) {
			if("other".equals(extra)) {
				 String[] seasons = Converts._toStrings(Converts.repeat(checkeds));
				 if (Asserts.empty(seasons) || seasons.length < 2 || seasons.length > 4) {
                     throw new Errors("请选择2个到4个不同的季度进行对比！");
                 }
                
				 for (String e : seasons) {
                     //Validator.validate(e, R.INTEGER, R.LENGTH, "{5,5}");
					 if (!Validates.formatter(e.substring(0, 4), "yyyy") ) {
                         continue;
                     }
                     int year = Integer.valueOf(e.substring(0, 4));
                     int month = Integer.valueOf(e.substring(4)) * 3;
                     DateTime season = new DateTime().withYear(year).withMonthOfYear(month);
                     String beginTime = season.plusMonths(-2)
                             .dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue().toString("yyyy-MM-dd HH:mm:ss");
                     String endTime = season
                             .dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue().toString("yyyy-MM-dd HH:mm:ss");
                     paramsMap.put("from", beginTime);
	     			 paramsMap.put("to", endTime);
	     			 Domain d = getCampaignRange(paramsMap);
	    			 d.setType(year + "年" + e.substring(4) + "季度");
	    			 domainList.add(d);
				 }
			}else {
				int month = now.getMonthOfYear();
	            if (month % 3 != 0) {
	                month = month + (3 - (month % 3));
	            }
	            int range = Integer.valueOf(recents);
	            DateTime season = new DateTime().withMonthOfYear(month);
	            for (int i = 0; i < range; i++) {
	                String beginTime = season.plusMonths(-(month * i + 2))
	                        .dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue().toString("yyyy-MM-dd HH:mm:ss");
	                String endTime = season.plusMonths(-(month * i))
	                        .dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue().toString("yyyy-MM-dd HH:mm:ss");
	                paramsMap.put("from", beginTime);
	    			paramsMap.put("to", endTime);
	    			Domain d = getCampaignRange(paramsMap);
	    			d.setType(getType(mode,i));
	    			domainList.add(d);
	            }
			}
			
		}
		return domainList;
	}

	private String getType(String mode,int i) {
		String type = "";
		if("week".equals(mode)) {
			if(i==0){
				type="本周";
			}else if(i==1) {
				type="上一周";
			}else if(i==2) {
				type="上二周";
			}else if(i==3) {
				type="上三周";
			}
		}else if("month".equals(mode)) {
			if(i==0) {
				type = "本月";
			}else if(i==1){
				type = "上一月";
			}else if(i==2){
				type = "上二月";
			}else if(i==3){
				type = "上三月";
			}
		}else if("season".equals(mode)) {
			if(i==0) {
				type = "本季度";
			}else if(i==1){
				type = "上一季度";
			}else if(i==2){
				type = "上二季度";
			}else if(i==3){
				type = "上三季度";
			}
		}
		return type;
	}

	@Override
	public void writeCompareCsv(CSVWriter csvWriter, Integer campaignId,
			String flag, String mode, String recents, String checkeds,
			String extra) {
		comapreCsv(csvWriter, campaignId, flag, mode, recents, checkeds, extra);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Browser> getBrowsers(Integer campaignId) {
		return browserMapper.selectBrowser(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Lang> getLangs(Integer campaignId) {
		return langMapper.selectLang(campaignId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Os> getOss(Integer campaignId) {
		return osMapper.selectOs(campaignId);
	}

	@Override
	public Map<String,Browser> calcBrowserRate(List<Browser> browserList){
		Map<String,Browser> map = Maps.newHashMap();
		List<Browser> resultList = Lists.newArrayList();
		Integer total = 0;
		Browser other = new Browser();
		other.setName("其他");
		other.setBrowser(0);
		other.setOpenCount(0);
		if(browserList!=null&&browserList.size()>0) {
			for(Browser b:browserList) {
				total += b.getOpenCount();
				if(isInRange(b.getBrowser(), "browser")) {
					b.setName(Client.browser(b.getBrowser()));
					resultList.add(b);
				}else {
					other.setOpenCount(other.getOpenCount()+b.getOpenCount());
				}
			}
			for(Browser b:resultList) {
				double open = 0.00d;
				if(total!=0) {
					open = (b.getOpenCount()*100.00)/total;
				}
				b.setOpenRate(CommonUtil.getFormatNumber3(open));
				b.setTotal(total);
				map.put(String.valueOf(b.getBrowser()), b);
			}
			
			
		}
		double otherOpen = 0.00d;
		if(total!=0) {
			otherOpen = (other.getOpenCount()*100.00)/total;
		}
		other.setOpenRate(CommonUtil.getFormatNumber3(otherOpen));
		other.setTotal(total);
		map.put("0", other);
		handleBrowserMap(map);
		return map;
		
	}

	@Override
	public Map<String,Lang> calcLangRate(List<Lang> langList) {
		Map<String,Lang> map = Maps.newHashMap();
		List<Lang> resultList = Lists.newArrayList();
		Integer total = 0;
		Lang other = new Lang();
		other.setName("其他");
		other.setLang(0);
		other.setOpenCount(0);
		if(langList!=null&&langList.size()>0) {
			for(Lang b:langList) {
				total += b.getOpenCount();
				if(isInRange(b.getLang(), "lang")) {
					b.setName(Client.lang(b.getLang()));
					resultList.add(b);
				}else {
					other.setOpenCount(other.getOpenCount()+b.getOpenCount());
				}
			}
			for(Lang b:resultList) {
				double open = 0.00d;
				if(total!=0) {
					open = (b.getOpenCount()*100.00)/total;
				}
				b.setOpenRate(CommonUtil.getFormatNumber3(open));
				b.setTotal(total);
				map.put(String.valueOf(b.getLang()), b);
			}
			
			
		}
		double otherOpen = 0.00d;
		if(total!=0) {
			otherOpen = (other.getOpenCount()*100.00)/total;
		}
		other.setOpenRate(CommonUtil.getFormatNumber3(otherOpen));
		other.setTotal(total);
		map.put("0", other);
		handleLangMap(map);
		return map;
	}

	@Override
	public Map<String,Os> calcOsRate(List<Os> osList) {
		Map<String,Os> map = Maps.newHashMap();
		List<Os> resultList = Lists.newArrayList();
		Integer total = 0;
		Os other = new Os();
		other.setName("其他");
		other.setOs(0);
		other.setOpenCount(0);
		if(osList!=null&&osList.size()>0) {
			for(Os b:osList) {
				total += b.getOpenCount();
				if(isInRange(b.getOs(), "os")) {
					b.setName(Client.os(b.getOs()));
					resultList.add(b);
				}else {
					other.setOpenCount(other.getOpenCount()+b.getOpenCount());
				}
			}
			for(Os b:resultList) {
				double open = 0.00d;
				if(total!=0) {
					open = (b.getOpenCount()*100.00)/total;
				}
				b.setOpenRate(CommonUtil.getFormatNumber3(open));
				b.setTotal(total);
				map.put(String.valueOf(b.getOs()), b);
			}
			
			
		}
		double otherOpen = 0.00d;
		if(total!=0) {
			otherOpen = (other.getOpenCount()*100.00)/total;
		}
		other.setOpenRate(CommonUtil.getFormatNumber3(otherOpen));
		other.setTotal(total);
		map.put("0", other);
		handleOsMap(map);
		return map;
	}
	
	private void handleBrowserMap(Map<String,Browser> map) {
		
		for(Integer i=0;i<=8;i++) {
			Browser empty = new Browser();
			empty.setOpenCount(0);
			empty.setTotal(0);
			empty.setOpenRate(0.00f);
			String browser = "其他";
			if(map.get(String.valueOf(i))==null) {
			switch (i){
	            case 1: browser = "Opera"; break;
	            case 2: browser = "QQ"; break;
	            case 3: browser = "Chrome"; break;
	            case 4: browser = "UC"; break;
	            case 5: browser = "Safari"; break;
	            case 6: browser = "Firefox"; break;
	            case 7: browser = "360"; break;
	            case 8: browser = "IE"; break;
	        }
			empty.setName(browser);
			map.put(String.valueOf(i), empty);
			}
		}
	}
	
	private void handleLangMap(Map<String,Lang> map) {
		
		for(Integer i=0;i<=9;i++) {
			Lang empty = new Lang();
			empty.setOpenCount(0);
			empty.setTotal(0);
			empty.setOpenRate(0.00f);
			String lang = "其他";
			if(map.get(String.valueOf(i))==null) {
				switch (i) {
	            case 1: lang = "简体中文"; break;
	            case 2: lang = "繁体中文"; break;
	            case 3: lang = "英语"; break;
	            case 4: lang = "法语"; break;
	            case 5: lang = "德语"; break;
	            case 6: lang = "日语"; break;
	            case 7: lang = "韩语"; break;
	            case 8: lang = "西班牙语"; break;
	            case 9: lang = "瑞典语"; break;
	        }
	        empty.setName(lang);
			map.put(String.valueOf(i), empty);
			}
		}
	}
	
	private void handleOsMap(Map<String,Os> map) {
		
		for(Integer i=0;i<=8;i++) {
			Os empty = new Os();
			empty.setOpenCount(0);
			empty.setTotal(0);
			empty.setOpenRate(0.00f);
			String os = "其他";
			if(map.get(String.valueOf(i))==null) {
				switch (i) {
		            case 1: os = "Android"; break;
		            case 2: os = "iPhone"; break;
		            case 3: os = "MacOS"; break;
		            case 4: os = "Symbian"; break;
		            case 5: os = "iPad"; break;
		            case 6: os = "Windows"; break;
		            case 7: os = "Linux"; break;
		            case 8: os = "Windows Phone"; break;
				}
				empty.setName(os);
				map.put(i+"", empty);
			}
		}
	}
	
	private boolean isInRange(Integer id,String type) {
		boolean flag = false;
		if("browser".equals(type)) {
			int[] range = {1,2,3,4,5,6,7,8};
			for(int num:range) {
				if(id==num) {
					flag = true;
					break;
				}
			}
		}else if("lang".equals(type)){
			int[] range = {1,2,3,4,5,6,7,8,9};
			for(int num:range) {
				if(id==num) {
					flag = true;
					break;
				}
			}
		}else {
			int[] range = {1,2,3,4,5,6,7,8};
			for(int num:range) {
				if(id==num) {
					flag = true;
					break;
				}
			}
		}
		
		return flag;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public Map<String, Integer> getLastTime(Integer campaignId) {
		Map<String, Integer> lastMap = Maps.newHashMap();
		Date start = taskMapper.selectMinTime(campaignId);
		DateTime now = new DateTime();     
         // 历时
        int week = 0;
        int month = 0;
        int season = 0;
        if (start == null) start = now.toDate();
        DateTime min = Calendars.parse(new DateTime(start).toString(Calendars.DATE), Calendars.DATE);
        DateTime max = Calendars.parse(now.toString(Calendars.DATE), Calendars.DATE);
        week = Calendars.weeks(min, max);
        month = Calendars.months(min, max);
        season = Calendars.seasons(min, max);
        
        lastMap.put("week", week);
        lastMap.put("month", month);
        lastMap.put("season", season);
		return lastMap;
	}
	
	
	private void writeContent(CSVWriter csvWriter,Integer campaignId,String flag){
		if("overview".equals(flag)) {
			String[] title = { "发送总数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数","点击次数","退订人数","转发人数","无效人数","成功率","打开率","点击率"};
			csvWriter.writeNext(title);
			Domain domain = getCampaignSum(campaignId);
			String[] content = new String[title.length];
			content[0] = getValue(domain.getSentCount(),"0");
			content[1] = getValue(domain.getReachCount(),"0");
			
			content[2] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount(),"0");
			content[3] = getValue(domain.getReadUserCount(),"0");
			content[4] = getValue(domain.getReadCount(),"0");
			content[5] = getValue(domain.getClickUserCount(),"0");
			content[6] = getValue(domain.getClickCount(),"0");
			content[7] = getValue(domain.getUnsubscribeCount(),"0");
			content[8] = getValue(domain.getForwardCount(),"0");
			content[9] = getValue(domain.getAfterNoneCount(),"0");
			
			
			if(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount()!=0) {
				content[10] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getReachCount()*100.00/(domain.getSentCount()-domain.getBeforeNoneCount()-domain.getAfterNoneCount())), 2),"0")+"%";
			}else {
				content[10] = "\t"+"0.00%";
			}
			
			if(domain.getReachCount()!=0) {
				content[11] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getReadUserCount()*100.00/domain.getReachCount()),2),"0")+"%";
			}else {
				content[11] = "\t"+"0.00%";
			}
			
			if(domain.getReadUserCount()!=0) {
				content[12] = "\t"+getValue(CommonUtil.getFormatNumber4((domain.getClickUserCount()*100.00/domain.getReadUserCount()),2),"0")+"%";
			}else {
				content[12] = "\t"+"0.00%";
			}
			csvWriter.writeNext(content);
		}else if("send".equals(flag)) {
			String[] title2 = { "弹回详情", "弹回数", "占比"};
			csvWriter.writeNext(title2);
			Domain domain = getCampaignSendBackSum(campaignId);
			if(domain!=null) {
				String[] email = new String[title2.length];
				email[0] = "邮箱无效";
				email[1] = getValue(domain.getAfterNoneCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					email[2] = getValue(domain.getAfterNoneCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					email[2] = "0.00%";
				}
				csvWriter.writeNext(email);
				
				String[] dns = new String[title2.length];
				dns[0] = "域名无效";
				dns[1] = getValue(domain.getDnsFailCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					dns[2] = getValue(domain.getDnsFailCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					dns[2] = "0.00%";
				}
				csvWriter.writeNext(dns);
				
				
				String[] global = new String[title2.length];
				global[0] = "判定垃圾";
				global[1] = getValue(domain.getDnsFailCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					global[2] = getValue(domain.getSpamGarbageCount()*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					global[2] = "0.00%";
				}
				csvWriter.writeNext(global);
				
				String[] other = new String[title2.length];
				other[0] = "其他";
				other[1] = getValue(domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount(),"0");
				if(domain.getSoftBounceCount()+domain.getHardBounceCount()!=0) {
					other[2] = getValue((domain.getSoftBounceCount()+domain.getHardBounceCount()-domain.getAfterNoneCount()-domain.getDnsFailCount()-domain.getSpamGarbageCount())*100.00/(domain.getSoftBounceCount()+domain.getHardBounceCount()),"0")+"%";;
				}else {
					other[2] = "0.00%";
				}
				csvWriter.writeNext(other);
			}
			
		}else if("open".equals(flag)) {
			String[] title2 = {"邮件地址","所属地区", "打开时间"};
			csvWriter.writeNext(title2);
			
			List<Log> openList = Lists.newArrayList();
			List<Task> tasks = taskMapper.selectTasksByCampaignId(campaignId);
			if(tasks!=null&&tasks.size()>0) {
				String[] sids = new String[tasks.size()];
				String[] times = new String[tasks.size()];
				for(int i=0;i<tasks.size();i++) {
					//打开的详细列表
					Task task = tasks.get(i);
					String sid = Params.sid(task.getCorpId(), task.getTaskId(), task.getTemplateId());
					String time = new DateTime(task.getDeliveryTime()).toString("yyyyMMdd");
					sids[i] = sid;
					times[i] = time;
				}
				openList = Loggers.openCampaign(sids, times,"head", "utf-8");
			}
			
			
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
		}else if("click".equals(flag)){
			String[] title2 = { "链接地址", "点击次数", "占比"};
			csvWriter.writeNext(title2);
			List<Url> urlList = getCampaignClick(campaignId);
			Integer totalClick = getCampaignClickSum(campaignId);
			if(urlList==null||urlList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(int i=0;i<urlList.size();i++) {
					Url u = urlList.get(i);
					content2[0] = u.getUrl();
					content2[1] = getValue(u.getClickCount(),"0");
					if(totalClick!=0) {
						content2[2] =  getValue(u.getClickCount()*100.00/totalClick,"0")+"%";
					}else {
						content2[2] = "0.00%";
					}
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
		}else if("domain".equals(flag)){
			String[] title2 = { "域名", "发送数", "成功数", "弹回数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			
			List<Domain> domainList = getCampaignDomainSum(campaignId);
			
			if(domainList==null||domainList.size()==0) {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}else {
				String[] content2 = new String[title2.length];
				for(Domain d:domainList) {
					
					content2[0] = d.getEmailDomain();
					content2[1] = getValue(d.getSentCount(),"0");
					
					content2[2] = getValue(d.getReachCount(),"0");
					content2[3] = getValue(d.getSoftBounceCount()+d.getHardBounceCount(),"0");
					content2[4] = getValue(d.getReadUserCount(),"0");
					content2[5] = getValue(d.getReadCount(),"0");
					content2[6] = getValue(d.getClickUserCount(),"0");
					content2[7] = getValue(d.getClickCount(),"0");
					content2[8] = getValue(d.getUnsubscribeCount(),"0");
					content2[9] = getValue(d.getForwardCount(),"0");
					csvWriter.writeNext(content2);
				}
				content2 = null;
			}
		}else if("region".equals(flag)){
			String[] title2 = {"地区","打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数"};
			csvWriter.writeNext(title2);
			
			List<Region> provinceRegionList = getCampaignProvince(campaignId);
			List<Region> cityRegionList = getCampaignCity(campaignId);
			
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
		}else if("client".equals(flag)){
			String[] titleBrowser = {"浏览器","打开人数", "占比"};
			csvWriter.writeNext(titleBrowser);
			
			List<Browser> browserList = getBrowsers(campaignId);
			if(browserList !=null &&browserList.size()>0) {
				Map<String,Browser> browserMap = calcBrowserRate(browserList);
				String[] contentBrowser = new String[titleBrowser.length];
				for(int i=1;i<=8;i++) {
					contentBrowser[0] = "\t"+Client.browser(i);
					contentBrowser[1] = getValue(browserMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentBrowser[2] = CommonUtil.getFormatNumber(browserMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentBrowser);
				}
				contentBrowser[0] = Client.browser(browserMap.get("0").getBrowser());
				contentBrowser[1] = getValue(browserMap.get("0").getOpenCount(), "0");
				contentBrowser[2] = CommonUtil.getFormatNumber(browserMap.get("0").getOpenRate());
				csvWriter.writeNext(contentBrowser);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});	
			}
				
			csvWriter.writeNext(new String[]{""});
			String[] titleOs = {"操作系统","打开人数", "占比"};
			csvWriter.writeNext(titleOs);
			
			List<Os> osList = getOss(campaignId);
			if(osList !=null &&osList.size()>0) {
				Map<String,Os> osMap = calcOsRate(osList);
				String[] contentOs = new String[titleOs.length];
				for(int i=1;i<=8;i++) {
					contentOs[0] = Client.os(i);
					contentOs[1] = getValue(osMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentOs[2] = CommonUtil.getFormatNumber(osMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentOs);
				}
				contentOs[0] = Client.os(osMap.get("0").getOs());
				contentOs[1] = getValue(osMap.get("0").getOpenCount(), "0");
				contentOs[2] = CommonUtil.getFormatNumber(osMap.get("0").getOpenRate());
				csvWriter.writeNext(contentOs);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			csvWriter.writeNext(new String[]{""});
			
			
			String[] titleLang = {"语言环境","打开人数", "占比"};
			csvWriter.writeNext(titleLang);
			List<Lang> langList = getLangs(campaignId);
			if(osList !=null &&osList.size()>0) {
				Map<String,Lang> langMap = calcLangRate(langList);
				String[] contentLang = new String[titleLang.length];
				for(int i=1;i<=9;i++) {
					contentLang[0] = Client.lang(i);
					contentLang[1] = getValue(langMap.get(String.valueOf(i)).getOpenCount(), "0");
					contentLang[2] = CommonUtil.getFormatNumber(langMap.get(String.valueOf(i)).getOpenRate());
					csvWriter.writeNext(contentLang);
				}
				contentLang[0] = Client.os(langMap.get("0").getLang());
				contentLang[1] = getValue(langMap.get("0").getOpenCount(), "0");
				contentLang[2] = CommonUtil.getFormatNumber(langMap.get("0").getOpenRate());
				csvWriter.writeNext(contentLang);
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
		}else if("delivery".equals(flag)) {
			String[] title2 = { "任务名称","发送总数", "成功数", "弹回数", "打开人数", "点击人数","退订人数","转发人数","无效人数","成功率","打开率","点击率","投递时间"};
			csvWriter.writeNext(title2);
			List<Result> results = resultMapper.selectCampaignResult(campaignId);
			domainCopy(results);
			if(results!=null&&results.size()>0) {
				for(Result r:results) {
					double success = 0.00d;
					if((r.getSentCount()-r.getAfterNoneCount())!=0) {
						success = (r.getReachCount()*100.00)/(r.getSentCount()-r.getAfterNoneCount());
					}
					r.setSuccessRate(CommonUtil.getFormatNumber4(success,2));
					
					double open = 0.00d;
					if(r.getReachCount()!=0) {
						open = (r.getReadUserCount()*100.00)/r.getReachCount();
					}
					r.setOpenRate(CommonUtil.getFormatNumber4(open,2));
					
					double click = 0.00d;
					if(r.getReadUserCount()!=0) {
						click = (r.getClickUserCount()*100.00)/r.getReadUserCount();
					}
					r.setClickRate(CommonUtil.getFormatNumber4(click,2));
				}
			}
			Collections.sort(results, new ResultSortComparator("deliveryTime"));
			if(results!=null&&results.size()>0) {
				for(Result result:results) {
					if(result!=null) {
						String[] content2 = new String[title2.length];
						content2[0] = result.getTaskName();
						content2[1] = getValue(result.getSentCount(),"0");
						content2[2] = getValue(result.getReachCount(),"0");
						content2[3] = getValue(result.getSoftBounceCount()+result.getHardBounceCount(),"0");
						content2[4] = getValue(result.getReadUserCount(),"0");
						content2[5] = getValue(result.getClickUserCount(),"0");
						content2[6] = getValue(result.getUnsubscribeCount(),"0");
						content2[7] = getValue(result.getForwardCount(),"0");
						content2[8] = getValue(result.getAfterNoneCount(),"0");
						if(result.getSentCount()-result.getBeforeNoneCount()-result.getAfterNoneCount()!=0) {
							content2[9] = "\t"+result.getSuccessRate()+"%";
						}else {
							content2[9] = "\t"+"0.00%";
						}
						
						if(result.getReachCount()!=0) {
							//content2[10] = "\t"+getValue(result.getReadUserCount()*100.00/result.getReachCount(),"0")+"%";
							content2[10] = "\t"+result.getOpenRate()+"%";
						}else {
							content2[10] = "\t"+"0.00%";
						}
						
						if(result.getReadUserCount()!=0) {
							//content2[11] = "\t"+getValue(result.getClickUserCount()*100.00/result.getReadUserCount(),"0")+"%";
							content2[11] = "\t"+result.getClickRate()+"%";
						}else {
							content2[11] = "\t"+"0.00%";
						}
						
						
						String time = "";
						if(result.getDeliveryTime() != null) {
							time = new DateTime(result.getDeliveryTime()).toString("yyyy-MM-dd HH:mm:ss");
							
						}
						content2[12] = "\t"+time;
						csvWriter.writeNext(content2);
					}else {
						csvWriter.writeNext(new String[]{"暂无数据"});
					}
					
				}
			}else {
				csvWriter.writeNext(new String[]{"暂无数据"});
			}
			
		}else if("touch".equals(flag)) {
			List<Task> taskList = taskService.getTouchTask(campaignId, "campaign");
			Map<String, List<Domain>> domainMap = Maps.newHashMap(); // 有活动的触发计划，以任务区分，数据形式：Map<taskName_taskId, domainList>
			Integer[] taskIds = null;
			if(taskList!=null && taskList.size()>0){
				String taskIdStr = "";
				for(Task t : taskList) {
					taskIdStr += t.getTaskId() + ",";
				}
				taskIds = Converts._toIntegers(taskIdStr);
				List<Touch> touchList = touchService.getByParentIds(taskIds);
				String touchIdStr = ""; 
				for(int i=0,len=touchList.size(); i<len; i++) {
					Touch t = touchList.get(i);
					touchIdStr += t.getTaskId() + ",";
				}
				Integer[] touchIds = Converts._toIntegers(touchIdStr);
				List<Domain> domains = null;
				if(touchIds!=null && touchIds.length>0){
					Map<String, Object> paramsMap = Maps.newHashMap();
            		paramsMap.put("taskIds", touchIds);
            		List<Domain> domainList = domainService.getCampaignTouchList(paramsMap);
            		for(Domain d : domainList) {
            			String key = d.getTaskName() + "_" + d.getTaskId();
            			domainMap.put(key, null);
            		}
            		for(String k : domainMap.keySet()) {
            			domains = Lists.newArrayList();
            			for(Domain d : domainList) {
            				String key = d.getTaskName() + "_" + d.getTaskId();
            				if(k.equals(key)){
            					Map<String, Object> pMap = Maps.newHashMap();
            					pMap.put("taskId", d.getTaskId());
            					Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
            					pMap.put("types", types);
            					Domain domain = domainService.getSum(pMap);
            					Integer parentReadUserCount = 0;
            					if(domain != null){
            						parentReadUserCount = domain.getReadUserCount();
            					}
            					d.setParentReadUserCount(parentReadUserCount);
            					domains.add(d);
            				}
            			}
            			domainMap.put(k, domains);
            		}
				}
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
			}else{
				csvWriter.writeNext(new String[]{ "暂无数据" });
			}
		}
		
	}
	
	private void touchCsv(CSVWriter csvWriter,Integer campaignId) {
		Campaign campaign = this.getById(campaignId);
        int touchCount = taskService.getTouchCount(campaignId, "campaign");
        campaign.setTouchCount(touchCount);
        long taskNum = this.getTaskNumById(campaignId);
        campaign.setTaskCount((int) taskNum);
        String latestTime = this.getLatestDeliveryTimeById(campaignId);
        campaign.setLatestTime(latestTime);
        
        Map<String, List<Domain>> domainMap = Maps.newHashMap(); // 有活动的触发计划，以任务区分，数据形式：Map<taskName_taskId, domainList>
        List<Domain> domainList = null;
        Integer[] taskIds = null;
        List<Task> taskList = taskService.getTouchTask(campaignId, "campaign");
        if(taskList!=null && taskList.size()>0){
        	String taskIdStr = "";
        	for(Task t : taskList) {
        		taskIdStr += t.getTaskId() + ",";
        	}
        	taskIds = Converts._toIntegers(taskIdStr);
        	List<Touch> touchList = touchService.getByParentIds(taskIds);
        	String touchIdStr = ""; 
        	for(int i=0,len=touchList.size(); i<len; i++) {
        		Touch t = touchList.get(i);
        		touchIdStr += t.getTaskId() + ",";
        	}
        	Integer[] touchIds = Converts._toIntegers(touchIdStr);
        	List<Domain> domains = null;
        	if(touchIds!=null && touchIds.length>0){
        		Map<String, Object> paramsMap = Maps.newHashMap();
        		paramsMap.put("taskIds", touchIds);
        		domainList = domainService.getCampaignTouchList(paramsMap);
        		for(Domain d : domainList) {
        			String key = d.getTaskName() + "_" + d.getTaskId();
        			domainMap.put(key, null);
        		}
        		for(String k : domainMap.keySet()) {
        			domains = Lists.newArrayList();
        			for(Domain d : domainList) {
        				String key = d.getTaskName() + "_" + d.getTaskId();
        				if(k.equals(key)){
        					Map<String, Object> pMap = Maps.newHashMap();
        					pMap.put("taskId", d.getTaskId());
        					Integer[] types = new Integer[]{ TaskTypeConstant.TASK_SINGLE, TaskTypeConstant.TASK_TEST, TaskTypeConstant.TASK_SON_PLAN, TaskTypeConstant.TASK_API_GROUPS};
        					pMap.put("types", types);
        					Domain domain = domainService.getSum(pMap);
        					Integer parentReadUserCount = 0;
        					if(domain != null){
        						parentReadUserCount = domain.getReadUserCount();
        					}
        					d.setParentReadUserCount(parentReadUserCount);
        					domains.add(d);
        				}
        			}
        			domainMap.put(k, domains);
        		}
        	}
        }
    	domainService.writeCsvCampaignTouch(csvWriter, domainMap, taskIds, campaign);
    	
	}
	
	private void domainCopy(List<Result> sourceResults) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : sourceResults) {
			sbff.append(result.getCampaignId()).append(",");
		}
		List<Domain> domainList = this.getCampaignSumList(ConvertUtils.str2Int(sbff.toString()), "task");
		ResultDataOption.copy("task", domainList, sourceResults);
	}

}
