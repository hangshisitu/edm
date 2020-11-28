package cn.edm.controller.property;

import java.io.File;
import java.net.ConnectException;
import java.util.Collections;
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
import org.springframework.web.bind.annotation.ResponseBody;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Message;
import cn.edm.constant.PropertyConstant;
import cn.edm.domain.Label;
import cn.edm.domain.LabelCategory;
import cn.edm.domain.LabelHistory;
import cn.edm.domain.Task;
import cn.edm.exception.Errors;
import cn.edm.service.LabelCategoryService;
import cn.edm.service.LabelHistoryService;
import cn.edm.service.LabelService;
import cn.edm.service.TaskService;
import cn.edm.util.Apis;
import cn.edm.util.CSRF;
import cn.edm.util.CommonUtil;
import cn.edm.util.DateUtil;
import cn.edm.util.Download;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.PropertyStatComparator;
import cn.edm.util.Regex;
import cn.edm.util.ValidUtil;
import cn.edm.util.XssFilter;
import cn.edm.vo.PropertyVo;
import cn.edm.vo.PropertyVoStat;

@Controller
@RequestMapping("/label")
public class LabelController {
	
	private static Logger log = LoggerFactory.getLogger(LabelController.class);
	
	@Autowired
	private LabelCategoryService labelCategoryService;
	
	@Autowired
	private LabelService labelService;
	
	@Autowired
	private LabelHistoryService labelHistoryService;
	
