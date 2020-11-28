/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "web/urlhelper.h"
#include "base/stringtokenizer.h"
#include "base/formatstream.h"
#include <list>
#include <set>

RFC_NAMESPACE_BEGIN

typedef std::list<stdstring>		typeDomainTokenList;

class CheckValidDomainEnv
{
public:
	CheckValidDomainEnv(void) { resetEnv(); }
	bool				isUSState(const stdstring & strState) const;
	bool				isGlobalDomain(const stdstring & strLevel2Domain) const;

private:
	typedef std::set<stdstring, IgnoreCaseStringCompare> typeURLSet;
	typeURLSet			m_setUSStates;
	typeURLSet			m_setGlobalDomain;
	static void			setDoimainList(const stdstring & strList, typeURLSet & setResult);
	void				resetEnv(void);
};

void CheckValidDomainEnv::setDoimainList(const stdstring & strList, typeURLSet & setResult)
{
	StringTokenizer<stdstring> tokenDomain(strList.data(), strList.size());
	tokenDomain.setDelimiter(",; \t\r\n");
	stdstring strDomain;
	while ( tokenDomain.getNext(strDomain) )
		setResult.insert(strDomain);
}

void CheckValidDomainEnv::resetEnv(void)
{
	stdstring strUSState = "ak,al,ar,az,ca,co,ct,dc,de,fl,ga,gu,hi,ia,id,il,in,ks,ky,la,ma,md,me,mi,mn,mo,ms,mt,nc,nd,ne,nh,"
		"nj,nm,nv,ny,oh,ok,or,pa,pr,ri,sc,sd,tn,tx,ut,va,vi,vt,wa,wi,wv,wy";
	setDoimainList(strUSState, m_setUSStates);

	stdstring strGlobalDomain = "fed.us,dni.us,com.ac,edu.ac,gov.ac,mil.ac,net.ac,org.ac,ac.ae,co.ae,com.ae,gov.ae,net.ae,org.ae,"
		"pro.ae,sch.ae,com.ai,edu.ai,gov.ai,org.ai,com.ar,edu.ar,gov.ar,int.ar,mil.ar,net.ar,org.ar,e164.arpa,ac.at,co.at,gv.at,or.at,"
		"priv.at,asn.au,com.au,conf.au,csiro.au,edu.au,gov.au,id.au,info.au,net.au,org.au,otc.au,oz.au,telememo.au,com.az,net.az,org.az,"
		"com.bb,net.bb,org.bb,ac.be,belgie.be,dns.be,fgov.be,com.bh,edu.bh,gov.bh,net.bh,org.bh,com.bm,edu.bm,gov.bm,net.bm,org.bm,art.br,"
		"com.br,etc.br,g12.br,gov.br,ind.br,inf.br,mil.br,net.br,org.br,psi.br,rec.br,sp.br,tmp.br,com.bs,net.bs,org.bs,ab.ca,bc.ca,mb.ca,"
		"nb.ca,nf.ca,nl.ca,ns.ca,nt.ca,nu.ca,on.ca,pe.ca,qc.ca,sk.ca,yk.ca,co.ck,edu.ck,gov.ck,net.ck,org.ck,ac.cn,ah.cn,bj.cn,com.cn,cq.cn,"
		"edu.cn,gd.cn,gov.cn,gs.cn,gx.cn,gz.cn,hb.cn,he.cn,hi.cn,hk.cn,hl.cn,hn.cn,jl.cn,js.cn,ln.cn,mo.cn,net.cn,nm.cn,nx.cn,org.cn,qh.cn,"
		"sc.cn,sh.cn,sn.cn,sx.cn,tj.cn,tw.cn,xj.cn,xz.cn,yn.cn,zj.cn,arts.co,com.co,edu.co,firm.co,gov.co,info.co,int.co,mil.co,nom.co,org.co,"
		"rec.co,store.co,web.co,lkd.co,plc.co,au.com,br.com,cn.com,de.com,eu.com,gb.com,hu.com,no.com,qc.com,ru.com,sa.com,se.com,uk.com,us.com,"
		"uy.com,za.com,ac.cr,co.cr,ed.cr,fi.cr,go.cr,or.cr,sa.cr,com.cu,net.cu,org.cu,ac.cy,com.cy,gov.cy,net.cy,org.cy,co.dk,art.do,com.do,"
		"edu.do,gov.do,mil.do,net.do,org.do,web.do,art.dz,ass.dz,com.dz,edu.dz,gov.dz,net.dz,org.dz,pol.dz,com.ec,edu.ec,fin.ec,gov.ec,k12.ec,"
		"med.ec,mil.ec,net.ec,org.ec,com.eg,edu.eg,eun.eg,gov.eg,net.eg,org.eg,sci.eg,biz.et,com.et,edu.et,gov.et,info.et,name.et,net.et,org.et,"
		"ac.fj,com.fj,gov.fj,id.fj,org.fj,school.fj,ac.fk,com.fk,gov.fk,net.fk,nom.fk,org.fk,aeroport.fr,assedic.fr,asso.fr,avocat.fr,avoues.fr,"
		"barreau.fr,cci.fr,chambagri.fr,chirurgiens-dentistes.fr,com.fr,experts-comptables.fr,geometre-expert.fr,gouv.fr,greta.fr,"
		"huissier-justice.fr,medecin.fr,nom.fr,notaires.fr,pharmacien.fr,port.fr,prd.fr,presse.fr,tm.fr,veterinaire.fr,com.ge,edu.ge,gov.ge,"
		"mil.ge,net.ge,org.ge,pvt.ge,ac.gg,alderney.gg,co.gg,gov.gg,guernsey.gg,ind.gg,ltd.gg,net.gg,org.gg,sark.gg,sch.gg,com.gu,edu.gu,gov.gu,"
		"mil.gu,net.gu,org.gu,com.hk,edu.hk,gov.hk,idv.hk,net.hk,org.hk,2000.hu,agrar.hu,bolt.hu,casino.hu,city.hu,co.hu,erotica.hu,erotika.hu,"
		"film.hu,forum.hu,games.hu,hotel.hu,info.hu,ingatlan.hu,jogasz.hu,konyvelo.hu,lakas.hu,media.hu,news.hu,org.hu,priv.hu,reklam.hu,"
		"sex.hu,shop.hu,sport.hu,suli.hu,szex.hu,tm.hu,tozsde.hu,utazas.hu,video.hu,ac.id,co.id,go.id,mil.id,net.id,or.id,ac.il,co.il,gov.il,"
		"idf.il,k12.il,muni.il,net.il,org.il,ac.im,co.im,gov.im,net.im,nic.im,org.im,ac.in,co.in,ernet.in,firm.in,gen.in,gov.in,ind.in,mil.in,"
		"net.in,nic.in,org.in,res.in,ac.je,co.je,gov.je,ind.je,jersey.je,ltd.je,net.je,org.je,sch.je,com.jo,edu.jo,gov.jo,mil.jo,net.jo,org.jo,"
		"ac.jp,ad.jp,aichi.jp,akita.jp,aomori.jp,chiba.jp,co.jp,ed.jp,ehime.jp,fukui.jp,fukuoka.jp,fukushima.jp,gifu.jp,go.jp,gov.jp,gr.jp,"
		"gunma.jp,hiroshima.jp,hokkaido.jp,hyogo.jp,ibaraki.jp,ishikawa.jp,iwate.jp,kagawa.jp,kagoshima.jp,kanagawa.jp,kanazawa.jp,kawasaki.jp,"
		"kitakyushu.jp,kobe.jp,kochi.jp,kumamoto.jp,kyoto.jp,lg.jp,matsuyama.jp,mie.jp,miyagi.jp,miyazaki.jp,nagano.jp,nagasaki.jp,nagoya.jp,"
		"nara.jp,ne.jp,net.jp,niigata.jp,oita.jp,okayama.jp,okinawa.jp,org.jp,or.jp,osaka.jp,saga.jp,saitama.jp,sapporo.jp,sendai.jp,shiga.jp,"
		"shimane.jp,shizuoka.jp,takamatsu.jp,tochigi.jp,tokushima.jp,tokyo.jp,tottori.jp,toyama.jp,utsunomiya.jp,wakayama.jp,yamagata.jp,"
		"yamaguchi.jp,yamanashi.jp,yokohama.jp,com.kh,edu.kh,gov.kh,mil.kh,net.kh,org.kh,per.kh,ac.kr,co.kr,go.kr,kyonggi.kr,ne.kr,or.kr,"
		"pe.kr,re.kr,seoul.kr,com.kw,edu.kw,gov.kw,net.kw,org.kw,com.la,net.la,org.la,com.lb,edu.lb,gov.lb,mil.lb,net.lb,org.lb,com.lc,edu.lc,"
		"gov.lc,net.lc,org.lc,asn.lv,com.lv,conf.lv,edu.lv,gov.lv,id.lv,mil.lv,net.lv,org.lv,com.ly,net.ly,org.ly,ac.ma,co.ma,net.ma,org.ma,"
		"press.ma,com.mk,com.mm,edu.mm,gov.mm,net.mm,org.mm,com.mo,edu.mo,gov.mo,net.mo,org.mo,com.mt,edu.mt,net.mt,org.mt,tm.mt,uu.mt,com.mx,"
		"net.mx,org.mx,com.my,edu.my,gov.my,net.my,org.my,alt.na,com.na,cul.na,edu.na,net.na,org.na,telecom.na,unam.na,com.nc,net.nc,org.nc,"
		"de.net,gb.net,uk.net,ac.ng,com.ng,edu.ng,gov.ng,net.ng,org.ng,sch.ng,com.ni,edu.ni,gob.ni,net.ni,nom.ni,org.ni,tel.no,com.np,edu.np,"
		"gov.np,net.np,org.np,fax.nr,mobile.nr,mobil.nr,mob.nr,tel.nr,tlf.nr,ac.nz,co.nz,cri.nz,geek.nz,gen.nz,govt.nz,iwi.nz,maori.nz,mil.nz,"
		"net.nz,org.nz,school.nz,ac.om,biz.om,com.om,co.om,edu.om,gov.om,med.om,mod.om,museum.om,net.om,org.om,pro.om,dk.org,eu.org,ac.pa,"
		"com.pa,edu.pa,gob.pa,net.pa,org.pa,sld.pa,com.pe,edu.pe,gob.pe,mil.pe,net.pe,nom.pe,org.pe,ac.pg,com.pg,net.pg,com.ph,mil.ph,"
		"net.ph,ngo.ph,org.ph,biz.pk,com.pk,edu.pk,fam.pk,gob.pk,gok.pk,gon.pk,gop.pk,gos.pk,gov.pk,net.pk,org.pk,web.pk,agro.pl,aid.pl,"
		"atm.pl,auto.pl,biz.pl,com.pl,edu.pl,gmina.pl,gsm.pl,info.pl,mail.pl,media.pl,miasta.pl,mil.pl,net.pl,nieruchomosci.pl,nom.pl,"
		"org.pl,pc.pl,powiat.pl,priv.pl,realestate.pl,rel.pl,sex.pl,shop.pl,sklep.pl,sos.pl,szkola.pl,targi.pl,tm.pl,tourism.pl,travel.pl,"
		"turystyka.pl,edu.ps,gov.ps,plo.ps,sec.ps,com.py,edu.py,net.py,org.py,com.qa,edu.qa,gov.qa,net.qa,org.qa,asso.re,com.re,nom.re,"
		"com.ru,net.ru,org.ru,pp.ru,com.sa,edu.sa,gov.sa,med.sa,net.sa,org.sa,pub.sa,sch.sa,com.sb,edu.sb,gov.sb,net.sb,org.sb,com.sd,"
		"edu.sd,gov.sd,med.sd,net.sd,org.sd,sch.sd,com.sg,edu.sg,gov.sg,net.sg,org.sg,per.sg,com.sh,edu.sh,gov.sh,mil.sh,net.sh,org.sh,"
		"com.sv,edu.sv,gob.sv,org.sv,red.sv,com.sy,gov.sy,net.sy,org.sy,ac.th,co.th,go.th,net.th,or.th,com.tn,edunet.tn,ens.tn,fin.tn,"
		"gov.tn,ind.tn,info.tn,intl.tn,nat.tn,net.tn,org.tn,rnrt.tn,rns.tn,rnu.tn,tourism.tn,bbs.tr,com.tr,edu.tr,gen.tr,gov.tr,k12.tr,"
		"mil.tr,net.tr,org.tr,at.tt,au.tt,be.tt,biz.tt,ca.tt,com.tt,co.tt,de.tt,dk.tt,edu.tt,es.tt,eu.tt,fr.tt,gov.tt,info.tt,it.tt,name.tt,"
		"net.tt,nic.tt,org.tt,pro.tt,se.tt,uk.tt,us.tt,co.tv,com.tw,edu.tw,gove.tw,idv.tw,net.tw,org.tw,com.ua,edu.ua,gov.ua,net.ua,org.ua,"
		"ac.ug,co.ug,go.ug,or.ug,ac.uk,co.uk,edu.uk,gov.uk,ltd.uk,me.uk,mod.uk,net.uk,nhs.uk,nic.uk,org.uk,plc.uk,police.uk,sch.uk,com.uy,"
		"edu.uy,gub.uy,mil.uy,net.uy,org.uy,arts.ve,bib.ve,com.ve,co.ve,edu.ve,firm.ve,gov.ve,info.ve,int.ve,mil.ve,net.ve,nom.ve,org.ve,"
		"rec.ve,store.ve,tec.ve,web.ve,co.vi,net.vi,org.vi,ac.vn,biz.vn,com.vn,edu.vn,gov.vn,health.vn,info.vn,int.vn,name.vn,net.vn,org.vn,"
		"pro.vn,ch.vu,com.vu,de.vu,edu.vu,fr.vu,net.vu,org.vu,com.ws,edu.ws,gov.ws,net.ws,org.ws,com.ye,edu.ye,gov.ye,mil.ye,net.ye,org.ye,"
		"ac.yu,co.yu,edu.yu,org.yu,ac.za,alt.za,bourse.za,city.za,co.za,edu.za,gov.za,law.za,mil.za,net.za,ngo.za,nom.za,org.za,school.za,"
		"tm.za,web.za,ac.zw,co.zw,gov.zw,org.zw,";
	setDoimainList(strGlobalDomain, m_setGlobalDomain);
}

