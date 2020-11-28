/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_PROTOCOLENCODER_H_201008
#define RFC_PROTOCOLENCODER_H_201008

#include "protocolbuf/protocolvariant.h"

RFC_NAMESPACE_BEGIN

struct ProtocolHeader
{
	enum ProtocolPrefix
	{
		PROTOCOL_PREFIX_1 = 0xFFFE,
	};

	enum ProtocolVersion
	{
		PROTOCOL_VERSION_1 = 1,
	};

	ProtocolHeader(void) : m_nProtocolPrefix(PROTOCOL_PREFIX_1), m_nProtocolVersion(PROTOCOL_VERSION_1) {}
	static rfc_uint_32		getHeaderSize(void) { return sizeof(rfc_uint_16) * 2; }
	bool					isValid(void) const;
	bool					loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd);
	bool					saveToBinary(BinaryStream & binBuffer) const;

	rfc_uint_16				m_nProtocolPrefix;
	rfc_uint_16				m_nProtocolVersion;
};

class ProtocolReadStream
{
public:
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_8 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_8 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }
	
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_16 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_16 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }

	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_32 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_32 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }

	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_int_64 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }
	static inline bool	smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_64 & nVal) { return smartReadIntImp(pBegin, pEnd, nVal); }

	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, double & dbVal);
	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, DateTime & dtVal) { return false; }
	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, stdstring & strVal);
	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, CharsetString & cstrVal);
	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, BinaryStream & binVal);
	static bool			smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, ProtocolVariant & tVar);

protected:
	template<typename typeInt>
	static bool			smartReadIntImp(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeInt & nVal)
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

	template<typename typeStringMap>
	static bool			smartReadStringMap(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, typeStringMap & mapData)
	{
		rfc_uint_32 nCount = 0;
		if ( !smartReadSize(pBegin, pEnd, nCount) )
			return false;

		stdstring strKey;
		for ( size_t i = 0; i < nCount; ++i )
		{
			if ( !smartRead(pBegin, pEnd, strKey) )
				return false;
			if ( !smartRead(pBegin, pEnd, mapData[strKey]) )
				return false;
		} //for ( size_t i = 0; i < nCount; ++i )
		return true;
	}

	static bool			smartReadSize(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_32 & nSize);
	static bool			smartReadInt(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, bool bInt16, rfc_int_32 & nVal);
	static bool			smartReadBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, void * lpBuf, size_t nSize);
};

class ProtocolWriteStream
{
public:
	static inline bool	smartWrite(rfc_uint_8 nVal, BinaryStream & binBuffer)
	{
		static const size_t nSizeOfByte = sizeof(rfc_uint_8);
		return binBuffer.write(&nVal, nSizeOfByte) == nSizeOfByte;
	}
	static inline bool	smartWrite(rfc_int_8 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_8>(nVal), binBuffer); }

	static bool			smartWrite(rfc_uint_16 nVal, BinaryStream & binBuffer);
	static inline bool	smartWrite(rfc_int_16 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_16>(nVal), binBuffer); }

	static bool			smartWrite(rfc_uint_32 nVal, BinaryStream & binBuffer);
	static inline bool	smartWrite(rfc_int_32 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_32>(nVal), binBuffer); }

	static bool			smartWrite(rfc_uint_64 nVal, BinaryStream & binBuffer);
	static inline bool	smartWrite(rfc_int_64 nVal, BinaryStream & binBuffer) { return smartWrite(static_cast<rfc_uint_64>(nVal), binBuffer); }

	static bool			smartWrite(const double & dbVal, BinaryStream & binBuffer) { return false; }
	static bool			smartWrite(const DateTime & dtVal, BinaryStream & binBuffer) { return false; }
	static bool			smartWrite(const stdstring & strVal, BinaryStream & binBuffer)
	{ return smartWriteBinary(strVal.data(), strVal.size(), binBuffer); }
	static bool			smartWrite(const CharsetString & cstrVal, BinaryStream & binBuffer) { return false; }
	static bool			smartWrite(const BinaryStream & binVal, BinaryStream & binBuffer)
	{ return smartWriteBinary(binVal.data(), binVal.size(), binBuffer); }
	static bool			smartWrite(const ProtocolVariant & tVar, BinaryStream & binBuffer);

protected:
	template<typename typeStringMap>
	static bool			smartWriteStringMap(const typeStringMap & mapData, BinaryStream & binBuffer)
	{
		if ( !smartWriteSize(mapData.size(), binBuffer) )
			return false;
		typename typeStringMap::const_iterator it, itEnd = mapData.end();
		for ( it = mapData.begin(); it != itEnd; ++it )
		{
			if ( !smartWrite(it->first, binBuffer) )
				return false;
			if ( !smartWrite(it->second, binBuffer) )
				return false;
		} //for ( it = mapData.begin(); it != itEnd; ++it )
		return true;
	}

	static bool			smartWriteSize(rfc_uint_32 nSize, BinaryStream & binBuffer);
	static bool			smartWriteInt(bool bInt16, rfc_int_32 nVal, BinaryStream & binBuffer);
	static bool			smartWriteBinary(const void * lpBuf, size_t nSize, BinaryStream & binBuffer);
};

