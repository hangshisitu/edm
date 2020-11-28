package cn.edm.service;

import java.util.List;

import org.joda.time.DateTime;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Cos;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.Asserts;
import cn.edm.utils.Values;
import cn.edm.utils.except.Errors;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class CosService {
	
    @Autowired
    private Dao dao;
    @Autowired
    private TaskService taskService;

    public void save(Cos cos) {
        if (cos.getCosId() != null) {
            dao.update("Cos.update", cos);
        } else {
            dao.save("Cos.save", cos);
        }
    }
    
    public Cos get(Integer cosId) {
        return dao.get("Cos.query", new MapBean("cosId", cosId));
    }
    
    
    public List<Cos> getCosByStatus(Integer type,String status,String remind) {
    	MapBean mb = new MapBean();
        Pages.put(mb, "status",status);
        Pages.put(mb, "type",type);
        Pages.put(mb, "remind",remind);
        return dao.find("Cos.query",mb);
    }

    /**
     * 校验账户的剩余发送量.
     */
	public void balance(Integer[] corpIds, int cosId, int holdCount, int emailCount, DateTime jobTime) {
		if (emailCount < 1) {
			throw new Errors("收件人数不能少于1");
		}
		
		DateTime startOfMonth = jobTime.dayOfMonth().withMinimumValue().millisOfDay().withMinimumValue();
		DateTime endOfMonth = jobTime.dayOfMonth().withMaximumValue().millisOfDay().withMaximumValue();
		
		Cos cos = get(cosId);
		Asserts.isNull(cos, "套餐");
		
		int totalSend = Values.get(cos.getTotalSend());
		int monthSend = Values.get(cos.getMonthSend());
		int daySend = Values.get(cos.getDaySend());
		
		int totalSent = taskService.sentCount(corpIds, null, null);
		int monthSent = taskService.sentCount(corpIds, startOfMonth, endOfMonth);
		int daySent = taskService.sentCount(corpIds, jobTime, jobTime);

		send(totalSend, totalSent, holdCount, emailCount);
		send(monthSend, monthSent, holdCount, emailCount);
		send(daySend, daySent, holdCount, emailCount);
	}

	private void send(int send, int sent, int holdCount, int emailCount) {
		if (send != -1) {
			int result = send + holdCount - sent - emailCount;
			if (result < 0) {
				throw new Errors("账户的剩余发送量不足");
			}
		}
	}
}
