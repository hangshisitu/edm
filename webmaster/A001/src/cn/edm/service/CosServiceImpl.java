package cn.edm.service;

import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.domain.Cos;
import cn.edm.persistence.CosMapper;
import cn.edm.util.CommonUtil;
import cn.edm.util.Pagination;

/**
 * 套餐业务接口实现
 * @author xiaobo
 *
 */
@Service
public class CosServiceImpl implements CosService{
	
	private static Logger log = LoggerFactory.getLogger(CosServiceImpl.class);
	
	@Autowired
	private CosMapper cosMapper;
	
	@Override
	public List<Cos> getByType(int type){
		return cosMapper.selectByType(type);
	}
	
	@Override
//	@Cacheable(cacheName="userCache")
	public List<Cos> selectByTypeAndStatus(Map<String, Object> paramsMap) {
		return cosMapper.selectByTypeAndStatus(paramsMap);
	}

	@Override
//	@Cacheable(cacheName="userCache")
	public List<Cos> selectAll() {
		return cosMapper.selectAll();
	}

	@Override
//	@Cacheable(cacheName="userCache")
	public Cos getCosByCosId(int cosId) {
		return cosMapper.selectByCosId(cosId);
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
//	@TriggersRemove(cacheName="userCache",removeAll=true)
	public void save(Cos cos) {
		cosMapper.insertCos(cos);
	}

	@Override
	public Pagination<Cos> getPagination(Map<String, Object> paramsMap,
			String orderBy, int currentPage, int pageSize) {
		int startIndex = CommonUtil.getStartIndex(currentPage, pageSize);
		paramsMap.put("startIndex", startIndex);
		paramsMap.put("pageSize", pageSize);
		paramsMap.put("orderBy", orderBy);

		List<Cos> recordList = cosMapper.selectCoses(paramsMap);
		long recordCount = cosMapper.selectCosesCount(paramsMap);
		
		Pagination<Cos> pagination = new Pagination<Cos>(currentPage,pageSize,recordCount,recordList);
		
		return pagination;
	}

	@Override
	@Transactional(propagation=Propagation.REQUIRED)
	public void updateCosStatus(Map<String, Object> paramMap) {
		cosMapper.updateCosStatus(paramMap);
	}
	
	@Override
	public boolean deleteCosById(Map<String, Object> paramsMap) {
		try{
		   cosMapper.deleteCosById(paramsMap);
		   return true;
		}catch(Exception e){
			log.info("delete class of service fail ...");
		   return false;
		}
	}

}
