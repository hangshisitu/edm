package cn.edm.web.action.account;

import java.util.ArrayList;
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
import cn.edm.model.Corp;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.StreamToString;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Corps;

@Controller
@RequestMapping("/account/corp/**")
public class CorpAction extends Action {
    
    private static final Logger logger = LoggerFactory.getLogger(CorpAction.class);
    
    @RequestMapping(method = RequestMethod.GET)
    public String page(HttpServletRequest request, ModelMap map) {
        try {
            String company = StreamToString.getStringByUTF8(Https.getStr(request, "company", R.CLEAN));
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
            
            MapBean mb = new MapBean();
            
            List<Corp> corpList = corpService.findChildren(mb, UserSession.getCorpId(), company, beginTime, endTime);
            Corp corp = corpService.get(UserSession.getCorpId());
            List<Corp> corps = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Integer levelCount = 0;
            levelCount = Corps.getLevel(corps, currentCorp, levelCount);
            
            map.put("corp", corp);
            map.put("corpList", corpList);
            map.put("levelCount", levelCount);
            map.put("mb", mb);
            
            return "default/account/corp_page";
        } catch (Exception e) {
            logger.error("(Corp:page) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String add(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);

            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            Corp corp = null;

            if (corpId != null) {
                corp = corpService.getChild(UserSession.getCorpId(), corpId);
            }
            if (corp == null) {
                corp = new Corp();
            }
            map.put("corp", corp);
            return "default/account/corp_add";
        } catch (Exception e) {
            logger.error("(Corp:add) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(method = RequestMethod.POST)
    public void add(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        Integer corpId = null;

        try {
            CSRF.validate(request);
            
            corpId = Https.getInt(request, "corpId", R.CLEAN, R.INTEGER);
            
            String orgCompany = Https.getStr(request, "orgCompany", R.CLEAN, R.LENGTH, "{1,20}", "机构名称");
            String company = Https.getStr(request, "company", R.CLEAN, R.LENGTH, "{1,20}", "机构名称");
            
            Asserts.notUnique(corpService.unique("company", company, orgCompany), "子机构名称");
            
            List<Corp> corpList = corpService.find();
            Corp currentCorp = corpService.get(UserSession.getCorpId());
            Corp root = Corps.getRoot(corpList, currentCorp);
            Asserts.isNull(root, "一级机构");
            
            Corp corp = null;
            if (corpId != null) {
                corp = corpService.getChild(UserSession.getCorpId(), corpId);
                Asserts.isNull(corp, "机构");
            } else {
                Integer levelCount = 1;
                levelCount = Corps.getLevel(corpList, currentCorp, levelCount);
                if (levelCount > 6) {
                    throw new Errors("机构层级最大为6级");
                }
                corp = new Corp();
                corp.setCosId(root.getCosId());
                corp.setFormalId(root.getFormalId());
                corp.setTestedId(root.getTestedId());
                corp.setSenderValidate(root.getSenderValidate());
                corp.setParentId(UserSession.getCorpId());
                corp.setStatus(Status.ENABLED);
                
            }
            corp.setModifyTime(new Date());
            corp.setCompany(company);
            corp.setManagerId(UserSession.getUserId());
            corpService.save(corp);
            
            Date now = new Date();
            mb.put("corpId", corp.getCorpId());
            mb.put("company", corp.getCompany());
            mb.put("managerId", corp.getManagerId());
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
            
            Views.ok(mb, (corpId != null ? "修改" : "新建") + "子机构成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, (corpId != null ? "修改" : "新建") + "子机构失败");
            logger.error("(Corp:add) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    @RequestMapping(method = RequestMethod.GET)
    public String del(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);
            
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            int[] corpIds = Converts._toInts(checkedIds);

            if (corpIds != null && corpIds.length == 1) {
                Corp corp = corpService.getChild(UserSession.getCorpId(), corpIds[0]);
                if (corp != null) {
                    map.put("checkedName", corp.getCompany());
                }
            }

            Views.map(map, "del", "", "删除", "所选机构", "checkedIds", checkedIds);
            return "default/common_form";
        } catch (Exception e) {
            logger.error("(Corp:del) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(method = RequestMethod.POST)
    public void del(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        String checkedIds = null;
        try {
            CSRF.validate(request);
            
            checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
            int[] corpIds = Converts._toInts(checkedIds);
            
            boolean lock = false;
            
            if (corpIds != null && corpIds.length == 1) {
                List<Corp> corpList = corpService.find();
                List<Integer> corpChildrenIds = new ArrayList<Integer>();
                corpService.cross(corpList, UserSession.getCorpId(), corpChildrenIds);
                if (userService.children(Converts._toIntegers(corpChildrenIds), null) > 0) {
                    lock = true;
                }
                
                if (!lock && !Asserts.empty(corpChildrenIds)) {
                    corpService.delete(Converts._toIntegers(corpChildrenIds));
                }
            }

            if (lock) {
                Views.error(mb, "所选的部分子机构已关联子账号，禁止删除");
            } else {
                Views.ok(mb, "删除子机构成功");
            }
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "删除子机构失败");
            logger.error("(Corp:del) error: ", e);
        }

        mb.put("checkedIds", checkedIds);
        ObjectMappers.renderJson(response, mb);
    }
}
