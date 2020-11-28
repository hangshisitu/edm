package cn.edm.controller.property;

import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseBody;

import com.google.common.collect.Maps;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.PropertyConstant;
import cn.edm.domain.Label;
import cn.edm.domain.LabelCategory;
import cn.edm.service.LabelCategoryService;
import cn.edm.service.LabelService;
import cn.edm.util.Apis;
import cn.edm.util.PropertiesUtil;
import cn.edm.vo.PropStat;
import cn.edm.vo.PropertyVo;
import cn.edm.vo.PropertyVoStat;

@Controller
@RequestMapping("/property")
public class PropertyController {
	
	private static Logger log = LoggerFactory.getLogger(PropertyController.class);
	
	@Autowired
	private LabelCategoryService labelCategoryService;
	
	@Autowired
	private LabelService labelService;
	
	@RequestMapping("/propertyIndex")
	public String propertyIndex(HttpServletRequest request,ModelMap modelMap) {
		List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
		if(labelCategoryList != null&&labelCategoryList.size()>0) {
			LabelCategory industryCategory = null;
			LabelCategory societyCategory = null;
			LabelCategory ageCategory = null;
			LabelCategory genderCategory = null;
			for(LabelCategory lc:labelCategoryList) {
				if(lc.getCategoryId()==1) {
					industryCategory = lc;
				}else if(lc.getCategoryId()==2) {
					societyCategory = lc;
				}else if(lc.getCategoryId()==3) {
					ageCategory = lc;
				}else if(lc.getCategoryId()==4) {
					genderCategory = lc;
				}
				
			}
			modelMap.put("industryCategory", industryCategory);
			modelMap.put("societyCategory", societyCategory);
			modelMap.put("ageCategory", ageCategory);
			modelMap.put("genderCategory", genderCategory);
		}
		
		
		
		/*//获取类别
		List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
		
		//存放统计结果
		Map<Integer,PropertyVoStat> statMap = Maps.newLinkedHashMap();
		
		PropertyVoStat industryStat = null;
		PropertyVoStat societyStat = null;
		PropertyVoStat ageStat = null;
		PropertyVoStat genderStat = null;
		int totalEmailNum = 0;
		
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
					industryStat = statMap.get(lc.getCategoryId());
					if(industryStat!=null) {
						totalEmailNum += industryStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==2) {
					societyStat = statMap.get(lc.getCategoryId());
					if(societyStat!=null) {
						totalEmailNum += societyStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==3) {
					ageStat = statMap.get(lc.getCategoryId());
					if(ageStat!=null) {
						totalEmailNum += ageStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==4) {
					genderStat = statMap.get(lc.getCategoryId());
					if(genderStat!=null) {
						totalEmailNum += genderStat.getEmailNum();
					}
				}
				
			}
		}
		
		modelMap.put("industryStat", industryStat);
		modelMap.put("societyStat", societyStat);
		modelMap.put("ageStat", ageStat);
		modelMap.put("genderStat", genderStat);
		modelMap.put("totalEmailNum", totalEmailNum);*/
		return "property/propertyIndex";
	}
	
	@RequestMapping("/ajaxStat")
	public @ResponseBody Map<String,Object> ajaxStat(HttpServletRequest request) {
		Map<String,Object> map = Maps.newHashMap();
		
		//获取类别
		List<LabelCategory> labelCategoryList = labelCategoryService.getAll();
		
		//存放统计结果
		Map<Integer,PropertyVoStat> statMap = Maps.newLinkedHashMap();
		
		PropertyVoStat industryStat = null;
		PropertyVoStat societyStat = null;
		PropertyVoStat ageStat = null;
		PropertyVoStat genderStat = null;
		int totalEmailNum = 0;
		
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
					List<PropertyVo> propertyVoList = labelService.getPropertyIndexVo(content,labelList);
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
					industryStat = statMap.get(lc.getCategoryId());
					if(industryStat!=null) {
						totalEmailNum += industryStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==2) {
					societyStat = statMap.get(lc.getCategoryId());
					if(societyStat!=null) {
						totalEmailNum += societyStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==3) {
					ageStat = statMap.get(lc.getCategoryId());
					if(ageStat!=null) {
						totalEmailNum += ageStat.getEmailNum();
					}
				}else if(lc.getCategoryId()==4) {
					genderStat = statMap.get(lc.getCategoryId());
					if(genderStat!=null) {
						totalEmailNum += genderStat.getEmailNum();
					}
				}
				
			}
		}
		
		PropStat propStat = new PropStat();
		propStat.setIndustryStat(industryStat);
		propStat.setSocietyStat(societyStat);
		propStat.setAgeStat(ageStat);
		propStat.setGenderStat(genderStat);
		propStat.setTotalEmailNum(totalEmailNum);
		map.put("propStat", propStat);
		return map;
	}
	
}
