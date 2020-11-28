/*package test.edm.utils;

import org.apache.commons.lang.StringUtils;
import org.junit.Test;

import cn.edm.app.auth.Auth;
import cn.edm.consts.MyCode;
import cn.edm.consts.mapper.UrlMap;
import cn.edm.modules.utils.web.Randoms;
import cn.edm.utils.web.Webs;

public class AuthTest {
	
    @Test
	public void license() {
		String robot = "78:2b:cb:57:e9:86"; // /etc/sysconfig/network-scripts/ifcfg-eth0
		String code = "27fqgjo6"; // 随机码8位
		String expire = MyCode.generate("20131230", 0, 8); // 有效日期
		String send1 = MyCode.generate("10000", 0, 8);  // 总发送量
		String send2 = MyCode.generate("", 0, 8);  // 总发送量

		robot = StringUtils.reverse(StringUtils.replace(robot, ":", ""));
		
		System.out.println("robot: " + Auth.md5(robot, code, null));
		
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(32))); // 干扰线
		System.out.print(code);
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, null));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(expire);
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(send1);
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(send2);
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "account:off"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "account.user:off"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "datasource:off"));
        System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
        System.out.print(Auth.md5(robot, code, "mailing.template:on"));
        System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
        System.out.print(Auth.md5(robot, code, "mailing.campaign:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "mailing.task:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "mailing.audit:on"));
        System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
        System.out.print(Auth.md5(robot, code, "report.campaign:on"));
        System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
        System.out.print(Auth.md5(robot, code, "report.task:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "report.triger:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "report.detail:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "report.export:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(8)));
		System.out.print(Auth.md5(robot, code, "step:on"));
		System.out.print(StringUtils.lowerCase(Randoms.getRandomString(32)));
	}
	
	public void load() {
		Auth.load(Webs.root() + "/WEB-INF/classes/license");
		System.out.println(Auth.MAP.get(UrlMap.CODE.getAction()));
		System.out.println(Auth.MAP.size());
	}
	
	public void robot() {
		Auth.robot(Webs.root() + "/WEB-INF/classes/ifcfg-eth0");
	}
	
	public void sent() {
		System.out.println(MyCode.generate("0", 8, 24));
		String text = "nyxb4ibwsm7tsul4xumuo48wk1ozcrzt2lyzbz6p7pnbdpoaahdmzlwl186kgs1o4fpdpisoyokfjippobecum0rt7kqcfsr7c7fwlz4jufyx2gxgep5icgrq7dq9f5q";
		System.out.println(MyCode.get(StringUtils.substring(text, 32, 96)));
	}
}
*/