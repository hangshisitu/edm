/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/globalfunc.h"
#include <stdlib.h>

RFC_NAMESPACE_BEGIN

const rfc_int_32 GlobalConstant::g_nMaxInt32 = 0x7fffffff;
const rfc_int_32 GlobalConstant::g_nMinInt32 = -0x80000000;
const rfc_int_64 GlobalConstant::g_nMaxInt64 = 0x7fffffffffffffffLL;
const rfc_int_64 GlobalConstant::g_nMinInt64 = -0x8000000000000000LL;

const rfc_uint_32 GlobalConstant::g_uMaxUInt32 = static_cast<rfc_uint_32>( -1 );
const rfc_uint_64 GlobalConstant::g_uMaxUInt64 = static_cast<rfc_uint_64>( -1 );
const size_t	GlobalConstant::g_uMaxSize = static_cast<size_t>( -1 );

const char * CompareFunc::getCompareSymbol(CompareFunc::CompareOP nCompareOP)
{
	switch( nCompareOP )
	{
	case COMPARE_OP_EQAUL:
		return " == ";
		break;
	case COMPARE_OP_NOT_EQAUL:
		return " != ";
		break;
	case COMPARE_OP_GREATER_THAN:
		return " > ";
		break;
	case COMPARE_OP_GREATER_OR_EQAUL:
		return " >= ";
		break;
	case COMPARE_OP_LESS_THAN:
		return " < ";
		break;
	case COMPARE_OP_LESS_OR_EQAUL:
		return " <= ";
		break;
	default:
		return " EOP ";
	} //switch( nCompareOP )
	return " EOP ";
}

bool CompareFunc::compareOK(const double& arg1, const double& arg2, CompareFunc::CompareOP nCompareOP)
{
	static const double g_dPrecision = 0.00001;
	double d = arg1 - arg2;
	if ( nCompareOP == COMPARE_OP_EQAUL )
		return (d <= g_dPrecision && d >= -g_dPrecision);
	else if ( nCompareOP == COMPARE_OP_NOT_EQAUL )
		return (d > g_dPrecision || d < -g_dPrecision);
	else if ( nCompareOP == COMPARE_OP_GREATER_THAN )
		return (d > g_dPrecision);
	else if ( nCompareOP == COMPARE_OP_GREATER_OR_EQAUL )
		return (d >= -g_dPrecision);
	else if ( nCompareOP == COMPARE_OP_LESS_THAN )
		return (d < -g_dPrecision);
	else if ( nCompareOP == COMPARE_OP_LESS_OR_EQAUL )
		return (d <= g_dPrecision);
	return false;
}

//////////////////////////////////////////////////////////////////////////

rfc_uint_32 GlobalFunc::threadSafeRand(rfc_uint_32 * pRandomSeed)
{
	return static_cast<rfc_uint_32>( rand_r(pRandomSeed) );
}

rfc_uint_8 GlobalFunc::calcBinaryBit1Count(rfc_uint_64 n)
{
	rfc_uint_8 nCount = 0;
	for ( nCount = 0; n > 0; n &= n - 1 )
		++nCount;
	return nCount;
}

bool GlobalFunc::getIPV4Integer(const char * pHostAddr, size_t nHostAddrSize, rfc_uint_32 & nIPInteger)
{
	if ( pHostAddr == NULL || *pHostAddr == RFC_CHAR_NULL || nHostAddrSize == 0 )
		return false;

	nIPInteger = 0;
	int	nPartCount = 1, nPartNumber = -1;
	for ( size_t i = 0; i < nHostAddrSize; ++i )
	{
		char ch = pHostAddr[i];
		if ( ::isdigit(ch) )
		{
			if ( nPartNumber == 0 )
				return false;
			nPartNumber = ( nPartNumber == -1 ) ? ( ch - '0' ) : ( nPartNumber * 10 + ch - '0' );
		}
		else if ( ch == '.' )
		{
			if ( nPartNumber < 0 || nPartNumber > 255 )
				return false;
			nIPInteger = ( nIPInteger << 8 ) + static_cast<rfc_uint_32>( nPartNumber );
			nPartNumber = -1;
			++nPartCount;
		}
		else
			return false;
	} // for ( size_t i = 0; i < nHostAddrSize; ++i )

	nIPInteger = ( nIPInteger << 8 ) + static_cast<rfc_uint_32>( nPartNumber );
	return ( nPartCount == 4 && nPartNumber >= 0 && nPartNumber <= 255 );
}

