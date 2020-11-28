/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_PROTOCOLVARIANT_H_201008
#define RFC_PROTOCOLVARIANT_H_201008

#include "base/datetime.h"
#include "base/charsetstring.h"
#include "stream/binarystream.h"
#include "base/exception.h"

#include <vector>
#include <map>

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(ProtocalVarException, Exception)
DECLARE_EXCEPTION(ProtocalVarTypeException, ProtocalVarException)

class ProtocolVariant
{
public:
	typedef	std::vector<ProtocolVariant>											typeVariantArray;
	typedef std::map<rfc_int_32, ProtocolVariant>									typeIntVariantMap;
	typedef std::map<stdstring, ProtocolVariant>									typeStringVariantMap;
	typedef std::map<stdstring, ProtocolVariant, IgnoreCaseStringCompare>			typeNocaseStringVariantMap;

	enum VariantType
	{
		VARIANT_TYPE_NULL,
		VARIANT_TYPE_BOOL,
		VARIANT_TYPE_INT_32,
		VARIANT_TYPE_INT_64,
		VARIANT_TYPE_DOUBLE,
		VARIANT_TYPE_DATATIME,
		VARIANT_TYPE_STRING,
		VARIANT_TYPE_CHARSET_STRING,			//包含编码信息的字符串
		VARIANT_TYPE_BINARY,
		VARIANT_TYPE_ARRAY,
		VARIANT_TYPE_INT_MAP,
		VARIANT_TYPE_STRING_MAP,
		VARIANT_TYPE_NOCASE_STRING_MAP,
	};

	static const ProtocolVariant			g_varNULL;
	static const char *	getTypeString(VariantType nVariantType);
	static VariantType	getVariantType(const char * pVariantType);

	ProtocolVariant(void) : m_nVariantType(VARIANT_TYPE_NULL), m_nInternalFalgs(static_cast<rfc_uint_8>(FLAGS_NONE)) {}
	ProtocolVariant(VariantType nVariantType) : m_nVariantType(VARIANT_TYPE_NULL), m_nInternalFalgs(static_cast<rfc_uint_8>(FLAGS_NONE))
	{ safeConvertVariant(nVariantType); }
	ProtocolVariant(const ProtocolVariant & tVar) { initImp(tVar); }
	ProtocolVariant(bool bValue) { initImp(bValue); }
	ProtocolVariant(rfc_int_32 nValue) { initImp(nValue); }
	ProtocolVariant(rfc_uint_32 nValue) { initImp(static_cast<rfc_int_32>(nValue)); }
	ProtocolVariant(rfc_int_64 nValue) { initImp(nValue); }
	ProtocolVariant(rfc_uint_64 nValue) { initImp(static_cast<rfc_int_64>(nValue)); }
	ProtocolVariant(double dValue) { initImp(dValue); }
	ProtocolVariant(const DateTime & dt) { initImp(dt); }
	ProtocolVariant(const char * lpString) { initImp(StringFunc::g_strEmptyString); assign(lpString); }
	ProtocolVariant(const char * lpString, size_t nSize) { initImp(StringFunc::g_strEmptyString); assignString(lpString, nSize); }
	ProtocolVariant(const stdstring & str) { initImp(str); }
	ProtocolVariant(const CharsetString & cstr) { initImp(cstr); }
	ProtocolVariant(const BinaryStream & bin) { initImp(bin); }
	ProtocolVariant(const typeVariantArray & vecVar) { initImp(vecVar); }
	ProtocolVariant(const typeIntVariantMap & mapInt) { initImp(mapInt); }
	ProtocolVariant(const typeStringVariantMap & mapStr) { initImp(mapStr); }
	ProtocolVariant(const typeNocaseStringVariantMap & mapNocaseStr) { initImp(mapNocaseStr); }
	~ProtocolVariant(void) { setNULL(); }

