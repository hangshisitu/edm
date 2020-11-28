/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "protocolbuf/protocolencoder.h"
#include "base/byteorderfunc.h"

RFC_NAMESPACE_BEGIN

bool ProtocolHeader::isValid(void) const
{
	return ( m_nProtocolPrefix == PROTOCOL_PREFIX_1 && m_nProtocolVersion == PROTOCOL_VERSION_1 );
}

bool ProtocolHeader::loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd)
{
	if ( !ProtocolReadStream::smartRead(pBegin, pEnd, m_nProtocolPrefix)
		|| !ProtocolReadStream::smartRead(pBegin, pEnd, m_nProtocolVersion) )
		return false;
	return isValid();
}

bool ProtocolHeader::saveToBinary(BinaryStream & binBuffer) const
{
	return ( ProtocolWriteStream::smartWrite(m_nProtocolPrefix, binBuffer)
			&& ProtocolWriteStream::smartWrite(m_nProtocolVersion, binBuffer) );
}

//////////////////////////////////////////////////////////////////////////

bool ProtocolReadStream::smartReadSize(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, rfc_uint_32 & nSize)
{
	if ( pBegin >= pEnd )
		return false;
	nSize = static_cast<rfc_uint_8>( *pBegin++ );
	if ( nSize > 0x7F )
	{
		if ( !smartRead(--pBegin, pEnd, nSize) )
			return false;
		nSize &= 0x7FFFFFFF;
	}
	return true;
}

bool ProtocolReadStream::smartReadInt(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, bool bInt16, rfc_int_32 & nVal)
{
	if ( !bInt16 )
		return smartRead(pBegin, pEnd, nVal);

	rfc_int_16 nInt16 = 0;
	if ( !smartRead(pBegin, pEnd, nInt16) )
		return false;
	nVal = nInt16;
	return true;
}

bool ProtocolReadStream::smartReadBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, void * lpBuf, size_t nSize)
{
	if ( pBegin + nSize > pEnd )
		return false;
	memcpy(lpBuf, pBegin, nSize);
	pBegin += nSize;
	return true;
}

bool ProtocolReadStream::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, double & dbVal)
{
	rfc_int_64 * pVal = (rfc_int_64 *)(&dbVal);
	return smartRead(pBegin, pEnd, *pVal);
}

bool ProtocolReadStream::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, stdstring & strVal)
{
	rfc_uint_32 nSize = 0;
	if ( !smartReadSize(pBegin, pEnd, nSize) )
		return false;
	strVal.clear();
	strVal.resize(nSize);
	return smartReadBinary(pBegin, pEnd, const_cast<char *>(strVal.data()), nSize);
}

bool ProtocolReadStream::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, CharsetString & cstrVal)
{
	return false;
}

bool ProtocolReadStream::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, BinaryStream & binVal)
{
	rfc_uint_32 nSize = 0;
	if ( !smartReadSize(pBegin, pEnd, nSize) )
		return false;
	binVal.clearAndReserve(nSize);
	binVal.resize(nSize);
	return smartReadBinary(pBegin, pEnd, binVal.data(), nSize);
}

