/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/exception.h"
#include "base/autoarray.h"
#include <stdlib.h>
#include <execinfo.h>

RFC_NAMESPACE_BEGIN

#if 0

bool Exception::backtraceStackInfo(int nTraceStackCount, stdstring & strStackTraceInfo)
{
	if ( nTraceStackCount <= 0 )
		return false;

	AutoArray<char *> auBuffer( new char * [nTraceStackCount] );
	int nStackSize = backtrace(reinterpret_cast<void**>( auBuffer.get() ), nTraceStackCount);
	char ** ppBackTracesSymbols = backtrace_symbols(reinterpret_cast<void**>( auBuffer.get() ), nStackSize);
	if ( nStackSize <= 0 || ppBackTracesSymbols == NULL )
		return false;

	for ( int i = 0; i < nStackSize; ++i )
	{
		strStackTraceInfo += ppBackTracesSymbols[i];
		strStackTraceInfo += ',';
	}

	free (ppBackTracesSymbols);
	return true;
}

void Exception::setEnv(int nErrorCode, const char * pErrorInfo, int nTraceStackCount)
{
	m_nErrorCode = nErrorCode;
	m_strErrorInfo = pErrorInfo;
	backtraceStackInfo(nTraceStackCount, m_strStackTraceInfo);
}

#endif

RFC_NAMESPACE_END