bool CheckValidDomainEnv::isUSState(const stdstring & strState) const
{
	return ( m_setUSStates.find(strState) != m_setUSStates.end() );
}

bool CheckValidDomainEnv::isGlobalDomain(const stdstring & strLevel2Domain) const
{
	return ( m_setGlobalDomain.find(strLevel2Domain) != m_setGlobalDomain.end() );
}

//////////////////////////////////////////////////////////////////////////

// Base on wget
enum
{
	R		= 1,					// rfc1738 reserved chars, preserved from encoding.
	U		= 2,					// rfc1738 unsafe chars, plus some more.
	RU		= R|U
};
static const unsigned char g_lpURLTable[] =
{
	U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
	U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
	U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
	U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
	U,  0,  U, RU,   0,  U,  R,  0,   /* SP  !   "   #    $   %   &   '   */
	0,  0,  0,  R,   0,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
	0,  0, RU,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
	RU,  0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
	0,  0,  0, RU,   U, RU,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
	U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
	0,  0,  0,  U,   U,  U,  U,  U,   /* x   y   z   {    |   }   ~   DEL */

	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,

	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
	U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};

// Base on RFC 1738, the follow char SHOULD NOT escape, and the other SHOULD escape
// 'a'-'z', 'A'-'Z', '0'-'9', , '-', '_', '.', '*', 
enum { ESC = 1 };
static const unsigned char g_lpURLEscape[256]=
{
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//0-15
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//16-31
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 0,   0,   ESC, 	//32-47 ( - . )
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ESC, ESC, ESC, ESC, ESC, ESC, 	//48-63 ( 0~9 )
	ESC, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   	//64-79 ( A~Z )
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ESC, ESC, ESC, ESC, 0,   	//80-95 ( _ )
	ESC, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   	//96-111( a~z )
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ESC, ESC, ESC, ESC, ESC, 	//112-127
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//128-143
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//144-159
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//160-175
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//176-191
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//192-207
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//208-223
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//224-239
	ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, 	//240-255
};

