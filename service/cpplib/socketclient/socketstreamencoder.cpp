/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#include "socketclient/socketstreamencoder.h"

RFC_NAMESPACE_BEGIN

bool SocketStreamEncoder::smartReadBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, void * lpBuf, size_t nSize)
{
	if ( pBegin + nSize > pEnd )
		return false;
	memcpy(lpBuf, pBegin, nSize);
	pBegin += nSize;
	return true;
}

bool SocketStreamEncoder::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, double & dbVal)
{
	rfc_int_64 * pVal = (rfc_int_64 *)(&dbVal);
	return smartRead(pBegin, pEnd, *pVal);
}

bool SocketStreamEncoder::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, DateTime & dtVal)
{
	rfc_uint_32 nSecond = 0, nMicroSecond = 0;
	if ( !smartRead(pBegin, pEnd, nSecond) )
		return false;
	if ( !smartRead(pBegin, pEnd, nMicroSecond) )
		return false;
	dtVal.setStandardTime(nSecond, nMicroSecond);
	return true;
}

bool SocketStreamEncoder::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, stdstring & strVal)
{
	rfc_uint_32 nSize = 0;
	if ( !smartRead(pBegin, pEnd, nSize) )
		return false;
	strVal.clear();
	strVal.resize(nSize);
	return smartReadBinary(pBegin, pEnd, const_cast<char *>(strVal.data()), nSize);
}

bool SocketStreamEncoder::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, BinaryStream & binVal)
{
	rfc_uint_32 nSize = 0;
	if ( !smartRead(pBegin, pEnd, nSize) )
		return false;
	binVal.clearAndReserve(nSize);
	binVal.resize(nSize);
	return smartReadBinary(pBegin, pEnd, binVal.data(), nSize);
}

//////////////////////////////////////////////////////////////////////////

bool SocketStreamEncoder::smartWriteBinary(const void * lpBuf, size_t nSize, BinaryStream & binBuffer)
{
	if ( !smartWrite(static_cast<rfc_uint_32>(nSize), binBuffer) )
		return false;
	return ( binBuffer.write(lpBuf, nSize) == nSize );
}

bool SocketStreamEncoder::smartWrite(rfc_uint_8 nVal, BinaryStream & binBuffer)
{
	static const size_t nSizeOfByte = sizeof(rfc_uint_8);
	return binBuffer.write(&nVal, nSizeOfByte) == nSizeOfByte;
}

bool SocketStreamEncoder::smartWrite(rfc_uint_16 nVal, BinaryStream & binBuffer)
{
	return ( smartWrite(static_cast<rfc_uint_8>( (nVal >> 8) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( nVal & 0xFF ), binBuffer) );
}

bool SocketStreamEncoder::smartWrite(rfc_uint_32 nVal, BinaryStream & binBuffer)
{
	return ( smartWrite(static_cast<rfc_uint_8>( (nVal >> 24) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 16) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 8) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( nVal & 0xFF ), binBuffer) );
}

bool SocketStreamEncoder::smartWrite(rfc_uint_64 nVal, BinaryStream & binBuffer)
{
	return ( smartWrite(static_cast<rfc_uint_8>( (nVal >> 56) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 48) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 40) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 32) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 24) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 16) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 8) & 0xFF ), binBuffer)
		&& smartWrite(static_cast<rfc_uint_8>( nVal & 0xFF ), binBuffer) );
}

bool SocketStreamEncoder::smartWrite(const double & dbVal, BinaryStream & binBuffer)
{
	rfc_int_64 * pVal = (rfc_int_64 *)(&dbVal);
	return smartWrite(*pVal, binBuffer);
}

bool SocketStreamEncoder::smartWrite(const DateTime & dtVal, BinaryStream & binBuffer)
{
	time_t nSecond = 0;
	long nMicroSecond = 0;
	dtVal.getStandardTime(nSecond, nMicroSecond);
	if ( !smartWrite(static_cast<rfc_uint_32>(nSecond), binBuffer) )
		return false;
	return smartWrite(static_cast<rfc_uint_32>(nMicroSecond), binBuffer);
}

bool SocketStreamEncoder::smartWrite(const stdstring & strVal, BinaryStream & binBuffer)
{
	return smartWriteBinary(strVal.data(), strVal.size(), binBuffer);
}

bool SocketStreamEncoder::smartWrite(const BinaryStream & binVal, BinaryStream & binBuffer)
{
	return smartWriteBinary(binVal.data(), binVal.size(), binBuffer);
}

RFC_NAMESPACE_END
