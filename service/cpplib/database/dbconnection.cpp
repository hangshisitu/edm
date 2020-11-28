/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/dbconnection.h"

RFC_NAMESPACE_BEGIN

SafeDBStatement DBConnection::getStatement(const stdstring & strSQL, size_t nBegin, size_t nCount)
{
	SafeDBStatement auStatement( createStatement() );
	if ( auStatement.getConstPtr() == NULL )
		throw DBException("createStatement() return NULL in DBConnection::getStatement");
	if ( !strSQL.empty() )
		auStatement->prepareSQL(strSQL, nBegin, nCount);
	return auStatement;
}

SafeDBStatement DBConnection::getExecStatement(const stdstring & strSQL, size_t nBegin, size_t nCount)
{
	SafeDBStatement auStatement = getStatement(strSQL, nBegin, nCount);
	auStatement->execSQL();
	return auStatement;
}

DataBaseEnv * DBConnection::getDBEnv(void)
{
	if ( m_pDBEnv == NULL )
		throw DBException("m_pDBEnv is NULL in DBConnection::getDBEnv");
	return m_pDBEnv;
}

const DataBaseEnv * DBConnection::getDBEnv(void) const
{
	if ( m_pDBEnv == NULL )
		throw DBException("m_pDBEnv is NULL in DBConnection::getDBEnv");
	return m_pDBEnv;
}

RFC_NAMESPACE_END