//////////////////////////////////////////////////////////////////////////

bool URLHelper::isValidURLChar(unsigned char ch)
{
	return ( (g_lpURLTable[ch] & U) == 0 );
}

bool URLHelper::isValidURLEncodeChar(char ch)
{
	return ( isValidURLChar(ch) || ch == '%' || ch == '\\' );			// '\' 等价 '/'
}

bool URLHelper::isURLEscapeChar(unsigned char ch)
{
	return ( (g_lpURLEscape[ch] & ESC) != 0 );
}

bool URLHelper::checkURLPrefix(const stdstring & strURL)
{
	return ( StringFunc::strnCasecmp(strURL, "http://", 7) == 0
			|| StringFunc::strnCasecmp(strURL, "https://", 8) == 0
			|| StringFunc::strnCasecmp(strURL, "ftp://", 6) == 0
			|| StringFunc::strnCasecmp(strURL, "www.", 4) == 0 );
}

void URLHelper::getURLWithPrefix(const char * pText, size_t nTextSize, const char * pURLPrefix, size_t nPrefixLen, URLVisitor & urlVisitor)
{
	if ( pText == NULL || pURLPrefix == NULL || nTextSize <= nPrefixLen )
		return;

	const char * pTextEnd = pText + nTextSize;
	FindSubString strKey(pURLPrefix, nPrefixLen);
	for ( size_t nPos = strKey.findIgnoreCase(pText, nTextSize); nPos != stdstring::npos; nPos = strKey.findIgnoreCase(pText, pTextEnd - pText) )
	{
		const char * pURLBegin = pText + nPos;
		pText = pURLBegin + nPrefixLen;		// pText is end of url
		while ( pText < pTextEnd && isValidURLEncodeChar(*pText) )
			++pText;
		if ( pURLBegin + nPrefixLen < pText )
			urlVisitor.onURL(pURLBegin, pText);
	} //for
}