bool ProtocolReadStream::smartRead(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd, ProtocolVariant & tVar)
{
	rfc_uint_8 nType = ProtocolEncoder::ENCODE_VARIANT_TYPE_NULL;
	if ( !smartRead(pBegin, pEnd, nType) )
		return false;

	switch( nType )
	{
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_NULL:
		tVar.setNULL();
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BOOL_TRUE:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BOOL_FALSE:
		tVar.assign( nType == ProtocolEncoder::ENCODE_VARIANT_TYPE_BOOL_TRUE );
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_16:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_32:
		if ( !smartReadInt(pBegin, pEnd, nType == ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_16, tVar.asInteger32()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_64:
		if ( !smartRead(pBegin, pEnd, tVar.asInteger64()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_DOUBLE:
		if ( !smartRead(pBegin, pEnd, tVar.asDouble()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_STRING:
		if ( !smartRead(pBegin, pEnd, tVar.asString()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BINARY:
		if ( !smartRead(pBegin, pEnd, tVar.asBinary()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_ARRAY:
		{
			rfc_uint_32 nCount = 0;
			if ( !smartReadSize(pBegin, pEnd, nCount) )
				return false;

			ProtocolVariant::typeVariantArray & varArray = tVar.asArray();
			varArray.clear();
			varArray.resize(nCount);
			for ( size_t i = 0; i < nCount; ++i )
			{
				if ( !smartRead(pBegin, pEnd, varArray[i]) )
					return false;
			} //for ( size_t i = 0; i < nCount; ++i )
			break;
		}
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT16_MAP:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_MAP:
		{
			rfc_uint_32 nCount = 0;
			if ( !smartReadSize(pBegin, pEnd, nCount) )
				return false;

			rfc_int_32 nVal = 0;
			ProtocolVariant::typeIntVariantMap & varIntMap = tVar.asIntMap();
			for ( size_t i = 0; i < nCount; ++i )
			{
				if ( !smartReadInt(pBegin, pEnd, nType == ProtocolEncoder::ENCODE_VARIANT_TYPE_INT16_MAP, nVal) )
					return false;
				if ( !smartRead(pBegin, pEnd, varIntMap[nVal]) )
					return false;
			} //for ( size_t i = 0; i < nCount; ++i )
			break;
		}
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_STRING_MAP:
		if ( !smartReadStringMap(pBegin, pEnd, tVar.asStringMap()) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_NOCASE_STRING_MAP:
		if ( !smartReadStringMap(pBegin, pEnd, tVar.asNocaseStringMap()) )
			return false;
		break;
	default:
		throw ProtocalVarException(GlobalConstant::RET_ERROR, "Unknown variant type on smartRead");
		break;
	} //switch( nType )
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ProtocolWriteStream::smartWriteSize(rfc_uint_32 nSize, BinaryStream & binBuffer)
{
	if ( nSize > 0x7F )
	{
		nSize |= 0x80000000;
		return smartWrite(nSize, binBuffer);
	}
	return smartWrite(static_cast<rfc_uint_8>(nSize), binBuffer);
}

bool ProtocolWriteStream::smartWriteInt(bool bInt16, rfc_int_32 nVal, BinaryStream & binBuffer)
{
	if ( bInt16 )
		return smartWrite(static_cast<rfc_int_16>(nVal), binBuffer);
	return smartWrite(nVal, binBuffer);
}

bool ProtocolWriteStream::smartWriteBinary(const void * lpBuf, size_t nSize, BinaryStream & binBuffer)
{
	return ( smartWriteSize(nSize, binBuffer) && binBuffer.write(lpBuf, nSize) == nSize );
}

bool ProtocolWriteStream::smartWrite(rfc_uint_16 nVal, BinaryStream & binBuffer)
{
	return ( smartWrite(static_cast<rfc_uint_8>( (nVal >> 8) & 0xFF ), binBuffer)
			&& smartWrite(static_cast<rfc_uint_8>( nVal & 0xFF ), binBuffer) );
}

bool ProtocolWriteStream::smartWrite(rfc_uint_32 nVal, BinaryStream & binBuffer)
{
	return ( smartWrite(static_cast<rfc_uint_8>( (nVal >> 24) & 0xFF ), binBuffer)
			&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 16) & 0xFF ), binBuffer)
			&& smartWrite(static_cast<rfc_uint_8>( (nVal >> 8) & 0xFF ), binBuffer)
			&& smartWrite(static_cast<rfc_uint_8>( nVal & 0xFF ), binBuffer) );
}

bool ProtocolWriteStream::smartWrite(rfc_uint_64 nVal, BinaryStream & binBuffer)
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

bool ProtocolWriteStream::smartWrite(const ProtocolVariant & tVar, BinaryStream & binBuffer)
{
	ProtocolEncoder::EncodeVariantType nType = ProtocolEncoder::getEncodeVariantType(tVar);
	smartWrite(static_cast<rfc_uint_8>(nType), binBuffer);
	switch( nType )
	{
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_NULL:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BOOL_TRUE:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BOOL_FALSE:
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_16:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_32:
		if ( !smartWriteInt(nType == ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_16, tVar.asInteger32(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_64:
		if ( !smartWrite(tVar.asInteger64(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_DOUBLE:
		if ( !smartWrite(tVar.asDouble(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_DATATIME:
		if ( !smartWrite(tVar.asDateTime(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_STRING:
		if ( !smartWrite(tVar.asString(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_CHARSET_STRING:
		if ( !smartWrite(tVar.asCharsetString(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_BINARY:
		if ( !smartWrite(tVar.asBinary(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_ARRAY:
		{
			const ProtocolVariant::typeVariantArray & varArray = tVar.asArray();
			if ( !smartWriteSize(varArray.size(), binBuffer) )
				return false;
			ProtocolVariant::typeVariantArray::const_iterator it, itEnd = varArray.end();
			for ( it = varArray.begin(); it != itEnd; ++it )
			{
				if ( !smartWrite(*it, binBuffer) )
					return false;
			} // for ( it = varArray.begin(); it != itEnd; ++it )
			break;
		}
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT16_MAP:
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_INT_MAP:
		{
			const ProtocolVariant::typeIntVariantMap & varIntMap = tVar.asIntMap();
			if ( !smartWriteSize(varIntMap.size(), binBuffer) )
				return false;
			ProtocolVariant::typeIntVariantMap::const_iterator it, itEnd = varIntMap.end();
			for ( it = varIntMap.begin(); it != itEnd; ++it )
			{
				if ( !smartWriteInt(nType == ProtocolEncoder::ENCODE_VARIANT_TYPE_INT16_MAP, it->first, binBuffer) )
					return false;
				if ( !smartWrite(it->second, binBuffer) )
					return false;
			}
			break;
		}
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_STRING_MAP:
		if ( !smartWriteStringMap(tVar.asStringMap(), binBuffer) )
			return false;
		break;
	case ProtocolEncoder::ENCODE_VARIANT_TYPE_NOCASE_STRING_MAP:
		if ( !smartWriteStringMap(tVar.asNocaseStringMap(), binBuffer) )
			return false;
		break;
	default:
		throw ProtocalVarException(GlobalConstant::RET_ERROR, "Unknown variant type on smartWrite");
		break;
	} //switch( nType )
	return true;
}

//////////////////////////////////////////////////////////////////////////

ProtocolEncoder::EncodeVariantType	ProtocolEncoder::getEncodeVariantType(const ProtocolVariant & tVar)
{
	EncodeVariantType nType = static_cast<EncodeVariantType>( tVar.getVariantType() );
	if ( (nType == ENCODE_VARIANT_TYPE_BOOL_TRUE) && !tVar.asBool() )
		nType = ENCODE_VARIANT_TYPE_BOOL_FALSE;
	else if ( (nType == ENCODE_VARIANT_TYPE_INT_32) && isInt16(tVar.asInteger32()) )
		nType = ENCODE_VARIANT_TYPE_INT_16;
	else if ( nType == ENCODE_VARIANT_TYPE_INT_MAP )
	{
		const ProtocolVariant::typeIntVariantMap & varMapData = tVar.asIntMap();
		if ( !varMapData.empty() && isInt16(varMapData.begin()->first) && isInt16(varMapData.rbegin()->first) )
			nType = ENCODE_VARIANT_TYPE_INT16_MAP;
	} //else if ( nType == ENCODE_VARIANT_TYPE_INT_MAP )
	return nType;
}

rfc_uint_32 ProtocolEncoder::calcVariantSize(const ProtocolVariant & tVar)
{
	rfc_uint_32 nTotalSize = sizeof(rfc_uint_8);				//for m_nVariantType
	ProtocolEncoder::EncodeVariantType nType = getEncodeVariantType(tVar);
	switch ( nType )
	{
	case ENCODE_VARIANT_TYPE_NULL:
	case ENCODE_VARIANT_TYPE_BOOL_TRUE:
	case ENCODE_VARIANT_TYPE_BOOL_FALSE:
		break;
	case ENCODE_VARIANT_TYPE_INT_16:
	case ENCODE_VARIANT_TYPE_INT_32:
		nTotalSize += calcIntSize(nType == ENCODE_VARIANT_TYPE_INT_16);
		break;
	case ENCODE_VARIANT_TYPE_INT_64:
	case ENCODE_VARIANT_TYPE_DOUBLE:
	case ENCODE_VARIANT_TYPE_DATATIME:
		nTotalSize += sizeof(rfc_int_64);
		break;
	case ENCODE_VARIANT_TYPE_STRING:
		nTotalSize += smartCalcSize(tVar.asString().size()) + tVar.asString().size();
		break;
	case ENCODE_VARIANT_TYPE_CHARSET_STRING:
		nTotalSize += smartCalcSize(tVar.asCharsetString().size()) + tVar.asCharsetString().size();
		break;
	case ENCODE_VARIANT_TYPE_BINARY:
		nTotalSize += smartCalcSize(tVar.asBinary().size()) + tVar.asBinary().size();
		break;
	case ENCODE_VARIANT_TYPE_ARRAY:
		{
			const ProtocolVariant::typeVariantArray & listData = tVar.asArray();
			nTotalSize += smartCalcSize(listData.size());
			for ( ProtocolVariant::typeVariantArray::const_iterator it = listData.begin(); it != listData.end(); ++it )
				nTotalSize += calcVariantSize(*it);
			break;
		}
	case ENCODE_VARIANT_TYPE_INT16_MAP:
	case ENCODE_VARIANT_TYPE_INT_MAP:
		{
			const ProtocolVariant::typeIntVariantMap & listData = tVar.asIntMap();
			nTotalSize += smartCalcSize(listData.size());
			nTotalSize += listData.size() * calcIntSize(nType == ENCODE_VARIANT_TYPE_INT16_MAP);
			for ( ProtocolVariant::typeIntVariantMap::const_iterator it = listData.begin(); it != listData.end(); ++it )
				nTotalSize += calcVariantSize(it->second);
			break;
		}
	case ENCODE_VARIANT_TYPE_STRING_MAP:
		nTotalSize += calcStringMapSize(tVar.asStringMap());
		break;
	case ENCODE_VARIANT_TYPE_NOCASE_STRING_MAP:
		nTotalSize += calcStringMapSize(tVar.asNocaseStringMap());
		break;
	default:
		throw ProtocalVarException(GlobalConstant::RET_ERROR, "Unknown variant type on calcVariantSize");
		break;
	} //switch ( getVariantType() )

	return nTotalSize;	
}

bool ProtocolEncoder::loadWithoutSize(const rfc_uint_8 * pBegin, size_t nSize, ProtocolVariant & varRes, ProtocolHeader & protocolHdr)
{
	const rfc_uint_8 * pEnd = pBegin + nSize;
	if ( !protocolHdr.loadFromBinary(pBegin, pEnd) )
		return false;
	return ProtocolReadStream::smartRead(pBegin, pEnd, varRes);
}

bool ProtocolEncoder::saveWithoutSize(const ProtocolVariant & tVar, const ProtocolHeader & protocolHdr, BinaryStream & binBuffer)
{
	binBuffer.clearAndReserve( ProtocolHeader::getHeaderSize() + calcVariantSize(tVar) );
	if ( !protocolHdr.saveToBinary(binBuffer) )
		return false;
	return ProtocolWriteStream::smartWrite(tVar, binBuffer);
}

bool ProtocolEncoder::loadFromBinary(const rfc_uint_8 * pBegin, size_t nSize, ProtocolVariant & varRes, ProtocolHeader & protocolHdr)
{
	size_t nSizeOfUint32 = sizeof(rfc_uint_32);
	if ( nSize < nSizeOfUint32 )
		return false;

	const rfc_uint_32 * pSize = reinterpret_cast<const rfc_uint_32 *>(pBegin);
	if ( nSize < nSizeOfUint32 + ByteOrderFunc::ntoh(*pSize) )
		return false;
	return loadWithoutSize(pBegin + nSizeOfUint32, nSize - nSizeOfUint32, varRes, protocolHdr);
}

bool ProtocolEncoder::saveToBinary(const ProtocolVariant & tVar, const ProtocolHeader & protocolHdr, BinaryStream & binBuffer)
{
	rfc_uint_32 nPackageSize = ProtocolHeader::getHeaderSize() + calcVariantSize(tVar);
	binBuffer.clearAndReserve( sizeof(nPackageSize) + nPackageSize );
	nPackageSize = ByteOrderFunc::hton(nPackageSize);			// ÒÔÍøÂç×Ö½ÚË³Ðò´æ´¢
	if ( binBuffer.write(&nPackageSize, sizeof(nPackageSize)) != sizeof(nPackageSize) )
		return false;

	if ( !protocolHdr.saveToBinary(binBuffer) )
		return false;
	return ProtocolWriteStream::smartWrite(tVar, binBuffer);
}

RFC_NAMESPACE_END

