package cn.edm.controller.delivery;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.util.WebUtils;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.constant.SearchDefine;
import cn.edm.service.DeliveryService;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.EncodeUtils;
import cn.edm.util.IOUitl;
import cn.edm.util.Pagination;
import cn.edm.vo.DeliveryLogVo;
import cn.edm.vo.DeliveryVo;

import com.google.common.collect.Lists;


@Controller
@RequestMapping("/delivery")
public class DeliveryController {
	
	private static Logger log = LoggerFactory.getLogger(DeliveryController.class);
	
	@Autowired
	private DeliveryService deliveryService;
	
	/**
	 * 发件域监控首页
	 */
	@RequestMapping("/sendDomainMonitor")
	public String sendDomainMonitor(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize){
		try {
			
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			
			List<DeliveryVo> deliveryList = deliveryService.getDeliveryVoList(String.valueOf(realParamsMap.get("fromDomain")),String.valueOf(realParamsMap.get("status")));
			
			List<DeliveryVo> pageResultList = Lists.newArrayList();
			 //对结果集合进行分页处理
	        if(deliveryList != null && deliveryList.size()>0) {
	        	int first = (currentPage-1)*pageSize;
	 	        int end = first + pageSize -1;
	 	        if(end>=deliveryList.size()) {
	 	        	end = deliveryList.size()-1;
	 	        }
	 	        for(int i=first;i<=end;i++) {
	 	        	pageResultList.add(deliveryList.get(i));
	 	        }
	        }
			
	        Pagination<DeliveryVo> pagination = new Pagination<DeliveryVo>(currentPage,pageSize,deliveryList!=null?deliveryList.size():0,pageResultList);
			modelMap.put("pagination", pagination);
		}catch(Exception e) {
			e.printStackTrace();
		}
		
		return "delivery/sendDomainMonitor";
	}

	@RequestMapping("/ipDeliveryLog")
	public String ipDeliveryLog(HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value="currentPage",defaultValue="1") int currentPage,
			@RequestParam(value="pageSize",defaultValue="20") int pageSize){
		try {
			
			int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
			
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			String date = String.valueOf(realParamsMap.get("queryTime"));
			if(date == null || "null".equals(date.trim())|| "".equals(date.trim())){
				date = DateUtil.getDateStr("yyyy-MM-dd");
			}
			Map<String,DeliveryLogVo> dataMap = deliveryService.getDeliveryLogList(String.valueOf(realParamsMap.get("ip")), date,startIndex,pageSize);
			
			modelMap.put("queryTime", date);
			Integer total = dataMap.get("total").getTotal();
			if(total==null) {
				total = 0;
			}
			dataMap.remove("total");
			modelMap.put("dataMap", dataMap);
			
			Pagination pagination = new Pagination(currentPage,pageSize,total,null);
			modelMap.put("pagination", pagination);
		}catch(Exception e) {
			e.printStackTrace();
		}
		
		return "delivery/ipDeliveryLog";
	}
	
	@RequestMapping(value="/exportLog",method=RequestMethod.POST)
	public void exportLog(HttpServletRequest request,HttpServletResponse response,ModelMap modelMap) {
		ServletOutputStream out = null;
		CSVWriter csvWriter = null;
		try {
			
			out = IOUitl.getOut(response,EncodeUtils.urlEncode("IP投递日志.csv"));
			csvWriter = new CSVWriter(new OutputStreamWriter(out));
			
			//获取查询参数
			Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
			//过滤参数中的空值
			Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap,modelMap);
			
			String ip = String.valueOf(realParamsMap.get("ip"));
			String date = String.valueOf(realParamsMap.get("queryTime"));
			
			String[] checkedIds = request.getParameterValues("checkedIds");
			if(checkedIds == null || checkedIds.length<=0) {
				checkedIds = new String[]{ip};
			}
			if(date == null || "null".equals(date.trim())|| "".equals(date.trim())){
				date = DateUtil.getDateStr("yyyy-MM-dd");
			}
			deliveryService.writeCsv(csvWriter, date, checkedIds);
			csvWriter.flush();
		}catch(Exception e) {
			log.error(e.getMessage(), e);
		}finally {
			try {
				if(out != null) {
					out.close();
				}
				if(csvWriter != null) {
					csvWriter.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	@RequestMapping("/ruleSet")
	public String ruleSet(){
		
		return "delivery/ruleSet";
	}
	
	@RequestMapping("/ruleCustomSet")
	public String ruleCustomSet(){
		
		return "delivery/ruleCustomSet";
	}
	
	@RequestMapping("/ruleQuery")
	public String ruleQuery(){
	
		return "delivery/ruleQuery";
	}
	
}
