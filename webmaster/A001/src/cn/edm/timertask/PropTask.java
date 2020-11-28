package cn.edm.timertask;

import java.util.List;
import java.util.Map;
import java.util.TimerTask;

import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.impl.client.DefaultHttpClient;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;

import com.google.common.collect.Maps;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.PropertyConstant;
import cn.edm.domain.LabelHistory;
import cn.edm.service.LabelHistoryService;
import cn.edm.util.Apis;
import cn.edm.util.PropertiesUtil;

public class PropTask extends TimerTask {
	
	private static Logger log = LoggerFactory.getLogger(PropTask.class);
	
	@Autowired
	private LabelHistoryService labelHistoryService;
	
	@Override
	public void run() {
		 synchronized(this){
			 String hrCatalogShow = PropertiesUtil.get(ConfigKeys.HR_CATALOG_SHOW);
			 if(StringUtils.isBlank(hrCatalogShow) || "true".equals(hrCatalogShow)) {
				 handleTask();
			 }
		 }
	}
	
	
	private void handleTask() {
		HttpClient client = null;
		try {
			Map<String,Object> params = Maps.newHashMap();
			params.put("status", String.valueOf(PropertyConstant.HANDLING));
			List<LabelHistory> records = labelHistoryService.getLabelHistory(params);
			if(records!=null&&records.size()>0) {
				//log.info("==========================执行导入属性库任务========================");
				System.out.println("==========================执行导入属性库任务=========");
				for(LabelHistory lh:records) {
					client = new DefaultHttpClient();
					log.info("===>当前任务ID:"+lh.getCorpId()+"."+lh.getTaskId()+"."+lh.getTemplateId());
					String ids = lh.getCorpId()+"."+lh.getTaskId()+"."+lh.getTemplateId();
					String url = PropertiesUtil.get(ConfigKeys.PROPERTY_URL)+PropertyConstant.METHOD_INSERT+"label="+lh.getLabelIds()+"&taskid="+ids;
					HttpResponse response = Apis.getHttpResponse(client,url, "utf-8");
					int code = response.getStatusLine().getStatusCode();
					Map<String,Object> map = Maps.newHashMap();
					map.put("corpId", lh.getCorpId());
					map.put("taskId", lh.getTaskId());
					map.put("templateId", lh.getTemplateId());
					map.put("status",code);
					log.info("====>API执行返回响应代码："+code);
					if(code==200) {
						map.put("labelStatus", PropertyConstant.HANDLED_SUCCESS);
					}else {
						map.put("labelStatus", PropertyConstant.HANDLED_FAILURE);
					}
					labelHistoryService.updateStatus(map);
					client.getConnectionManager().shutdown();
				}
			}else {
				//log.info("==========================没有可导入的任务========================");
			}
		}catch(Exception e) {
			//e.printStackTrace();
			log.error(e.getMessage(), e);
		}finally {
			if(client!=null) {
				client.getConnectionManager().shutdown();
			}
		}
	}

}
