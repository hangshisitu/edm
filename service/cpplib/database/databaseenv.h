/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DATABASEENV_H_201008
#define RFC_DATABASEENV_H_201008

#include "system/shareptr.h"
#include "base/globalfunc.h"
#include "base/exception.h"
#include <map>

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(DBException, Exception)

class DBStatement;
class DBConnection;
typedef SharePtr<DBStatement>		SafeDBStatement;
typedef SharePtr<DBConnection>		SafeDBConnection;

class DBEnvParams : public std::map<stdstring, stdstring, IgnoreCaseStringCompare>
{
public:
	static const stdstring			m_strDriverKey;
	static const stdstring			m_strServerKey;
	static const stdstring			m_strPortKey;
	static const stdstring			m_strUserKey;
	static const stdstring			m_strPasswordKey;
	static const stdstring			m_strDatabaseKey;
	static const stdstring			m_strCharsetKey;
	static const stdstring			m_strEnvVariableKey;
	static const stdstring			m_strConnectionCountKey;
	static const stdstring			m_strTimeoutKey;
	static const stdstring			m_strConnectionMaxUsedCountKey;
	static const stdstring			m_strIdleTimeoutKey;
	static const stdstring			m_strRetryCountKey;

	typedef std::map<stdstring, stdstring, IgnoreCaseStringCompare> typeParentMap;
	void							setEnvParam(const stdstring & strKey, const stdstring & strValue);
	const stdstring &				getEnvParam(const stdstring & strKey) const throw(DBException);
	const stdstring &				getEnvParam(const stdstring & strKey, const stdstring & strDefault) const;
};

class DataBaseEnv : private NonCopyAble
{
public:
	virtual ~DataBaseEnv(void) {}

	virtual SafeDBConnection			createConnection(const DBEnvParams & dbEnvParam) = 0;
	virtual bool						encodeString(const stdstring & strSrc, stdstring & strDest) const = 0;
};

RFC_NAMESPACE_END

#endif	//RFC_DATABASEENV_H_201008
