/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "protocolbuf/protocolvariant.h"
#include "base/stringtokenizer.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

const ProtocolVariant ProtocolVariant::g_varNULL;

const char * lpszTypeString[] = 
{
	"[NUL]",
	"[BOL]",
	"[INT32]",
	"[INT64]",
	"[DOUBLE]",
	"[DATETIME]",
	"[STR]",
	"[CSTR]",
	"[BIN]",
	"[ARY]",
	"[INTMAP]",
	"[STRMAP]",
	"[NOCASESTRMAP]",
	""
};

const char * ProtocolVariant::getTypeString(VariantType nVariantType)
{
	size_t nSize = sizeof(lpszTypeString) / sizeof(lpszTypeString[0]);
	if ( static_cast<size_t>(nVariantType) >= nSize )
		return "";
	return lpszTypeString[static_cast<size_t>(nVariantType)];
}

ProtocolVariant::VariantType ProtocolVariant::getVariantType(const char * pVariantType)
{
	return VARIANT_TYPE_NULL;
}

void ProtocolVariant::setNULL(void)
{
	if ( m_nVariantType == VARIANT_TYPE_NULL )
		return;

	if ( (m_nInternalFalgs & FLAGS_AUTODEL) != 0 )
	{
		switch ( m_nVariantType )
		{
		case VARIANT_TYPE_INT_64:
			delete m_varData.m_pInt64Value;
			break;
		case VARIANT_TYPE_DOUBLE:
			delete m_varData.m_pDoubleValue;
			break;
		case VARIANT_TYPE_DATATIME:
			delete m_varData.m_pDatatimeValue;
			break;
		case VARIANT_TYPE_STRING:
			delete m_varData.m_pStrValue;
			break;
		case VARIANT_TYPE_CHARSET_STRING:
			delete m_varData.m_pCharsetStrValue;
			break;
		case VARIANT_TYPE_BINARY:
			delete m_varData.m_pBinaryValue;
			break;
		case VARIANT_TYPE_ARRAY:
			delete m_varData.m_pArrayValue;
			break;
		case VARIANT_TYPE_INT_MAP:
			delete m_varData.m_pIntMapValue;
			break;
		case VARIANT_TYPE_STRING_MAP:
			delete m_varData.m_pStrMapValue;
			break;
		case VARIANT_TYPE_NOCASE_STRING_MAP:
			delete m_varData.m_pNocaseStrMapValue;
			break;
		}
	} //if ( m_nInternalFalgs & FLAGS_AUTODEL )

	m_nVariantType = VARIANT_TYPE_NULL;
	m_nInternalFalgs = static_cast<rfc_uint_8>( FLAGS_NONE );
	m_varData.m_nInt32Value = 0;
	m_varData.m_pInt64Value = NULL;
}

void ProtocolVariant::assignBinary(const void * lpBuffer, size_t nSize)
{
	BinaryStream & binStream = asBinary();
	binStream.clear();
	binStream.write(lpBuffer, nSize);
	binStream.seekg(0);
}

void ProtocolVariant::assign(const ProtocolVariant & tVar)
{
	if ( this == &tVar )
		return;

	switch( tVar.m_nVariantType )
	{
	case VARIANT_TYPE_NULL:
		setNULL();
		break;
	case VARIANT_TYPE_BOOL:
		assign(tVar.asBool());
		break;
	case VARIANT_TYPE_INT_32:
		assign(tVar.asInteger32());
		break;
	case VARIANT_TYPE_INT_64:
		assign(tVar.asInteger64());
		break;
	case VARIANT_TYPE_DOUBLE:
		assign(tVar.asDouble());
		break;
	case VARIANT_TYPE_DATATIME:
		assign(tVar.asDateTime());
		break;
	case VARIANT_TYPE_STRING:
		assign(tVar.asString());
		break;
	case VARIANT_TYPE_CHARSET_STRING:
		assign(tVar.asCharsetString());
		break;
	case VARIANT_TYPE_BINARY:
		assign(tVar.asBinary());
		break;
	case VARIANT_TYPE_ARRAY:
		assign(tVar.asArray());
		break;
	case VARIANT_TYPE_INT_MAP:
		assign(tVar.asIntMap());
		break;
	case VARIANT_TYPE_STRING_MAP:
		assign(tVar.asStringMap());
		break;
	case VARIANT_TYPE_NOCASE_STRING_MAP:
		assign(tVar.asNocaseStringMap());
		break;
	default:
		throw ProtocalVarException( FormatString("ProtocolVariant::assign unknown type:").arg(tVar.getTypeString()).str() );
		break;
	} //switch( tVar.m_nVariantType )
}

