package cn.edm.controller.report;

import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.util.WebUtils;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.domain.Domain;
import cn.edm.domain.Task;
import cn.edm.service.DomainService;
import cn.edm.service.ResultService;
import cn.edm.service.TaskService;
import cn.edm.util.CommonUtil;
import cn.edm.util.ConvertUtils;
import cn.edm.util.MyHashMap;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.StreamToString;

/**
 * 发件人域名统计
 * @author Luxh
 */
@Controller
@RequestMapping("/senderDomain")
public class SenderDomainStatController {
	
	@Autowired
	private TaskService taskService;
	
	@Autowired
	private ResultService resultService;
	
	@Autowired
	private DomainService domainService;
	
	@SuppressWarnings("unchecked")
	@RequestMapping(value="/list", method=RequestMethod.GET)
	public String list(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		try {
			String senderEmail =StreamToString.getStringByUTF8(request,"senderEmail");
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			initParamsMap.put("senderEmail",senderEmail);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			
			List<Task> taskList = taskService.getBySenderEmail(realParamsMap);
			List<Domain> domainList = Lists.newArrayList();
			List<Domain> pageDomainList = Lists.newArrayList();
			if(taskList!=null && taskList.size()>0) {
				//存放域名和使用该域名发送的任务id
				//key=163.com(域名)  value=1,2,3(任务id)
				Map<String,Object> domain2TaskIdMap = new MyHashMap();
				String senderDomain = null;
				for(Task task : taskList) {
					senderDomain = StringUtils.substringAfterLast(task.getSenderEmail(), "@");
					if(senderDomain != null && !"".equals(senderDomain)) {
						domain2TaskIdMap.put(senderDomain, task.getTaskId());
					}
					senderDomain = null;
				}
				
				//根据发送域名关联的任务id查询统计结果
				Map<String,Object> paramsMap = Maps.newHashMap();
				Iterator<Entry<String,Object>>  iterator =  domain2TaskIdMap.entrySet().iterator();
				while(iterator.hasNext()) {
					Entry<String,Object> entry = iterator.next();
				    String key = entry.getKey();
				    String value = String.valueOf(entry.getValue());
				    Integer[] taskIds = ConvertUtils.str2Int(value);
				    paramsMap.put("taskIds", taskIds);
				    Domain domain = domainService.getByDomainStat(paramsMap);
				    if(domain != null) {
				    	domain.setSenderDomain(key);
				    	domainList.add(domain);
				    }
				    
				}
				
				//对结果集合进行分页处理
		        if(domainList != null && domainList.size()>0) {
		        	int first = (currentPage-1)*pageSize;
		 	        int end = first + pageSize -1;
		 	        if(end>=domainList.size()) {
		 	        	end = domainList.size()-1;
		 	        }
		 	        for(int i=first;i<=end;i++) {
		 	        	pageDomainList.add(domainList.get(i));
		 	        }
		        }
			}
			
			Pagination<Domain> pagination = new Pagination<Domain>(currentPage, pageSize, domainList.size(), pageDomainList);
			modelMap.put("pagination", pagination);
		}catch(Exception e) {
			e.printStackTrace();
			modelMap.put(Message.MSG, "查询数据出错！");
		}
		
		return "report/senderDomainList";
		
	}
	
	
	@RequestMapping(value="/detailList", method=RequestMethod.GET)
	public String detailList(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize) {
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
		String senderEmail = XssFilter.filterXss(request.getParameter("senderEmail"));
		
		realParamsMap.put("senderEmail", senderEmail);
		modelMap.put("senderEmail", senderEmail);
		
		List<Task> taskList = taskService.getBySenderEmail(realParamsMap);
		
		if(taskList!=null && taskList.size()>0) {
			Integer[] taskIds = new Integer[taskList.size()];
			for(int i=0;i<taskList.size();i++) {
				taskIds[i] = taskList.get(i).getTaskId();
			}
			
			//Map<String,Object> paramsMap = Maps.newHashMap();
			//paramsMap.put("taskIds", taskIds);
			realParamsMap.put("taskIds",taskIds);
			Pagination<Domain> pagination = domainService.getPaginationByDomain(realParamsMap, "", currentPage, pageSize);
			modelMap.put("pagination", pagination);
		}
		
		return "report/senderDomainDetailList";
	}
}
