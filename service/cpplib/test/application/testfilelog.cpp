/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "application/filelog.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

onUnitTest(FileLog)
{
	stdstring strLogPath("/tmp/");
	const char * lpszEnv = getenv("HOME");
	if ( lpszEnv != NULL )
		strLogPath = lpszEnv;
	FileSystem::toPathName(strLogPath, strLogPath);
	strLogPath += "logs/testfilelog.log";

	FileLog aFileLog;
	assertTest( aFileLog.init(strLogPath, 8192) );
	for ( int i = 0; i < 5; ++i )
	{
		for ( int j = 0; j < 128; ++j )
		{
			stdstring strLog = FormatString("% FileSystem::toPathName(strLogPath, strLogPath); \r\n")
				.arg(StringFunc::intToString(i * 128 + j, 10, true, 3, '0')).str();
			aFileLog.writeLog( strLog );
		}
	} // for ( int i = 0; i < 5; ++i )
}

RFC_NAMESPACE_END