void ProtocolVariant::attach(stdstring * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_STRING, bAutoDelete, m_varData.m_pStrValue, pSrc);
}

void ProtocolVariant::attach(CharsetString * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_CHARSET_STRING, bAutoDelete, m_varData.m_pCharsetStrValue, pSrc);
}

void ProtocolVariant::attach(BinaryStream * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_BINARY, bAutoDelete, m_varData.m_pBinaryValue, pSrc);
}

void ProtocolVariant::attach(typeVariantArray * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_ARRAY, bAutoDelete, m_varData.m_pArrayValue, pSrc);
}

void ProtocolVariant::attach(typeIntVariantMap * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_INT_MAP, bAutoDelete, m_varData.m_pIntMapValue, pSrc);
}

void ProtocolVariant::attach(typeStringVariantMap * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_STRING_MAP, bAutoDelete, m_varData.m_pStrMapValue, pSrc);
}

void ProtocolVariant::attach(typeNocaseStringVariantMap * pSrc, bool bAutoDelete)
{
	attachImp(VARIANT_TYPE_NOCASE_STRING_MAP, bAutoDelete, m_varData.m_pNocaseStrMapValue, pSrc);
}

rfc_int_64 ProtocolVariant::getInteger64(void) const
{
	if ( isBool() )
		return asBool() ? 1 : 0;
	if ( isInteger32() )
		return asInteger32();
	return asInteger64();
}

size_t ProtocolVariant::arraySize(void) const
{
	if ( isArray() )
		return m_varData.m_pArrayValue->size();
	return 1;
}

size_t ProtocolVariant::mapSize(void) const
{
	if ( isIntMap() )
		return m_varData.m_pIntMapValue->size();
	else if ( isStringMap() )
		return m_varData.m_pStrMapValue->size();
	else if ( isNocaseStringMap() )
		return m_varData.m_pNocaseStrMapValue->size();
	return 0;
}

ProtocolVariant * ProtocolVariant::find(rfc_int_32 nKey)
{
	if ( !isIntMap() )
		return end();

	typeIntVariantMap::iterator it = asIntMap().find(nKey);
	if ( it != asIntMap().end() )
		return &it->second;
	return end();
}

const ProtocolVariant * ProtocolVariant::find(rfc_int_32 nKey) const
{
	if ( !isIntMap() )
		return end();

	typeIntVariantMap::const_iterator it = asIntMap().find(nKey);
	if ( it != asIntMap().end() )
		return &it->second;
	return end();
}

ProtocolVariant * ProtocolVariant::findStringKey(const stdstring & strKey, bool bCaseSensitive)
{
	if ( isStringMap() )
	{
		typeStringVariantMap::iterator it = m_varData.m_pStrMapValue->find(strKey);
		if ( it != m_varData.m_pStrMapValue->end() )	// use find method first
			return &it->second;

		for ( it = m_varData.m_pStrMapValue->begin(); !bCaseSensitive && it != m_varData.m_pStrMapValue->end(); ++it )
		{
			if ( StringFunc::strCaseEqual(strKey, it->first) )
				return &it->second;
		}
	} // if ( isStringMap() )

	if ( isNocaseStringMap() )
	{
		typeNocaseStringVariantMap::iterator it = m_varData.m_pNocaseStrMapValue->find(strKey);
		if ( it != m_varData.m_pNocaseStrMapValue->end() )
			return &it->second;
	}
	return end();
}

