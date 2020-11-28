/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_URLHELPER_H_201008
#define RFC_URLHELPER_H_201008

#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

class URLVisitor
{
public:
	virtual ~URLVisitor(void) {}
	virtual	void		onURL(const char * pURL, const char * pURLEnd) = 0;
};

class URLHelper
{
public:
	static bool			isValidURLChar(unsigned char ch);
	static bool			isValidURLChar(char ch) { return isValidURLChar(static_cast<unsigned char>(ch)); }
	//static bool		isReservedURLChar(unsigned char ch);
	//static bool		isReservedURLChar(char ch) { return isReservedURLChar(static_cast<unsigned char>(ch)); }
	static bool			isValidURLEncodeChar(char ch);
	static bool			isURLEscapeChar(unsigned char ch);

	static bool			checkURLPrefix(const stdstring & strURL);

	static void			getURLWithPrefix(const char * pText, size_t nTextSize, const char * pURLPrefix, size_t nPrefixLen, URLVisitor & urlVisitor);

	static void			getURLWithPostfix(const char * pText, size_t nTextSize, const char * pURLPostfix, size_t nPostfixLen, URLVisitor & urlVisitor);

	static bool			getDomainFromURL(const char * pURL, size_t nURLLen, stdstring & strDomain, stdstring & strSubDomain);
	static bool			getDomainFromURL(const stdstring & strURL, stdstring & strDomain, stdstring & strSubDomain);

	static stdstring &	encodeURLArg(stdstring & strResult, const char * pURL, const char * pURLEnd);
	static stdstring	encodeURLArg(const char * pURL, const char * pURLEnd);
	static stdstring &	encodeURLArg(stdstring & strResult, const stdstring & strURL);

	static stdstring &	decodeURLArg(stdstring & strResult, const char * pURL, const char * pURLEnd);
	static stdstring	decodeURLArg(const char * pURL, const char * pURLEnd);
	static stdstring &	decodeURLArg(stdstring & strResult, const stdstring & strURL);
};

RFC_NAMESPACE_END

#endif	//RFC_URLHELPER_H_201008