	void				setNULL(void);
	void				assign(bool bValue) { asBool() = bValue; }
	void				assign(rfc_int_32 nValue) { asInteger32() = nValue; }
	void				assign(rfc_uint_32 nValue) { asInteger32() = static_cast<rfc_int_32>(nValue); }
	void				assign(rfc_int_64 nValue) { asInteger64() = nValue; }
	void				assign(rfc_uint_64 nValue) { asInteger64() = static_cast<rfc_int_64>(nValue); }
	void				assign(double dValue) { asDouble() = dValue; }
	void				assign(const DateTime & dt) { asDateTime() = dt; }
	void				assignString(const char * lpString, size_t nSize) { asString().assign(lpString, nSize); }
	void				assign(const char * lpString) { asString().assign(lpString); }
	void				assign(const stdstring & str) { asString() = str; }
	void				assign(const CharsetString & cstr) { asCharsetString() = cstr; }
	void				assignBinary(const void * lpBuffer, size_t nSize);
	void				assign(const BinaryStream & bin) { assignBinary(bin.data(), bin.size()); asBinary().seekg(bin.tellg()); }
	void				assign(const typeVariantArray & vecVar) { asArray() = vecVar; }
	void				assign(const typeIntVariantMap & mapInt) { asIntMap() = mapInt; }
	void				assign(const typeStringVariantMap & mapStr) { asStringMap() = mapStr; }
	void				assign(const typeNocaseStringVariantMap & mapNocaseStr) { asNocaseStringMap() = mapNocaseStr; }
	void				assign(const ProtocolVariant & tVar);

	template<class typeData>
	ProtocolVariant & operator = (const typeData & tVar)	{ assign(tVar); return *this; }
	ProtocolVariant & operator = (const ProtocolVariant & tVar) { assign(tVar); return *this; }

	void				attach(stdstring * pSrc, bool bAutoDelete);
	void				attach(CharsetString * pSrc, bool bAutoDelete);
	void				attach(BinaryStream * pSrc, bool bAutoDelete);
	void				attach(typeVariantArray * pSrc, bool bAutoDelete);
	void				attach(typeIntVariantMap * pSrc, bool bAutoDelete);
	void				attach(typeStringVariantMap * pSrc, bool bAutoDelete);
	void				attach(typeNocaseStringVariantMap * pSrc, bool bAutoDelete);

	inline bool			isNULL(void) const { return checkVariantType(VARIANT_TYPE_NULL); }
	inline bool			isBool(void) const { return checkVariantType(VARIANT_TYPE_BOOL); }
	inline bool			isInteger32(void) const { return checkVariantType(VARIANT_TYPE_INT_32); }
	inline bool			isInteger64(void) const { return checkVariantType(VARIANT_TYPE_INT_64); }
	inline bool			isDouble(void) const { return checkVariantType(VARIANT_TYPE_DOUBLE); }
	inline bool			isDateTime(void) const { return checkVariantType(VARIANT_TYPE_DATATIME); }
	inline bool			isString(void) const { return checkVariantType(VARIANT_TYPE_STRING); }
	inline bool			isCharsetString(void) const { return checkVariantType(VARIANT_TYPE_CHARSET_STRING); }
	inline bool			isBinary(void) const { return checkVariantType(VARIANT_TYPE_BINARY); }
	inline bool			isArray(void) const { return checkVariantType(VARIANT_TYPE_ARRAY); }
	inline bool			isIntMap(void) const { return checkVariantType(VARIANT_TYPE_INT_MAP); }
	inline bool			isStringMap(void) const { return checkVariantType(VARIANT_TYPE_STRING_MAP); }
	inline bool			isNocaseStringMap(void) const { return checkVariantType(VARIANT_TYPE_NOCASE_STRING_MAP); }

