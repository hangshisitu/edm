/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "web/urlargparser.h"
#include "web/urlhelper.h"

RFC_NAMESPACE_BEGIN

void URLArgParser::parse(const char * pArg, size_t nSize, char chSplitter, char chAssigner)
{
	const char * pKeyBegin = pArg;
	const char * pKeyEnd = pArg;
	const char * pValueBegin = pArg;
	const char * pValueEnd = pArg;
	for ( const char * pEnd = pArg + nSize; pArg < pEnd; ++pArg )
	{
		if ( *pArg == chSplitter )
		{
			pValueEnd = pArg;
			if ( pKeyBegin < pKeyEnd || pValueBegin < pValueEnd )
				onURLArg(pKeyBegin, pKeyEnd, pValueBegin, pValueEnd);
			pKeyBegin = pKeyEnd = pValueBegin = pValueEnd = pArg + 1;
		}
		else if ( *pArg == chAssigner )
		{
			pKeyEnd = pArg;
			pValueBegin = pArg + 1;
			pValueEnd = pEnd;
		}
	} // for ( const char * pEnd = pArg + nSize; pArg < pEnd; ++pArg )

	if ( pKeyBegin < pKeyEnd || pValueBegin < pValueEnd )
		onURLArg(pKeyBegin, pKeyEnd, pValueBegin, pValueEnd);
}

void URLArgParser::parse(const stdstring & strArg, char chSplitter, char chAssigner)
{
	return parse(strArg.data(), strArg.size(), chSplitter, chAssigner);
}

void IgnoreCaseURLArgParser::onURLArg(const char * pKeyBegin, const char * pKeyEnd, const char * pValueBegin, const char * pValueEnd)
{
	stdstring strKey, strValue;
	URLHelper::decodeURLArg(strKey, pKeyBegin, pKeyEnd);
	URLHelper::decodeURLArg(strValue, pValueBegin, pValueEnd);
	m_mapURLArg[strKey].swap(strValue);
}

void URLArgHelper::parseURLArg(typeURLArgMap & mapURLArg, const char * pArg, size_t nSize, char chSplitter, char chAssigner)
{
	IgnoreCaseURLArgParser parser(mapURLArg);
	parser.parse(pArg, nSize, chSplitter, chAssigner);
}

void URLArgHelper::parseURLArg(typeURLArgMap & mapURLArg, const stdstring & strArg, char chSplitter, char chAssigner)
{
	parseURLArg(mapURLArg, strArg.data(), strArg.size(), chSplitter, chAssigner);
}

void URLArgHelper::formatURLArg(stdstring & strResult, const typeURLArgMap & mapURLArg, char chSplitter, char chAssigner)
{
	typeURLArgMap::const_iterator it, itEnd = mapURLArg.end();
	for ( it = mapURLArg.begin(); it != itEnd; ++it )
	{
		if ( !strResult.empty() )
			strResult += chSplitter;
		URLHelper::encodeURLArg(strResult, it->first);
		strResult += chAssigner;
		URLHelper::encodeURLArg(strResult, it->second);
	} // for ( it = mapURLArg.begin(); it != itEnd; ++it )
}

RFC_NAMESPACE_END