const ProtocolVariant * ProtocolVariant::findStringKey(const stdstring & strKey, bool bCaseSensitive) const
{
	if ( isStringMap() )
	{
		typeStringVariantMap::const_iterator it = m_varData.m_pStrMapValue->find(strKey);
		if ( it != m_varData.m_pStrMapValue->end() )	// use find method first
			return &it->second;

		for ( it = m_varData.m_pStrMapValue->begin(); !bCaseSensitive && it != m_varData.m_pStrMapValue->end(); ++it )
		{
			if ( StringFunc::strCaseEqual(strKey, it->first) )
				return &it->second;
		}
	} // if ( isStringMap() )

	if ( isNocaseStringMap() )
	{
		typeNocaseStringVariantMap::const_iterator it = m_varData.m_pNocaseStrMapValue->find(strKey);
		if ( it != m_varData.m_pNocaseStrMapValue->end() )
			return &it->second;
	}
	return end();
}

ProtocolVariant * ProtocolVariant::find(const stdstring & strKey)
{
	return findStringKey(strKey, true);
}

const ProtocolVariant * ProtocolVariant::find(const stdstring & strKey) const
{
	return findStringKey(strKey, true);
}

ProtocolVariant * ProtocolVariant::findIgnoreCase(const stdstring & strKey)
{
	return findStringKey(strKey, false);
}

const ProtocolVariant * ProtocolVariant::findIgnoreCase(const stdstring & strKey) const
{
	return findStringKey(strKey, false);
}

ProtocolVariant * ProtocolVariant::findByPath(const stdstring & strPath, bool bCaseSensitive, const stdstring & strDelimiter)
{
	stdstring strKey;
	StringTokenizer<stdstring> tokenPath(strPath.data(), strPath.size());
	tokenPath.setDelimiter(strDelimiter.c_str());
	ProtocolVariant * pResult = this;
	while ( pResult != end() && tokenPath.getNext(strKey) )
		pResult = pResult->findStringKey(strKey, bCaseSensitive);
	return pResult;
}

const ProtocolVariant *	ProtocolVariant::findByPath(const stdstring & strPath, bool bCaseSensitive, const stdstring & strDelimiter) const
{
	stdstring strKey;
	StringTokenizer<stdstring> tokenPath(strPath.data(), strPath.size());
	tokenPath.setDelimiter(strDelimiter.c_str());
	const ProtocolVariant * pResult = this;
	while ( pResult != end() && tokenPath.getNext(strKey) )
		pResult = pResult->findStringKey(strKey, bCaseSensitive);
	return pResult;
}

void ProtocolVariant::swap(ProtocolVariant & tVar)
{
	if ( this == &tVar )
		return;

	std::swap(m_nVariantType, tVar.m_nVariantType);
	std::swap(m_nInternalFalgs, tVar.m_nInternalFalgs);
	std::swap(m_varData.m_pInt64Value, tVar.m_varData.m_pInt64Value);
}

ProtocolVariant & ProtocolVariant::getArrayItem(size_t nIndex) throw(ProtocalVarException)
{
	if ( !isArray() && nIndex == 0 )
		return *this;
	if ( isArray() && nIndex < arraySize() )
		return asArray()[nIndex];
	throw ProtocalVarException( FormatString("ProtocolVariant::getArrayItem Out of bounds:%(%)").arg(nIndex).arg(arraySize()).str() );
}

const ProtocolVariant & ProtocolVariant::getArrayItem(size_t nIndex) const throw(ProtocalVarException)
{
	if ( !isArray() && nIndex == 0 )
		return *this;
	if ( isArray() && nIndex < arraySize() )
		return asArray()[nIndex];
	throw ProtocalVarException( FormatString("const ProtocolVariant::getArrayItem Out of bounds:%(%)").arg(nIndex).arg(arraySize()).str() );
}

ProtocolVariant & ProtocolVariant::set(rfc_int_32 nKey)
{
	return findOrInsertImp(nKey, asIntMap());
}

ProtocolVariant & ProtocolVariant::set(const stdstring & strKey)
{
	if ( isNocaseStringMap() )
		return findOrInsertImp(strKey, asNocaseStringMap());
	return findOrInsertImp(strKey, asStringMap());	//默认大小写敏感
}

ProtocolVariant & ProtocolVariant::add(rfc_int_32 nKey)
{
	return insertOrAppendImp(nKey, asIntMap());
}