	bool &							asBool(void) { safeConvertVariant(VARIANT_TYPE_BOOL); return m_varData.m_bBoolValue; }
	rfc_int_32 &					asInteger32(void) { safeConvertVariant(VARIANT_TYPE_INT_32); return m_varData.m_nInt32Value; }
	rfc_int_64 &					asInteger64(void) { safeConvertVariant(VARIANT_TYPE_INT_64); return *m_varData.m_pInt64Value; }
	double &						asDouble(void) { safeConvertVariant(VARIANT_TYPE_DOUBLE); return *m_varData.m_pDoubleValue; }
	DateTime &						asDateTime(void) { safeConvertVariant(VARIANT_TYPE_DATATIME); return *m_varData.m_pDatatimeValue; }
	stdstring &						asString(void) { safeConvertVariant(VARIANT_TYPE_STRING); return *m_varData.m_pStrValue; }
	CharsetString &					asCharsetString(void) { safeConvertVariant(VARIANT_TYPE_CHARSET_STRING); return *m_varData.m_pCharsetStrValue; }
	BinaryStream &					asBinary(void) { safeConvertVariant(VARIANT_TYPE_BINARY); return *m_varData.m_pBinaryValue; }
	typeVariantArray &				asArray(void) { safeConvertVariant(VARIANT_TYPE_ARRAY); return *m_varData.m_pArrayValue; }
	typeIntVariantMap &				asIntMap(void) { safeConvertVariant(VARIANT_TYPE_INT_MAP); return *m_varData.m_pIntMapValue; }
	typeStringVariantMap &			asStringMap(void) { safeConvertVariant(VARIANT_TYPE_STRING_MAP); return *m_varData.m_pStrMapValue; }
	typeNocaseStringVariantMap &	asNocaseStringMap(void) { safeConvertVariant(VARIANT_TYPE_NOCASE_STRING_MAP); return *m_varData.m_pNocaseStrMapValue; }

	bool							asBool(void) const { assertVariantType(VARIANT_TYPE_BOOL); return m_varData.m_bBoolValue; }
	rfc_int_32						asInteger32(void) const { assertVariantType(VARIANT_TYPE_INT_32); return m_varData.m_nInt32Value; }
	rfc_int_64						asInteger64(void) const { assertVariantType(VARIANT_TYPE_INT_64); return *m_varData.m_pInt64Value; }
	double							asDouble(void) const { assertVariantType(VARIANT_TYPE_DOUBLE); return *m_varData.m_pDoubleValue; }
	const DateTime &				asDateTime(void) const { assertVariantType(VARIANT_TYPE_DATATIME); return *m_varData.m_pDatatimeValue; }
	const stdstring &				asString(void) const { assertVariantType(VARIANT_TYPE_STRING); return *m_varData.m_pStrValue; }
	const CharsetString &			asCharsetString(void) const { assertVariantType(VARIANT_TYPE_CHARSET_STRING); return *m_varData.m_pCharsetStrValue; }
	const BinaryStream &			asBinary(void) const { assertVariantType(VARIANT_TYPE_BINARY); return *m_varData.m_pBinaryValue; }
	const typeVariantArray &		asArray(void) const { assertVariantType(VARIANT_TYPE_ARRAY); return *m_varData.m_pArrayValue; }
	const typeIntVariantMap &		asIntMap(void) const { assertVariantType(VARIANT_TYPE_INT_MAP); return *m_varData.m_pIntMapValue; }
	const typeStringVariantMap &	asStringMap(void) const { assertVariantType(VARIANT_TYPE_STRING_MAP); return *m_varData.m_pStrMapValue; }
	const typeNocaseStringVariantMap &	asNocaseStringMap(void) const { assertVariantType(VARIANT_TYPE_NOCASE_STRING_MAP); return *m_varData.m_pNocaseStrMapValue; }

	typeVariantArray &	convertToArray(void);
	inline const char *	getTypeString(void) const { return getTypeString(static_cast<VariantType>(m_nVariantType)); }
	inline VariantType	getVariantType(void) const { return static_cast<VariantType>(m_nVariantType); }
	bool				getBool(void) const { return getInteger64() != 0; }
	rfc_int_32			getInteger32(void) const { return static_cast<rfc_int_32>( getInteger64() ); }
	rfc_int_64			getInteger64(void) const;
	size_t				arraySize(void) const;
	size_t				mapSize(void) const;

	inline ProtocolVariant * end(void) const { return NULL; }

	ProtocolVariant *			find(rfc_int_32 nKey);
	const ProtocolVariant *		find(rfc_int_32 nKey) const;

	ProtocolVariant *			find(const stdstring & strKey);
	const ProtocolVariant *		find(const stdstring & strKey) const;

	ProtocolVariant *			findIgnoreCase(const stdstring & strKey);
	const ProtocolVariant *		findIgnoreCase(const stdstring & strKey) const;

