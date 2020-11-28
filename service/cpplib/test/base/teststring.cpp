/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/string.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

onUnitTest(StringFunc)
{
	assertEqual( StringFunc::toupper('&'), static_cast<int>('&') );
	assertEqual( StringFunc::toupper('0'), static_cast<int>('0') );
	assertEqual( StringFunc::toupper('9'), static_cast<int>('9') );
	assertEqual( StringFunc::toupper('a'), static_cast<int>('A') );
	assertEqual( StringFunc::toupper('Z'), static_cast<int>('Z') );
	assertEqual( StringFunc::toupper('u'), static_cast<int>('U') );

	assertEqual( StringFunc::tolower('@'), static_cast<int>('@') );
	assertEqual( StringFunc::tolower('5'), static_cast<int>('5') );
	assertEqual( StringFunc::tolower('7'), static_cast<int>('7') );
	assertEqual( StringFunc::tolower('B'), static_cast<int>('b') );
	assertEqual( StringFunc::tolower('f'), static_cast<int>('f') );
	assertEqual( StringFunc::tolower('P'), static_cast<int>('p') );

	stdstring strVal = "#$^&8HfhdsY%^&89";
	assertEqual( StringFunc::toUpper(strVal), stdstring("#$^&8HFHDSY%^&89") );
	assertEqual( StringFunc::toLower(strVal), stdstring("#$^&8hfhdsy%^&89") );

	assertEqual( StringFunc::getUpperHexChar(0), '0' );
	assertEqual( StringFunc::getUpperHexChar(10), 'A' );
	assertEqual( StringFunc::getUpperHexChar(15), 'F' );
	assertEqual( StringFunc::getUpperHexChar(-1), RFC_CHAR_NULL );
	assertEqual( StringFunc::getUpperHexChar(16), RFC_CHAR_NULL );

	assertEqual( StringFunc::getLowerHexChar(0), '0' );
	assertEqual( StringFunc::getLowerHexChar(10), 'a' );
	assertEqual( StringFunc::getLowerHexChar(15), 'f' );
	assertEqual( StringFunc::getLowerHexChar(-1), RFC_CHAR_NULL );
	assertEqual( StringFunc::getLowerHexChar(16), RFC_CHAR_NULL );

	assertEqual( StringFunc::length("1234567890"), static_cast<size_t>(10) );
	assertEqual( StringFunc::length("1234567890abcdefghijklmnopqrstuvwxyz"), static_cast<size_t>(36) );

	assertEqual( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", "#$^&8hfhdsy%^&89"), 0 );
	assertEqual( StringFunc::strCasecmp("#$^&8HFHds", 10, "#$^&8hfhds", 10), 0 );
	assertEqual( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), "#$^&8hfhdsy%^&89"), 0 );
	assertEqual( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertEqual( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertEqual( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), 2, 6, stdstring("#$XXX^&8hfhdsy%^&89"), 5, 6), 0 );

	assertGreater( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", "#$^&8hfhdsy%^&8"), 0 );
	assertGreater( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", "#$^&8hfhdsy%^&88aaaaa"), 0 );
	assertGreater( StringFunc::strCasecmp("#$^&8HFHds", 10, "#$^&8hfhds", 9), 0 );
	assertGreater( StringFunc::strCasecmp("#$^&8HFHds", 10, "#$^&8hfhdaa", 11), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), "#$^&8hfhdsy%^&8"), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), "#$^&8hfhdsy%^&88888888"), 0 );
	assertGreater( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", stdstring("#$^&8hfhdsy%^&8")), 0 );
	assertGreater( StringFunc::strCasecmp("#$^&8HFHDSY%^&89", stdstring("#$^&8hfhdsy%^&888888")), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), stdstring("#$^&8hfhdsy%^&8")), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), stdstring("#$^&8hfhdsy%^&88888")), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), 2, 6, stdstring("#$XXX^&8hfhdsy%^&89"), 5, 5), 0 );
	assertGreater( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), 2, 6, stdstring("#$XXX^&8hfaaaaaadsy%^&89"), 5, 10), 0 );

	assertLess( StringFunc::strCasecmp("#$^&8HFHDSY%^&8", "#$^&8hfhdsy%^&89"), 0 );
	assertLess( StringFunc::strCasecmp("#$^&8HFHDSY%^&80000", "#$^&8hfhdsy%^&89"), 0 );
	assertLess( StringFunc::strCasecmp("#$^&8HFHds", 9, "#$^&8hfhds", 10), 0 );
	assertLess( StringFunc::strCasecmp("#$^&8HFHdaaaaaa", 12, "#$^&8hfhds", 10), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&8"), "#$^&8hfhdsy%^&89"), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&8888888888"), "#$^&8hfhdsy%^&89"), 0 );
	assertLess( StringFunc::strCasecmp("#$^&8HFHDSY%^&8", stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertLess( StringFunc::strCasecmp("#$^&8HFHDSY%^&8888", stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&8"), stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&8000000000"), stdstring("#$^&8hfhdsy%^&89")), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFHDSY%^&89"), 2, 5, stdstring("#$XXX^&8hfhdsy%^&89"), 5, 6), 0 );
	assertLess( StringFunc::strCasecmp(stdstring("#$^&8HFaaaaaaaaaHDSY%^&89"), 2, 10, stdstring("#$XXX^&8hfHdsy%^&89"), 5, 6), 0 );

	assertEqual( StringFunc::strnCasecmp("#$^&8HFHDSY%^&89", "#$^&8hfhdsy%^&89", 10), 0 );
	assertEqual( StringFunc::strnCasecmp(stdstring("KO#$^&8HFHDSY%^&89"), "ko#$^&8hfhdsy%^&89", 10), 0 );
	assertEqual( StringFunc::strnCasecmp("ok#$^&8HFHDSY%^&89", stdstring("OK#$^&8hfhdsy%^&89"), 10), 0 );
	assertEqual( StringFunc::strnCasecmp(stdstring("ok#$^&8HFHDSY%^&89"), stdstring("OK#$^&8hfhdsy%^&89"), 10), 0 );

	assertGreater( StringFunc::strnCasecmp("#$^&8KOHFHDSY%^&89", "#$^&8kAhfhdsy%^&89", 10), 0 );
	assertGreater( StringFunc::strnCasecmp("#$^&8KO", "#$^&8kAhfhdsy%^&89", 10), 0 );
	assertGreater( StringFunc::strnCasecmp(stdstring("#$^&8KOHFHDSY%^&89"), "#$^&8kAhfhdsy%^&89", 10), 0 );
	assertGreater( StringFunc::strnCasecmp(stdstring("#$^&8KO"), "#$^&8kAhfhdsy%^&89", 10), 0 );
	assertGreater( StringFunc::strnCasecmp("#$^&8KOHFHDSY%^&89", stdstring("#$^&8kAhfhdsy%^&89"), 10), 0 );
	assertGreater( StringFunc::strnCasecmp("#$^&8KO", stdstring("#$^&8kAhfhdsy%^&89"), 10), 0 );
	assertGreater( StringFunc::strnCasecmp(stdstring("#$^&8KOHFHDSY%^&89"), stdstring("#$^&8kAhfhdsy%^&89"), 10), 0 );
	assertGreater( StringFunc::strnCasecmp(stdstring("#$^&8KO"), stdstring("#$^&8kAhfhdsy%^&89"), 10), 0 );

	assertLess( StringFunc::strnCasecmp("#$^&8KaHFHDSY%^&89", "#$^&8kOhfhdsy%^&89", 10), 0 );
	assertLess( StringFunc::strnCasecmp("#$^&8Ka", "#$^&8kOhfhdsy%^&89", 10), 0 );
	assertLess( StringFunc::strnCasecmp(stdstring("#$^&8KaHFHDSY%^&89"), "#$^&8kohfhdsy%^&89", 10), 0 );
	assertLess( StringFunc::strnCasecmp(stdstring("#$^&8Ka"), "#$^&8kohfhdsy%^&89", 10), 0 );
	assertLess( StringFunc::strnCasecmp("#$^&8KaHFHDSY%^&89", stdstring("#$^&8kohfhdsy%^&89"), 10), 0 );
	assertLess( StringFunc::strnCasecmp("#$^&8Ka", stdstring("#$^&8kohfhdsy%^&89"), 10), 0 );
	assertLess( StringFunc::strnCasecmp(stdstring("#$^&8KAHFHDSY%^&89"), stdstring("#$^&8kohfhdsy%^&89"), 10), 0 );
	assertLess( StringFunc::strnCasecmp(stdstring("#$^&8KA"), stdstring("#$^&8kohfhdsy%^&89"), 10), 0 );

	char p[11];
	StringFunc::strnCpy(p, "12345", 10);
	assertEqual(stdstring(p), stdstring("12345") );
	StringFunc::strnCpy(p, "1234567890", 10);
	assertEqual( stdstring(p), stdstring("1234567890") );

	strVal.clear();
	assertEqual( StringFunc::reverseAppend(strVal, static_cast<const char *>(NULL), static_cast<size_t>(0)), StringFunc::g_strEmptyString );
	assertEqual( StringFunc::reverseAppend(strVal, "1234567890", 0), StringFunc::g_strEmptyString );
	assertEqual( StringFunc::reverseAppend(strVal, "1234567890", 10), stdstring("0987654321") );
	strVal.clear();
	assertEqual( StringFunc::reverseAppend(strVal, StringFunc::g_strEmptyString), StringFunc::g_strEmptyString );
	assertEqual( StringFunc::reverseAppend(strVal, stdstring("1234567890")), stdstring("0987654321") );

	assertTest( StringFunc::checkPrefix(stdstring("file.txt"), stdstring("file")) );
	assertTest( !StringFunc::checkPrefix(stdstring("file.txt"), stdstring("File")) );
	assertTest( StringFunc::checkPrefix(stdstring("file.txt"), stdstring("File"), false) );

	assertTest( StringFunc::checkPostfix(stdstring("file.txt"), stdstring(".txt")) );
	assertTest( !StringFunc::checkPostfix(stdstring("file.txt"), stdstring(".Txt")) );
	assertTest( !StringFunc::checkPostfix(stdstring("有效管理经销商的六大系统.xls"), stdstring(".txt")) );
	assertTest( !StringFunc::checkPostfix(stdstring("有效管理经销商的六大系统.xls"), stdstring(".htm")) );
	assertTest( !StringFunc::checkPostfix(stdstring("有效管理经销商的六大系统.xls"), stdstring(".html")) );
	assertTest( StringFunc::checkPostfix(stdstring("file.txt"), stdstring(".TXt"), false) );

	strVal = " \t\r\n1234567890 \t\r\n";
	assertEqual( StringFunc::trimLeft(strVal, StringFunc::g_strSpaceString), stdstring("1234567890 \t\r\n") );
	strVal = " \t\r\n1234567890 \t\r\n";
	assertEqual( StringFunc::trimRight(strVal, StringFunc::g_strSpaceString), stdstring(" \t\r\n1234567890") );
	strVal = " \t\r\n1234567890 \t\r\n";
	assertEqual( StringFunc::trim(strVal, StringFunc::g_strSpaceString), stdstring("1234567890") );

	assertEqual( StringFunc::stringToInt<rfc_int_32>("+123456789"), 123456789 );
	assertEqual( StringFunc::stringToInt<rfc_int_32>("234567890"), 234567890 );
	assertEqual( StringFunc::stringToInt<rfc_int_32>("-987654321"), -987654321 );
	assertEqual( StringFunc::stringToInt<rfc_int_64>("+123456789"), 123456789 );
	assertEqual( StringFunc::stringToInt<rfc_int_64>("234567890"), 234567890 );
	assertEqual( StringFunc::stringToInt<rfc_int_64>("-987654321"), -987654321 );

	assertEqual( StringFunc::stringToInt<rfc_int_32>("7FAc", 4, 16), 32684 );
	assertEqual( StringFunc::stringToInt<rfc_int_32>("bcD2F8A", 7, 16), 197996426 );

	assertEqual( StringFunc::intToString(123456789), stdstring("123456789") );
	assertEqual( StringFunc::intToString(234567890), stdstring("234567890") );
	assertEqual( StringFunc::intToString(-987654321), stdstring("-987654321") );
	assertEqual( StringFunc::intToString(32684, 16), stdstring("7FAC") );
	assertEqual( StringFunc::intToString(197996426, 16), stdstring("BCD2F8A") );

	assertEqual( StringFunc::convertSizeUnitToByte("983"), static_cast<rfc_int_64>(983) );
	assertEqual( StringFunc::convertSizeUnitToByte("1K856"), static_cast<rfc_int_64>(1880) );
	assertEqual( StringFunc::convertSizeUnitToByte("12k279"), static_cast<rfc_int_64>(12567) );
	assertEqual( StringFunc::convertSizeUnitToByte("1m"), static_cast<rfc_int_64>(1048576) );
	assertEqual( StringFunc::convertSizeUnitToByte("1M243"), static_cast<rfc_int_64>(1048819) );
	assertEqual( StringFunc::convertSizeUnitToByte("1m8k775"), static_cast<rfc_int_64>(1057543) );
	assertEqual( StringFunc::convertSizeUnitToByte("1g"), static_cast<rfc_int_64>(1073741824) );
	assertEqual( StringFunc::convertSizeUnitToByte("1G823"), static_cast<rfc_int_64>(1073742647) );
	assertEqual( StringFunc::convertSizeUnitToByte("1G3M823"), static_cast<rfc_int_64>(1076888375) );
	assertEqual( StringFunc::convertSizeUnitToByte("1G5m8K823"), static_cast<rfc_int_64>(1078993719) );
	assertEqual( StringFunc::convertSizeUnitToByte("2G"), static_cast<rfc_int_64>(2147483648LL) );
}

onUnitTest(FindSubString)
{
	stdstring strKey("kaKKk中国人卡aJFDSKLJFKJfdsnjjfsdub中华佛教的审计法"), strValue("阿娇附近的21ABcd34卡积分的");
	FindSubString strFinder;		//empty
	stdstring strContent(strValue);
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), static_cast<size_t>(0) );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), static_cast<size_t>(0) );
	StringFunc::toUpper(strContent);
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), static_cast<size_t>(0) );
	StringFunc::toLower(strContent);
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), static_cast<size_t>(0) );

	strFinder.compileKeyWord(strKey.data(), strKey.size());
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), stdstring::npos );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), stdstring::npos );
	StringFunc::toUpper(strContent);
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), stdstring::npos );
	StringFunc::toLower(strContent);
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), stdstring::npos );

	strContent = strValue + strKey + strValue;
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), strValue.size() );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), strValue.size() );
	StringFunc::toUpper(strContent);
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), stdstring::npos );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), strValue.size() );
	StringFunc::toLower(strContent);
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), stdstring::npos );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), strValue.size() );

	strKey = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	strContent = "1" + strKey + "<DIV><FONT face=Verdana size=2>aaaaaaaaaaaaaaaaaaaaaaaaa</FONT></DIV>";
	strFinder.compileKeyWord(strKey.data(), strKey.size());
	size_t nPos = strContent.find(strKey);
	assertNotEqual(strContent.find(strKey), stdstring::npos);
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), nPos );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), nPos );

	StringFunc::toLower(strContent);
	assertEqual( strFinder.findCaseSensitive(strContent.data(), strContent.size()), stdstring::npos );
	assertEqual( strFinder.findIgnoreCase(strContent.data(), strContent.size()), nPos );
}

RFC_NAMESPACE_END
