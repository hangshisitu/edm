package cn.edm.web.facade;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.joda.time.DateTime;

import au.com.bytecode.opencsv.CSVWriter;
import cn.edm.consts.mapper.BrowserMap;
import cn.edm.consts.mapper.LangMap;
import cn.edm.consts.mapper.OsMap;
import cn.edm.consts.mapper.StatusMap;
import cn.edm.model.Browser;
import cn.edm.model.Campaign;
import cn.edm.model.Domain;
import cn.edm.model.Lang;
import cn.edm.model.Os;
import cn.edm.model.Plan;
import cn.edm.model.Region;
import cn.edm.model.Result;
import cn.edm.model.Setting;
import cn.edm.model.Task;
import cn.edm.model.Triger;
import cn.edm.model.Url;
import cn.edm.modules.orm.MapBean;
import cn.edm.modules.utils.encoder.Md5;
import cn.edm.utils.Asserts;
import cn.edm.utils.Calendars;
import cn.edm.utils.Converts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.web.View;
import cn.edm.web.api.Loggers;
import cn.edm.web.api.Senders;

import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

public class CSV {

    public static final String DEFAULT = "default";
    public static final String PLAN = "plan";
    public static final String LOCAL = "local";
    public static final String API= "api";

    /**
     * 投递任务头部.
     */
    public static void header(CSVWriter writer, Task task, Result result) {
        String header = "任务名称,邮件主题,当前状态,投递时间";
        writer.writeNext(Converts._toStrings(header));
        if (task == null || result == null) return;
        String name = task.getTaskName();
        String subject = task.getSubject();
        String status = StatusMap.getName(task.getStatus());
        String time = time(task.getDeliveryTime(), result.getEndSendTime());
        writer.writeNext(new String[] { name, subject, status, time });
    }
    
    /**
     * 触发任务头部.
     */
    public static void header(CSVWriter writer, Triger triger, Result result) {
        String header = "任务名称,邮件主题,投递时间";
        writer.writeNext(Converts._toStrings(header));
        if (triger == null || result == null) return;
        String name = triger.getTaskName();
        String subject = triger.getSubject();
        String time = time(result.getBeginSendTime(), result.getEndSendTime());
        writer.writeNext(new String[] { name, subject, time });
    }
    
    /**
     * 计划任务头部.
     */
    public static void header(CSVWriter writer, View view, Plan plan, Task task) {
        String header = "任务名称,邮件主题,周期时间,已投递次数,有效期";
        writer.writeNext(Converts._toStrings(header));
        if (task == null) return;
        String name = task.getTaskName();
        String subject = task.getSubject();
        String cron = view.cron(plan.getCron());
        String taskCount = String.valueOf(plan.getTaskCount());
        String time = time(plan.getBeginTime(), plan.getEndTime());
        writer.writeNext(new String[] { name, subject, cron, taskCount, time });
    }
    
    /**
     * 活动头部.
     */
    public static void header(CSVWriter writer, Campaign campaign, MapBean recent) {
        String header = "活动名称,活动描述,投递次数,更新时间";
        writer.writeNext(Converts._toStrings(header));
        if (campaign == null || recent == null) return;
        String name = campaign.getCampaignName();
        String desc = campaign.getCampaignDesc();
        String count = Values.get(recent.getLong("taskCount"), "0");
        String time = Values.get(Calendars.format((Date) recent.get("maxDeliveryTime"), Calendars.DATE_TIME), "-");
        writer.writeNext(new String[] { name, desc, count, time });
    }
    
    public static void blank(CSVWriter writer) {
        writer.writeNext(new String[] {});
    }
    
    public static void name(CSVWriter writer, String name) {
        writer.writeNext(new String[] { name });
    }
    
    private static String time(Date begin, Date end) {
        return Values.get(Calendars.format(begin, Calendars.DATE_TIME)) + "至" + Values.get(Calendars.format(end, Calendars.DATE_TIME), "现在");
    }
    
    
    /**
     * 概览.
     */
    public static void cover(CSVWriter writer, String t, Domain domain) {
        String header = null;
        if (t.equals(DEFAULT)) {
            header = "发送总数,成功数,弹回数,打开人数,打开次数,点击人数,点击次数,退订人数,转发人数,无效人数,成功率,打开率,点击率";
        } else {
            header = "发送总数,成功数,弹回数,打开人数,点击人数,成功率,打开率,点击率";
        }
        
        writer.writeNext(Converts._toStrings(header));
        if (domain == null) return;
        int[] fields = Counts.fields(domain);
        String[] counts = Counts.counts(fields);
        String[] percents = Counts.percents(Counts.percents(fields));
        
        if (t.equals(DEFAULT)) {
            writer.writeNext(new String[] {
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[5], counts[6], counts[7], counts[8], counts[9], counts[11],
                    percents[0] + "%", percents[1] + "%", percents[2] + "%"
            });
        } else {
            writer.writeNext(new String[] {
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[6],
                    percents[0] + "%", percents[1] + "%", percents[2] + "%"
            });
        }
    }
    