void URLHelper::getURLWithPostfix(const char * pText, size_t nTextSize, const char * pURLPostfix, size_t nPostfixLen, URLVisitor & urlVisitor)
{
	if ( pText == NULL || pURLPostfix == NULL || nTextSize <= nPostfixLen )
		return;

	const char * pTextBegin = pText;
	const char * pTextEnd = pText + nTextSize;
	FindSubString strKey(pURLPostfix, nPostfixLen);
	for ( size_t nPos = strKey.findIgnoreCase(pText, nTextSize); nPos != stdstring::npos; nPos = strKey.findIgnoreCase(pText, pTextEnd - pText) )
	{
		const char * pURLBegin = pText + nPos;
		pText = pURLBegin + nPostfixLen;	// pText is end of url
		if ( isValidURLChar(*pText) && *pText != '/' && *pText != '\\' )		// 后缀不可以和合法字符连在一起
			continue;
		// 向前扫描URL
		while ( pURLBegin > pTextBegin && isValidURLEncodeChar(*pURLBegin) )
			--pURLBegin;
		if ( pURLBegin > pTextBegin && *pURLBegin == ':' )		// skip ':' in http://
			--pURLBegin;
		while ( pURLBegin > pTextBegin && isValidURLEncodeChar(*pURLBegin) )
			--pURLBegin;

		// 只能以数字/字母开头,并且检查已经获得有效的url
		while ( pURLBegin < pText && !::isalnum(*pURLBegin) )
			++pURLBegin;
		if ( pURLBegin + nPostfixLen >= pText )
			continue;
		while ( pText < pTextEnd && isValidURLEncodeChar(*pText) )	//获取url参数
			++pText;
		urlVisitor.onURL(pURLBegin, pText);
	} //for
}

