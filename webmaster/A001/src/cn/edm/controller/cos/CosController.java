package cn.edm.controller.cos;

import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.constants.Status;
import cn.edm.domain.Cos;
import cn.edm.service.CosService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.Calendars;
import cn.edm.utils.StreamToString;

import com.google.gson.Gson;

@Controller
@RequestMapping("/cos")
public class CosController {

	@Autowired
	private CosService cosService;

	/**
	 * 获取套餐信息列表
	 * 
	 * @param request
	 * @return
	 */
	@RequestMapping("/cosList")
	public String getCosInfoList(
			HttpServletRequest request,ModelMap modelMap,
			@RequestParam(value = "currentPage", defaultValue = "1") int currentPage,
			@RequestParam(value = "pageSize", defaultValue = "20") int pageSize) {
		/*			request.setCharacterEncoding("UTF-8");*/ //get方式提交的表单数据，中文乱码，utf-8设置无效
					// 获取查询参数
					String conName =StreamToString.getStringByUTF8(request, "cosName");
					Map<String, Object> initParamsMap = WebUtils.getParametersStartingWith(
							request, SearchDefine.SEARCH_PREFIX);
					initParamsMap.put("cosName",conName);
					// 过滤参数中的空值			
					Map<String, Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
					Pagination<Cos> pagination = cosService.getPagination(realParamsMap,"", currentPage, pageSize);
					modelMap.put("pagination", pagination);		
					
		            return "/cos/cosList";
	}
	
	/**
	 * 根据套餐id获取套餐信息
	 * 
	 * @param request
	 * @return
	 */
	@RequestMapping("/getByCosId")
	public void getCosInfo(HttpServletRequest request,
			HttpServletResponse response) {

		response.setContentType("application/json;charset=utf-8");
		response.setHeader("pragma", "no-cache");
		response.setHeader("cache-control", "no-cache");
		PrintWriter out = null;
		Gson gson = new Gson();
		try {
			String cosIdTemp = request.getParameter("cosId");
			out = response.getWriter();
			if (cosIdTemp != null && !"".equals(cosIdTemp)) {
				int cosId = Integer.parseInt(cosIdTemp);
				Cos cos = cosService.getCosByCosId(cosId);
				String result = gson.toJson(cos);
				out.write(result);
				out.flush();
			}
		} catch (Exception e) {
			e.printStackTrace();
			out.write("服务器端出错");
		} finally {
			if (out != null) {
				out.close();
			}
		}

	}

	/**
	 * 自定义套餐
	 * 
	 * @return
	 */
	@RequestMapping("/cos")
	public String cos(HttpServletRequest request) {
		// CSRF.generate(request);
		return "/cos/cos";
	}

	/**
	 * 保存自定义套餐
	 * 
	 * @return
	 */
	@RequestMapping("/save")
	public @ResponseBody Map<String, Object> save(HttpServletRequest request) {
		// CSRF.validate(request);
		Map<String, Object> resultMap = new HashMap<String, Object>();
		String type = (String) request.getParameter("type");
		String cosName = (String) request.getParameter("cosName");
		String userCount = (String) request.getParameter("userCount");
		String daySend = (String) request.getParameter("daySend");
		String weekSend = (String) request.getParameter("weekSend");
		String monthSend = (String) request.getParameter("monthSend");
		String totalSend = (String) request.getParameter("totalSend");
		String startTime = (String) request.getParameter("startTime");
		String endTime = (String) request.getParameter("endTime");
		String remind = (String) request.getParameter("remind");

		// 试用套餐
		if (type == "0") {
			if (startTime.compareTo(endTime) > -1) {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "有效期限开始日期必须小于结束日期");
				return resultMap;
			}
			if (startTime.compareTo(new DateTime().toString(Calendars.SIMPLE_DATE_TIME)) < 1) {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "有效期限开始日期必须大于当前日期");
				return resultMap;
			}

		}

		try {
			if (XssFilter.isValid(cosName)) {
				// cos.setCosName("自定义每日发送"+cos.getDaySend());

				List<Cos> all = cosService.selectAll();
				boolean flag = false;
				if (all != null && all.size() > 0) {
					for (Cos c : all) {
						if (c.getCosName().equals(cosName.trim())) {
							flag = true;
							break;
						}
					}
				}

				if (flag) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "套餐名称已存在！");
				} else {
					Cos cos = new Cos();
					cos.setCosName(cosName.trim());
					cos.setType(Integer.parseInt(type));
					cos.setStatus(""+Status.DISABLED); // 1:可用，0：不可用
					cos.setUserCount(Integer.parseInt(userCount));
					cos.setDaySend(Integer.parseInt(daySend));
					cos.setWeekSend(Integer.parseInt(weekSend));
					cos.setMonthSend(Integer.parseInt(monthSend));
					cos.setTotalSend(Integer.parseInt(totalSend));
					if(type.equals("0") && startTime !=null && endTime != null){
						cos.setStartTime(Calendars.parse(startTime, Calendars.SIMPLE_DATE_TIME)
								.toDate());
						cos.setEndTime(Calendars.parse(endTime, Calendars.SIMPLE_DATE_TIME)
								.toDate());
						cos.setRemind(remind);	
					}					
					cosService.save(cos);
					resultMap.put(Message.RESULT, Message.SUCCESS);
					resultMap.put(Message.MSG, "保存成功");
					resultMap.put("cos", cos);
				}

			} else {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "套餐名称含有非法字符！");
			}

		} catch (Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "保存出错");
		}
		return resultMap;
	}
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/freeze")
	public String freeze(HttpServletRequest request,ModelMap modelMap,
			RedirectAttributes redirectAttributes,@RequestParam(value="cosId") String cosId){
		Cos cos = cosService.getCosByCosId(Integer.parseInt(cosId));
		
		if(cos != null){
			Map<String,Object> paramMap = new HashMap<String,Object>();
			paramMap.put("status",Status.DISABLED );
			paramMap.put("cosId",cosId);
			cosService.updateCosStatus(paramMap);
			redirectAttributes.addFlashAttribute("message", "套餐冻结成功");
		}else{
			redirectAttributes.addFlashAttribute("message", "套餐不存在！");
		}
		
		return "redirect:cosList";	
	}
	
	
	
	/**
	 * @param request
	 * @param modelMap
	 * @return  返回企业定制页面
	 */
	@RequestMapping(value="/unfreeze")
	public String unfreeze(HttpServletRequest request,ModelMap modelMap,
			RedirectAttributes redirectAttributes,@RequestParam(value="cosId") String cosId){
		//CSRF.validate(request);
        //激活后可以显示在机构选择套餐里面
			Cos cos = cosService.getCosByCosId(Integer.parseInt(cosId));
			if(cos == null) {
				redirectAttributes.addFlashAttribute("message", "套餐不存在！");
			}else {
				Map<String,Object> paramMap = new HashMap<String,Object>();
				paramMap.put("status", Status.ENABLED);
				paramMap.put("cosId",cosId);				
				cosService.updateCosStatus(paramMap);				
			}
		    return "redirect:cosList";	
	}
	
	/**
	 * 查看
	 */
	@RequestMapping(value="/view", method=RequestMethod.GET)
	public String view(HttpServletRequest request, ModelMap modelMap, @RequestParam(value="cosId") String cosId) {
		CSRF.generate(request);
		cosId = (String) XssFilter.filter(cosId);
		Cos cos = cosService.getCosByCosId(Integer.parseInt(cosId));
		if(cos == null) {
			throw new RuntimeException("用户不存在");
		}
		modelMap.put("cos",cos);		
		return "/cos/cosView";
	}
	
	
}
