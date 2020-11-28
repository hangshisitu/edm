/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "web/urlargparser.h"

RFC_NAMESPACE_BEGIN

onUnitTest(URLArgParser)
{
	typeURLArgMap mapURLArg;
	URLArgHelper::parseURLArg(mapURLArg, "1=1&2=&=3");
	assertEqual( mapURLArg.size(), static_cast<size_t>(3));
	assertEqual( mapURLArg["1"], stdstring("1"));
	assertEqual( mapURLArg["2"], StringFunc::g_strEmptyString);
	assertEqual( mapURLArg[StringFunc::g_strEmptyString], stdstring("3"));
	assertEqual( mapURLArg["0"], StringFunc::g_strEmptyString);		// not found

	stdstring strResult;
	URLArgHelper::formatURLArg(strResult, mapURLArg);
	assertEqual( strResult, "=3&0=&1=1&2=");
}

RFC_NAMESPACE_END
