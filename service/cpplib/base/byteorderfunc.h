/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_BYTEORDERFUNC_H_201008
#define RFC_BYTEORDERFUNC_H_201008

#include "rfcdefine.h"
#include <arpa/inet.h>

RFC_NAMESPACE_BEGIN

class ByteOrderFunc
{
public:
	static const bool 			m_bLittleEndian;
	static const bool 			m_bBigEndian;

	static const char *			getUTF16Charset(void);

	static inline rfc_uint_8	hton(rfc_uint_8 nVal)  { return nVal; }
	static inline rfc_uint_8	ntoh(rfc_uint_8 nVal)  { return nVal; }
	static inline rfc_int_8		hton(rfc_int_8 nVal)  { return nVal; }
	static inline rfc_int_8		ntoh(rfc_int_8 nVal)  { return nVal; }

	static inline rfc_uint_16	hton(rfc_uint_16 nVal)  { return htons(nVal); }
	static inline rfc_uint_16	ntoh(rfc_uint_16 nVal)  { return ntohs(nVal); }
	static inline rfc_uint_16	hton(rfc_int_16 nVal)   { return htons(nVal); }
	static inline rfc_uint_16	ntoh(rfc_int_16 nVal)   { return ntohs(nVal); }

	static inline rfc_uint_32	hton(rfc_uint_32 nVal)  { return htonl(nVal); }
	static inline rfc_uint_32	ntoh(rfc_uint_32 nVal)  { return ntohl(nVal); }
	static inline rfc_uint_32	hton(rfc_int_32 nVal)  { return htonl(nVal); }
	static inline rfc_uint_32	ntoh(rfc_int_32 nVal)  { return ntohl(nVal); }

	static rfc_uint_64			hton(rfc_int_64 nVal);
	static rfc_uint_64			ntoh(rfc_int_64 nVal);
	static inline rfc_uint_64	hton(rfc_uint_64 nVal) { return hton( static_cast<rfc_int_64>(nVal) ); }
	static inline rfc_uint_64	ntoh(rfc_uint_64 nVal) { return ntoh( static_cast<rfc_int_64>(nVal) ); }

	template<typename typeInt>
	static bool					readIntLittleEndian(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		size_t nByteCount = sizeof(typeInt);
		if ( nByteCount < 1 || pBegin + nByteCount > pEnd )
			return false;

		pBegin = pBegin + nByteCount - 1;
		nVal = static_cast<rfc_uint_8>( *pBegin-- ) & 0xff;
		for ( size_t i = 1; i < nByteCount; ++i )
		{
			nVal <<= 8;
			nVal |= static_cast<rfc_uint_8>( *pBegin-- ) & 0xff;
		} // for ( size_t i = 1; i < nByteCount; ++i )
		pBegin = pBegin + nByteCount + 1;
		return true;
	}

	template<typename typeInt>
	static bool					readIntBigEndian(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		size_t nByteCount = sizeof(typeInt);
		if ( nByteCount < 1 || pBegin + nByteCount > pEnd )
			return false;

		nVal = static_cast<rfc_uint_8>( *pBegin++ ) & 0xff;
		for ( size_t i = 1; i < nByteCount; ++i )
		{
			nVal <<= 8;
			nVal |= static_cast<rfc_uint_8>( *pBegin++ ) & 0xff;
		} //for ( size_t i = 1; i < nByteCount; ++i )
		return true;
	}

	template<typename typeInt>
	static bool					readIntByHostOrder(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		return readIntLittleEndian(pBegin, pEnd, nVal);
	}

	template<typename typeInt>
	static bool					readIntByNetOrder(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		return readIntBigEndian(pBegin, pEnd, nVal);
	}

	template<typename typeInt>
	static bool					readInt(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		if ( m_bLittleEndian )
			return readIntLittleEndian(pBegin, pEnd, nVal);
		return readIntBigEndian(pBegin, pEnd, nVal);
	}
};

RFC_NAMESPACE_END

#endif	//RFC_BYTEORDERFUNC_H_201008
