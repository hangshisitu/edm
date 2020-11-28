package cn.edm.service;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import javax.servlet.http.HttpServletRequest;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.impl.client.DefaultHttpClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;


import com.google.common.collect.Lists;
import com.google.common.collect.Maps;

import cn.edm.constant.ConfigKeys;
import cn.edm.constant.Message;
import cn.edm.constant.PropertyConstant;
import cn.edm.domain.Label;
import cn.edm.domain.LabelCategory;
import cn.edm.persistence.LabelMapper;
import cn.edm.persistence.TaskMapper;
import cn.edm.util.Apis;
import cn.edm.util.DateUtil;
import cn.edm.util.PropMap;
import cn.edm.util.PropertiesUtil;
import cn.edm.util.PropertyStatComparator;
import cn.edm.util.XssFilter;
import cn.edm.vo.PropertyVo;
import cn.edm.vo.PropertyVoStat;

@Service
public class LabelServiceImpl implements LabelService{
	
	@Autowired
	private LabelMapper labelMapper;
	
	@Autowired
	private TaskMapper taskMapper;

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void save(Label label) {
		labelMapper.insert(label);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED,readOnly=true)
	public List<Label> getByCategoryId(Integer categoryId) {
		return labelMapper.selectByCategoryId(categoryId);
	}

	@Override
	public String getLabelId(List<Label> lableList) {
		String lableIds = "";
		if(lableList != null && lableList.size()>0) {
			int i;
			int len = lableList.size();
			for(i=0;i<len;i++) {
				lableIds += lableList.get(i).getLableId();
				if(i != len-1) {
					lableIds += ",";
				}
			}
		}
		return lableIds;
	}

