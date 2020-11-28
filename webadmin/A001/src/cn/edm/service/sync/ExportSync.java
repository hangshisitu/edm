package cn.edm.service.sync;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.task.TaskExecutor;
import org.springframework.stereotype.Component;

import cn.edm.consts.mapper.PathMap;
import cn.edm.modules.cache.Ehcache;
import cn.edm.modules.utils.spring.CtxHolder;
import cn.edm.service.ExportService;
import cn.edm.utils.except.Errors;
import cn.edm.utils.except.ServiceErrors;
import cn.edm.utils.execute.Counter;
import cn.edm.utils.helper.Concurrencys;

@Component
public class ExportSync {

	@Autowired
	private Ehcache ehcache;
	@Autowired
	private ExportService exportService;

	public void execute(final String opt, final int corpId, final String userId, final Integer[] includeTagIds, final Integer[] excludeTagIds, final String activeCnd, final String inCnd, final Integer filterId, final Integer tagId) {
		TaskExecutor executor = CtxHolder.getBean("export");
		final Counter counter = Concurrencys.getCounter(ehcache, true, userId);
		
		executor.execute(new Runnable() {
			@Override
			public void run() {
				try {
					String fileId = "export/" + userId;
					exportService.execute(opt, corpId, userId, PathMap.EXECUTE, fileId, includeTagIds, excludeTagIds, activeCnd, inCnd, filterId, tagId, counter);
					counter.setMessage("收件人导出成功");
				} catch (Errors e) {
					counter.setMessage(e.getMessage());
					throw new Errors(e.getMessage(), e);
				} catch (ServiceErrors e) {
					counter.setMessage("不可能抛出的异常");
					throw new ServiceErrors(e);
				} finally {
					counter.setCode(Counter.COMPLETED);
				}
			}
		});
	}
}
