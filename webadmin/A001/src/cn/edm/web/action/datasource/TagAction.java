package cn.edm.web.action.datasource;

import java.util.Date;
import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Cnds;
import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.consts.mapper.PathMap;
import cn.edm.consts.mapper.TagMap;
import cn.edm.model.Category;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.modules.utils.mapper.ObjectMappers;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.StreamToString;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.file.Files;
import cn.edm.utils.web.CSRF;
import cn.edm.utils.web.Https;
import cn.edm.utils.web.Pages;
import cn.edm.utils.web.R;
import cn.edm.utils.web.Views;
import cn.edm.utils.web.Webs;
import cn.edm.web.UserSession;
import cn.edm.web.action.Action;
import cn.edm.web.facade.Categorys;

import com.google.common.collect.Lists;

@Controller
@RequestMapping("/datasource/tag/**")
public class TagAction extends Action {

	private static final Logger logger = LoggerFactory.getLogger(TagAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			Page<Tag> page = new Page<Tag>();
			Pages.page(request, page);

			String tagName = StreamToString.getStringByUTF8(Https.getStr(request, "tagName", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);
			
			MapBean mb = new MapBean();
			page = tagService.search(page, mb, UserSession.getCorpId(), tagName, beginTime, endTime);
			keyword(page.getResult());
			List<Category> categoryList = categoryService.find(null, UserSession.getCorpId(), Category.TAG, CategoryMap.FILE.getName(), Cnds.NOT_EQ);
			map.put("page", page);
			map.put("categoryList", categoryList);
			map.put("mb", mb);

			return "default/datasource/tag_page";
		} catch (Exception e) {
			logger.error("(Tag:page) error: ", e);
			return Views._404();
		}
	}
	
	private void keyword(List<Tag> tagList) {
		if (Asserts.empty(tagList)) {
			return;
		}

		for (Tag tag : tagList) {
			tag.setKeyword(TagMap.getName(tag.getTagName()) != null);
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void array(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		try {
			CSRF.validate(request);
			
			Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "类别");
			List<Tag> tagList = tagService.find(UserSession.getCorpId(), categoryId);
			Asserts.isEmpty(tagList, "标签");
			
			List<MapBean> tagMaps = Lists.newArrayList();

			for (Tag tag : tagList) {
				MapBean tagMap = new MapBean();
				tagMap.put("tagId", tag.getTagId());
				tagMap.put("tagName", Values.get(tag.getTagName()));
				tagMap.put("tagDesc", Values.get(tag.getTagDesc()));
				tagMap.put("emailCount", Values.get(tag.getEmailCount()));
				tagMap.put("createTime", Calendars.format(tag.getCreateTime(), Calendars.DATE_TIME));
				tagMap.put("modifyTime", Calendars.format(tag.getModifyTime(), Calendars.DATE_TIME));
				tagMap.put("categoryName", Values.get(tag.getCategoryName()));
				tagMaps.add(tagMap);
			}
			
			mb.put("tagMaps", tagMaps);
			Views.ok(mb, "");
		} catch (Exception e) {
			logger.error("(Tag:array) error: ", e);
			Views.error(mb, "");
		}
		
		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);

			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^tag|task|selection.import|selection.export|form|recipient$");
			action = Values.get(action, "tag");

			Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.INTEGER);
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			
			Tag tag = null;

            if (tagId != null) {
                tag = tagService.get(UserSession.getCorpId(), null, tagId);
                if (!tag.getTagName().equals(TagMap.UNSUBSCRIBE.getName())) {
                    if (!tag.getUserId().equals(UserSession.getUserId())) {
                        tag = null;
                    }
                }
            }
			if (tag == null) {
				tag = new Tag();
                if (action.equals("task") || action.equals("form")) {
                    tag.setCategoryId(categoryId);
                }
			}

			map.put("action", action);
			map.put("tag", tag);

