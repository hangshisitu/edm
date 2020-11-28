package cn.edm.controller.delivery;

import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.util.WebUtils;

import com.google.common.collect.Maps;

import cn.edm.constant.Const;
import cn.edm.constant.Message;
import cn.edm.constant.SearchDefine;
import cn.edm.domain.Smtp;
import cn.edm.exception.Errors;
import cn.edm.service.SmtpService;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;
import cn.edm.util.XssFilter;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
/**
 * Smtp
 * 
 * @Date 2014年6月3日 下午4:10:34
 * @author Lich
 *
 */
@Controller
@RequestMapping("/delivery/smtp")
public class SmtpController {
	
	private static Logger logger = LoggerFactory.getLogger(DeliveryController.class);
	
	@Autowired
	private SmtpService smtpService;

	/**
	 * 设置SMTP
	 * @return
	 */
	@RequestMapping(value="/add", method=RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		// 传参数Map
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		try {
			CSRF.generate(request);
			
			Integer id = XssFilter.filterXssInt(request.getParameter("id"));
			Smtp smtp = null;
			
			if (id != null) {
				paramsMap.put("id", id);
				smtp = smtpService.getSmtpById(paramsMap);
			}
			if (smtp == null) {
				smtp = new Smtp();
			}

			map.put("smtp", smtp);

			return "delivery/smtp/smtp_add";
		} catch (Exception e) {
			logger.error("(Smtp:add) error: ", e);
			return Views._404();
		}
	}
	/**
	 * 保存与更新SMTP
	 * @param request
	 * @param response
	 */
	@RequestMapping(value="/add", method=RequestMethod.POST)
	public @ResponseBody Map<String,Object> add(HttpServletRequest request, HttpServletResponse response) {
		CSRF.validate(request);
		// 返回结果Map
		Map<String,Object> resultMap = new HashMap<String,Object>();
		// 传参数Map
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		
		Integer id = XssFilter.filterXssInt(request.getParameter("id"));
		Smtp smtp = null;
		String oldIP = null;
		Map<String, Integer> oldIpMap = Maps.newHashMap();
		if(id != null){
			paramsMap.put("id", id);
			smtp = smtpService.getSmtpById(paramsMap);
			oldIP = smtp.getIp();
			String[] oldIpTmp = oldIP.split(";"); 
			for(String ipTemp : oldIpTmp) {
				oldIpMap.put(ipTemp, 0);
			}
			
		}else{
			smtp = new Smtp();
			smtp.setCreateTime(new Date());
		}
		try {
			String ipStr = request.getParameter("ip");
			if(StringUtils.isNotBlank(ipStr)){
				String[] ips = StringUtils.split(ipStr, ";");
				if(ips.length > 10){
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "每次ip提交不能超过10个");
					return resultMap;
				}
				// 先对传进来的参数进去是否重复记录判断
				String repeatIps = "";
				Map<String, Object> repeatIpMap = Maps.newHashMap();
				for(String ip : ips) {
					if(repeatIpMap.containsKey(ip)){
						repeatIps += ip + ",";
					}else{
						repeatIpMap.put(ip, 0);
					}
				}
				if(StringUtils.isNotBlank(repeatIps)){
					repeatIps = repeatIps.substring(0, repeatIps.length()-1);
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "本保存的记录的ip: " + repeatIps + " 有重复记录");
					return resultMap;
				}
				
				for(String ip : ips) {
					paramsMap.clear();
					paramsMap.put("ip", ip);
					List<Smtp> smtps = smtpService.getSmtpsByIp(paramsMap);
					if(smtps!=null && smtps.size()>0){
						for(Smtp s : smtps) {
							String exitsIp = s.getIp();
							String[] exitsIps = exitsIp.split(";");
							for(String p : exitsIps){
								// 在更新操作时，不对本记录的IP进行重复IP验证
								if(oldIpMap.containsKey(ip)){
									continue;
								}else{
									if(p.equals(ip)){
										repeatIps += ip + ",";
									}
								}
							}
						}
					}
				}
				if(StringUtils.isNotBlank(repeatIps)){
					repeatIps = repeatIps.substring(0, repeatIps.length()-1);
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "ip: " + repeatIps + " 已存在");
					return resultMap;
				}
			}else{
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "ip不能为空");
				return resultMap;
			}
			Integer quarter = XssFilter.filterXssInt(request.getParameter("quarter"));
			Integer hour = XssFilter.filterXssInt(request.getParameter("hour"));
			Integer day = XssFilter.filterXssInt(request.getParameter("day"));
			int maxValue = 999999999;
			// 输入的数值不能超过 9 位数字
			if(quarter>maxValue || hour>maxValue || day>maxValue){
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "输入的数值不能超过 9 位数字");
				return resultMap;
			}
			// 15分钟发送量<1小时发送量<1天发送量
			if(quarter>hour || quarter>day || hour>day) {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "频率必须满足：15分钟发送量<1小时发送量<1天发送量");
				return resultMap;
			}
			Integer size = XssFilter.filterXssInt(request.getParameter("size"));
			// 最大10240KB
			if(size > (Const.KB * 10)){
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "邮件大小不超过" + (Const.KB * 10) + "Kb");
				return resultMap;
			}
			
			smtp.setIp(ipStr);
			smtp.setDay(day);
			smtp.setHour(hour);
			smtp.setQuarter(quarter);
			smtp.setSize(size);
			
			smtpService.saveOrUpdateSmtp(smtp);

			resultMap.put(Message.RESULT, Message.SUCCESS);
			resultMap.put(Message.MSG, (id!=null ? "修改" : "新建") + "SMTP成功");
		} catch (Errors e) {
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, e.getMessage());
			logger.error("(Smtp:add) error: ", e);
			e.printStackTrace();
		} catch (Exception e) {
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, (id!=null ? "修改" : "新建") + "SMTP失败");
			logger.error("(Smtp:add) error: ", e);
			e.printStackTrace();
		}

		return resultMap;
	}
	/**
	 * SMTP列表
	 * @return
	 */
	@RequestMapping(value="/smtpList", method=RequestMethod.GET)
	public String smtpList(HttpServletRequest request, ModelMap modelMap, 
			@RequestParam(value="currentPage", defaultValue="1") int currentPage,
			@RequestParam(value="pageSize", defaultValue="20") int pageSize) {
		
		//获取查询参数
		Map<String,Object> initParamsMap = WebUtils.getParametersStartingWith(request, SearchDefine.SEARCH_PREFIX);
		//过滤参数中的空值
		Map<String,Object> realParamsMap = CommonUtil.getRealSearchParams(initParamsMap, modelMap);
		
		Pagination<Smtp> pagination = smtpService.getSmtpPage(realParamsMap, "", currentPage, pageSize);
		modelMap.put("pagination", pagination);
		
		return "delivery/smtp/smtp_list";
	}
	@RequestMapping(value="/delete", method=RequestMethod.POST)
	public String delete(HttpServletRequest request, HttpServletResponse response, RedirectAttributes redirectAttributes) {
		Integer id = Https.getInt(request, "id", R.CLEAN);
		Map<String, Object> paramsMap = new HashMap<String, Object>();
		paramsMap.put("id", id);
		Smtp smtp = smtpService.getSmtpById(paramsMap);
		if(smtp == null) {
			redirectAttributes.addFlashAttribute(Message.MSG, "删除SMTP配置不存在");
		}else{
			smtpService.deleteSmtpById(paramsMap);
			redirectAttributes.addFlashAttribute(Message.MSG, "删除SMTP成功");
		}
		return "redirect:smtpList";
	}
}