	@Autowired
	private TaskService taskService;
	
	
	@RequestMapping("/list")
	public String list(HttpServletRequest request,ModelMap modelMap) {
		
		String categoryIdTemp = XssFilter.filterXss(request.getParameter("categoryId"));
		if(StringUtils.isNotEmpty(categoryIdTemp)) {
			Integer categoryId = Integer.parseInt(categoryIdTemp);
			LabelCategory lc = labelCategoryService.getById(categoryId);
			List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
			modelMap.put("labelCategoryList", labelCategoryList);
			modelMap.put("labelCategory", lc);
			if(lc != null) {
				List<Label> labelList = labelService.getByCategoryId(lc.getCategoryId());
				//获取标签id
				String lableIds = labelService.getLabelId(labelList);
				PropertyVoStat propertyVoStat = null;
				if(StringUtils.isEmpty(lableIds)) {
					propertyVoStat = new PropertyVoStat();
					propertyVoStat.setEmailNum(0);
					propertyVoStat.setLabelNum(0);
				}else {
					//返回api的url
					String url = PropertiesUtil.get(ConfigKeys.PROPERTY_URL)+PropertyConstant.METHOD_QUERY+"label="+lableIds+"&type="+PropertyConstant.TYPE_COUNT;
					String content = Apis.getProperty(url, "utf-8");
					//对api返回的结果进行封装
					List<PropertyVo> propertyVoList = labelService.getPropertyVo(content,labelList);
					/*int sum = 0;
					for(PropertyVo pv:propertyVoList) {
						sum += pv.getNum();
					}*/
					//统计分析
					if(propertyVoList != null&&propertyVoList.size()>0) {
						Collections.sort(propertyVoList,new PropertyStatComparator());
					}
					propertyVoStat = labelService.stat(lc, propertyVoList,labelList.size());
				}
				modelMap.put("propertyVoStat", propertyVoStat);
			}
			
		}
		return "property/labelList";
	}
	
	
	@RequestMapping("/add")
	public String add(HttpServletRequest request,ModelMap modelMap) {
		CSRF.generate(request);
		List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
		modelMap.put("labelCategoryList", labelCategoryList);
		return "property/labelEdit";
	}
	
	
	@RequestMapping("/saveLabel")
	public @ResponseBody Map<String,Object> saveLabel(HttpServletRequest request,Label label) {
		CSRF.validate(request);
		Map<String,Object> resultMap = Maps.newHashMap();
		try {
			Map<String,Object> map = labelService.verifyXss(label);
			if((Boolean) map.get(Message.RESULT)) {
				if(labelService.isLabelExist(label.getLabelName().trim())) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "标签名称已存在!");
				}else {
					label.setLabelName(label.getLabelName().trim());
					label.setParentId(0);
					labelService.save(label);
					resultMap.put(Message.RESULT, Message.SUCCESS);
					resultMap.put(Message.MSG, "新建标签成功!");
					resultMap.put("lcId", label.getCategoryId());
				}
			}else {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, map.get(Message.MSG));
			}
		}catch(Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "新建标签出错！");
		}
		return resultMap;
	}
	
	@RequestMapping("/import")
	public String importLabel(HttpServletRequest request,ModelMap modelMap) {
		CSRF.generate(request);
		String taskIdTemp = XssFilter.filterXss(request.getParameter("taskId"));
		if(StringUtils.isNotEmpty(taskIdTemp)) {
			Integer taskId = Integer.parseInt(taskIdTemp);
			Task task = taskService.getByTaskId(taskId);
			if(task!=null) {
				modelMap.put("taskId", task.getCorpId()+"."+task.getTaskId()+"."+task.getTemplateId());
			}
		}
		
		String labelIdTemp = XssFilter.filterXss(request.getParameter("labelId"));
		if(StringUtils.isNotEmpty(labelIdTemp)) {
			Integer labelId = Integer.parseInt(labelIdTemp);
			modelMap.put("labelId", labelId);
		}
		
		
		List<Label> industryList = Lists.newArrayList();
		List<Label> societyList = Lists.newArrayList();
		List<Label> ageList = Lists.newArrayList();
		List<Label> genderList = Lists.newArrayList();
		List<Label> labelList = labelService.getAll();
		if(labelList!=null&labelList.size()>0) {
			for(Label l:labelList) {
				if(1==l.getCategoryId()) {
					industryList.add(l);
				}else if(2==l.getCategoryId()) {
					societyList.add(l);
				}else if(3==l.getCategoryId()){
					ageList.add(l);
				}else if(4==l.getCategoryId()){
					genderList.add(l);
				}
			}
		}
		
		modelMap.put("industryList", industryList);
		modelMap.put("societyList", societyList);
		modelMap.put("ageList", ageList);
		modelMap.put("genderList", genderList);
		return "property/import";
	}
	
	@RequestMapping("/importPropLib")
	public @ResponseBody Map<String,Object> importPropLib(HttpServletRequest request) {
		CSRF.validate(request);
		Map<String,Object> resultMap = Maps.newHashMap();
		try {
			String labelIds = XssFilter.filterXss(request.getParameter("label_ids"));
			String task =  XssFilter.filterXss(request.getParameter("task"));
			
			if(StringUtils.isNotEmpty(task)) {
				
				String[] ids = StringUtils.split(task, ".");
				if(ids==null||ids.length!=3) {
					resultMap.put(Message.RESULT, Message.FAILURE);
					resultMap.put(Message.MSG, "任务ID格式不正确！");
				}else {
					Integer corpId = Integer.parseInt(ids[0].trim());
					Integer taskId = Integer.parseInt(ids[1].trim());
					Integer templateId = Integer.parseInt(ids[2].trim());
					
					//判断是否已导入
					Map<String,Object> params = Maps.newHashMap();
					params.put("corpId", corpId);
					params.put("taskId", taskId);
					params.put("templateId", templateId);
					List<LabelHistory> historyList = labelHistoryService.getLabelHistory(params);
					if(historyList!=null && historyList.size()>0) {
						LabelHistory history = historyList.get(0);
						if(String.valueOf(PropertyConstant.HANDLING).equals(history.getStatus())||"200".equals(history.getStatus())){
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, "该任务已提交处理，不能重复提交！");
						}else if("201".equals(history.getStatus())){
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, "任务ID不存在！");
						}else if("203".equals(history.getStatus())){
							resultMap.put(Message.RESULT, Message.FAILURE);
							resultMap.put(Message.MSG, "任务ID不存在！");
						}else {
							params.put("status", String.valueOf(PropertyConstant.HANDLING));
							params.put("labelStatus",String.valueOf(PropertyConstant.HANDLING));
							params.put("labelIds", labelIds);
							labelHistoryService.updateStatusAndLabel(params);
							resultMap.put(Message.RESULT, Message.SUCCESS);
							resultMap.put(Message.MSG, "已提交后台处理！");
						}
					}else {
						List<Task> tasks = taskService.getTasks(params);
						if(tasks != null&&tasks.size()>0) {
							Task importTask = tasks.get(0);
							if(importTask.getLabelStatus()!=null&&(importTask.getLabelStatus()==PropertyConstant.HANDLED_SUCCESS||importTask.getLabelStatus()==PropertyConstant.HANDLING)) {
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, "该任务已提交处理，不能重复提交！");
							}else if(!DateUtil.isTimeEnough(importTask.getCreateTime())){
								String days= PropertiesUtil.get(ConfigKeys.PROPERTY_DAYS);
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, "任务创建超过"+days+"天后方可导入！");
							}else if(importTask.getType()==1){
								resultMap.put(Message.RESULT, Message.FAILURE);
								resultMap.put(Message.MSG, "测试任务不可导入！");
							}else {
								LabelHistory lh = new LabelHistory();
								lh.setCorpId(corpId);
								lh.setTaskId(taskId);
								lh.setTemplateId(templateId);
								lh.setStatus(String.valueOf(PropertyConstant.HANDLING));
								lh.setLabelIds(labelIds);
								labelHistoryService.save(lh);
								resultMap.put(Message.RESULT, Message.SUCCESS);
								resultMap.put(Message.MSG, "已提交后台处理！");
							}
						}else {
							LabelHistory lh = new LabelHistory();
							lh.setCorpId(corpId);
							lh.setTaskId(taskId);
							lh.setTemplateId(templateId);
							lh.setStatus(String.valueOf(PropertyConstant.HANDLING));
							lh.setLabelIds(labelIds);
							labelHistoryService.save(lh);
							resultMap.put(Message.RESULT, Message.SUCCESS);
							resultMap.put(Message.MSG, "已提交后台处理！");
						}
					}
				}
				
			}else {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "任务ID格式不正确！");
			}
		}catch(NumberFormatException nfe) {
			log.error(nfe.getMessage(),nfe);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "任务ID格式不正确！");
		}catch(Exception e) {
			log.error(e.getMessage(),e);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "提交失败！");
		}
		return resultMap;
	}
	
	@RequestMapping("/export")
	public String exportLabel(HttpServletRequest request,ModelMap modelMap) {
		try {
			CSRF.generate(request);
			String labelIdTemp = XssFilter.filterXss(request.getParameter("labelId"));
			if(StringUtils.isNotEmpty(labelIdTemp)) {
				Integer labelId = Integer.parseInt(labelIdTemp);
				modelMap.put("labelId", labelId);
			}
			
			//获取类别
			List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
			
			//存放统计结果
			Map<Integer,PropertyVoStat> statMap = Maps.newLinkedHashMap();
			
			
			List<PropertyVo> industryList = null;
			List<PropertyVo> societyList = null;
			List<PropertyVo> ageList = null;
			List<PropertyVo> genderList = null;
			
			
			
			if(labelCategoryList != null && labelCategoryList.size()>0) {
				//获取各个类别下的标签
				for(LabelCategory lc:labelCategoryList) {
					List<Label> labelList = labelService.getByCategoryId(lc.getCategoryId());
					
					//获取标签id
					String lableIds = labelService.getLabelId(labelList);
					PropertyVoStat propertyVoStat = null;
					if(StringUtils.isNotEmpty(lableIds)) {
						//返回api的url
						String url = PropertiesUtil.get(ConfigKeys.PROPERTY_URL)+PropertyConstant.METHOD_QUERY+"label="+lableIds+"&type="+PropertyConstant.TYPE_COUNT;
						log.info("=============查询总数API---:"+url+"============");
						String content = Apis.getProperty(url, "utf-8");
						//对api返回的结果进行封装
						List<PropertyVo> propertyVoList = labelService.getPropertyVo(content,labelList);
						propertyVoStat = labelService.stat(lc, propertyVoList,labelList.size());
					}else {
						propertyVoStat = new PropertyVoStat();
						propertyVoStat.setLabelCategory(lc);
						propertyVoStat.setEmailNum(0);
						propertyVoStat.setLabelNum(0);
					}
					
					//统计分析
					statMap.put(lc.getCategoryId(), propertyVoStat);
					
				}
				
				for(LabelCategory lc:labelCategoryList) {
					if(lc.getCategoryId()==1) {
						industryList = statMap.get(lc.getCategoryId()).getPropertyVoList();
					}else if(lc.getCategoryId()==2) {
						societyList = statMap.get(lc.getCategoryId()).getPropertyVoList();
					}else if(lc.getCategoryId()==3) {
						ageList = statMap.get(lc.getCategoryId()).getPropertyVoList();
					}else if(lc.getCategoryId()==4) {
						genderList = statMap.get(lc.getCategoryId()).getPropertyVoList();
					}
					
				}
			}
			modelMap.put("industryList", industryList);
			modelMap.put("societyList", societyList);
			modelMap.put("ageList", ageList);
			modelMap.put("genderList", genderList);
		}catch(Exception e) {
			if(e instanceof ConnectException) {
				modelMap.put(Message.MSG, "无法访问属性库接口！");
			}
			e.printStackTrace();
		}
		
		return "property/export";
	}
	
	
	
	@RequestMapping("/statExportNum")
	public @ResponseBody Map<String,Object> statExportNum(HttpServletRequest request) {
		CSRF.validate(request);
		Map<String,Object> resultMap = Maps.newHashMap();
		try {
			String includeTagIds = XssFilter.filterXss(request.getParameter("includeTagIds"));
			String excludeTagIds =  XssFilter.filterXss(request.getParameter("excludeTagIds"));
			
			
			String months = XssFilter.filterXss(request.getParameter("months"));
			String scores = XssFilter.filterXss(request.getParameter("scores"));
			String inCnd = XssFilter.filterXss(request.getParameter("inCnd"));
			String[] date = DateUtil.getDateRange(Integer.parseInt(months));
			String[] score = CommonUtil.getScore(scores);
			List<String> labelIds = labelService.getLabelIds(includeTagIds, excludeTagIds);
			if(labelIds!=null&&labelIds.size()>0) {
				Map<String,Object> params = Maps.newHashMap();
				params.put("labelIds", labelIds);
				List<Label>  labelList = labelService.getLabelByIds(params);
				resultMap = labelService.generateFile(request, labelList, date,score,inCnd,excludeTagIds);
			}else {
				resultMap.put(Message.RESULT, Message.FAILURE);
				resultMap.put(Message.MSG, "没有标签！");
			}
			
		}catch(Exception e) {
			log.error(e.getMessage(), e);
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "统计数据出错！");
		}
		return resultMap;
	}
	
	
	@RequestMapping("/download")
	public void download(HttpServletRequest request, HttpServletResponse response) {
		String fileName = XssFilter.filterXss(request.getParameter("exportPath"));
		if(StringUtils.isEmpty(fileName)) {
			throw new Errors("文件路径非法！");
		}else if(!ValidUtil.regex(fileName, Regex.FILENAME)){
			throw new Errors("文件路径非法！");
		}else {
			String dir = request.getSession().getServletContext().getRealPath(PropertyConstant.PROP_FILE_PATH);
			String filePath = dir + System.getProperty("file.separator")+fileName;
			String paths = StringUtils.substringBeforeLast(filePath, System.getProperty("file.separator"));
			if(!dir.equals(paths)) {
				throw new Errors("文件路径非法！");
			}else {
				File file = new File(filePath);
				Download.downloadFile(response,file);
			}
			
			
		}
		
		
	}
	
	
	
	
	
}