ProtocolVariant & ProtocolVariant::add(const stdstring & strKey)
{
	if ( isNocaseStringMap() )
		return insertOrAppendImp(strKey, asNocaseStringMap());
	return insertOrAppendImp(strKey, asStringMap());	//默认大小写敏感
}

bool ProtocolVariant::erase(rfc_int_32 nKey)
{
	if ( isIntMap() )
		return ( asIntMap().erase(nKey) != 0 );
	return false;
}

bool ProtocolVariant::erase(const stdstring & strKey)
{
	if ( isStringMap() )
		return ( asStringMap().erase(strKey) != 0 );
	else if ( isNocaseStringMap() )
		return ( asNocaseStringMap().erase(strKey) != 0 );
	return false;
}

const ProtocolVariant & ProtocolVariant::get(rfc_int_32 nKey) const
{
	const ProtocolVariant * it = find(nKey);
	if ( it == end() )
		return g_varNULL;
	return (*it);
}

const ProtocolVariant & ProtocolVariant::get(const stdstring & strKey) const
{
	const ProtocolVariant * it = find(strKey);
	if ( it == end() )
		return g_varNULL;
	return (*it);
}

const ProtocolVariant & ProtocolVariant::getException(rfc_int_32 nKey) const throw(ProtocalVarException)
{
	const ProtocolVariant * it = find(nKey);
	if ( it == end() )
		throw ProtocalVarException(FormatString("int key [%] not found in variant").arg(nKey).str());
	return (*it);
}

const ProtocolVariant & ProtocolVariant::getException(const stdstring & strKey) const throw(ProtocalVarException)
{
	const ProtocolVariant * it = find(strKey);
	if ( it == end() )
		throw ProtocalVarException(FormatString("string key [%] not found in variant").arg(strKey).str());
	return (*it);
}

const stdstring & ProtocolVariant::getDefault(rfc_int_32 nKey, const stdstring & strDefault) const
{
	const ProtocolVariant * it = find(nKey);
	if ( it == end() || !it->isString() )
		return strDefault;
	return it->asString();
}

const stdstring & ProtocolVariant::getDefault(const stdstring & strKey, const stdstring & strDefault) const
{
	const ProtocolVariant * it = find(strKey);
	if ( it == end() || !it->isString() )
		return strDefault;
	return it->asString();
}

