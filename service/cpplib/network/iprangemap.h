/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_IPRANGEMAP_H_201008
#define RFC_IPRANGEMAP_H_201008

#include "network/internetaddr.h"
#include "base/rangestorage.h"
#include "base/stringtokenizer.h"

RFC_NAMESPACE_BEGIN

class IPRangeParser
{
public:
	static bool		parseIPRange(const stdstring & strOneRange, typeDefaultINetIP & ipBegin, typeDefaultINetIP & ipEnd);
};

template<typename typeData>
class IPRangeMap : public RangeDataMap<typeDefaultINetIP::typeIPInteger, typeData>
{
public:
	typedef RangeDataMap<typeDefaultINetIP::typeIPInteger, typeData> typeParent;
	
	bool			loadIPRange(const stdstring & strIPRange, const typeData & nData);
	bool			loadIPRange(const stdstring & strIPRange) { return loadIPRange(strIPRange, typeData()); }
};

template<typename typeData>
bool IPRangeMap<typeData>::loadIPRange(const stdstring & strIPRange, const typeData & nData)
{
	typeParent::clear();

	typeParent::setNegative( !strIPRange.empty() && strIPRange[0] == '!' );
	size_t nSkipNegative = ( typeParent::isNegative() ? 1 : 0 );
	StringTokenizer<stdstring> tokenRange(strIPRange.data() + nSkipNegative, strIPRange.size() - nSkipNegative);
	stdstring strOneRange;
	typeDefaultINetIP ipBegin, ipEnd;
	while ( tokenRange.getNext(strOneRange, ";,") )
	{
		GlobalFunc::removeSpace(strOneRange);
		if ( strOneRange.empty() )
			continue;
		if ( !IPRangeParser::parseIPRange(strOneRange, ipBegin, ipEnd) )
			return false;
		typeParent::updateOrInsert(ipBegin.getIPInteger(), ipEnd.getIPInteger(), true);
	} // while ( tokenRange.getNext(strOneRange, ";,") )

	return true;
}

typedef IPRangeMap<bool>	typeIPRangeChecker;

RFC_NAMESPACE_END

#endif	//RFC_IPRANGEMAP_H_201008
