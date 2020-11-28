/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/iprangemap.h"

RFC_NAMESPACE_BEGIN

bool IPRangeParser::parseIPRange(const stdstring & strOneRange, typeDefaultINetIP & ipBegin, typeDefaultINetIP & ipEnd)
{
	stdstring strPart1, strPart2;
	size_t nPos = strOneRange.find('-');
	if ( nPos != stdstring::npos )		// 192.168.1.100 - 192.168.1.200
	{
		strPart1 = strOneRange.substr(0, nPos);
		strPart2 = strOneRange.substr(nPos + 1);
		GlobalFunc::removeSpace(strPart1);
		GlobalFunc::removeSpace(strPart2);
		if ( !ipBegin.setIPAddress(strPart1.c_str()) || !ipEnd.setIPAddress(strPart2.c_str()) )
			return false;
		if ( ipBegin > ipEnd )
			return false;
	}
	else if ( (nPos = strOneRange.find('/')) != stdstring::npos )	// 192.168.1.100/24
	{
		strPart1 = strOneRange.substr(0, nPos);
		strPart2 = strOneRange.substr(nPos + 1);
		GlobalFunc::removeSpace(strPart1);
		GlobalFunc::removeSpace(strPart2);
		size_t nBits = StringFunc::stringToInt<size_t>(strPart2.data(), strPart2.size());
		if ( !ipBegin.setIPAddress(strPart1.c_str()) || nBits > 32 )
			return false;
		ipEnd = ipBegin;
		ipBegin.setSubnetMask( nBits, false );
		ipEnd.setSubnetMask( nBits, true );
	}
	else	// single ip 192.168.1.100
	{
		strPart1 = strOneRange;
		GlobalFunc::removeSpace(strPart1);
		if ( !ipBegin.setIPAddress(strPart1.c_str()) )
			return false;
		ipEnd = ipBegin;
	}
	return true;
}

RFC_NAMESPACE_END
