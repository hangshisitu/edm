package cn.edm.controller.filter;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

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

import cn.edm.constant.mapper.CategoryMap;
import cn.edm.constants.Cnds;
import cn.edm.constants.Id;
import cn.edm.constants.Status;
import cn.edm.constants.mapper.RoleMap;
import cn.edm.domain.Tag;
import cn.edm.domain.User;
import cn.edm.exception.Errors;
import cn.edm.model.Recipient;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.service.CategoryService;
import cn.edm.service.EmailService;
import cn.edm.service.MyService;
import cn.edm.service.TagService;
import cn.edm.util.Asserts;
import cn.edm.util.Values;
import cn.edm.utils.Converts;
import cn.edm.utils.MailBox;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.helper.Concurrencys;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller
@RequestMapping("/filter/recipient")
public class RecipientController {

    private static final Logger logger = LoggerFactory.getLogger(RecipientController.class);

    @Autowired
    private Ehcache ehcache;
    @Autowired
    private CategoryService categoryService;
    @Autowired
    private EmailService emailService;
    @Autowired
    private TagService tagService;
    @Autowired
    private MyService myService;
    
    @RequestMapping(value="page",method = RequestMethod.GET)
    public String page(HttpServletRequest request, ModelMap map) {
        try {
            
            String token = CSRF.generate(request);
            CSRF.generate(request, "/filter/recipient/unsubscribe", token);
            
            String m = Https.getStr(request, "m", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose|unsubscribe$");
            m = Values.get(m, "red");
            
            Page<Recipient> page = new Page<Recipient>();
            Pages.page(request, page);
            
            String email = Https.getStr(request, "email", R.CLEAN, R.LENGTH, "{1,64}");
            String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
            String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
            String _id = Https.getStr(request, "_id", R.CLEAN, R.LENGTH, R.REGEX, "{24,24}", "regex:^[a-f0-9]+");

            Map<Integer, String> mapper = mapper(page.getPageNo(), _id);

            MapBean mb = new MapBean();
            Tag tag = null;
            if (m.equals("unsubscribe")) {
                Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.REQUIRED, R.INTEGER);
                String userId = Https.getStr(request, "userId", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}");

                Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.REQUIRED, R.INTEGER);
                
                List<User> userList = myService.find(Status.ENABLED);
                Map<String, String> corpMap = Maps.newLinkedHashMap();
                Map<String, List<String>> userMap = Maps.newLinkedHashMap();
                
                Integer startCorp = 0;
                String startUser = "";
                for (User user : userList) {
                    if (user.getCorpId() == null)
                        continue;

                    corpMap.put(String.valueOf(user.getCorpId()), user.getCompany());
                    
                    List<String> userIdList = userMap.get(String.valueOf(user.getCorpId()));
                    if (userIdList == null) {
                        userIdList = Lists.newArrayList();
                    }
                    userIdList.add(user.getUserId());
                    userMap.put(String.valueOf(user.getCorpId()), userIdList);
                    
                    if (startCorp == 0 && StringUtils.isBlank(startUser)) {
                        if (user.getCorpId() != null
                                && StringUtils.isNotBlank(user.getUserId())) {
                            startCorp = user.getCorpId();
                            startUser = user.getUserId();
                        }
                    }
                }

                User user = myService.getBSN(RoleMap.BSN.getId(), Status.ENABLED);
                if (user != null) {
                    corpId = Values.get(corpId, user.getCorpId());
                    userId = Values.get(userId, user.getUserId());
                } else {
                    corpId = Values.get(corpId, startCorp);
                    userId = Values.get(userId, startUser);
                }
                
                for(Entry<String, List<String>> entry : userMap.entrySet()) {
                    List<String> list = entry.getValue();
                    Collections.sort(list, new Comparator<String>() {
                        @Override
                        public int compare(String o1, String o2) {
                            return o1.compareTo(o2);
                        }
                    });
//                    entry.setValue(list);
                }
                
                map.put("corpMap", corpMap);
                map.put("userMap", userMap);
                
                if (tagId == null) {
                    tag = tagService.get(corpId, userId, "默认退订");
                } else {
                    tag = tagService.get(corpId, userId, tagId);
                }
                
                if (tag != null) {
                    page = emailService.search(page, mb, mapper, tag.getDbId(), tag.getUserId(), m, tag.getTagId(), email, beginTime, endTime);
                    Pages.put(mb, "tagId", tag.getTagId());
                }
               
                
                mb.put("corpId", corpId);
                mb.put("userId", userId);
                mb.put("tagId", tagId);
            } else {
                tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(m.toUpperCase()));
                Asserts.isNull(tag, "标签");
                
                page = emailService.search(page, mb, mapper, tag.getDbId(), tag.getUserId(), m, tag.getTagId(), email, beginTime, endTime);
                Pages.put(mb, "tagId", tag.getTagId());
            }
            map.put("page", page);
            map.put("mb", mb);
            map.put("m", m);
            map.put("tag", tag);
            map.put("_id", _id(mapper));
            
