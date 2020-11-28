package cn.edm.web.action;

import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.joda.time.DateTime;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.app.security.Securitys;
import cn.edm.consts.Cnds;
import cn.edm.consts.Status;
import cn.edm.consts.Type;
import cn.edm.consts.mapper.RoleMap;
import cn.edm.model.Corp;
import cn.edm.model.Cos;
import cn.edm.model.Domain;
import cn.edm.model.History;
import cn.edm.model.Result;
import cn.edm.model.Task;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.service.CorpService;
import cn.edm.service.CosService;
import cn.edm.service.DomainService;
import cn.edm.service.HistoryService;
import cn.edm.service.ResultService;
import cn.edm.service.TaskService;
import cn.edm.utils.Asserts;
import cn.edm.utils.Converts;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.facade.Corps;
import cn.edm.web.facade.Domains;

import com.google.common.collect.Lists;

@Controller
public class IndexAction {

	private static final Logger logger = LoggerFactory.getLogger(IndexAction.class);
	
	@Autowired
	private CorpService corpService;
	@Autowired
	private CosService cosService;
	@Autowired
	private DomainService domainService;
	@Autowired
	private HistoryService historyService;
	@Autowired
	private ResultService resultService;
	@Autowired
	private TaskService taskService;
	
	@RequestMapping(value = { "/", "/index" }, method = RequestMethod.GET)
	public String index(HttpServletRequest request, ModelMap map) {
		try {
			
			/*int corpId = UserSession.getCorpId();
	        Corp corp = corpService.get(corpId);
	        Cos  cos  = cosService.get(corp.getCosId());
	        Integer type = cos.getType();
	        Integer status = cos.getType();
	        int deadLine =0;
	        int remindDate =0;
	        if(status.equals("0")){
	        	logger.info("[ 试用账号 ] : 试用套餐已被禁用或者已过期    - 请联系相关人员续费套餐!");
	        	request.setAttribute("message","试用账号已被禁用或者已过期,请联系相关人员续费套餐!");
	        	return "login";
	        }
	        if( type.equals("0") && cos.getEndTime() != null ){
	        	deadLine = DateUtil.compareMills(cos.getEndTime(),new Date());
	        	remindDate = DateUtil.compareDay(cos.getEndTime(),new Date());
	        }
	        //测试账号 如有提醒选项 提前七天登录弹框提示
	        if(type.equals("0")){      		
	        	if(deadLine <= 0){
	        		logger.info("[ 试用账号 ] : 试用账号已到期 - 请联系相关人员续费套餐!");  
	                if(cos.getRemind()!=null && cos.getRemind() != "" && cos.getRemind() == "1"){  
	                	request.setAttribute("message","试用账号已经到期,请联系相关人员续费套餐!");
	                }
	        		return "login";
	        	}        		       		
	        }*/
	        
			audit("waited", map);
			audit("completed", map);
			report(map);
			account(map);
			history(map);
			
/*			if(remindDate > 0 && remindDate <=7){
				logger.info("[ 试用账号 ] : 试用账号将要到期 - 请联系相关人员续费套餐!");
	        	if(cos.getRemind()!=null && cos.getRemind() != "" && cos.getRemind() == "1"){   			        			
	        		request.setAttribute("message","试用账号到期提醒 - 还剩 ("+deadLine+") 天!");
	    		}        		
	    	}*/
			
			return "index";
		} catch (Exception e) {
			logger.error("(Index:index) error: ", e);
			return Views._404();
		}
	}
	
	private void audit(String m, ModelMap map) {
		if (Securitys.hasAnyRole(new String[] { RoleMap.ADM.getName(), RoleMap.MGR.getName(), RoleMap.ALL.getName() })) {
			Page<Task> page = new Page<Task>();
			page.setPageNo(1);
			page.setPageSize(6);
			page.setAutoCount(false);
			Integer[] statuses = Status.task(m);
			MapBean mb = new MapBean();
			List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
            Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
			page = taskService.search(page, mb, corpIds, null, UserSession.getCorpId(), null, null, null, null, statuses, m, Cnds.EQ);
			map.put(m + "Page", page);
		}
	}

	private void report(ModelMap map) {
		Page<Result> page = new Page<Result>();
		page.setPageNo(1);
		page.setPageSize(6);
		page.setAutoCount(false);
		
		MapBean mb = new MapBean();
		List<Corp> corpChildren = corpService.findChildren(UserSession.getCorpId());
        Integer[] corpIds = Corps.combine(UserSession.getCorpId(), corpChildren);
		page = resultService.search(page, mb, corpIds, null, null,
		        null, null, null, new Integer[] { Type.FORMAL } , Cnds.EQ);
		domain(corpIds, UserSession.containsCorpId(), page.getResult());
		
		map.put("reportPage", page);
	}
	
	private void domain(Integer[] corpIds, Integer corpId, List<Result> resultList) {
		StringBuffer sbff = new StringBuffer();
		for (Result result : resultList) {
			sbff.append(result.getTaskId()).append(",");
		}
		
		List<Domain> domainList = domainService.group(corpIds, corpId, UserSession.bindCorpId(), Converts._toIntegers(sbff.toString()));
        Domains.copy("task", domainList, resultList);
	}
	
	private void account(ModelMap map) {
		DateTime now = new DateTime();
		Corp corp = corpService.get(UserSession.getCorpId());
		Cos cos = cosService.get(corp.getCosId());
		Asserts.isNull(cos, "套餐");
		
		DateTime startOfMonth = now.dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue();
		DateTime endOfMonth = now.dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue();
		
        List<Corp> corpList = corpService.find();
        Corp currentCorp = corpService.get(corp.getCorpId());
        Corp root = Corps.getRoot(corpList, currentCorp);
        List<Integer> corpIdList = Lists.newArrayList();
        corpService.cross(corpList, root.getCorpId(), corpIdList);
        corpIdList.add(root.getCorpId()); //算发送量的时候需要加上顶级父机构的发送量
        Integer[] corpIds = Converts._toIntegers(corpIdList);
         
		int totalSent = taskService.sentCount(corpIds, null, null);
		int monthSent = taskService.sentCount(corpIds, startOfMonth, endOfMonth);
		int daySent = taskService.sentCount(corpIds, now, now);
		
		map.put("company", corp.getCompany());
		map.put("cos", cos);
		map.put("totalSent", totalSent);
		map.put("monthSent", monthSent);
		map.put("daySent", daySent);
	}
	
	private void history(ModelMap map) {
		Page<History> page = new Page<History>();
		page.setPageNo(1);
		page.setPageSize(6);
		page.setAutoCount(false);

		MapBean mb = new MapBean();
		page = historyService.search(page, mb, UserSession.getCorpId(), UserSession.getUserId());

		map.put("historyPage", page);
		map.put("mb", mb);
	}

	@RequestMapping(value = "/404", method = RequestMethod.GET)
	public String _404() {
		return "common/404";
	}

	@RequestMapping(value = "/403", method = RequestMethod.GET)
	public String _403() {
		return "common/403";
	}
}
