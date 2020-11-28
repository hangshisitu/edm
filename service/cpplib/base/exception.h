/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_EXCEPTION_H_201008
#define RFC_EXCEPTION_H_201008

#include "base/string.h"
#include <exception>
#include <errno.h>
#include <stdlib.h>
#include <execinfo.h>

RFC_NAMESPACE_BEGIN

class Exception : public std::exception
{
public:
	explicit Exception(const char * pErrorInfo, bool bTraceStack = false) { setEnv(errno, pErrorInfo, bTraceStack); }
	Exception(int nErrorCode, const char * pErrorInfo, bool bTraceStack = false) { setEnv(nErrorCode, pErrorInfo, bTraceStack); }
	explicit Exception(const stdstring & strErrorInfo, bool bTraceStack = false) { setEnv(errno, strErrorInfo.c_str(), bTraceStack); }
	Exception(int nErrorCode, const stdstring & strErrorInfo, bool bTraceStack = false) { setEnv(nErrorCode, strErrorInfo.c_str(), bTraceStack); }
	virtual ~Exception(void) throw() {}

	static void					backtraceStackInfo(stdstring & strStackTraceInfo)
	{
			static const int g_nStackCount = 10;
			void *auBuf[g_nStackCount];
			int nStackSize = backtrace(auBuf, g_nStackCount);
			char ** ppBackTracesSymbols = backtrace_symbols(auBuf, nStackSize);

			for ( int i = 0; i < nStackSize; ++i )
			{
				strStackTraceInfo += ppBackTracesSymbols[i];
				strStackTraceInfo += ',';
			}
			free (ppBackTracesSymbols);
	}

	int							getErrorCode(void) const { return m_nErrorCode; }
	const stdstring &				getErrorInfo(void) const { return m_strErrorInfo; }
	virtual const char *		what() const throw() {  return m_strErrorInfo.c_str(); }
	const stdstring &				getStackTraceInfo(void) const { return m_strStackTraceInfo; }

protected:
	void						setEnv(int nErrorCode, const char * pErrorInfo, bool bTraceStack)
	{
		m_nErrorCode = nErrorCode;
		m_strErrorInfo = pErrorInfo;
		if ( bTraceStack )
			backtraceStackInfo(m_strStackTraceInfo);
		if ( m_nErrorCode == 0 )	// ÉèÖÃÄÚ²¿´íÎóÂë
			m_nErrorCode = -2;
	}
	int                                             m_nErrorCode;
	stdstring                                         m_strErrorInfo;
	stdstring                                         m_strStackTraceInfo;
};

#define DECLARE_EXCEPTION(exp, expParent) class exp : public expParent \
{	\
public: \
	explicit exp(const char * pErrorInfo, bool bTraceStack = false) : expParent(pErrorInfo, bTraceStack) {}	\
	exp(int nErrorCode, const char * pErrorInfo, bool bTraceStack = false) : expParent(nErrorCode, pErrorInfo, bTraceStack) {}	\
	explicit exp(const stdstring & strErrorInfo, bool bTraceStack = false) : expParent(strErrorInfo, bTraceStack) {}	\
	exp(int nErrorCode, const stdstring & strErrorInfo, bool bTraceStack = false) : expParent(nErrorCode, strErrorInfo, bTraceStack) {}	\
};

RFC_NAMESPACE_END

#endif	//RFC_EXCEPTION_H_201008