	@Override
	public List<PropertyVo> getPropertyVo(String content,List<Label> labelList) {
		if(StringUtils.isEmpty(content)) {
			return null;
		}
		
		List<PropertyVo> propertyVoList = Lists.newArrayList();
		String[] results = StringUtils.split(content, "\r\n");
		if(results.length>1) {
			
			Map<Integer,Label> labelMap = Maps.newHashMap();
			for(Label label:labelList) {
				labelMap.put(label.getLableId(), label);
			}
			
			for(String line:results) {
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 4 || StringUtils.isBlank(records[0])) {
					continue;
				}
				
				PropertyVo pv = new PropertyVo();
				pv.setLabelDesc(labelMap.get(Integer.parseInt(records[0])).getLabelDesc());
				pv.setLabelName(labelMap.get(Integer.parseInt(records[0])).getLabelName());
				pv.setLabelId(Integer.parseInt(records[0]));
				pv.setNum(Integer.parseInt(records[1]));
				pv.setCreateTime(records[2]);
				pv.setUpdateTime(records[3]);
				propertyVoList.add(pv);
			}
		}
		return propertyVoList;
	}

	@Override
	public PropertyVoStat stat(LabelCategory lc,
			List<PropertyVo> propertyVoList,int labelNum) {
		PropertyVoStat pvs = new PropertyVoStat();
		pvs.setLabelNum(labelNum);
		if(propertyVoList != null&&propertyVoList.size()>0) {
			
			int sum = 0;
			for(PropertyVo pv:propertyVoList) {
				sum += pv.getNum();
			}
			
			for(PropertyVo pv:propertyVoList) {
				double percent = (sum==0)?0.00:(pv.getNum()*1.0/sum*100.0);
				pv.setPercentage(percent);
			}
			pvs.setLabelCategory(lc);
			pvs.setEmailNum(sum);
			pvs.setPropertyVoList(propertyVoList);
		}
		return pvs;
	}

	@Override
	public boolean isLabelExist(String labelName) {
		List<Label> labels = labelMapper.selectByName(labelName);
		return (labels!=null&&labels.size()>0)?true:false;
	}

	@Override
	public Map<String, Object> verifyXss(Label label) {
		Map<String,Object> map = new HashMap<String,Object>();
		String message = "";
		boolean isValid = false;
		if(!XssFilter.isValid(label.getLabelName())) {
			message = "标签名称含有非法字符！";
		}else if(!XssFilter.isValid(label.getLabelDesc())){
			message = "标签描述含有非法字符！";
		}else {
			isValid = true;
		}
		map.put(Message.RESULT, isValid);
		map.put(Message.MSG, message);
		return map;
	}

	@Override
	public List<Label> getAll() {
		return labelMapper.selectAll();
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void importPropLib(Map<String,Object> paramsMap) {
		taskMapper.updateLabelStatus(paramsMap);
	}

	@Override
	public List<PropertyVo> getPropertyIndexVo(String content,
			List<Label> labelList) {
		if(StringUtils.isEmpty(content)) {
			return null;
		}
		List<PropertyVo> propertyVoList = Lists.newArrayList();
		String[] results = StringUtils.split(content, "\r\n");
		if(results.length>1) {
			
			Map<Integer,Label> labelMap = Maps.newHashMap();
			for(Label label:labelList) {
				labelMap.put(label.getLableId(), label);
			}
			
			for(String line:results) {
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 4 || StringUtils.isBlank(records[0])) {
					continue;
				}
				
				PropertyVo pv = new PropertyVo();
				pv.setLabelDesc(labelMap.get(Integer.parseInt(records[0])).getLabelDesc());
				pv.setLabelName(labelMap.get(Integer.parseInt(records[0])).getLabelName());
				pv.setLabelId(Integer.parseInt(records[0]));
				pv.setNum(Integer.parseInt(records[1]));
				pv.setCreateTime(records[2]);
				pv.setUpdateTime(records[3]);
				propertyVoList.add(pv);
			}
		}
		
		List<PropertyVo> resultList = Lists.newArrayList();
		if(propertyVoList.size()>=15) {
			Collections.sort(propertyVoList,new PropertyStatComparator());
			
			for(int i=0;i<15;i++) {
				resultList.add(propertyVoList.get(i));
			}
			
			PropertyVo pv = new PropertyVo();
			pv.setLabelName("其他标签");
			int num = 0;
			for(int i=15;i<propertyVoList.size();i++) {
				PropertyVo temp = propertyVoList.get(i);
				num += temp.getNum();
			}
			pv.setNum(num);
			resultList.add(pv);
			return resultList;
		}else {
			return propertyVoList;
		}
		
		
		//return propertyVoList;
	}

	@Override
	public List<String> getLabelIds(String includeIds, String excludeIds) {
		List<String> includeList = Lists.newArrayList();
		if(StringUtils.isNotEmpty(includeIds)){
			String[] include = StringUtils.split(includeIds, ",");
			
			Map<String,String> includeMap = Maps.newHashMap();
			for(int i=0;i<include.length;i++) {
				includeMap.put(include[i], include[i]);
			}
			
			if(excludeIds != null){
				String[] exclude = StringUtils.split(excludeIds, ",");
				for(int j=0;j<exclude.length;j++) {
					if(includeMap.get(exclude[j])!=null) {
						includeMap.remove(exclude[j]);
					}
				}
			}
			
			Iterator<Entry<String,String>>  iterator =  includeMap.entrySet().iterator();
			while(iterator.hasNext()) {
			    Entry<String,String> entry = iterator.next();
			    includeList.add(entry.getValue());
			}
			
			
		}
		
		return includeList;
	}

	@Override
	public List<Label> getLabelByIds(Map<String, Object> params) {
		// TODO Auto-generated method stub
		return labelMapper.selectByLabelIds(params);
	}

	@SuppressWarnings("unchecked")
	@Override
	public Map<String,Object> generateFile(HttpServletRequest request, List<Label> labelList,String[] date,String[] score,String cnd,String excludeTagIds) {
		Map<String,Object> resultMap = Maps.newHashMap();
		StringBuilder sb = new StringBuilder();
		sb.append("邮箱").append(",").append("标签").append(",").append("评分").append("\r\n");
		String dir = request.getSession().getServletContext().getRealPath(PropertyConstant.PROP_FILE_PATH);
		File dirFile = new File(dir);
		if(!dirFile.exists()) {
			System.out.println("目录不存在");
			dirFile.mkdir();
		}
		String fileName = DateUtil.getDateStr()+".csv";
		String filePath = dir + System.getProperty("file.separator")+fileName;
		int counter = 0;
		HttpClient client = new DefaultHttpClient();
		try {
			if(labelList != null&&labelList.size()>0) {
				///query?label=1&type=detail&btime=xxx&etime=xxx
				Map<String,Integer> statMap = new PropMap();
				for(Label label:labelList){
					String url = PropertiesUtil.get(ConfigKeys.PROPERTY_URL)+PropertyConstant.METHOD_QUERY+"label="+label.getLableId()+"&type="+PropertyConstant.TYPE_DETAIL
								+"&btime="+date[0]+"&etime="+date[1]+"&minscore="+score[0]+"&maxscore="+score[1];
					HttpResponse response = Apis.getHttpResponse(client,url, "utf-8");
					int code = response.getStatusLine().getStatusCode();
					if(code==HttpStatus.SC_OK) {
						HttpEntity entity = response.getEntity();
						if(entity != null) {
							String content = IOUtils.toString(entity.getContent(), "utf-8");
							String[] result = getResult(content,label.getLabelName(),statMap);
						    sb.append(result[0]);
							counter += Integer.parseInt(result[1]);
						}
					}
				}
				
				//排除的标签
				Map<String,String> excludeMap = Maps.newHashMap();
				if(StringUtils.isNotEmpty(excludeTagIds)) {
					String url = PropertiesUtil.get(ConfigKeys.PROPERTY_URL)+PropertyConstant.METHOD_QUERY+"label="+excludeTagIds+"&type="+PropertyConstant.TYPE_DETAIL;
					HttpResponse response = Apis.getHttpResponse(client,url, "utf-8");
					if(response.getStatusLine().getStatusCode()==HttpStatus.SC_OK) {
						HttpEntity entity = response.getEntity();
						if(entity != null) {
							String content = IOUtils.toString(entity.getContent(), "utf-8");
							excludeMap = getExclude(content);
						}
						
					}
				}
				
				
				//数据处理
				//交集
				if("and".equals(cnd)) {
					String[] array = getAndContent(sb.toString(),statMap,labelList.size(),excludeMap);
					writeCsv(filePath,array[0]);
					counter = Integer.parseInt(array[1]);
				}else {
					String[] array = getAfterExclude(sb.toString(),excludeMap);
					counter = Integer.parseInt(array[1]);
					writeCsv(filePath,array[0]);
				}
			}
			resultMap.put(Message.RESULT, Message.SUCCESS);
			//resultMap.put("filePath", filePath);
			resultMap.put("counter", counter);
			resultMap.put("fileName", fileName);
		}catch(Exception e) {
			e.printStackTrace();
			resultMap.put(Message.RESULT, Message.FAILURE);
			resultMap.put(Message.MSG, "统计数据出错！");
		}finally {
			client.getConnectionManager().shutdown();
		}
		
		return resultMap;
	}
	
	
	private String[] getAfterExclude(String content,Map<String,String> excludeMap){
		String[] records = StringUtils.split(content, "\r\n");
		int counter = 0;
		String[] returnArray = new String[2];
		StringBuilder sb = new StringBuilder();
		if(records!=null&&records.length>0) {
			sb.append(records[0]).append("\r\n");
			for(int i=1;i<records.length;i++) {
				//不保存排除标签的
				String[] line = StringUtils.split(records[i], ",");
				if(excludeMap.get(line[0].trim())!=null) {
					continue;
				}
				sb.append(records[i]).append("\r\n");
				counter++;
			}
		}
		returnArray[0] = sb.toString();
		returnArray[1] = String.valueOf(counter);
		return returnArray;
	}
	
	
	
	private String[] getResult(String content,String labelName,Map<String,Integer> statMap) {
		String[] returnArray = new String[2];
		if(StringUtils.isEmpty(content)) {
			return null;
		}
		
		StringBuilder sb = new StringBuilder();
		String[] results = StringUtils.split(content, "\r\n");
		//int count=0;
		int counter=0;
		if(results.length>0) {
			for(String line:results) {
				//行业标签,邮箱,评分,创建时间,更新时间
				/*count++;
				if(count==1) {
					continue;
				}*/
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 5 || StringUtils.isBlank(records[0])) {
					continue;
				}
				sb.append(records[1]).append(",").append(labelName).append(",").append(records[2]).append("\r\n");
				statMap.put(records[1].trim(),1);
				counter++;
			}
			
		}
		returnArray[0] = sb.toString();
		returnArray[1] = String.valueOf(counter);
		return returnArray;
	}
	
	
	private Map<String,String> getExclude(String content) {
		String[] results = StringUtils.split(content, "\r\n");
		Map<String,String> excludeMap = Maps.newHashMap();
		if(results.length>0) {
			for(String line:results) {
				//行业标签,邮箱,评分,创建时间,更新时间
				String[] records = StringUtils.splitPreserveAllTokens(line, ",");
				if (records.length < 5 || StringUtils.isBlank(records[0])) {
					continue;
				}
				excludeMap.put(records[1].trim(), records[1].trim());
			}
			
		}
		return excludeMap;
	}
	
	
	private void writeCsv(String filePath,String content){
		//InputStream is = IOUtils.toInputStream(content, "utf-8");
		BufferedReader bufferedReader = null; 
	    BufferedWriter bufferedWriter = null;
	    File csvFile = new File(filePath);
	    try {
	    	bufferedReader = new BufferedReader(new StringReader(content)); 
	        bufferedWriter = new BufferedWriter(new FileWriter(csvFile)); 
	        char buf[] = new char[1024];         //字符缓冲区 
	        int len; 
	        while ((len = bufferedReader.read(buf)) != -1) { 
	             bufferedWriter.write(buf, 0, len); 
	         } 
	    }catch(Exception e) {
	    	e.printStackTrace();
	    }finally {
	    	try {
	            bufferedWriter.flush();
	            bufferedWriter.close();
	            bufferedReader.close(); 
	         } catch (IOException e) {
	            e.printStackTrace();
	         } 

	    }
		 
	}
	
	private String[] getAndContent(String content,Map<String,Integer> statMap,int num,Map<String,String> excludeMap){
		String[] array = new String[2];
		int counter = 0;
		StringBuilder andSb = new StringBuilder();
		String[] records = StringUtils.split(content, "\r\n");
		if(records != null && records.length>0) {
			andSb.append(records[0]).append("\r\n");
			if(records.length>1) {
				for(int i=1;i<records.length;i++) {
					
					//不保存排除标签的
					String[] line = StringUtils.split(records[i], ",");
					if(excludeMap.get(line[0].trim())!=null) {
						continue;
					}
					
					//保存交集的数据
					if(statMap.get(line[0].trim())==num) {
						andSb.append(records[i]).append("\r\n");
						counter++;
					}
				}
			}
		}
		array[0] = andSb.toString();
		array[1] = String.valueOf(counter);
		return array;
	}
}
