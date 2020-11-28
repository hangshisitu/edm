package cn.edm.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import cn.edm.model.Plan;
import cn.edm.modules.orm.MapBean;
import cn.edm.repository.Dao;
import cn.edm.utils.web.Pages;

@Transactional
@Service
public class PlanService {

    @Autowired
    private Dao dao;

    public void save(Plan plan) {
        if (plan.getPlanId() != null) {
            dao.update("Plan.update", plan);
        } else {
            dao.save("Plan.save", plan);
        }
    }

    public Plan get(Integer[] corpIds, Integer corpId, Integer planId) {
        MapBean mb = new MapBean();
        Pages.put(mb, "corpIds", corpIds);
        Pages.put(mb, "corpId", corpId);
        mb.put("planId", planId);
        return dao.get("Plan.query", mb);
    }

	public List<Plan> iter(String beginTime, String endTime, int last, int row) {
		MapBean mb = new MapBean();
		Pages.put(mb, "beginTime", beginTime);
		Pages.put(mb, "endTime", endTime);
		Pages.put(mb, "last", last);
		Pages.put(mb, "row", row);
		return dao.find("Plan.iter", mb);
	}
	
	public List<Plan> iter(Integer[] planIds, String beginTime, String endTime, int last, int row) {
		MapBean mb = new MapBean();
		Pages.put(mb, "planIds", planIds);
		Pages.put(mb, "beginTime", beginTime);
		Pages.put(mb, "endTime", endTime);
		Pages.put(mb, "last", last);
		Pages.put(mb, "row", row);
		return dao.find("Plan.iter", mb);
	}
	
	public Integer exclude(Integer planId) {
		return dao.get("Plan.exclude", new MapBean("planId", planId));
	}
	
}