    /**
     * 发送.
     */
    public static void send(CSVWriter writer, Domain domain) {
        String header = "弹回详情,弹回数,占比";
        writer.writeNext(Converts._toStrings(header));
        if (domain == null) return;
        int[] fields = Counts.fields(domain);
        String[] counts = Counts.counts(fields);
        String[] percents = Counts.percents(Counts.percents(fields));
        writer.writeNext(new String[] { "邮箱无效", counts[11], percents[3] + "%" });
        writer.writeNext(new String[] { "域名无效", counts[12], percents[4] + "%" });
        writer.writeNext(new String[] { "判定垃圾", counts[13], percents[5] + "%" });
        writer.writeNext(new String[] { "其他", String.valueOf((fields[2] + fields[3]) - (fields[11] + fields[12] + fields[13])), percents[6] + "%" });
    }
    public static void send(CSVWriter writer, Result result, String action) {
        String header = "邮件地址,发送时间";
        writer.writeNext(Converts._toStrings(header));
        if (result == null) return;
        String sid = Tasks.id(result.getCorpId(), result.getTaskId(), result.getTemplateId());
        api(Senders.get(action, sid), writer, "sender");
    }
    
    /**
     * 打开.
     */
    public static void open(CSVWriter writer, Result result) {
        String header = "邮件地址,所属地区,打开时间";
        writer.writeNext(Converts._toStrings(header));
        if (result == null) return;
        String sid = Tasks.id(result.getCorpId(), result.getTaskId(), result.getTemplateId());
        String time = new DateTime(result.getBeginSendTime()).toString("yyyyMMdd");
        api(Loggers.open(sid, time, "detail"), writer, "logger");
    }
    
    /**
     * 点击.
     */
    public static void click(CSVWriter writer, Result result) {
        String header = "邮件地址,所属地区,点击时间";
        writer.writeNext(Converts._toStrings(header));
        if (result == null) return;
        String sid = Tasks.id(result.getCorpId(), result.getTaskId(), result.getTemplateId());
        String time = new DateTime(result.getBeginSendTime()).toString("yyyyMMdd");
        api(Loggers.click(sid, time, "detail"), writer, "logger");
    }

