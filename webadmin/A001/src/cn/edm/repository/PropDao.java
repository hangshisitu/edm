package cn.edm.repository;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Value;
import cn.edm.consts.mapper.PropMap;
import cn.edm.consts.mapper.TypeMap;
import cn.edm.model.Prop;

import com.google.common.collect.Lists;

@Repository
public class PropDao {
	
	@Autowired
	private Dao dao;
	
	public void save(int corpId) {
		List<Prop> props = Lists.newArrayList();
		props.add(prop(corpId, null, PropMap.EMAIL.getId(), PropMap.EMAIL.getDesc(), TypeMap.STRING.getType()));
		props.add(prop(corpId, null, PropMap.TRUE_NAME.getId(), PropMap.TRUE_NAME.getDesc(), TypeMap.STRING.getType()));
		props.add(prop(corpId, null, PropMap.NICK_NAME.getId(), PropMap.NICK_NAME.getDesc(), TypeMap.STRING.getType()));
		props.add(prop(corpId, null, PropMap.GENDER.getId(), PropMap.GENDER.getDesc(), TypeMap.STRING.getType()));
		props.add(prop(corpId, null, PropMap.JOB.getId(), PropMap.JOB.getDesc(), TypeMap.STRING.getType()));
		props.add(prop(corpId, null, PropMap.AGE.getId(), PropMap.AGE.getDesc(), TypeMap.INTEGER.getType()));
		props.add(prop(corpId, null, PropMap.BIRTHDAY.getId(), PropMap.BIRTHDAY.getDesc(), TypeMap.DATE.getType()));
		dao.save("Prop.batchSave", props);
	}
	
    private Prop prop(int corpId, String userId, String propName, String propDesc, String type) {
		Prop prop = new Prop();
		prop.setCorpId(corpId);
		prop.setUserId(userId);
		prop.setPropName(propName);
		prop.setPropDesc(propDesc);
		prop.setType(type);
		prop.setRequired(Value.F);
		return prop;
	}

}
