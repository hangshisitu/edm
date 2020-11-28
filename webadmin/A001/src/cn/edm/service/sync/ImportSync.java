package cn.edm.service.sync;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.task.TaskExecutor;
import org.springframework.stereotype.Component;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.consts.Options;
import cn.edm.consts.mapper.PathMap;
import cn.edm.model.Tag;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.service.ImportService;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.helper.Concurrencys;

@Component
public class ImportSync {

	@Autowired
	private Ehcache ehcache;
	@Autowired
	private ImportService importService;

	public void execute(final int corpId, final String userId, final MultipartFile upload, final String charset, final Integer[] propIds, final Integer tagId) {
		TaskExecutor executor = CtxHolder.getBean("import");
		final Counter counter = Concurrencys.getCounter(ehcache, true, userId);
		
		executor.execute(new Runnable() {
			@Override
			public void run() {
				try {
					String fileId = "import/" + userId;
					Integer[] excludeIds = { tagId };
					importService.execute(Options.INPUT, corpId, userId, PathMap.EXECUTE, fileId, upload, charset, propIds, excludeIds, tagId, counter);
					counter.setMessage("收件人导入成功");
				} catch (Errors e) {
					counter.setMessage(e.getMessage());
					throw new Errors(e.getMessage(), e);
				} catch (ServiceErrors e) {
					counter.setMessage("不可能抛出的异常");
					throw new ServiceErrors(e.getMessage(), e);
				} finally {
					counter.setCode(Counter.COMPLETED);
				}
			}
		});
	}
	
	public void executeTemp(final int corpId, final String userId, final MultipartFile upload, final String charset, final Integer[] propIds, final Tag tag) {
		TaskExecutor executor = CtxHolder.getBean("export");
		final Counter counter = Concurrencys.getCounter(ehcache, true, userId);
		
		executor.execute(new Runnable() {
			@Override
			public void run() {
				try {
					String fileId = "export/" + userId + "/" + tag.getTagDesc();
					importService.executeTemp(Options.EXPORT, corpId, userId, PathMap.R, fileId, 
							upload, charset, propIds, tag.getTagId(), counter);
					counter.setMessage("收件人导入成功");
				} catch (Errors e) {
					counter.setMessage(e.getMessage());
					throw new Errors(e.getMessage(), e);
				} catch (ServiceErrors e) {
					counter.setMessage("不可能抛出的异常");
					throw new ServiceErrors(e.getMessage(), e);
				} finally {
					counter.setCode(Counter.COMPLETED);
				}
			}
		});
	}
}