bool URLHelper::getDomainFromURL(const char * pURL, size_t nURLLen, stdstring & strDomain, stdstring & strSubDomain)
{
	stdstring strDecodedURL;
	decodeURLArg(strDecodedURL, pURL, pURL + nURLLen);
	StringFunc::toLower(strDecodedURL);
	const char * pDecodeURL = strDecodedURL.data();
	const char * const pDecodeURLEnd = pDecodeURL + strDecodedURL.size();

	// http://www.example.com:8080/aaa
	//     ^                 ^
	const char * pColonPos = std::find(pDecodeURL, pDecodeURLEnd, ':');
	const char * pPointPos = std::find(pDecodeURL, pDecodeURLEnd, '.');
	if ( pColonPos < pDecodeURLEnd && pPointPos < pDecodeURLEnd && pPointPos < pColonPos )	// 避免找到端口处的冒号
		pColonPos = pDecodeURLEnd;
	if ( pColonPos < pDecodeURLEnd )
	{
		// http://www.example.com:8080/aaa
		//        ^
		// http://.www.example.com:8080/aaa
		//         ^
		for ( pDecodeURL = pColonPos + 1; pDecodeURL < pDecodeURLEnd; ++pDecodeURL )
		{
			if ( *pDecodeURL != '/' && *pDecodeURL != '.' )
				break;
		}
		if ( pDecodeURL >= pDecodeURLEnd )
			return false;
	} // if ( pColonPos < pDecodeURLEnd )

	// ftp://user:password@ftp.example.com/
	//                     ^
	const char * pUserInfoPos = std::find(pDecodeURL, pDecodeURLEnd, '@');
	const char * pSlashPos = std::find(pDecodeURL, pDecodeURLEnd, '/');
	if ( pUserInfoPos < pDecodeURLEnd && pSlashPos < pDecodeURLEnd && pSlashPos < pUserInfoPos )	// 避免找到端口处的冒号
		pUserInfoPos = pDecodeURLEnd;
	if ( pUserInfoPos < pDecodeURLEnd )
		pDecodeURL = pUserInfoPos + 1;

	// www.example.com
	//               ^
	// www.example.com/aaa
	//               ^
	// www.example.com:8080/aaa
	//               ^
	// http://redirect.alibaba.com?http://page.china.alibaba.com/shtml/
	//                           ^
	const char * pDomainBegin = pDecodeURL;
	for ( ; pDecodeURL < pDecodeURLEnd; ++pDecodeURL )
	{
		if ( *pDecodeURL == '/' || *pDecodeURL == '\\' || *pDecodeURL == ':' || *pDecodeURL == '?' )
			break;
		if ( !isValidURLChar(*pDecodeURL) )
			return false;
	} // for ( ; pDecodeURL < pDecodeURLEnd; ++pDecodeURL )

	// www.example.com.. -> www.example.com
	if ( pDecodeURL <= pDomainBegin )
		return false;
	for ( --pDecodeURL; pDecodeURL > pDomainBegin && *pDecodeURL == '.'; )
		--pDecodeURL;
	strDomain.assign(pDomainBegin, pDecodeURL + 1 - pDomainBegin);

	// www..example.com -> www.example.com
	GlobalFunc::replace(strDomain, "..", ".");
	strSubDomain = strDomain;
	if ( GlobalFunc::isValidIPV4Addr(strDomain.c_str()) )
		return true;
	if ( !GlobalFunc::isValidDomain(strDomain) )
		return false;

	// www.example.com -> example.com
	typeDomainTokenList listDomainLevel;
	StringTokenizer<stdstring> tokenDomain(strDomain.data(), strDomain.size());
	stdstring strDomainToken;
	while ( tokenDomain.getNext(strDomainToken, ".") )
		listDomainLevel.push_front(strDomainToken);
	stdstring arDomainLevel[5];		// 最多只保存4级域名
	typeDomainTokenList::const_iterator itToken = listDomainLevel.begin(), itTokenEnd = listDomainLevel.end();
	for ( size_t nDomainTokenCount = 0; nDomainTokenCount < 5 && itToken != itTokenEnd; ++nDomainTokenCount, ++itToken )
		arDomainLevel[nDomainTokenCount] = (*itToken);


	// 是否4级域名?
	// http://www.neustar.us/policies/docs/rfc_1480.txt
	// "Fire-Dept.CI.Los-Angeles.CA.US"
	// "<school-name>.PVT.K12.<state>.US"
	//
	// pvt\.k12\.${US_STATES}\.us|
	// c[io]\.[^\.]+\.${US_STATES}\.us
	static const CheckValidDomainEnv g_CheckValidDomainEnv;
	if ( arDomainLevel[0] == "us" && g_CheckValidDomainEnv.isUSState(arDomainLevel[1]) && !arDomainLevel[3].empty() )
	{
		strDomain.clear();
		if ( !arDomainLevel[4].empty() && ((arDomainLevel[2] == "k12" && arDomainLevel[3] == "pvt") || arDomainLevel[3]=="ci" || arDomainLevel[3]=="co") )
			strDomain = arDomainLevel[4] + '.';
		strDomain += FormatString("%.%.%.%").arg(arDomainLevel[3]).arg(arDomainLevel[2]).arg(arDomainLevel[1]).arg(arDomainLevel[0]).str();
	}
	// 是否3级域名
	// http://www.neustar.us/policies/docs/rfc_1480.txt
	// demon.co.uk
	else if( arDomainLevel[0] == "uk" && arDomainLevel[1] == "co" && arDomainLevel[2] == "demon" && !arDomainLevel[3].empty()  )
	{
		strDomain = FormatString("%.%.%.%").arg(arDomainLevel[3]).arg(arDomainLevel[2]).arg(arDomainLevel[1]).arg(arDomainLevel[0]).str();
	}
	else
	{
		strDomain = arDomainLevel[1] + '.' + arDomainLevel[0];	// 普通域名
		if ( g_CheckValidDomainEnv.isGlobalDomain(strDomain) && !arDomainLevel[2].empty() )	// 是否2级域名
			strDomain = arDomainLevel[2] + '.' + strDomain;
	}
	return true;
}

