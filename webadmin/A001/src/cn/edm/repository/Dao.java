package cn.edm.repository;

import org.springframework.stereotype.Repository;

import cn.edm.modules.orm.ibatis.SimpleSqlMapClientDao;

@Repository
public class Dao extends SimpleSqlMapClientDao<Object, Integer> {
}