void ProtocolVariant::toString(stdstring & str) const
{
	switch( m_nVariantType )
	{
	case VARIANT_TYPE_NULL:
		str += "[NUL]";
		break;
	case VARIANT_TYPE_BOOL:
		str += m_varData.m_bBoolValue ? "[BOL]:True" : "[BOL]:False";
		break;
	case VARIANT_TYPE_INT_32:
		str += "[INT32]:";
		StringFunc::addIntToString(str, m_varData.m_nInt32Value);
		break;
	case VARIANT_TYPE_INT_64:
		str += "[INT64]:";
		StringFunc::addIntToString(str, *m_varData.m_pInt64Value);
		break;
	case VARIANT_TYPE_DOUBLE:
		str += FormatString("[DOUBLE]:%").arg(*m_varData.m_pDoubleValue).str();
		break;
	case VARIANT_TYPE_DATATIME:
		str += "[DATETIME]:";
		str += m_varData.m_pDatatimeValue->formatDateTime();
		break;
	case VARIANT_TYPE_STRING:
		str += "[STR(";
		StringFunc::addIntToString(str, m_varData.m_pStrValue->size());
		str += ")]:";
		str += *m_varData.m_pStrValue;
		break;
	case VARIANT_TYPE_CHARSET_STRING:
		str += "[CSTR(";
		StringFunc::addIntToString(str, m_varData.m_pCharsetStrValue->size());
		str += ")]:";
		str += m_varData.m_pCharsetStrValue->data();
		break;
	case VARIANT_TYPE_BINARY:
		str += "[BIN(";
		StringFunc::addIntToString(str, m_varData.m_pBinaryValue->size());
		str += ")]:";
		GlobalFunc::binaryToHex(m_varData.m_pBinaryValue->data(), m_varData.m_pBinaryValue->size(), str);
		break;
	case VARIANT_TYPE_ARRAY:
		{
			str += "[Ary(";
			StringFunc::addIntToString(str, m_varData.m_pArrayValue->size());
			str += ")]{";
			ProtocolVariant::typeVariantArray::const_iterator it = m_varData.m_pArrayValue->begin(), itEnd = m_varData.m_pArrayValue->end();
			for ( int i = 0; it != itEnd; ++i, ++it )
			{
				if ( it != m_varData.m_pArrayValue->begin() )
					str += ',';
				StringFunc::addIntToString(str, i);
				str += '=';
				it->toString(str);
			} // for ( int i = 0; it != itEnd; ++i, ++it )
			str += '}';
		}
		break;
	case VARIANT_TYPE_INT_MAP:
		{
			str += "[INTMAP(";
			StringFunc::addIntToString(str, m_varData.m_pIntMapValue->size());
			str += ")]{";
			ProtocolVariant::typeIntVariantMap::const_iterator it = m_varData.m_pIntMapValue->begin(), itEnd = m_varData.m_pIntMapValue->end();
			for ( int i = 0; it != itEnd; ++i, ++it )
			{
				if ( it != m_varData.m_pIntMapValue->begin() )
					str += ',';
				StringFunc::addIntToString(str, it->first);
				str += '=';
				it->second.toString(str);
			} // for ( int i = 0; it != itEnd; ++i, ++it )
			str += '}';
		}
		break;
	case VARIANT_TYPE_STRING_MAP:
		{
			str += "[STRMAP(";
			StringFunc::addIntToString(str, m_varData.m_pStrMapValue->size());
			str += ")]{";
			ProtocolVariant::typeStringVariantMap::const_iterator it = m_varData.m_pStrMapValue->begin(), itEnd = m_varData.m_pStrMapValue->end();
			for ( int i = 0; it != itEnd; ++i, ++it )
			{
				if ( it != m_varData.m_pStrMapValue->begin() )
					str += ',';
				str += it->first;
				str += '=';
				it->second.toString(str);
			} // for ( int i = 0; it != itEnd; ++i, ++it )
			str += '}';
		}
		break;
	case VARIANT_TYPE_NOCASE_STRING_MAP:
		{
			str += "[NOCASESTRMAP(";
			StringFunc::addIntToString(str, m_varData.m_pNocaseStrMapValue->size());
			str += ")]{";
			ProtocolVariant::typeNocaseStringVariantMap::const_iterator it = m_varData.m_pNocaseStrMapValue->begin(), itEnd = m_varData.m_pNocaseStrMapValue->end();
			for ( int i = 0; it != itEnd; ++i, ++it )
			{
				if ( it != m_varData.m_pNocaseStrMapValue->begin() )
					str += ',';
				str += it->first;
				str += '=';
				it->second.toString(str);
			} // for ( int i = 0; it != itEnd; ++i, ++it )
			str += '}';
		}
		break;
	default:
		break;
	} //switch( m_nVariantType )
}

//////////////////////////////////////////////////////////////////////////

void ProtocolVariant::assertVariantType(VariantType nVariantType) const throw (ProtocalVarTypeException)
{
	if ( !checkVariantType(nVariantType) )
		throw ProtocalVarTypeException(FormatString("Type not match, my:%, request:%").arg(getTypeString())
			.arg(ProtocolVariant::getTypeString(nVariantType)).str());
}

