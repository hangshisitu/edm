/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/databaseenv.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

const stdstring DBEnvParams::m_strDriverKey = "Driver";
const stdstring DBEnvParams::m_strServerKey = "Server";
const stdstring DBEnvParams::m_strPortKey = "Port";
const stdstring DBEnvParams::m_strUserKey = "User";
const stdstring DBEnvParams::m_strPasswordKey = "Password";
const stdstring DBEnvParams::m_strDatabaseKey = "Database";
const stdstring DBEnvParams::m_strCharsetKey = "Charset";
const stdstring DBEnvParams::m_strEnvVariableKey = "EnvVariable";
const stdstring DBEnvParams::m_strConnectionCountKey = "ConnectionCount";
const stdstring DBEnvParams::m_strTimeoutKey = "Timeout";
const stdstring DBEnvParams::m_strConnectionMaxUsedCountKey = "ConnectionMaxUsedCount";
const stdstring DBEnvParams::m_strIdleTimeoutKey = "IdleTimeout";
const stdstring DBEnvParams::m_strRetryCountKey = "RetryCount";

void DBEnvParams::setEnvParam(const stdstring & strKey, const stdstring & strValue)
{
	std::pair<typeParentMap::iterator, bool> itResult = typeParentMap::insert( typeParentMap::value_type(strKey, strValue) );
	if ( !itResult.second )
		itResult.first->second = strValue;
}

const stdstring & DBEnvParams::getEnvParam(const stdstring & strKey) const throw(DBException)
{
	typeParentMap::const_iterator it = typeParentMap::find(strKey);
	if ( it == typeParentMap::end() )
		throw DBException( "DBEnvParams::getEnvParam key not exist:" + strKey );
	return it->second;
}

const stdstring & DBEnvParams::getEnvParam(const stdstring & strKey, const stdstring & strDefault) const
{
	typeParentMap::const_iterator it = typeParentMap::find(strKey);
	if ( it == typeParentMap::end() )
		return strDefault;
	return it->second;
}


RFC_NAMESPACE_END
