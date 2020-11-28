package cn.edm.repository;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import cn.edm.consts.Fqn;
import cn.edm.consts.Queue;
import cn.edm.consts.Value;
import cn.edm.modules.cache.Ehcache;
import cn.edm.utils.Asserts;
import cn.edm.utils.web.Webs;

@Repository
public class Ips {

    private static final Logger logger = LoggerFactory.getLogger(Ips.class);

    @Autowired
    private Ehcache ehcache;

    @SuppressWarnings("unchecked")
    public Map<String, Integer> query() {
        Map<String, Integer> map = (Map<String, Integer>) ehcache.get(Fqn.IPS, Queue.IPS);
        if (Asserts.empty(map)) {
            map = new ConcurrentHashMap<String, Integer>();

            BufferedReader reader = null;
            try {
                String path = Webs.rootPath() + "/WEB-INF/classes/ips";
                reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), "UTF-8"));

                String line = null;
                while ((line = reader.readLine()) != null) {
                    if (StringUtils.isBlank(line)) continue;
                    map.put(line, Value.T);
                }
                ehcache.put(Fqn.IPS, Queue.IPS, map);
                logger.info("(Ips:query) ip_map: " + map);
            } catch (Exception e) {
                logger.warn("(Ips:query) warn: " + e);
            } finally {
                IOUtils.closeQuietly(reader);
            }
        }

        return map;
    }
}