bool URLHelper::getDomainFromURL(const stdstring & strURL, stdstring & strDomain, stdstring & strSubDomain)
{
	return getDomainFromURL(strURL.data(), strURL.size(), strDomain, strSubDomain);
}

stdstring & URLHelper::encodeURLArg(stdstring & strResult, const char * pURL, const char * pURLEnd)
{
	strResult.reserve( strResult.size() + pURLEnd - pURL );
	for ( ; pURL < pURLEnd; ++pURL )
	{
		unsigned char ch = static_cast<unsigned char>(*pURL);
		if ( ch == ' ' )
			strResult += '+';
		else if ( (g_lpURLEscape[ch] & ESC) == 0 )		// 不需要转义
			strResult += ( *pURL );
		else
		{
			strResult += '%';
			strResult += StringFunc::g_strUpperHexChar[ch >> 4];
			strResult += StringFunc::g_strUpperHexChar[ch % 16];
		}
	} // for ( ; pURL < pURLEnd; ++pURL )
	return strResult;
}

stdstring URLHelper::encodeURLArg(const char * pURL, const char * pURLEnd)
{
	stdstring strResult;
	return encodeURLArg(strResult, pURL, pURLEnd);
}

stdstring & URLHelper::encodeURLArg(stdstring & strResult, const stdstring & strURL)
{
	encodeURLArg(strResult, strURL.data(), strURL.data() + strURL.size());
	return strResult;
}