	ProtocolVariant *			findByPath(const stdstring & strPath, bool bCaseSensitive, const stdstring & strDelimiter = "/");
	const ProtocolVariant *		findByPath(const stdstring & strPath, bool bCaseSensitive, const stdstring & strDelimiter = "/") const;

	void	swap(ProtocolVariant & tVar);

	ProtocolVariant & getArrayItem(size_t nIndex) throw(ProtocalVarException);
	const ProtocolVariant & getArrayItem(size_t nIndex) const throw(ProtocalVarException);
	ProtocolVariant & operator [](size_t nIndex) throw(ProtocalVarException) { return getArrayItem(nIndex); }
	const ProtocolVariant & operator [](size_t nIndex) const throw(ProtocalVarException) { return getArrayItem(nIndex); }

	ProtocolVariant &	set(rfc_int_32 nKey);
	template<class typeData>
	ProtocolVariant &	set(rfc_int_32 nKey, const typeData & tVar)
	{ set(nKey).assign(tVar); return *this; }

	ProtocolVariant &	set(const stdstring & strKey);
	template<class typeData>
	ProtocolVariant &	set(const stdstring & strKey, const typeData & tVar)
	{ set(strKey).assign(tVar); return *this; }

	ProtocolVariant &	add(rfc_int_32 nKey);
	template<class typeData>
	ProtocolVariant &	add(rfc_int_32 nKey, const typeData & tVar)
	{ add(nKey).assign(tVar); return *this; }

	ProtocolVariant &	add(const stdstring & strKey);
	template<class typeData>
	ProtocolVariant &	add(const stdstring & strKey, const typeData & tVar)
	{ add(strKey).assign(tVar); return *this; }

	//return false: no element has been removed from the map
	bool				erase(rfc_int_32 nKey);
	bool				erase(const stdstring & strKey);

	//if not exist, will return g_varNULL
	const ProtocolVariant & get(rfc_int_32 nKey) const;
	const ProtocolVariant & get(const stdstring & strKey) const;

	// throw ProtocalVarException if not found
	const ProtocolVariant & getException(rfc_int_32 nKey) const throw(ProtocalVarException);
	const ProtocolVariant & getException(const stdstring & strKey) const throw(ProtocalVarException);

	template<typename typeInteger>
	typeInteger				getDefault(rfc_int_32 nKey, typeInteger nDefault) const
	{
		const ProtocolVariant * it = find(nKey);
		if ( it == end() )
			return nDefault;

		if ( it->isBool() || it->isInteger32() || it->isInteger64() )
			return static_cast<typeInteger>( it->getInteger64() );
		return nDefault;
	}

	template<typename typeInteger>
	typeInteger				getDefault(const stdstring & strKey, typeInteger nDefault) const
	{
		const ProtocolVariant * it = find(strKey);
		if ( it == end() )
			return nDefault;

		if ( it->isBool() || it->isInteger32() || it->isInteger64() )
			return static_cast<typeInteger>( it->getInteger64() );
		return nDefault;
	}

	bool			 		getDefault(rfc_int_32 nKey, bool bDefault) const
	{ int n = ( bDefault ? 1 : 0 ); return ( getDefault<int>(nKey, n) != 0 ); }
	bool			 		getDefault(const stdstring & strKey, bool bDefault) const
	{ int n = ( bDefault ? 1 : 0 ); return ( getDefault<int>(strKey, n) != 0 ); }

	const stdstring & 		getDefault(rfc_int_32 nKey, const stdstring & strDefault) const;
	const stdstring & 		getDefault(const stdstring & strKey, const stdstring & strDefault) const;

	bool operator ==	(const ProtocolVariant & tVar) const { return compare(tVar) == 0; }
	bool operator !=	(const ProtocolVariant & tVar) const { return compare(tVar) != 0; }
	bool operator >=	(const ProtocolVariant & tVar) const { return compare(tVar) >= 0; }
	bool operator <=	(const ProtocolVariant & tVar) const { return compare(tVar) <= 0; }
	bool operator >		(const ProtocolVariant & tVar) const { return compare(tVar) > 0; }
	bool operator <		(const ProtocolVariant & tVar) const { return compare(tVar) < 0; }