bool GlobalFunc::getIPV4Integer(const char * lpszHostAddr, rfc_uint_32 & nIPInteger)
{
	if ( lpszHostAddr == NULL || *lpszHostAddr == RFC_CHAR_NULL || *lpszHostAddr == '.' )
		return false;
	return getIPV4Integer(lpszHostAddr, StringFunc::length(lpszHostAddr), nIPInteger);
}

bool GlobalFunc::getIPV4Integer(const stdstring & strHostAddr, rfc_uint_32 & nIPInteger)
{
	return getIPV4Integer(strHostAddr.data(), strHostAddr.size(), nIPInteger);
}

bool GlobalFunc::isValidIPV4Addr(const char * pHostAddr, size_t nHostAddrSize)
{
	rfc_uint_32 nIPInteger = 0;
	return getIPV4Integer(pHostAddr, nHostAddrSize, nIPInteger);
}

bool GlobalFunc::isValidIPV4Addr(const char * lpszHostAddr)
{
	rfc_uint_32 nIPInteger = 0;
	return getIPV4Integer(lpszHostAddr, nIPInteger);
}

bool GlobalFunc::isValidHostName(const char * pHostName, size_t nHostNameSize)
{
	static size_t const g_nValidHostNameMaxLen		= 255;		//RFC 1035
	static size_t const g_nValidLabelMaxLen			= 63;		//RFC 1035

	if ( pHostName == NULL || *pHostName == RFC_CHAR_NULL || nHostNameSize == 0 || nHostNameSize > g_nValidHostNameMaxLen )
		return false;

	bool bDashChar = false;		// 不能以破折号'-'开始或结束,'-'后不能跟'.'
	size_t nLabelSize = 0, nLabelCount = 0;
	for ( size_t i = 0; i < nHostNameSize; ++i )
	{
		char ch = pHostName[i];
		if ( ::isalnum(ch) || ch == '_' || ch == '-' )
		{
			bDashChar = (ch == '-');
			++nLabelSize;
			if ( (bDashChar && nLabelSize == 1) || (nLabelSize > g_nValidLabelMaxLen) )
				return false;
		}
		else if ( ch == '.' )
		{
			if ( bDashChar || nLabelSize == 0 )
				return false;
			nLabelSize = 0;
			++nLabelCount;
		}
		else
			return false;
	} //for ( size_t i = 0; i < nHostNameSize; ++i )
	return ( nLabelCount > 0 && nLabelSize != 0 && !bDashChar );
}

bool GlobalFunc::isValidHostName(const char * lpszHostName)
{
	if ( lpszHostName == NULL || *lpszHostName == RFC_CHAR_NULL )
		return false;
	return isValidHostName(lpszHostName, StringFunc::length(lpszHostName));
}

void GlobalFunc::binaryToHex(const void * pSrcBinary, size_t nSize, stdstring & strRetHex, char chDelimiter)
{
	const rfc_uint_8 * p = static_cast<const rfc_uint_8*>(pSrcBinary);
	const rfc_uint_8 * pEnd = p + nSize;
	for( ; p < pEnd; ++p )
	{
		if ( chDelimiter != RFC_CHAR_NULL && !strRetHex.empty() )
			strRetHex += chDelimiter;
		int n = static_cast<rfc_uint_8>(*p);
		strRetHex += StringFunc::getUpperHexChar( (n & 0xF0) >> 4 );
		strRetHex += StringFunc::getUpperHexChar( n & 0x0F );
	} //for( ; p < pEnd; ++p )
}

bool GlobalFunc::hexToBinary(const void * pSrcHex, size_t nSize, stdstring & strRetBinary, char chDelimiter)
{
	const char * p = static_cast<const char*>(pSrcHex);
	const char * pEnd = p + nSize;
	for( ; p + 1 < pEnd; p += 2 )
	{
		if ( chDelimiter != RFC_CHAR_NULL && p[0] == chDelimiter )
		{
			--p;			// p += 2 in for loop
			continue;
		}
		if( !::isxdigit(p[0]) || !::isxdigit(p[1]) )
			return false;
		int ch1 = ::isdigit(p[0]) ? (p[0] - '0') : (StringFunc::tolower(p[0])  + 10 - 'a');
		int ch2 = ::isdigit(p[1]) ? (p[1] - '0') : (StringFunc::tolower(p[1])  + 10 - 'a');
		strRetBinary += static_cast<char>( ch1 * 16 + ch2 );
	} //for( ; p + 1 < pEnd; p += 2 )
	return (p == pEnd);
}

RFC_NAMESPACE_END
