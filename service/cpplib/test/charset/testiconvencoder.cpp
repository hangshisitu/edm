/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "charset/iconvencoder.h"

#if (SUPPORT_ICONV_ENCODER != 0)

RFC_NAMESPACE_BEGIN

static void onTestIConvEncoder(const char * lpszSrcCharset, const char * lpszTarCharset, const stdstring & strSrc, const stdstring & strTar)
{
	stdstring strConvertResult;
	IConvEncoder iconvEncoder;
	iconvEncoder.setCharset(lpszSrcCharset, lpszTarCharset);
	assertTest( iconvEncoder.convert(strSrc, strConvertResult) );
	assertEqual(strConvertResult.size(), strTar.size());
	assertEqual(strConvertResult, strTar);

	strConvertResult.clear();
	IConvProxy::convert(lpszSrcCharset, lpszTarCharset, strSrc.data(), strSrc.size(), strConvertResult);
	assertEqual(strConvertResult.size(), strTar.size());
	assertEqual(strConvertResult, strTar);

	strConvertResult.clear();
	iconvEncoder.setCharset(lpszTarCharset, lpszSrcCharset);
	assertTest( iconvEncoder.convert(strTar, strConvertResult) );
	assertEqual(strConvertResult.size(), strSrc.size());
	assertEqual(strConvertResult, strSrc);

	strConvertResult.clear();
	IConvProxy::convert(lpszTarCharset, lpszSrcCharset, strTar.data(), strTar.size(), strConvertResult);
	assertEqual(strConvertResult.size(), strSrc.size());
	assertEqual(strConvertResult, strSrc);
}

onUnitTest(IConvEncoder)
{
	onTestIConvEncoder("gbk", "utf-7", "你好", "+T2BZfQ-");
	onTestIConvEncoder("gb18030", "utf-7", "你kakaak好", "+T2A-kakaak+WX0-");
}

static stdstring onDecodeHtml(const stdstring & strHtml)
{
	return DecodeHtmlHelper::decodeHtml(strHtml.data(), strHtml.size());
}

onUnitTest(DecodeHtmlHelper)
{
	stdstring strHtmlEncode = "&#22914;";
	assertEqual(DecodeHtmlHelper::decodeHtml(strHtmlEncode.data(), strHtmlEncode.size()), stdstring("如"));

	stdstring strUTF8(strHtmlEncode), strGBK;
	DecodeHtmlHelper::decodeHtml(strUTF8, "utf-8");
	IConvProxy::convert("utf-8", "gbk", strUTF8.data(), strUTF8.size(), strGBK);
	assertEqual(strGBK, stdstring("如"));

	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4"), stdstring("=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("&#22914;=EF=BC中文=8C=E4"), stdstring("如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#22914;文=8C=E4"), stdstring("=EF=BC中如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#22914;"), stdstring("=EF=BC中文=8C=E4如"));
	assertEqual(onDecodeHtml("&#x5982;=EF=BC中文=8C=E4"), stdstring("如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#x5982;文=8C=E4"), stdstring("=EF=BC中如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#x5982;"), stdstring("=EF=BC中文=8C=E4如"));
	assertEqual(onDecodeHtml("&#X5982;=EF=BC中文=8C=E4"), stdstring("如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#X5982;文=8C=E4"), stdstring("=EF=BC中如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#X5982;"), stdstring("=EF=BC中文=8C=E4如"));

	assertEqual(onDecodeHtml("&#22914;&#22914;=EF=BC中文=8C=E4"), stdstring("如如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#22914;&#22914;文=8C=E4"), stdstring("=EF=BC中如如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#22914;&#22914;"), stdstring("=EF=BC中文=8C=E4如如"));
	assertEqual(onDecodeHtml("&#x5982;&#x5982;=EF=BC中文=8C=E4"), stdstring("如如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#x5982;&#x5982;文=8C=E4"), stdstring("=EF=BC中如如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#x5982;&#x5982;"), stdstring("=EF=BC中文=8C=E4如如"));
	assertEqual(onDecodeHtml("&#X5982;&#X5982;=EF=BC中文=8C=E4"), stdstring("如如=EF=BC中文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中&#X5982;&#X5982;文=8C=E4"), stdstring("=EF=BC中如如文=8C=E4"));
	assertEqual(onDecodeHtml("=EF=BC中文=8C=E4&#X5982;&#X5982;"), stdstring("=EF=BC中文=8C=E4如如"));

	assertEqual(onDecodeHtml("&#X5982;=EF=BC中&#26524;&#x5982;文=8C=E4&#x5982;&#35813;"), stdstring("如=EF=BC中果如文=8C=E4如该"));
	assertEqual(onDecodeHtml("&#x5982;&#26524;=EF=BC中&#26524;文=8C=E4&#26524;&#35813;"), stdstring("如果=EF=BC中果文=8C=E4果该"));
}

RFC_NAMESPACE_END

#endif	// #if (SUPPORT_ICONV_ENCODER != 0)
