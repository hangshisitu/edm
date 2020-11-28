/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/database.h"
#include "database/mysqldb.h"
#include "base/deleter.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

DataBase::~DataBase(void)
{
	std::for_each(m_mapNameToEnv.begin(), m_mapNameToEnv.end(), SecondDeleter());
	m_mapNameToEnv.clear();
}

DataBase & DataBase::getInstance(void)
{
	static DataBase g_Database;
	return g_Database;
}

bool DataBase::initDatebase(void)
{
	if ( !m_mapNameToEnv.empty() )
		return true;
#if (SUPPORT_MYSQL_DB != 0)
	m_mapNameToEnv["mysql"] = new MysqlEnv;
#endif
	return true;
}

void DataBase::initDatebaseEx(void) throw(DBException)
{
	if ( !initDatebase() )
		throw DBException( stdstring("DataBase::initDatebase error:") + GlobalFunc::getSystemErrorInfo() );
}

DataBaseEnv	* DataBase::getDBEnv(const stdstring & strDriver) throw(DBException)
{
	typeDBEnvMap::iterator it = m_mapNameToEnv.find(strDriver);
	if ( it == m_mapNameToEnv.end() )
		throw DBException( FormatString("Database Driver[%] not init").arg(strDriver).str() );
	return it->second;
}

SafeDBConnection DataBase::getConnection(const DBEnvParams & dbEnvParam) throw(DBException)
{
	DBEnvParams::const_iterator itDriver = dbEnvParam.find(DBEnvParams::m_strDriverKey);
	if ( itDriver == dbEnvParam.end() )
		throw DBException("Database driver key not found");

	DataBaseEnv * pDBEnv = getDBEnv(itDriver->second);
	return pDBEnv->createConnection(dbEnvParam);
}

RFC_NAMESPACE_END
