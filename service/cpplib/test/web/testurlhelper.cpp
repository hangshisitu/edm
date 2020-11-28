/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "web/urlhelper.h"

RFC_NAMESPACE_BEGIN

onUnitTest(URLHelper)
{
	stdstring strSrcURL = "url ²Î Êý,-.";
	stdstring strExpectEncodeURL = "url+%B2%CE+%CA%FD%2C-.";
	assertEqual( URLHelper::encodeURLArg(strSrcURL.data(), strSrcURL.data() + strSrcURL.size()), strExpectEncodeURL );

	stdstring strDomain, strSubDomain;
	assertTest( !URLHelper::getDomainFromURL("http://abc:80/k=v", strDomain, strSubDomain) );
	assertTest( URLHelper::getDomainFromURL("http://www.example1.com:8080/aaa", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("example1.com"));
	assertEqual(strSubDomain, stdstring("www.example1.com"));

	assertTest( URLHelper::getDomainFromURL("http://.www.example2.com", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("example2.com"));
	assertEqual(strSubDomain, stdstring("www.example2.com"));

	assertTest( URLHelper::getDomainFromURL("ftp://user:password@ftp.example3.com/", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("example3.com"));
	assertEqual(strSubDomain, stdstring("ftp.example3.com"));

	assertTest( URLHelper::getDomainFromURL("http://redirect.example4.com?http://page.china.alibaba.com/shtml/", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("example4.com"));
	assertEqual(strSubDomain, stdstring("redirect.example4.com"));

	assertTest( URLHelper::getDomainFromURL("http://.www..example5.com", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("example5.com"));
	assertEqual(strSubDomain, stdstring("www.example5.com"));

	assertTest( URLHelper::getDomainFromURL("http://www.fire-dept.ci.los-angeles.ca.us/policies/docs/rfc_1480.txt", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("fire-dept.ci.los-angeles.ca.us"));
	assertEqual(strSubDomain, stdstring("www.fire-dept.ci.los-angeles.ca.us"));

	assertTest( URLHelper::getDomainFromURL("http://www.abc.demon.co.uk:8000/policies/docs/rfc_1480.txt", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("abc.demon.co.uk"));
	assertEqual(strSubDomain, stdstring("www.abc.demon.co.uk"));

	assertTest( URLHelper::getDomainFromURL("http://www.abc.com.cn", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("abc.com.cn"));
	assertEqual(strSubDomain, stdstring("www.abc.com.cn"));

	assertTest( URLHelper::getDomainFromURL("http://com.cn", strDomain, strSubDomain) );
	assertEqual(strDomain, stdstring("com.cn"));
	assertEqual(strSubDomain, stdstring("com.cn"));
}

RFC_NAMESPACE_END