            map.put("token", token);
            
            return "filter/recipient_page";
        } catch (Exception e) {
            logger.error("(Task:page) error: ", e);
            return Views._404();
        }
    }
    
    @RequestMapping(value = "unsubscribe", method = RequestMethod.POST)
    public void unsubscribe(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        try {
            CSRF.auth(request);
            
            Integer corpId = Https.getInt(request, "corpId", R.CLEAN, R.REQUIRED, R.INTEGER);
            String userId = Https.getStr(request, "userId", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,60}");
            
            if (corpId != null && StringUtils.isNotBlank(userId)) {
                List<Tag> unsubscribeList = tagService.find(corpId, userId, cn.edm.constants.mapper.CategoryMap.UNSUBSCRIBE.getName(), Cnds.EQ);

                List<List<String>> list = Lists.newArrayList();

                for (Tag tag : unsubscribeList) {
                    List<String> ll = Lists.newArrayList();
                    ll.add(String.valueOf(tag.getTagId()));
                    ll.add(tag.getTagName());
                    list.add(ll);
                }
                
                mb.put("list", list);
            }
           
            
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "删除收件人失败");
            logger.error("(Recipient:del) error: ", e);
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    @RequestMapping(value="add",method = RequestMethod.GET)
    public String add(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);
            
            String email = Https.getStr(request, "email", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}");
            
            String type = Https.getStr(request, "type", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            type = Values.get(type, "red");
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(type.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            Recipient recipient = null;
            if (tag != null && StringUtils.isNotBlank(email)) {
                recipient = emailService.get(tag, email, true);
            }

            if (recipient == null) {
                recipient = new Recipient();
            }
            
            map.put("recipient", recipient);
            map.put("type", type);
            
            return "filter/recipient_add";
        } catch (Exception e) {
            logger.error("(Recipient:add) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "add", method = RequestMethod.POST)
    public void add(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        String orgEmail = null;
        
        Counter counter = null;
        
        try {
            CSRF.auth(request);
            
            orgEmail = Https.getStr(request, "orgEmail", R.CLEAN, R.MAIL, R.LENGTH, "{1,64}", "邮箱地址");
            String email = Https.getStr(request, "email", R.CLEAN, R.REQUIRED, R.MAIL, R.LENGTH, "{1,64}", "邮箱地址");

            String desc = Https.getStr(request, "desc", R.CLEAN, R.LENGTH, "{1,60}", "描述");
            
            String type = Https.getStr(request, "type", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            type = Values.get(type, "red");
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(type.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            Asserts.notUnique(emailService.unique(tag, email, orgEmail), "邮箱地址");
            
            Recipient recipient = null;
            if (StringUtils.isNotBlank(orgEmail)) {
                recipient = emailService.get(tag, orgEmail, false);
                Asserts.isNull(recipient, "收件人");
            } else {
                recipient = new Recipient();
            }

            recipient.setUserId(Id.ROBOT_USER);
            recipient.setEmail(email);
            recipient.setDesc(desc);
            
            counter = Concurrencys.getCounter(ehcache, false, Id.ROBOT_USER);
            emailService.save(tag, recipient, null, orgEmail);
            counter.setCode(Counter.COMPLETED);
            
            Views.ok(mb, (StringUtils.isNotBlank(orgEmail) ? "修改" : "新建") + "收件人成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, (StringUtils.isNotBlank(orgEmail) ? "修改" : "新建") + "收件人失败");
            logger.error("(Recipient:add) error: ", e);
        } finally {
            if (counter != null) {
                Concurrencys.clearCounter(ehcache, Id.ROBOT_USER);
            }
        }

        ObjectMappers.renderJson(response, mb);
    }
    
    
    @RequestMapping(value = "del", method = RequestMethod.GET)
    public String del(HttpServletRequest request, ModelMap map) {
        try {
            CSRF.generate(request);
            
            String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            action = Values.get(action, "red");
            
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(action.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN);
            
//            validate(checkedIds);
            
            String[] emails = Converts._toStrings(checkedIds);
            
            if (emails != null && emails.length == 1) {
                Recipient recipient = emailService.get(tag, emails[0], false);
                if (recipient != null) {
                    map.put("checkedName", recipient.getEmail());
                }
            }
            
            Views.map(map, "del", action, "删除", "所选收件人", "checkedIds", checkedIds);
            return "common_form";
        } catch (Exception e) {
            logger.error("(Recipient:del) error: ", e);
            return Views._404();
        }
    }

    @RequestMapping(value = "del", method = RequestMethod.POST)
    public void del(HttpServletRequest request, HttpServletResponse response) {
        MapBean mb = new MapBean();
        String checkedIds = null;
        
        Counter counter = null;
        
        try {
            CSRF.auth(request);
            
            String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^red|black|refuse|expose$");
            action = Values.get(action, "red");
            Tag tag = tagService.get(Id.ROBOT_CORP, Id.ROBOT_USER, CategoryMap.name(action.toUpperCase()));
            Asserts.isNull(tag, "标签");
            
            checkedIds = Https.getStr(request, "checkedIds", R.CLEAN);
//            validate(checkedIds);
            
            String[] emails = Converts._toStrings(checkedIds);

            if (!Asserts.empty(emails)) {
                counter = Concurrencys.getCounter(ehcache, false, Id.ROBOT_USER);
                emailService.delete(tag, emails);
                counter.setCode(Counter.COMPLETED);
            }
            
            Views.ok(mb, "删除收件人成功");
        } catch (Errors e) {
            Views.error(mb, e.getMessage());
        } catch (Exception e) {
            Views.error(mb, "删除收件人失败");
            logger.error("(Recipient:del) error: ", e);
        } finally {
            if (counter != null) {
                Concurrencys.clearCounter(ehcache, Id.ROBOT_USER);
            }
        }

        mb.put("checkedIds", checkedIds);
        ObjectMappers.renderJson(response, mb);
    }
    
    private void validate(String checkedIds) {
        for (String email : StringUtils.split(checkedIds, ",")) {
            if (StringUtils.isNotBlank(email) && !MailBox.validate(email)) {
                throw new Errors("checkedIds不是合法值");
            }
        }
    }
    
    private Map<Integer, String> mapper(Integer pageNo, String _id) {
        Map<Integer, String> mapper = Maps.newHashMap();
        mapper.put(1, "ffffffffffffffffffffffff");
        if (pageNo != null && StringUtils.isNotBlank(_id)) {
            mapper.put(pageNo, _id);
        }

        return mapper;
    }
    
    private Map<String, String> _id(Map<Integer, String> mapper) {
        Map<String, String> _id = Maps.newHashMap();
        for (Integer pageNo : mapper.keySet()) {
            _id.put(String.valueOf(pageNo), mapper.get(pageNo));
        }

        return _id;
    }
}
