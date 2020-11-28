/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DBFIELD_H_201008
#define RFC_DBFIELD_H_201008

#include "database/databaseenv.h"
#include <vector>
#include <memory>

RFC_NAMESPACE_BEGIN

enum enumSQLErrorCode
{
	SQLERR_ERROR			= 1101,
	SQLERR_INIT_ERROR		= 1102,
	SQLERR_CONNECT_ERROR	= 1103,
	SQLERR_QUERY_ERROR		= 1104,
	SQLERR_RECORD_ERROR		= 1105,
	SQLERR_TYPE_ERROR		= 1106,
	SQLERR_ALLOC_ERROR		= 1107,
	SQLERR_TYPE_NOTSUPPORT	= 1108,
	SQLERR_TYPE_CONVERTERR	= 1109,
	SQLERR_OUTOF_SEEK_RANGE = 1110,
	SQLERR_FETCH			= 1111,
};

class DBField;
class DBParam;
typedef std::auto_ptr<DBField>		typeAutoDBField;
typedef std::auto_ptr<DBParam>		typeAutoDBParam;

typedef std::vector<DBField *>		typeDBFieldList;
typedef std::vector<DBParam *>		typeDBParamList;

// Column
class DBField : private NonCopyAble
{
public:
	enum { DBFIELD_SHIFT_BITS = 8 };
	enum DBFieldType
	{
		DBFIELD_TYPE_NULL			= 0,

		DBFIELD_TYPE_BOOL			= ( 1 << DBFIELD_SHIFT_BITS ),

		DBFIELD_TYPE_INTEGER		= ( 2 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_SHORT,
		DBFIELD_TYPE_UNSIGNED,
		DBFIELD_TYPE_INT64,

		DBFIELD_TYPE_DECIMAL		= ( 3 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_CURRECY,

		DBFIELD_TYPE_DOUBLE			= ( 4 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_FLOAT,

		DBFIELD_TYPE_DATETIME		= ( 5 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_DATE,
		DBFIELD_TYPE_TIME,
		DBFIELD_TYPE_TIMESTAMP,

		DBFIELD_TYPE_STRING			= ( 6 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_VARSTRING,
		DBFIELD_TYPE_LONG_VARSTRING,

		DBFIELD_TYPE_BINARY			= ( 7 << DBFIELD_SHIFT_BITS ),
		DBFIELD_TYPE_CLOB,
		DBFIELD_TYPE_BLOB,
		DBFIELD_TYPE_LONG_BINARY,

		DBFIELD_TYPE_UNKNOWN		= 0xffffffff,
	};

	DBField(void);
	DBField(const stdstring & strFieldName, int nFieldMaxSize, int nOriginalFieldType, DBFieldType nLocaleFieldType);
	virtual ~DBField(void) {}

	//bool				isPrimaryKey(void) const = 0;

	virtual bool		isNULL(void) const = 0;
	virtual bool		asBool(void) const = 0;
	virtual	rfc_int_32	asInteger32(void) const = 0;
	virtual rfc_int_64	asInteger64(void) const = 0;
	virtual	double		asDouble(void) const = 0;
	virtual	bool		asDateTime(DateTime & dt) const = 0;
	virtual stdstring &	asString(stdstring & strFieldData) const = 0;
	virtual size_t		readBinary(void * lpBuffer, size_t nSize, size_t nBeginPos) const = 0;

	inline void			asValue(bool & bValue) { bValue = asBool(); }
	inline void			asValue(rfc_int_8 & nValue) { nValue = static_cast<rfc_int_8>(asInteger32()); }
	inline void			asValue(rfc_uint_8 & nValue) { nValue = static_cast<rfc_uint_8>(asInteger32()); }
	inline void			asValue(rfc_int_16 & nValue) { nValue = static_cast<rfc_int_16>(asInteger32()); }
	inline void			asValue(rfc_uint_16 & nValue) { nValue = static_cast<rfc_uint_16>(asInteger32()); }
	inline void			asValue(rfc_int_32 & nValue) { nValue = asInteger32(); }
	inline void			asValue(rfc_uint_32 & nValue) { nValue = static_cast<rfc_uint_32>(asInteger32()); }
	inline void			asValue(rfc_int_64 & nValue) { nValue = asInteger64(); }
	inline void			asValue(rfc_uint_64 & nValue) { nValue = static_cast<rfc_uint_64>(asInteger64()); }
	inline void			asValue(double & dValue) { dValue = asDouble(); }
	inline void			asValue(DateTime & dt) { asDateTime(dt); }
	inline void			asValue(stdstring & str) { asString(str); }

	inline time_t		asDateTime(void) const { DateTime dt; asDateTime(dt); return dt.getStandardTime(); }

	virtual const void * getFieldData(void) const = 0;
	virtual size_t		getFieldDataSize(void) const = 0;
	virtual void		attachFieldData(void * lpBuffer, size_t nSize) = 0;
	virtual void		setNULL(void) = 0;
	virtual void		afterFetchData(void) = 0;

	GETER_SETER_MEMBER(stdstring, FieldName);
	GETER_SETER_MEMBER(int, FieldMaxSize);
	GETER_SETER_MEMBER(int, OriginalFieldType);
	GETER_SETER_MEMBER(DBFieldType, LocaleFieldType);
};

//////////////////////////////////////////////////////////////////////////

class DBParam : private NonCopyAble
{
public:
	DBParam(void) {}
	DBParam(const stdstring & strParamName) : m_strParamName(strParamName) {}
	virtual ~DBParam(void) {}

	static inline bool	isBeginSymbolOfParam(char ch) { return ( ch == ':' ); }
	static inline bool	isValidParamChar(char ch) { return ( ::isalnum(ch) || ch == '_' ); }

	virtual void		setNULL(void) = 0;
	virtual void		assignInteger(rfc_int_64 nValue) = 0;
	virtual void		assignDouble(double dValue) = 0;
	virtual void		assignDateTime(const DateTime & dt) = 0;
	virtual void		assignString(const char * lpString, size_t nSize) = 0;
	virtual size_t		writeBinary(const void * lpBuffer, size_t nSize) = 0;

	inline void			assign(bool bValue) { assignInteger(bValue ? 1 : 0); }
	inline void			assign(rfc_int_8 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_uint_8 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_int_16 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_uint_16 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_int_32 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_uint_32 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_int_64 nValue) { assignInteger(nValue); }
	inline void			assign(rfc_uint_64 nValue) { assignInteger(static_cast<rfc_int_64>(nValue)); }
	inline void			assign(double dValue) { assignDouble(dValue); }
	inline void			assign(const DateTime & dt) { assignDateTime(dt); }

	inline void			assign(const char * lpString) { assignString(lpString, StringFunc::length(lpString)); }
	inline void			assign(const stdstring & str) { assignString(str.data(), str.size()); }
	inline void			assignString(const stdstring & str) { assignString(str.data(), str.size()); }

	const stdstring &	getParamName(void) const { return m_strParamName; }

	virtual const void * getParamData(void) const = 0;
	virtual size_t		getParamDataSize(void) const = 0;
	virtual void		afterExecSQL(void) = 0;

protected:
	stdstring			m_strParamName;
};

RFC_NAMESPACE_END

#endif	//RFC_DBFIELD_H_201008
