package test.repository;

import org.junit.Before;
import org.junit.Test;

import cn.edm.constant.Config;
import cn.edm.modules.utils.Props;
import cn.edm.util.Mongos;
import cn.edm.utils.web.Webs;

public class MongoTest {

    @Before
    public void before() {
        Props.load(Webs.root() + "/WEB-INF/classes/app.properties");
        String host = Props.getStr(Config.MONGO_HOST);
        Integer port = Props.getInt(Config.MONGO_PORT);
        Integer poolSize = Props.getInt(Config.MONGO_POOL_SIZE);
        Mongos.getInstance().init(host, port, poolSize);
    }

    @Test
    public void test() {
        System.out.println(Mongos.db("tag_1").getCollectionNames());
    }
}
