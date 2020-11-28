/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#ifndef APPLIB_SOCKETSTREAMENCODER_H_201110
#define APPLIB_SOCKETSTREAMENCODER_H_201110

#include "stream/binarystream.h"
#include "base/datetime.h"

RFC_NAMESPACE_BEGIN

class SocketStreamEncoder
{
public:
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_8 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_8 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }

	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_16 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_16 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }

	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_32 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_32 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }

	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_64 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_64 & nVal) { return readIntByNetOrder(pBegin, pEnd, nVal); }

	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, double & dbVal);
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, DateTime & dtVal);
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, stdstring & strVal);
	static bool		smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, BinaryStream & binVal);

	//////////////////////////////////////////////////////////////////////////
	static bool		smartWrite(rfc_uint_8 nVal, BinaryStream & binBuffer);
	static bool		smartWrite(rfc_int_8 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_8>(nVal), binBuffer); }

	static bool		smartWrite(rfc_uint_16 nVal, BinaryStream & binBuffer);
	static bool		smartWrite(rfc_int_16 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_16>(nVal), binBuffer); }

	static bool		smartWrite(rfc_uint_32 nVal, BinaryStream & binBuffer);
	static bool		smartWrite(rfc_int_32 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_32>(nVal), binBuffer); }

	static bool		smartWrite(rfc_uint_64 nVal, BinaryStream & binBuffer);
	static bool		smartWrite(rfc_int_64 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_64>(nVal), binBuffer); }

	static bool		smartWrite(const double & dbVal, BinaryStream & binBuffer);
	static bool		smartWrite(const DateTime & dtVal, BinaryStream & binBuffer);
	static bool		smartWrite(const stdstring & strVal, BinaryStream & binBuffer);
	static bool		smartWrite(const BinaryStream & binVal, BinaryStream & binBuffer);

protected:
	template<typename typeInt>
	static bool		readIntByNetOrder(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
	{
		size_t nByteCount = sizeof(typeInt);
		if ( pBegin + nByteCount > pEnd )
			return false;

		nVal = static_cast<rfc_uint_8>( *pBegin++ ) & 0xff;
		for ( size_t i = 1; i < nByteCount; ++i )
		{
			nVal <<= 8;
			nVal |= static_cast<rfc_uint_8>( *pBegin++ ) & 0xff;
		} //for ( size_t i = 1; i < nByteCount; ++i )
		return true;
	}

	static bool		smartReadBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, void * lpBuf, size_t nSize);
	static bool		smartWriteBinary(const void * lpBuf, size_t nSize, BinaryStream & binBuffer);
};

RFC_NAMESPACE_END

#endif	//APPLIB_SOCKETSTREAMENCODER_H_201110