class ProtocolEncoder
{
public:
	enum EncodeVariantType
	{
		ENCODE_VARIANT_TYPE_NULL = ProtocolVariant::VARIANT_TYPE_NULL,
		ENCODE_VARIANT_TYPE_BOOL_TRUE = ProtocolVariant::VARIANT_TYPE_BOOL,
		ENCODE_VARIANT_TYPE_INT_32 = ProtocolVariant::VARIANT_TYPE_INT_32,
		ENCODE_VARIANT_TYPE_INT_64 = ProtocolVariant::VARIANT_TYPE_INT_64,
		ENCODE_VARIANT_TYPE_DOUBLE = ProtocolVariant::VARIANT_TYPE_DOUBLE,
		ENCODE_VARIANT_TYPE_DATATIME = ProtocolVariant::VARIANT_TYPE_DATATIME,
		ENCODE_VARIANT_TYPE_STRING = ProtocolVariant::VARIANT_TYPE_STRING,
		ENCODE_VARIANT_TYPE_CHARSET_STRING = ProtocolVariant::VARIANT_TYPE_CHARSET_STRING,			//包含编码信息的字符串
		ENCODE_VARIANT_TYPE_BINARY = ProtocolVariant::VARIANT_TYPE_BINARY,
		ENCODE_VARIANT_TYPE_ARRAY = ProtocolVariant::VARIANT_TYPE_ARRAY,
		ENCODE_VARIANT_TYPE_INT_MAP = ProtocolVariant::VARIANT_TYPE_INT_MAP,
		ENCODE_VARIANT_TYPE_STRING_MAP = ProtocolVariant::VARIANT_TYPE_STRING_MAP,
		ENCODE_VARIANT_TYPE_NOCASE_STRING_MAP = ProtocolVariant::VARIANT_TYPE_NOCASE_STRING_MAP,

		//for save to binary stream
		ENCODE_VARIANT_TYPE_BOOL_FALSE = 64,
		ENCODE_VARIANT_TYPE_INT_16,
		ENCODE_VARIANT_TYPE_INT16_MAP,
	};
	static EncodeVariantType	getEncodeVariantType(const ProtocolVariant & tVar);

	static rfc_uint_32	calcVariantSize(const ProtocolVariant & tVar);

	// pData不包含包的大小,只包含ProtocolHeader和ProtocolVariant数据
	static bool			loadWithoutSize(const rfc_uint_8 * pData, size_t nSize, ProtocolVariant & varRes, ProtocolHeader & protocolHdr);
	static bool			loadWithoutSize(const BinaryStream & binBuffer, ProtocolVariant & varRes, ProtocolHeader & protocolHdr)
	{	return			loadWithoutSize(binBuffer.data(), binBuffer.size(), varRes, protocolHdr); }
	static bool			loadWithoutSize(const rfc_uint_8 * pData, size_t nSize, ProtocolVariant & varRes)
	{	ProtocolHeader protocolHdr; return loadWithoutSize(pData, nSize, varRes, protocolHdr); }
	static bool			loadWithoutSize(const BinaryStream & binBuffer, ProtocolVariant & varRes)
	{	ProtocolHeader protocolHdr; return loadWithoutSize(binBuffer.data(), binBuffer.size(), varRes, protocolHdr); }

	static bool			saveWithoutSize(const ProtocolVariant & tVar, const ProtocolHeader & protocolHdr, BinaryStream & binBuffer);
	static bool			saveWithoutSize(const ProtocolVariant & tVar, BinaryStream & binBuffer)
	{	ProtocolHeader protocolHdr; return saveWithoutSize(tVar, protocolHdr, binBuffer); }

	static bool			loadFromBinary(const rfc_uint_8 * pData, size_t nSize, ProtocolVariant & varRes, ProtocolHeader & protocolHdr);
	static bool			loadFromBinary(const BinaryStream & binBuffer, ProtocolVariant & varRes, ProtocolHeader & protocolHdr)
	{	return			loadFromBinary(binBuffer.data(), binBuffer.size(), varRes, protocolHdr); }
	static bool			saveToBinary(const ProtocolVariant & tVar, const ProtocolHeader & protocolHdr, BinaryStream & binBuffer);

	//pData已包含ProtocolHeader,但客户不关心ProtocolHeader
	static bool			loadFromBinary(const rfc_uint_8 * pData, size_t nSize, ProtocolVariant & varRes)
	{	ProtocolHeader protocolHdr; return loadFromBinary(pData, nSize, varRes, protocolHdr); }
	static bool			loadFromBinary(const BinaryStream & binBuffer, ProtocolVariant & varRes)
	{	ProtocolHeader protocolHdr; return loadFromBinary(binBuffer.data(), binBuffer.size(), varRes, protocolHdr); }
	static bool			saveToBinary(const ProtocolVariant & tVar, BinaryStream & binBuffer)
	{	ProtocolHeader protocolHdr; return saveToBinary(tVar, protocolHdr, binBuffer); }

protected:
	static inline bool	isInt16(rfc_int_32 nVal) { return ( nVal >= -0x7FFF && nVal <= 0x7FFF); }
	//int32优先考虑用2个字节的rfc_int_16存储
	static rfc_uint_32	calcIntSize(bool bInt16) { return bInt16 ? sizeof(rfc_int_16) : sizeof(rfc_int_32); }

	//最大只能存储2G,第一个bit如果是1,则表示用4个字节表示;否则用1个字节表示
	static rfc_uint_32	smartCalcSize(size_t nSize)
	{
		return ( nSize > 0x7F ) ? sizeof(rfc_uint_32) : sizeof(rfc_uint_8);
	}

	template<typename typeStringMap>
	static rfc_uint_32	calcStringMapSize(const typeStringMap & mapData)
	{
		rfc_uint_32 nTotalSize = smartCalcSize(mapData.size());
		typename typeStringMap::const_iterator it = mapData.begin();
		for ( ; it != mapData.end(); ++it )
		{
			nTotalSize += smartCalcSize(it->first.size()) + it->first.size();
			nTotalSize += calcVariantSize(it->second);
		} //for ( ; it != mapData.end(); ++it )
		return nTotalSize;
	}
};

RFC_NAMESPACE_END

#endif	//RFC_PROTOCOLENCODER_H_201008