stdstring & URLHelper::decodeURLArg(stdstring & strResult, const char * pURL, const char * pURLEnd)
{
	strResult.reserve( strResult.size() + pURLEnd - pURL );
	for ( ; pURL < pURLEnd; ++pURL )
	{
		char ch = (*pURL);
		if ( ch == '+' )
			strResult += ' ';
		else if ( ch == '%' && pURL + 2 < pURLEnd )
		{
			int n1 = StringFunc::convertHexNumberToDec( *(pURL + 1) );
			int n2 = StringFunc::convertHexNumberToDec( *(pURL + 2) );
			if ( n1 >= 0 && n2 >= 0 )
			{
				strResult += static_cast<char>( (n1 << 4) + n2 );
				pURL += 2;
			}
			else
				strResult += ch;
		}
		else // if ( ch != ' ' ) // 协议指定是不应包含空格的,但为了兼容没完全按协议编码的情况,也保存空格
			strResult += ch;
	} // for ( ; pURL < pURLEnd; ++pURL )
	return strResult;
}

stdstring URLHelper::decodeURLArg(const char * pURL, const char * pURLEnd)
{
	stdstring strResult;
	return decodeURLArg(strResult, pURL, pURLEnd);
}

stdstring & URLHelper::decodeURLArg(stdstring & strResult, const stdstring & strURL)
{
	decodeURLArg(strResult, strURL.data(), strURL.data() + strURL.size());
	return strResult;
}

RFC_NAMESPACE_END