void ProtocolVariant::safeConvertVariant(VariantType nVariantType)
{
	if ( m_nVariantType == nVariantType )
		return;
	setNULL();

	m_nVariantType = nVariantType;
	m_nInternalFalgs = static_cast<rfc_uint_8>( FLAGS_AUTODEL );
	switch( nVariantType )
	{
	case VARIANT_TYPE_INT_64:
		m_varData.m_pInt64Value = new rfc_int_64(0);
		break;
	case VARIANT_TYPE_DOUBLE:
		m_varData.m_pDoubleValue = new double(0.0);
		break;
	case VARIANT_TYPE_DATATIME:
		m_varData.m_pDatatimeValue = new DateTime();
		break;
	case VARIANT_TYPE_STRING:
		m_varData.m_pStrValue = new stdstring();
		break;
	case VARIANT_TYPE_CHARSET_STRING:
		m_varData.m_pCharsetStrValue = new CharsetString();
		break;
	case VARIANT_TYPE_BINARY:
		m_varData.m_pBinaryValue = new BinaryStream();
		break;
	case VARIANT_TYPE_ARRAY:
		m_varData.m_pArrayValue = new typeVariantArray();
		break;
	case VARIANT_TYPE_INT_MAP:
		m_varData.m_pIntMapValue = new typeIntVariantMap();
		break;
	case VARIANT_TYPE_STRING_MAP:
		m_varData.m_pStrMapValue = new typeStringVariantMap();
		break;
	case VARIANT_TYPE_NOCASE_STRING_MAP:
		m_varData.m_pNocaseStrMapValue = new typeNocaseStringVariantMap();
		break;
	default:
		m_varData.m_nInt32Value = 0;
		break;
	} //switch( nVariantType )
}

ProtocolVariant::typeVariantArray & ProtocolVariant::convertToArray(void)
{
	if ( !isArray() )
	{
		ProtocolVariant tVar;
		swap(tVar);
		asArray().push_back(g_varNULL);
		asArray().back().swap(tVar);
	}
	return asArray();
}

int ProtocolVariant::compare(const ProtocolVariant & tVar) const
{
	if ( this == &tVar )
		return 0;
	if ( getVariantType() != tVar.getVariantType() )
		return static_cast<int>(getVariantType()) - static_cast<int>(tVar.getVariantType());

	switch( m_nVariantType )
	{
	case VARIANT_TYPE_BOOL:
		if ( m_varData.m_bBoolValue && !tVar.m_varData.m_bBoolValue )
			return 1;
		else if ( !m_varData.m_bBoolValue && tVar.m_varData.m_bBoolValue )
			return -1;
		break;
	case VARIANT_TYPE_INT_32:
		return m_varData.m_nInt32Value - tVar.m_varData.m_nInt32Value;
		break;
	case VARIANT_TYPE_INT_64:
		if ( *m_varData.m_pInt64Value > *tVar.m_varData.m_pInt64Value )
			return 1;
		else if ( *m_varData.m_pInt64Value < *tVar.m_varData.m_pInt64Value )
			return -1;
		break;
	/*case VARIANT_TYPE_DOUBLE:
		assign(tVar.asDouble());
		break;
	case VARIANT_TYPE_DATATIME:
		assign(tVar.asDateTime());
		break;*/
	case VARIANT_TYPE_STRING:
		return m_varData.m_pStrValue->compare(*tVar.m_varData.m_pStrValue);
		break;
	/*case VARIANT_TYPE_CHARSET_STRING:
		assign(tVar.asCharsetString());
		break;*/
	case VARIANT_TYPE_BINARY:
		if ( m_varData.m_pBinaryValue->size() != tVar.m_varData.m_pBinaryValue->size() )
			return static_cast<int>(m_varData.m_pBinaryValue->size() - tVar.m_varData.m_pBinaryValue->size());
		else if ( m_varData.m_pBinaryValue->size() == 0 )
			return 0;
		else 
			return memcmp(m_varData.m_pBinaryValue->data(), tVar.m_varData.m_pBinaryValue->data(), m_varData.m_pBinaryValue->size());
		break;
	/*case VARIANT_TYPE_ARRAY:
		assign(tVar.asArray());
		break;*/
	case VARIANT_TYPE_INT_MAP:
		return compare(*m_varData.m_pIntMapValue, *tVar.m_varData.m_pIntMapValue);
		break;
	case VARIANT_TYPE_STRING_MAP:
		return compare(*m_varData.m_pStrMapValue, *tVar.m_varData.m_pStrMapValue);
		break;
	case VARIANT_TYPE_NOCASE_STRING_MAP:
		return compare(*m_varData.m_pNocaseStrMapValue, *tVar.m_varData.m_pNocaseStrMapValue);
		break;
	default:
		break;
	} //switch( m_nVariantType )
	return 0;
}

RFC_NAMESPACE_END
