package cn.edm.web.action.datasource;

import java.util.Date;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.CategoryMap;
import cn.edm.model.Category;
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
import cn.edm.web.facade.Categorys;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

@Controller("DatasourceCategory")
@RequestMapping("/datasource/category/**")
public class CategoryAction extends Action {
	
	private static final Logger logger = LoggerFactory.getLogger(CategoryAction.class);

	@RequestMapping(method = RequestMethod.GET)
	public String page(HttpServletRequest request, ModelMap map) {
		try {
			String token = CSRF.generate(request);
			CSRF.generate(request, "/datasource/tag/array", token);
			
			Page<Category> page = new Page<Category>();
			Pages.page(request, page);

			String categoryName = StreamToString.getStringByUTF8(Https.getStr(request, "categoryName", R.CLEAN, R.LENGTH, "{1,20}"));
			String beginTime = Https.getStr(request, "beginTime", R.CLEAN, R.DATE);
			String endTime = Https.getStr(request, "endTime", R.CLEAN, R.DATE);

			MapBean mb = new MapBean();
			page = categoryService.search(page, mb, null, UserSession.getCorpId(), categoryName, Category.TAG, beginTime, endTime);
			tagCount(page.getResult());
			
			map.put("page", page);
			map.put("mb", mb);

			return "default/datasource/category_page";
		} catch (Exception e) {
			logger.error("(Category:page) error: ", e);
			return Views._404();
		}
	}
    
	private void tagCount(List<Category> categoryList) {
		if (Asserts.empty(categoryList)) {
			return;
		}

		List<Integer> categoryIds = Lists.newArrayList();
		
		for (Category category : categoryList) {
			categoryIds.add(category.getCategoryId());
			category.setKeyword(CategoryMap.getName(category.getCategoryName()) != null);
		}

		if (!Asserts.empty(categoryIds)) {
			List<Category> tagCounts = categoryService.tagCount(null, UserSession.getCorpId(), Converts._toIntegers(categoryIds));
			Map<Integer, Integer> tagCountMap = Maps.newHashMap();
			for (Category category : tagCounts) {
				tagCountMap.put(category.getCategoryId(), category.getCount());
			}
			for (Category category : categoryList) {
				Integer count = tagCountMap.get(category.getCategoryId());
				category.setCount(Values.get(count));
			}
			tagCountMap.clear();
		}
		categoryIds.clear();
	}

	@RequestMapping(method = RequestMethod.GET)
	public String add(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			Integer categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.INTEGER);
			Category category = null;

			if (categoryId != null) {
				category = categoryService.get(null, UserSession.getUserId(), categoryId, Category.TAG);
			}
			if (category == null) {
				category = new Category();
			}

			map.put("category", category);

			return "default/datasource/category_add";
		} catch (Exception e) {
			logger.error("(Category:add) error: ", e);
			return Views._404();
		}
	}

	@RequestMapping(method = RequestMethod.POST)
	public void add(HttpServletRequest request, HttpServletResponse response) {
		MapBean mb = new MapBean();
		Integer categoryId = null;

		try {
			CSRF.validate(request);
			
			categoryId = Https.getInt(request, "categoryId", R.CLEAN, R.INTEGER);
			String orgCategoryName = Https.getStr(request, "orgCategoryName", R.CLEAN, R.LENGTH, "{1,20}", "类别名称");
			String categoryName = Https.getStr(request, "categoryName", R.CLEAN, R.REQUIRED, R.LENGTH, "{1,20}", "类别名称");
			String categoryDesc = Https.getStr(request, "categoryDesc", R.CLEAN, R.LENGTH, "{1,60}", "类别描述");

			Categorys.isKeyword(orgCategoryName, categoryName);
			
			Asserts.notUnique(categoryService.unique(UserSession.getCorpId(), categoryName, orgCategoryName, Category.TAG), "类别");

			Category category = null;
			if (categoryId != null) {
				category = categoryService.get(null, UserSession.getUserId(), categoryId, Category.TAG);
				Asserts.isNull(category, "类别");
			} else {
				category = new Category();
				category.setCorpId(UserSession.getCorpId());
				category.setUserId(UserSession.getUserId());
				category.setType(Category.TAG);
				category.setCount(Value.I);
			}

			category.setCategoryName(categoryName);
			category.setCategoryDesc(categoryDesc);

			categoryService.save(category);

			mb.put("userId", UserSession.getUserId());
			mb.put("categoryId", category.getCategoryId());
			mb.put("categoryName", category.getCategoryName());
			mb.put("categoryDesc", category.getCategoryDesc());
			mb.put("categoryName", category.getCategoryName());
			mb.put("count", category.getCount());
			Date now = new Date();
            mb.put("createTime", Calendars.format(now, Calendars.DATE_TIME));
            mb.put("modifyTime", Calendars.format(now, Calendars.DATE_TIME));
			
			Views.ok(mb, (categoryId != null ? "修改" : "新建") + "类别成功");
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, (categoryId != null ? "修改" : "新建") + "类别失败");
			logger.error("(Category:add) error: ", e);
		}

		ObjectMappers.renderJson(response, mb);
	}

	@RequestMapping(method = RequestMethod.GET)
	public String del(HttpServletRequest request, ModelMap map) {
		try {
			CSRF.generate(request);
			
			String checkedIds = Https.getStr(request, "checkedIds", R.CLEAN, R.REGEX, "regex:^[0-9,]+$:不是合法值", "checkedIds");
			int[] categoryIds = Converts._toInts(checkedIds);

			if (categoryIds != null && categoryIds.length == 1) {
				Category category = categoryService.get(null, UserSession.getUserId(), categoryIds[0], Category.TAG);
				if (category != null) {
					map.put("checkedName", category.getCategoryName());
				}
			}

			Views.map(map, "del", "", "删除", "所选类别", "checkedIds", checkedIds);
			return "default/common_form";
		} catch (Exception e) {
			logger.error("(Category:del) error: ", e);
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
			Integer[] categoryIds = Converts._toIntegers(checkedIds);

			boolean lock = false;
			
			if (!Asserts.empty(categoryIds)) {
				List<Category> categoryList = categoryService.find(UserSession.getUserId(), categoryIds, Category.TAG);
				tagCount(categoryList);
				
				StringBuffer sbff = new StringBuffer();
				for (Category category : categoryList) {
					if (CategoryMap.getName(category.getCategoryName()) != null) {
						lock = true;
						continue;
					}
					if (category.getCount() > 0) {
						lock = true;
						continue;
					}
					sbff.append(category.getCategoryId()).append(",");
				}
				
				checkedIds = sbff.toString();
				categoryIds = Converts._toIntegers(sbff.toString());
				
				if (!Asserts.empty(categoryIds)) {
					categoryService.delete(UserSession.getUserId(), categoryIds, Category.TAG);
				}
			}

			if (lock) {
				Views.ok(mb, "所选的部分类别已关联标签，禁止删除");
			} else {
				Views.ok(mb, "删除类别成功");
			}
		} catch (Errors e) {
			Views.error(mb, e.getMessage());
		} catch (Exception e) {
			Views.error(mb, "删除类别失败");
			logger.error("(Category:del) error: ", e);
		}

		mb.put("checkedIds", checkedIds);
		ObjectMappers.renderJson(response, mb);
	}
}