			return "default/datasource/tag_add";
		} catch (Exception e) {
			logger.error("(Tag:add) error: ", e);
			return Views._404();
		}
	}
	
	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer tagId = null;
		
		try {
			CSRF.validate(request);

			String action = Https.getStr(request, "action", R.CLEAN, R.REGEX, "regex:^tag|task|selection.import|selection.export|form|recipient$");
			action = Values.get(action, "tag");
			
			tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			
			Integer orgCategoryId = null;
			Integer categoryId = null;
			String categoryName = null;
			
			if (action.equals("tag")) {
				if (tagId != null) {
					orgCategoryId = Https.getInt(request, "orgCategoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "标签类别");
				}
				categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.REQUIRED, R.INTEGER, "标签类别");
			} else {
				categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.INTEGER, "标签类别");
				if (categoryId == null) {
					categoryName = Https.getStr(request, "categoryName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "类别名称");
					Categorys.isKeyword(null, categoryName);
                    if (action.startsWith("selection")) {
                        if (StringUtils.equals(categoryName, CategoryMap.FORM.getName())) {
                            throw new Errors("标签类别不能为表单收集类别");
                        }
                    }
					Asserts.notUnique(categoryService.unique(UserSession.getCorpId(), categoryName, null, Category.TAG), "类别");
				}
			}
			
			String orgTagName = Https.getStr(request, "orgTagName", R.CLEAN, R.LENGTH, "{1,20}", "标签名称");
			String tagName = Https.getStr(request, "tagName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "标签名称");
			String tagDesc = Https.getStr(request, "tagDesc", R.CLEAN, R.LENGTH, "{1,60}", "标签描述");
			
			isKeyword(orgCategoryId, categoryId, orgTagName, tagName);
			
			Asserts.notUnique(tagService.unique(UserSession.getCorpId(), tagName, orgTagName), "标签");
			
			if (action.startsWith("selection")) {
				if (categoryId == null) {
					Category category = new Category();
					category.setCorpId(UserSession.getCorpId());
					category.setUserId(UserSession.getUserId());
					category.setCategoryName(categoryName);
					category.setType(Category.TAG);
					category.setCount(Value.I);

					categoryService.save(category);
					
					categoryId = category.getCategoryId();
				}
			}
			
			Category category = categoryService.get(UserSession.getCorpId(), null, categoryId, Category.TAG);
			Asserts.isNull(category, "类别");

            if (action.startsWith("selection")) {
			    if (StringUtils.equals(category.getCategoryName(), CategoryMap.FORM.getName())) {
                    throw new Errors("标签类别不能为表单收集类别");
                }
			}
			
			Tag tag = null;
			if (tagId != null) {
			    tag = tagService.get(UserSession.getCorpId(), null, tagId);
                if (!tag.getTagName().equals(TagMap.UNSUBSCRIBE.getName())) {
                    if (!tag.getUserId().equals(UserSession.getUserId())) {
                        tag = null;
                    }
                }
				Asserts.isNull(tag, "标签");
                isForm(orgCategoryId, category);
			} else {
				tag = new Tag();
				tag.setCorpId(UserSession.getCorpId());
				tag.setUserId(UserSession.getUserId());
				tag.setTagId(tagId);
				tag.setDbId(Value.I);
				tag.setEmailCount(Value.I);
			}

			tag.setCategoryId(categoryId);
			tag.setCategoryName(category.getCategoryName());
			
			tag.setTagName(tagName);
			tag.setTagDesc(tagDesc);
			
			tagService.save(tag);
			
			mb.put("userId", tag.getUserId());
			mb.put("tagId", tag.getTagId());
			mb.put("tagName", tag.getTagName());
			mb.put("tagDesc", tag.getTagDesc());
			mb.put("categoryName", tag.getCategoryName());
			mb.put("emailCount", tag.getEmailCount());
			Date now = new Date();
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
            
			Views.ok(mb, (tagId != null ? "修改" : "新建") + "标签成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (tagId != null ? "修改" : "新建") + "标签失败");
			logger.error("(Tag:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer tagId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
			Tag tag = null;

			if (tagId != null) {
				tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
				if (tag != null) {
					map.put("checkedName", tag.getTagName());
				}
			}
			
			Views.map(map, "del", "", "删除", "所选标签", "checkedIds", tagId);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Tag:del) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void del(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
			CSRF.validate(request);
			
			Integer tagId = Https.getInt(request, "checkedIds", R.CLEAN, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(), UserSession.getUserId(), tagId);
			Asserts.isNull(tag, "标签");
			Category category = categoryService.get(UserSession.getCorpId(), null, tag.getCategoryId(), Category.TAG);
			Asserts.isNull(category, "类别");
			
			if (TagMap.getName(tag.getTagName()) != null) {
				throw new Errors("不能删除系统定义的标签");
			}
			if (taskService.lockTag(UserSession.getCorpId(), UserSession.getUserId(), tagId,
					category.getCategoryName().equals(CategoryMap.UNSUBSCRIBE.getName()))) {
				throw new Errors("所选的标签已关联投递任务，禁止删除");
			}
            if (formService.lockTag(UserSession.getCorpId(), UserSession.getUserId(), tagId)) {
                throw new Errors("所选的标签已关联表单，禁止删除");
            }
			checkedIds = String.valueOf(tagId);
			tagService.delete(tag);
			if(CategoryMap.FILE.getName().equals(category.getCategoryName())){
				String filePath = PathMap.R.getPath()  + "export/" + UserSession.getUserId() + "/" + tag.getTagDesc();
				Files.delete(Webs.rootPath(), filePath, PathMap.R.getPath());
			}
			
			Views.ok(mb, "删除标签成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除标签失败");
			logger.error("(Tag:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	@RequestMapping(method = RequestMethod.GET)
	public String clear(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			Tag tag = null;

			if (tagId != null) {
				tag = tagService.get(UserSession.getCorpId(),null, tagId);
			}
			if (tag == null) {
				tag = new Tag();
			}
			map.put("tag", tag);
			
			return "default/datasource/tag_clear";
		} catch (Exception e) {
			logger.error("(Tag:clear) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void clear(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		String checkedIds = null;
		try {
			CSRF.validate(request);
			
			Integer tagId = Https.getInt(request, "tagId", R.CLEAN, R.INTEGER);
			Asserts.isNull(tagId, "标签");
			Tag tag = tagService.get(UserSession.getCorpId(),null, tagId);
			Asserts.isNull(tag, "标签");
			Category category = categoryService.get(UserSession.getCorpId(), null, tag.getCategoryId(), Category.TAG);
			Asserts.isNull(category, "类别");
			
			if (taskService.lockTag(UserSession.getCorpId(), UserSession.getUserId(), tagId,
					category.getCategoryName().equals(CategoryMap.UNSUBSCRIBE.getName()))) {
				throw new Errors("所选的标签已关联投递任务，禁止清空");
			}
			
			checkedIds = String.valueOf(tagId);
			tagService.clear(tag);
			
			Views.ok(mb, "清空标签成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "清空标签失败");
			logger.error("(Tag:clear) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
	
	private void isForm(Integer orgCategoryId, Category category) {
        if (Asserts.hasAny(null, new Object[] { orgCategoryId, category })) {
            return;
        }
        if (!orgCategoryId.equals(category.getCategoryId())) {
            Category orgCategory = categoryService.get(UserSession.getCorpId(), null, orgCategoryId, Category.TAG);
            Asserts.isNull(orgCategory, "类别");
            if (orgCategory.getCategoryName().equals(CategoryMap.FORM.getName())) {
                throw new Errors("标签类别只能为表单收集类别");
            }
            if (category.getCategoryName().equals(CategoryMap.FORM.getName())) {
                throw new Errors("标签类别不能修改为表单收集类别");
            }
        }
	}
	
	private void isKeyword(Integer orgCategoryId, Integer categoryId, String orgTagName, String tagName) {
		boolean isKeyword = false;
		for (TagMap mapper : TagMap.values()) {
			if (StringUtils.isNotBlank(orgTagName)) {
				if (StringUtils.equals(orgTagName, mapper.getName())) {
					if (orgCategoryId != null && !orgCategoryId.equals(categoryId)) {
						throw new Errors("不能修改系统定义的标签类别");
					}
					
					if (!StringUtils.equals(orgTagName, tagName)) {
						throw new Errors("不能修改系统定义的标签名称");
					}
				} else {
					if (!StringUtils.equals(orgTagName, tagName)) {
						if (StringUtils.equals(tagName, mapper.getName())) {
							isKeyword = true;
						}
					}
				}
			} else {
				if (StringUtils.equals(tagName, mapper.getName())) {
					isKeyword = true;
				}
			}
			
			if (isKeyword) {
				throw new Errors("标签名称不能含有系统定义的" + mapper.getName() + "关键词");
			}
		}
	}
}

