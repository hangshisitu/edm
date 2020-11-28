package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Copy;
import cn.edm.model.Recipient;
import cn.edm.model.Tag;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.orm.Page;
import cn.edm.repository.Dao;
import cn.edm.repository.RecipientDao;
import cn.edm.repository.TagDao;
import cn.edm.utils.Values;
import cn.edm.utils.web.Pages;

import com.google.common.collect.Lists;
import com.mongodb.DBCollection;

@Transactional
@Service
public class EmailService {

    @Autowired
    private Dao dao;
	@Autowired
	private TagDao tagDao;
	@Autowired
	private TagService tagService;
	@Autowired
	private RecipientDao recipientDao;
	@Autowired
	private RecipientService recipientService;

	public void save(Tag tag, Recipient recipient, Tag orgTag, String orgEmail) {
		recipient.setEmail(StringUtils.lowerCase(recipient.getEmail()));
		orgEmail = StringUtils.lowerCase(orgEmail);
		
		List<Integer> recipientDbIds = Lists.newArrayList();
		int maxRecipientDbId = recipientService.maxDbId();
		
		// CopyTrigger触发
        List<Copy> copyRecipientList = Lists.newArrayList();
		recipientDao.delete(recipient, maxRecipientDbId, copyRecipientList);
		recipientDao.save(maxRecipientDbId, recipient, recipientDbIds, copyRecipientList);
		dao.save("CopyRecipient.batchSave", copyRecipientList);
		
		recipientService.save(recipient.getCorpId(), maxRecipientDbId, recipientDbIds);

		if (StringUtils.isNotBlank(orgEmail) && orgTag != null) {
			if (!orgTag.getTagId().equals(tag.getTagId())) {
				tagDao.delete(orgTag, new String[] { orgEmail });
				tagService.save(orgTag);
			}
		}
		
		int maxTagDbId = tagService.maxDbId();
		tagDao.save(maxTagDbId, tag, recipient, orgEmail);
		tagService.save(tag);
		
		// CopyTrigger触发
        dao.save("CopyTag.save", new Copy(tag.getCorpId(), tag.getDbId(), tag.getTagId(), Copy.MODIFY));
	}
	
    public void save(Tag tag, Recipient recipient, boolean hasParameter) {
        recipient.setEmail(StringUtils.lowerCase(recipient.getEmail()));
        if (hasParameter) {
            List<Integer> recipientDbIds = Lists.newArrayList();
            int maxRecipientDbId = recipientService.maxDbId();
            // CopyTrigger触发
            List<Copy> copyRecipientList = Lists.newArrayList();
            recipientDao.save(maxRecipientDbId, recipient, recipientDbIds, copyRecipientList);
            dao.save("CopyRecipient.batchSave", copyRecipientList);
        }
        
        int maxTagDbId = tagService.maxDbId();
        tagDao.save(maxTagDbId, tag, recipient, null);
        tagService.save(tag);
        
        // CopyTrigger触发
        dao.save("CopyTag.save", new Copy(tag.getCorpId(), tag.getDbId(), tag.getTagId(), Copy.MODIFY));
    }

	public Recipient get(Tag tag, String email, boolean parameter) {
		email = StringUtils.lowerCase(email);
		
		Recipient recipient = null;
		if (tagDao.count(tag, email) > 0) {
			recipient = new Recipient();
			recipient.setEmail(email);

			if (parameter) {
				List<Recipient> recipientDbIds = recipientService.find(tag.getUserId());
				String parameters = recipientDao.getParameters(tag.getCorpId(), email, recipientDbIds);
				recipient.setParameterList(parameters);	
			}
		}

		return recipient;
	}

	public void delete(Tag tag, String[] emails) {
		for (String email : emails) {
			email = StringUtils.lowerCase(email);
		}
		
		tagDao.delete(tag, emails);
		tagService.save(tag);
		
		// CopyTrigger触发
        dao.save("CopyTag.save", new Copy(tag.getCorpId(), tag.getDbId(), tag.getTagId(), Copy.MODIFY));
	}

	public Page<Recipient> search(Page<Recipient> page, MapBean mb,
			Map<Integer, String> mapper, Integer dbId, Integer corpId,
			String activeCnd, Integer tagId, String email, String beginTime, String endTime) {
		email = StringUtils.lowerCase(email);

		Pages.put(mb, "email", email);
		Pages.put(mb, "beginTime", beginTime);
		Pages.put(mb, "endTime", endTime);
		Pages.search(mb, page);

		String pageNo = mapper.get(page.getPageNo());
		pageNo = Values.get(pageNo, mapper.get(1));

		if (dbId < 1) {
			page.setTotalItems(0);
			return page;
		}
		
		DBCollection coll = tagDao.getColl(dbId, corpId, tagId);
		page.setTotalItems(tagDao.count(coll, page, activeCnd, email, beginTime, endTime));
		
		page = tagDao.find(coll, page, activeCnd, pageNo, email, beginTime, endTime);
		mapper.put((int) page.getTotalPages(), tagDao.last(coll, page, activeCnd, email, beginTime, endTime));

		if (page.isHasNextPage()) {
			tagDao.slider(true, coll, page, mapper, activeCnd, pageNo, email, beginTime, endTime);
		}
		if (page.isHasPrePage()) {
			tagDao.slider(false, coll, page, mapper, activeCnd, pageNo, email, beginTime, endTime);
		}

		return page;
	}

	public boolean unique(Tag tag, String email, String orgEmail) {
		email = StringUtils.lowerCase(email);
		orgEmail = StringUtils.lowerCase(orgEmail);
		
		if (email == null || email.equals(orgEmail)) {
			return true;
		}

		return tagDao.count(tag, email) == 0;
	}
	
	public List<String> find(Integer corpId, Tag tag) {
		return tagDao.find(corpId, tag);
	}
}