	void					toString(stdstring & str) const;
	friend std::ostream &  operator << (std::ostream & stream, const ProtocolVariant & tVar)
	{ stdstring str; tVar.toString(str); return stream << str; }

protected:
	template<class typeData>
	void				initImp(const typeData & tVar)
	{
		m_nVariantType = VARIANT_TYPE_NULL;
		m_nInternalFalgs = static_cast<rfc_uint_8>(FLAGS_NONE);
		assign(tVar);
	}

	template<typename typeData>
	void				attachImp(VariantType nVariantType, bool bAutoDelete, typeData * & pDest, typeData * pSrc)
	{
		setNULL();
		m_nVariantType = nVariantType;
		m_nInternalFalgs = ( bAutoDelete ? FLAGS_AUTODEL : FLAGS_NONE );
		pDest = pSrc;
	}

	ProtocolVariant * findStringKey(const stdstring & strKey, bool bCaseSensitive);
	const ProtocolVariant * findStringKey(const stdstring & strKey, bool bCaseSensitive) const;

	template<typename typeMap>
	ProtocolVariant & findOrInsertImp(const typename typeMap::key_type & aKey, typeMap & mapData)
	{
		typename typeMap::iterator it = mapData.find(aKey);
		if ( it == mapData.end() )
			return mapData.insert( typename typeMap::value_type(aKey, g_varNULL) ).first->second;
		return it->second;
	}

	template<typename typeMap>
	ProtocolVariant & insertOrAppendImp(const typename typeMap::key_type & aKey, typeMap & mapData)
	{
		typename typeMap::iterator it = mapData.find(aKey);
		if ( it == mapData.end() )
			return mapData.insert( typename typeMap::value_type(aKey, g_varNULL) ).first->second;

		typeVariantArray & ar = it->second.convertToArray();
		ar.push_back(g_varNULL);
		return ar.back();
	}

	bool				checkVariantType(VariantType nVariantType) const { return getVariantType() == nVariantType; }
	void				assertVariantType(VariantType nVariantType) const throw (ProtocalVarTypeException);
	void				safeConvertVariant(VariantType nVariantType);

	int					compare(const ProtocolVariant & tVar) const;

	template<typename typeMap>
	int					compare(const typeMap & mapData1, const typeMap & mapData2) const
	{
		if ( mapData1.size() != mapData2.size() )
			return static_cast<int>(mapData1.size()) - static_cast<int>(mapData2.size());

		typename typeMap::const_iterator it1, itEnd1 = mapData1.end();
		typename typeMap::const_iterator it2, itEnd2 = mapData2.end();
		for ( it1 = mapData1.begin(); it1 != itEnd1; ++it1 )
		{
			it2 = mapData2.find(it1->first);
			if ( it2 == itEnd2 )		//实际上这是不准确的,以后再改正
				return 1;
			int n = it1->second.compare(it2->second);
			if ( n != 0 )
				return n;
		} //for ( it1 = mapData1.begin(); it1 != itEnd1; ++it1 )
		return 0;
	}

private:
	enum	InternalFlags
	{
		FLAGS_NONE					= 0,
		FLAGS_AUTODEL				= 1,
	};
	union	VariantData
	{
		VariantData(void) : m_pInt64Value(NULL) {}

		bool								m_bBoolValue;
		rfc_int_32							m_nInt32Value;
		rfc_int_64 *						m_pInt64Value;
		double *							m_pDoubleValue;
		DateTime *							m_pDatatimeValue;
		stdstring *							m_pStrValue;
		CharsetString *						m_pCharsetStrValue;
		BinaryStream *						m_pBinaryValue;
		typeVariantArray *					m_pArrayValue;
		typeIntVariantMap *					m_pIntMapValue;
		typeStringVariantMap *				m_pStrMapValue;
		typeNocaseStringVariantMap *		m_pNocaseStrMapValue;
	};

	rfc_uint_8								m_nVariantType;
	rfc_uint_8								m_nInternalFalgs;
	VariantData								m_varData;
};

typedef ProtocolVariant PVariant;
RFC_NAMESPACE_END

#endif	//RFC_PROTOCOLVARIANT_H_201008

