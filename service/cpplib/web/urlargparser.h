/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_URLARGPARSER_H_201008
#define RFC_URLARGPARSER_H_201008

#include "base/string.h"
#include <list>
#include <map>

RFC_NAMESPACE_BEGIN

typedef std::pair<stdstring, stdstring>	typeURLArgPair;
typedef std::list<typeURLArgPair>	typeURLArgList;
typedef std::map<stdstring, stdstring, IgnoreCaseStringCompare>	typeURLArgMap;

class URLArgParser
{
public:
	virtual ~URLArgParser(void) {}
	void				parse(const char * pArg, size_t nSize, char chSplitter = '&', char chAssigner = '=');
	void				parse(const stdstring & strArg, char chSplitter = '&', char chAssigner = '=');

protected:
	virtual	void		onURLArg(const char * pKeyBegin, const char * pKeyEnd, const char * pValueBegin, const char * pValueEnd) = 0;
};

class IgnoreCaseURLArgParser : public URLArgParser
{
public:
	explicit IgnoreCaseURLArgParser(typeURLArgMap & mapURLArg) : m_mapURLArg(mapURLArg) {}

protected:
	virtual	void		onURLArg(const char * pKeyBegin, const char * pKeyEnd, const char * pValueBegin, const char * pValueEnd);

private:
	typeURLArgMap &	m_mapURLArg;
};

class URLArgHelper
{
public:
	static void		parseURLArg(typeURLArgMap & mapURLArg, const char * pArg, size_t nSize, char chSplitter = '&', char chAssigner = '=');
	static void		parseURLArg(typeURLArgMap & mapURLArg, const stdstring & strArg, char chSplitter = '&', char chAssigner = '=');

	static void		formatURLArg(stdstring & strResult, const typeURLArgMap & mapURLArg, char chSplitter = '&', char chAssigner = '=');
};

RFC_NAMESPACE_END

#endif	//RFC_URLARGPARSER_H_201008
