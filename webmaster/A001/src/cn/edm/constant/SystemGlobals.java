package cn.edm.constant;

import java.util.Map;

import org.apache.commons.lang.StringUtils;

import com.google.common.collect.Maps;

public class SystemGlobals {

	public static final String NAME = "EDM_NAME"; 			// 姓名
	public static final String NICK_NAME = "EDM_NICK_NAME"; // 昵称
	public static final String GENDER = "EDM_GENDER"; 		// 性别
	public static final String BIRTHDAY = "EDM_BIRTHDAY"; 	// 出生日期
//	public static final String AGE = "EDM_AGE"; 			// 年龄
	public static final String COMPANY = "EDM_COMPANY"; 	// 公司
	public static final String JOB = "EDM_JOB"; 			// 职位
	public static final String MOBILE = "EDM_MOBILE"; 		// 手机

	public static String EMAIL_COUNT = "126.com,139.com,163.com,163bj.com,188.com,21cn.com,21cn.net,8848.net,963.net,btamail.net.cn,cdc.com,citiz.net,eastday.com,foxmail.com,gmail.com,google.com,headwaters.com,hotmail.com,kali.com.cn,qq.com,sina.cn,sina.com,sina.com.cn,sohu.com,sohu.net,tianya.cn,tom.com,vip.126.com,vip.163.com,vip.qq.com,vip.sina.com,vip.sina.com.cn,vip.sohu.com,xilu.com,yahoo.cn,yahoo.com,yahoo.com.cn,yahoo.hk,yeah.net,ymail.com";
	public static String DOMAINS = ",ac,ad,ae,aero,af,ag,ai,al,am,an,ao,aq,ar,arpa,as,asia,at,au,aw,ax,az,ba,bb,bd,be,bf,bg,bh,bi,biz,bj,bm,bn,bo,br,bs,bt,bw,by,bz,ca,cc,cd,cf,cg,ch,ci,ck,cl,cm,cn,co,com,coop,cr,cu,cv,cx,cy,cz,de,dj,dk,dm,do,dz,ec,edu,ee,eg,er,es,et,eu,fi,fj,fk,fm,fo,fr,ga,gd,ge,gf,gg,gh,gi,gl,gm,gn,gov,gp,gq,gr,gs,gt,gu,gw,gy,hk,hm,hn,hr,ht,hu,id,idv,ie,il,im,in,info,int,io,iq,ir,is,it,je,jm,jo,jobs,jp,ke,kg,kh,ki,km,kn,kp,kr,kw,ky,kz,la,lb,lc,li,lk,lr,ls,lt,lu,lv,ly,ma,mc,md,me,mg,mh,mil,mk,ml,mm,mn,mo,mobi,mp,mq,mr,ms,mt,mu,museum,mv,mw,mx,my,mz,na,name,nc,ne,net,nf,ng,ni,nl,no,np,nr,nu,nz,om,org,pa,pe,pf,pg,ph,pk,pl,pn,pr,pro,ps,pt,pw,py,qa,re,ro,rs,ru,rw,sa,sb,sc,sd,se,sg,sh,si,sk,sl,sm,sn,so,sr,ss,st,sv,sy,sz,tc,td,tel,tf,tg,th,tj,tk,tl,tm,tn,to,tr,travel,tt,tv,tw,tz,ua,ug,uk,us,uy,uz,va,vc,ve,vg,vi,vn,vu,wf,ws,xxx,ye,yu,za,zm,zw,";
	public static Map<String, String> DOMAIN_MAP = Maps.newHashMap();
	
	static {
		StringBuffer sbff = new StringBuffer();
		sbff.append("163.com=188.com,yeah.net,126.com,163.com,vip.126.com,vip.163.com")
			.append("&qq.com=qq.com,vip.qq.com,foxmail.com")
			.append("&139.com=139.com")
			.append("&sina.com=sina.com,vip.sina.com,vip.sina.com.cn,sina.cn,sina.com.cn")
			.append("&sohu.com=sohu.com,vip.sohu.com,sohu.net")
			.append("&21cn.com=21cn.com,21cn.net")
			.append("&gmail.com=gmail.com,google.com")
			.append("&yahoo.com.cn=yahoo.com.cn,yahoo.com,yahoo.cn,yahoo.hk,ymail.com,xilu.com");
		
		for (String d : StringUtils.split(sbff.toString(), "&")) {
			String k = StringUtils.substringBefore(d, "=");
			String v = StringUtils.substringAfter(d, "=");
			if (StringUtils.isNotBlank(k) && StringUtils.isNotBlank(v)) {
				SystemGlobals.DOMAIN_MAP.put(k, v);
			}
		}
	}
	
}