    /**
     * 链接.
     */
    public static void url(CSVWriter writer, List<Url> urlList, Url sumUrl) {
        String header = "链接地址,点击次数,占比";
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(urlList) || sumUrl == null) return;
        for (Url url : urlList) {
            int click = Values.get(url.getClickCount());
            int sumClick = Values.get(sumUrl.getSumClickCount());
            String name = url.getUrl();
            String count = String.valueOf(click);
            String persent = String.valueOf(sumClick == 0 ? 0 : (float) click * 100 / (float) sumClick + "%");
            writer.writeNext(new String[] { name, count, persent });
        }
    }

    /**
     * 域名.
     */
    public static void domain(CSVWriter writer, List<Domain> domainList) {
        String header = "域名,发送总数,成功数,弹回数,打开人数,打开次数,点击人数,点击次数,退订人数,转发人数";
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(domainList)) return;
        for (Domain domain : domainList) {
            int[] fields = Counts.fields(domain);
            String[] counts = Counts.counts(fields);
            String name = domain.getEmailDomain();
            writer.writeNext(
                    new String[] { name,
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[5], counts[6], counts[7], counts[8], counts[9]
            });
        }
    }

    /**
     * 地域.
     */
    public static void region(CSVWriter writer, List<Region> regionList) {
        String header = "地区,打开人数,打开次数,点击人数,点击次数,退订人数,转发人数";
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(regionList)) return;
        for (Region region : regionList) {
            int[] fields = Counts.fields(region);
            String[] counts = Counts.counts(fields);
            String name = region.getRegionName();
            writer.writeNext(new String[] { name, counts[4], counts[5], counts[6], counts[7], counts[8], counts[9] });
        }
    }
    
    /**
     * HTTP Client.
     */
    private static void api(String url, CSVWriter writer, String target) {
        HttpClient client = null;
        BufferedReader reader = null;
        try {
            client = new DefaultHttpClient();
            HttpGet get = new HttpGet(url);
            HttpResponse response = client.execute(get);
            HttpEntity entity = response.getEntity();
            
            int max = 0;
            if (target.equals("sender")) max = 4;
            else if (target.equals("logger")) max = 5;
            
            if (entity != null) {
                reader = new BufferedReader(new InputStreamReader(entity.getContent(), "gbk"));
                String line = null;
                int count = 0;
                
                while ((line = reader.readLine()) != null) {
                    count++;
                    if (count == 1)
                        continue;
                    
                    String[] records = StringUtils.splitPreserveAllTokens(line, ",");
                    if (records.length < max || StringUtils.isBlank(records[0]))
                        continue;
                    
                    String[] content = null;
                    if (target.equals("sender"))
                        content = new String[] { records[0], records[1] + " " + StringUtils.substringBeforeLast(records[2], ":") };
                    else if (target.equals("logger"))
                        content = new String[] { records[0], records[4], records[1] + " " + StringUtils.substringBeforeLast(records[2], ":") };
                    
                    writer.writeNext(content);
                }
            }
            get.abort();
        } catch (Exception e) {
            throw new Errors("(CSV:api) error: ", e);
        } finally {
            IOUtils.closeQuietly(reader);
            client.getConnectionManager().shutdown();
        }
    }
    
    /**
     * 历史.
     */
    public static void history(CSVWriter writer, String t, List<Domain> domainList) {
        String header = null;
        if (t.equals(DEFAULT)) {
            header = "任务名称,发送总数,成功数,打开人数,点击人数,退订人数,无效人数,成功率,打开率,点击率,投递时间";
        } else {
            header = "投递时间,发送状态,发送总数,成功数,打开人数,点击人数,成功率,打开率,点击率";
        }
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(domainList)) return;
        for (Domain domain : domainList) {
            int[] fields = Counts.fields(domain);
            String[] counts = Counts.counts(fields);
            String[] percents = Counts.percents(Counts.percents(fields));
            String name = domain.getTaskName();
            String time = Values.get(Calendars.format(domain.getDeliveryTime(), Calendars.DATE_TIME), "-");
            String status = StatusMap.getName(Values.get(domain.getStatus()));
            
            if (t.equals(DEFAULT)) {
                writer.writeNext(new String[] {
                        name,
                        counts[0], counts[1], counts[4], counts[6], counts[8], counts[11],
                        percents[0] + "%", percents[1] + "%", percents[2] + "%",
                        time
                });
            } else {
                writer.writeNext(new String[] {
                        time,
                        status,
                        counts[0], counts[1], counts[4], counts[6],
                        percents[0] + "%", percents[1] + "%", percents[2] + "%",
                });
            }
        }
    }
    
    /**
     * 终端.
     */
    public static void client(CSVWriter writer, List<Browser> browserList, List<Lang> langList, List<Os> osList) {
        if (!Asserts.empty(browserList)) {
            String header = "浏览器,打开次数,占比";
            writer.writeNext(Converts._toStrings(header));
            int sum = 0;
            for (Browser b : browserList) sum += Values.get(b.getOpenCount());
            for (Browser b : browserList) {
                writer.writeNext(new String[] { BrowserMap.getName(b.getBrowser()), Values.get(b.getOpenCount(), "0"), Values.get(b.getOpenCount()) * 100 / (float) sum + " %" });
            }
            blank(writer);
        }
        if (!Asserts.empty(osList)) {
            String header = "操作系统,打开次数,占比";
            writer.writeNext(Converts._toStrings(header));
            int sum = 0;
            for (Os o : osList) sum += Values.get(o.getOpenCount());
            for (Os o : osList) {
                writer.writeNext(new String[] { OsMap.getName(o.getOs()), Values.get(o.getOpenCount(), "0"), Values.get(o.getOpenCount()) * 100 / (float) sum + " %" });
            }
            blank(writer);
        }
        if (!Asserts.empty(langList)) {
            String header = "语言环境,打开次数,占比";
            writer.writeNext(Converts._toStrings(header));
            int sum = 0;
            for (Lang l : langList) sum += Values.get(l.getOpenCount());
            for (Lang l : langList) {
                writer.writeNext(new String[] { LangMap.getName(l.getLang()), Values.get(l.getOpenCount(), "0"), Values.get(l.getOpenCount()) * 100 / (float) sum + " %" });
            }
            blank(writer);
        }
    }
    
    /**
     * 对比.
     */
    public static void compare(CSVWriter writer, List<Domain> domainList) {
        String header = "对比名称,发送总数,成功数,弹回数,打开人数,打开次数,点击人数,点击次数,退订人数,转发人数,无效人数,成功率,打开率,点击率";
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(domainList)) return;
        for (Domain domain : domainList) {
            int[] fields = Counts.fields(domain);
            String[] counts = Counts.counts(fields);
            String[] percents = Counts.percents(Counts.percents(fields));
            String name = domain.getCompareName();
            writer.writeNext(new String[] {
                    name,
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[5], counts[6], counts[7], counts[8], counts[9], counts[11],
                    percents[0] + "%", percents[1] + "%", percents[2] + "%"
            });
        }
    }
    
    /**
     * 总览.
     */
    public static void sum(CSVWriter writer, String t, List<Domain> domainList) {
        String header = null;
        if (t.equals(LOCAL)) {
            header = "域名,发送总数,成功总数,弹回总数,打开总人数,打开总次数,点击总人数,点击总次数,退订总数,转发总数,成功率,打开率,点击率";
        } else {
            header = "API类型,发送总数,成功总数,弹回总数,打开总人数,打开总次数,点击总人数,点击总次数,退订总数,转发总数,成功率,打开率,点击率";
        }
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(domainList)) return;
        for (Domain d : domainList) {
            int[] fields = Counts.fields(d);
            String[] counts = Counts.counts(fields);
            String[] percents = Counts.percents(Counts.percents(fields));
            String name = t.equals(LOCAL) ? d.getEmailDomain() : d.getTaskName();
            writer.writeNext(new String[] {
                    name,
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[5], counts[6], counts[7], counts[8], counts[9],
                    percents[0] + "%", percents[1] + "%", percents[2] + "%"
            });
        }
    }
    
    /**
     * 合计.
     */
    public static void total(CSVWriter writer, List<Result> resultList, List<Domain> domainList) {
        String header = "任务名称,发送总数,成功总数,弹回总数,打开总人数,打开总次数,点击总人数,点击总次数,退订总数,转发总数,无效人数,成功率,打开率,点击率";
        writer.writeNext(Converts._toStrings(header));
        if (Asserts.empty(resultList) || Asserts.empty(domainList)) return;
        Map<Integer, Domain> domainMap = Maps.newHashMap();
        for (Domain d : domainList) {
            domainMap.put(d.getTaskId(), d);
        }
        for (Result result : resultList) {
            Domain d = domainMap.get(result.getTaskId());
            if (d == null) d = new Domain();
            int[] fields = Counts.fields(d);
            String[] counts = Counts.counts(fields);
            String[] percents = Counts.percents(Counts.percents(fields));
            String name = result.getTaskName();
            writer.writeNext(new String[] {
                    name,
                    counts[0], counts[1], String.valueOf(fields[2] + fields[3]),
                    counts[4], counts[5], counts[6], counts[7], counts[8], counts[9], counts[11],
                    percents[0] + "%", percents[1] + "%", percents[2] + "%"
            });
        }
    }
    /**
     * 触发计划模板设置
     * @param writer
     * @param setting
     */
    public static void touchSetting(CSVWriter writer, Setting setting) {
    	writer.writeNext(new String[]{ "模板名称", setting.getTemplateName() });
    	writer.writeNext(new String[]{ "模板编号", new Md5().encode(setting.getUserId() + setting.getTemplateId()) });
    }
    /**
     * 触发计划
     * @param writer
     * @param domainList
     * @param totalDomain
     * @param type
     */
    public static void touch(CSVWriter writer, List<Domain> domainList, Domain totalDomain, String type) {

    	if(Asserts.empty(domainList)) return;
		
		int len = domainList.size();
    	String[] taskNames = new String[len];
    	int[] touchCounts = new int[len];
    	int[] readCounts = new int[len];
    	int totalTouchCount = 0;
    	int totalReadUserCount = 0;
    	for(int i=0; i<len; i++) {
    		Domain d = domainList.get(i);
    		Integer touchCount = d.getTouchCount();
    		Integer readUserCount = d.getReadUserCount();
    		taskNames[i] = d.getTaskName();
    		touchCounts[i] = touchCount;
    		readCounts[i] = readUserCount;
    		totalTouchCount += Values.get(touchCount, 0);
    		totalReadUserCount += Values.get(readUserCount, 0);
		}
    	if("touchCount".equals(type)){
    		for(int i=0; i<len; i++) {
    			float percent = totalTouchCount==0 ? 0 : ((float) touchCounts[i]*100 / totalTouchCount);
    			writer.writeNext(new String[] {
    					taskNames[i], touchCounts[i]+"", percent + "%"
    			});
    		}
    	}else if("readCount".equals(type)){
    		for(int i=0; i<len; i++) {
    			float percent = totalReadUserCount==0 ? 0 : ((float) readCounts[i]*100 / totalReadUserCount);
    			writer.writeNext(new String[] {
    					taskNames[i], readCounts[i]+"", percent + "%"
    			});
    		}
    	}else if("detail".equals(type)){
    		for(Domain d : domainList) {
    			String touchUrl = "";
    			if(d.getUrlList() != null){
    				for(String url : d.getUrlList()) {
    					touchUrl += url + "；";
    				}
    			}
				writer.writeNext(new String[]{"触点链接", touchUrl});
    			String[] header = {"模板名称", "触发总数", "成功数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数", "触发率", "成功率", "打开率", "点击率"};
        		writer.writeNext(header);
        		int[] fields = Counts.fields(d);
    			String[] counts = Counts.counts(fields);
    			String[] percents = Counts.percents(Counts.percents(fields));
        		String name = d.getTemplateName();
        		float touchRate = getTouchRate(d);
    			writer.writeNext(new String[] {
    					name,
    					counts[14], counts[1], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9],
    					touchRate + "%", percents[0] + "%", percents[1] + "%", percents[2] + "%"
    			});
    			CSV.blank(writer);
    		}
    	}else{
    		String header = "任务来源,触发总数,成功数,打开人数,打开次数,点击人数,点击次数,退订人数,转发人数,触发率,成功率,打开率,点击率";
    		writer.writeNext(Converts._toStrings(header));
    		for(Domain d : domainList) {
    			int[] fields = Counts.fields(d);
    			String[] counts = Counts.counts(fields);
    			String[] percents = Counts.percents(Counts.percents(fields));
    			String name = d.getTaskName();
    			float touchRate = getTouchRate(d);
    			writer.writeNext(new String[] {
    					name,
    					counts[14], counts[1], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9],
    					touchRate + "%", percents[0] + "%", percents[1] + "%", percents[2] + "%"
    			});
    		}
    		if(totalDomain != null){
    			int[] fields = Counts.fields(totalDomain);
    			String[] counts = Counts.counts(fields);
    			String[] percents = Counts.percents(Counts.percents(fields));
    			String name = "合计";
    			Integer readUserCount = totalDomain.getParentReadUserCount();
    			float touchRate = readUserCount==0 ? 0 : (float) Values.get(totalDomain.getTouchCount(), 0)*100 / readUserCount; // 触发率
    			writer.writeNext(new String[] {
    					name,
    					counts[14], counts[1], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9],
    					touchRate + "%", percents[0] + "%", percents[1] + "%", percents[2] + "%"
    			});
    		}
    	}
    }
    /**
     * 活动发送－－触发计划 导出
     * @param writer
     * @param domainMap
     * @param taskIds 主模板任务ID集合
     */
    public static void touch(CSVWriter writer, Map<String, List<Domain>> domainMap, Integer[] taskIds) {
    	
    	if(Asserts.empty(domainMap)) return;
    	
    	Iterator<Entry<String, List<Domain>>> it = domainMap.entrySet().iterator();
    	while(it.hasNext()) {
    		Map.Entry<String, List<Domain>> entry = (Entry<String, List<Domain>>) it.next();
    		String taskName = entry.getKey();
    		List<Domain> domainList = entry.getValue();
    		for(Integer taskId : taskIds) {
    			String endStr = "_" + taskId;
    			if(taskName.contains(endStr)){
    				taskName = taskName.substring(0, taskName.lastIndexOf(endStr));
    			}
    		}
    		writer.writeNext(new String[]{"任务来源", taskName});
    		String[] header = {"模板名称", "触发总数", "成功数", "打开人数", "打开次数", "点击人数", "点击次数", "退订人数", "转发人数", "触发率", "成功率", "打开率", "点击率"};
    		writer.writeNext(header);
    		for(Domain d : domainList) {
    			int[] fields = Counts.fields(d);
    			String[] counts = Counts.counts(fields);
    			String[] percents = Counts.percents(Counts.percents(fields));
    			String name = d.getTemplateName();
    			Integer readUserCount = d.getParentReadUserCount();
    			float touchRate = readUserCount==0 ? 0 : (float) Values.get(d.getTouchCount(), 0)*100 / readUserCount; // 触发率
    			writer.writeNext(new String[] {
    					name,
    					counts[14], counts[1], counts[4], counts[5], counts[6], counts[7], counts[8], counts[9],
    					touchRate + "%", percents[0] + "%", percents[1] + "%", percents[2] + "%"
    			});
    		}
    	}
    	
    }
    /**
     * 触发：导出收件人详情
     * @param writer
     * @param result
     */
    public static void touch(CSVWriter writer, Result result) {
    	String header = "收件人,发送,打开,点击";
    	writer.writeNext(Converts._toStrings(header));
    	if (result == null) return;
    	String sid = Tasks.id(result.getCorpId(), result.getTouchId(), result.getTemplateId());
    	List<String> senders = Lists.newArrayList();
    	List<String> opens = Lists.newArrayList();
    	List<String> clicks = Lists.newArrayList();
        // 发送
    	touchApi(Senders.get("success", sid), "sender", senders); 
    	String time = new DateTime(result.getBeginSendTime()).toString("yyyyMMdd");
    	// 打开
    	touchApi(Loggers.open(sid, time, "detail"), "logger", opens);
        // 点击
    	touchApi(Loggers.click(sid, time, "detail"), "logger", clicks);
    	String[] contents = new String[4];
    	int totalSend = 0;
    	int totalOpen = 0;
    	int totalClick = 0;
    	if(!Asserts.empty(senders)){
    		for(int i=0,len=senders.size(); i<len; i++) {
    			String email = senders.get(i);
    			if(StringUtils.isBlank(email)) {
    				continue;
    			}
    			// 发送
    			contents[0] = email;
    			contents[1] = "1";
    			totalSend++;
    			// 打开
    			boolean iso = false;
    			for(String o : opens) {
    				if(email.equals(o)){
    					contents[2] = "1";
    					totalOpen++;
    					iso = true;
    					continue;
    				}
    			}
    			if(!iso) contents[2] = "0";
    			// 点击
    			boolean isc = false;
    			for(String c : clicks) {
    				if(email.equals(c)){
    					contents[3] = "1";
    					totalClick++;
    					isc = true;
    					continue;
    				}
    			}
    			if(!isc) contents[3] = "0";
    			writer.writeNext(contents);
    		}
    		writer.writeNext(new String[] { "合计", totalSend+"", totalOpen+"", totalClick+""});
    	}
    }
    /**
     * 触发： HTTP API.
     * 导出统计是否发送成功、打开、点击
     * @param url
     * @param target
     * @param contents
     */
    private static void touchApi(String url, String target, List<String> contents) {
        HttpClient client = null;
        BufferedReader reader = null;
        try {
            client = new DefaultHttpClient();
            HttpGet get = new HttpGet(url);
            HttpResponse response = client.execute(get);
            HttpEntity entity = response.getEntity();
            
            int max = 0;
            if (target.equals("sender")) max = 4;
            else if (target.equals("logger")) max = 5;
            
            if (entity != null) {
                reader = new BufferedReader(new InputStreamReader(entity.getContent(), "gbk"));
                String line = null;
                int count = 0;
                
                while ((line = reader.readLine()) != null) {
                    count++;
                    if (count == 1)
                        continue;
                    
                    String[] records = StringUtils.splitPreserveAllTokens(line, ",");
                    if (records.length < max || StringUtils.isBlank(records[0]))
                        continue;
                    
                    String content = null;
                    if (target.equals("sender"))
                        content = records[0];
                    else if (target.equals("logger"))
                        content = records[0];
                    
                    contents.add(content);
                }
            }
            get.abort();
        } catch (Exception e) {
            throw new Errors("(CSV:touchApi) error: ", e);
        } finally {
            IOUtils.closeQuietly(reader);
            client.getConnectionManager().shutdown();
        }
    }
    
    private static float getTouchRate(Domain d) {
    	Integer readUserCount = d.getParentReadUserCount()==null ? 0 : d.getParentReadUserCount();
    	return readUserCount==0 ? 0 : (float) Values.get(d.getTouchCount(), 0)*100 / readUserCount; // 触发率
    }
    
}
