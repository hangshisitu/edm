/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

onUnitTest(FormatStream)
{
	assertEqual(FormatString("123%abcd").arg(456789).str(), stdstring("123456789abcd"));
	assertEqual(FormatString("123%abcd%ABC").arg(456789).str(), stdstring("123456789abcd"));
	assertEqual(FormatString("123%abcd%ABC").arg(456789).flushStream().str(), stdstring("123456789abcdABC"));
	assertEqual(FormatString("123%abcd").arg(456789).arg("ABC").str(), stdstring("123456789abcdABC"));
	assertEqual(FormatString("123%%").arg('%').arg(80).str(), stdstring("123%80"));

	stdostringstream strStream;
	FormatOutputStream fmtStream(strStream.rdbuf());
	std::string strResult;
	fmtStream.format("123%abcd%ABC").arg(456789);
	strStream.str().swap(strResult);
	assertEqual(strResult, stdstring("123456789abcd"));
}

RFC_NAMESPACE_END
