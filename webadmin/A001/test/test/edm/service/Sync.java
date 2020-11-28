package test.edm.service;

import org.junit.Test;
import org.springframework.core.task.TaskExecutor;
import org.springframework.test.context.ContextConfiguration;

import cn.edm.modules.test.CtxTestCase;
import cn.edm.modules.utils.spring.CtxHolder;

@ContextConfiguration(locations = { "/app-ctx.xml", "/app-executor.xml" })
public class Sync extends CtxTestCase {

    @Test
    public void testThreadPoolExecutor() {
        TaskExecutor executor = CtxHolder.getBean("import");
        for (int i = 1; i <= 10; i++) {
            final int k = i;
            executor.execute(new Runnable() {

                @SuppressWarnings("static-access")
                @Override
                public void run() {
                    Thread thread = new Thread();
                    try {
                        thread.start();
                        thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    System.out.println("---------------" + k);
                }
            });

        }

    }
}
