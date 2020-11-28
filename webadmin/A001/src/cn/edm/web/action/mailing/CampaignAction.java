package cn.edm.web.action.mailing;

import java.util.Date;
import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Status;
import cn.edm.model.Campaign;
import cn.edm.model.Task;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;

@Controller
@RequestMapping("/mailing/campaign/**")
public class CampaignAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(CampaignAction.class);
	
	@RequestMapping
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Campaign> page = new Page<Campaign>();
			Pages.page(request, page);

			String campaignName = StreamToString.getStringByUTF8(Https.getStr(request, "campaignName", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			
			MapBean mb = new MapBean();
			page = campaignService.search(page, mb, UserSession.getCorpId(),
			        campaignName, beginTime, endTime, Status.task("all"));

			map.put("page", page);
			map.put("mb", mb);

			return "default/mailing/campaign_page";
		} catch (Exception e) {
			logger.error("(Campaign:page) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping
	public String add(HttpServletRequest request, ModelMap map) {
		try {
		    CSRF.generate(request);
		    
			Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
			Campaign campaign = null;

			if (campaignId != null) {
				campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
			}
			if (campaign == null) {
				campaign = new Campaign();
			}

			map.put("campaign", campaign);

			return "default/mailing/campaign_add";
		} catch (Exception e) {
			logger.error("(Campaign:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer campaignId = null;
		
		try {
		    CSRF.validate(request);
		    
		    campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
			String orgCampaignName = Https.getStr(request, "orgCampaignName", R.CLEAN, R.LENGTH, "{1,20}", "活动名称");
			
			String campaignName = Https.getStr(request, "campaignName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "活动名称");
			String campaignDesc = Https.getStr(request, "campaignDesc", R.CLEAN, R.LENGTH, "{1,60}", "活动描述");
			
			Asserts.notUnique(campaignService.unique(UserSession.getCorpId(), campaignName, orgCampaignName), "活动");

			Campaign campaign = null;
			if (campaignId != null) {
				campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
				Asserts.isNull(campaign, "活动");
			} else {
				campaign = new Campaign();
				campaign.setCorpId(UserSession.getCorpId());
				campaign.setUserId(UserSession.getUserId());
			}

			campaign.setCampaignName(campaignName);
            campaign.setCampaignDesc(campaignDesc);
            
			campaignService.save(campaign);
			
			mb.put("userId", UserSession.getUserId());
			mb.put("campaignId", campaign.getCampaignId());
            mb.put("campaignName", campaign.getCampaignName());
            mb.put("campaignDesc", campaign.getCampaignDesc());
            mb.put("taskCount", Values.get(campaign.getTaskCount()));
            Date now = new Date();
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
            
			Views.ok(mb, (campaignId != null ? "修改" : "新建") + "活动成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (campaignId != null ? "修改" : "新建") + "活动失败");
			logger.error("(Campaign:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping
	public String del(HttpServletRequest request, ModelMap map) {
		try {
		    CSRF.generate(request);
		    
		    Integer campaignId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
		    Campaign campaign = null;
		    
			if (campaignId != null) {
				campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
				if (campaign != null) {
					map.put("checkedName", campaign.getCampaignName());
				}
			}

			Views.map(map, "del", "", "删除", "活动", "checkedIds", campaignId);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Campaign:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
		    CSRF.validate(request);
		    
		    Integer campaignId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
		    Asserts.isNull(campaignId, "活动");
		    Campaign campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
		    Asserts.isNull(campaign, "活动");
		    
		    checkedIds = String.valueOf(campaignId);
			campaignService.delete(UserSession.getUserId(), campaignId);
			
			Views.ok(mb, "删除活动成功");
			mb.put("checkedIds", checkedIds);
		} catch (Exception e) {
			Views.error(mb, "删除活动失败");
			logger.error("(Campaign:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(value = "import", method = RequestMethod.GET)
    public String imports(HttpServletRequest request, HttpServletResponse response, ModelMap map) {
        try {
            CSRF.generate(request);
            
            List<Campaign> campaignList = campaignService.find(UserSession.getCorpId());
            List<Task> taskList = taskService.campaignNull(UserSession.getCorpId(), Status.task("all"));

            map.put("campaignList", campaignList);
            map.put("taskList", taskList);
            
            return "default/mailing/campaign_import";
        } catch (Exception e) {
            logger.error("(Campaign:imports) error: ", e);
            return Views._404();
        }
    }
	
	@RequestMapping(value = "import", method = RequestMethod.POST)
    public void imports(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.validate(request);
            
            Integer campaignId = Https.getInt(request, "campaignId", R.CLEAN, R.INTEGER);
            Asserts.isNull(campaignId, "活动");
            Campaign campaign = campaignService.get(UserSession.getCorpId(), UserSession.getUserId(), campaignId);
            Asserts.isNull(campaign, "活动");
            
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            Integer[] taskIds = Converts._toIntegers(checkedIds);
            Asserts.isEmpty(taskIds, "任务");
            List<Task> taskList = taskService.find(UserSession.getCorpId(), taskIds);
            Asserts.isEmpty(taskList, "任务");
            
            campaignService.bind(UserSession.getCorpId(), UserSession.getUserId(), campaignId, taskIds);
            
            Views.ok(mb, "导入任务成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "导入任务失败");
            logger.error("(Campaign:add) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
}

