/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "web/htmlparser.h"
#include "system/filesystem.h"

RFC_NAMESPACE_BEGIN

onUnitTest(HtmlParser)
{
	stdstring strFullPath = "/home/wengshanjin/test.htm";
	stdstring strLoadContent;
	assertTest( FileSystem::loadFileContent(strFullPath, strLoadContent) );

	HtmlTextParser htmlParser;
	htmlParser.parseHtml(strLoadContent.data(), strLoadContent.size());
	std::cout << htmlParser.getFullText() << std::endl;
}

RFC_NAMESPACE_END
