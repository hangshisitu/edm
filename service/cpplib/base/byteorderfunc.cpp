/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/byteorderfunc.h"

RFC_NAMESPACE_BEGIN

static const rfc_uint_32 g_nTestBigEndianValue = 0x12345678L;
const bool ByteOrderFunc::m_bLittleEndian = (*((rfc_uint_8 *)&g_nTestBigEndianValue) == 0x78);
const bool ByteOrderFunc::m_bBigEndian = !ByteOrderFunc::m_bLittleEndian;

const char * ByteOrderFunc::getUTF16Charset(void)
{
	return ( ByteOrderFunc::m_bBigEndian ? "utf-16be" : "utf-16le" );
}

rfc_uint_64 ByteOrderFunc::hton(rfc_int_64 nVal)
{
	union 
	{
		rfc_int_64		ll;
		rfc_uint_8		buf[8];
	} sData;

	sData.buf[0] = static_cast<char>((nVal >> 56) & 0xff)	;
	sData.buf[1] = static_cast<char>((nVal >> 48) & 0xff)	;
	sData.buf[2] = static_cast<char>((nVal >> 40) & 0xff)	;
	sData.buf[3] = static_cast<char>((nVal >> 32) & 0xff)	;
	sData.buf[4] = static_cast<char>((nVal >> 24) & 0xff)	;
	sData.buf[5] = static_cast<char>((nVal >> 16) & 0xff)	;
	sData.buf[6] = static_cast<char>((nVal >> 8 ) & 0xff)	;
	sData.buf[7] = static_cast<char>(nVal & 0xff)			;

	return static_cast<rfc_uint_64>( sData.ll );
}

rfc_uint_64 ByteOrderFunc::ntoh(rfc_int_64 nVal)
{
	union 
	{
		rfc_int_64	ll;
		rfc_uint_8	buf[8];
	} sData;

	sData.ll = nVal;
	rfc_uint_64 ullVal = static_cast<rfc_uint_64>(sData.buf[0]) << 56 |
						static_cast<rfc_uint_64>(sData.buf[1]) << 48 |
						static_cast<rfc_uint_64>(sData.buf[2]) << 40 |
						static_cast<rfc_uint_64>(sData.buf[3]) << 32 |
						static_cast<rfc_uint_64>(sData.buf[4]) << 24 |
						static_cast<rfc_uint_64>(sData.buf[5]) << 16 |
						static_cast<rfc_uint_64>(sData.buf[6]) << 8 |
						static_cast<rfc_uint_64>(sData.buf[7]);
	return ullVal;
}

RFC_NAMESPACE_END
