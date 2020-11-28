/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/oracledb.h"

#if (SUPPORT_ORACLE_DB != 0)

#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

OracleDB::OracleDB(void) : m_pEnv(NULL), m_pConn(NULL)
{
	m_pEnv = Environment::createEnvironment(Environment::THREADED_MUTEXED);
}

OracleDB::~OracleDB(void)
{
	disconnect();
	Environment::terminateEnvironment(m_pEnv);
}

bool OracleDB::connect(const stdstring & strUser, const stdstring & strPasswd, const stdstring & strConnectDBPath)
{
	disconnect();
	m_pConn = m_pEnv->createConnection(strUser, strPasswd, strConnectDBPath);
	return ( m_pConn != NULL );
}

bool OracleDB::connect(const stdstring & strUser, const stdstring & strPasswd,
					   const stdstring & strServer, const stdstring & strDBName, const stdstring & strPort)
{
	return connect(strUser, strPasswd, FormatString("//%:%/%").arg(strServer).arg(strPort).arg(strDBName).str());
}

void OracleDB::disconnect(void)
{
	if ( m_pConn != NULL )
	{
		m_pEnv->terminateConnection(m_pConn);
		m_pConn = NULL;
	}
}

bool OracleDB::shouldReConnect(int nErrorCode)
{
	switch ( nErrorCode )
	{
	case	22:
	case   378:						 
	case   602:							 
	case   603:							 
	case   604:							 
	case   609:							 
	case  1012:							 
	case  1033:							 
	case  1034:
	case  1041:							 
	case  1043:							 
	case  1089:							 
	case  1090:							 
	case  1092:							 
	case  3113:							 
	case  3114:							 
	case  3122:							 
	case  3135:							 
	case  12153:
	case  12571:
	case  17002:
	case  27146:							 
	case  28511:
		return true;
		break;
	default:
		return false;
		break;
	} // switch ( nErrorCode )

	return true;
}

//////////////////////////////////////////////////////////////////////////

OracleStatement::OracleStatement(OracleDB * pOracleDB) : m_pOracleDB(pOracleDB), m_pStmt(NULL), m_pResultSet(NULL), m_nEffectRowCount(0)
{
	m_pStmt = pOracleDB->getConnection()->createStatement();
}

OracleStatement::~OracleStatement(void)
{
	closeResultSet();
	m_pOracleDB->getConnection()->terminateStatement(m_pStmt);
}

bool OracleStatement::execSQL(const stdstring & strSQL)
{
	m_nEffectRowCount = 0;
	closeResultSet();
	size_t nPos = strSQL.find_first_not_of(StringFunc::g_strSpaceString);
	if ( nPos == stdstring::npos )
		return false;

	// 执行数据库更新操作，包括insert、update、delete操作
	if ( StringFunc::strnCasecmp(strSQL.data() + nPos, "update", 6) == 0
		|| StringFunc::strnCasecmp(strSQL.data() + nPos, "insert", 6) == 0
		|| StringFunc::strnCasecmp(strSQL.data() + nPos, "delete", 6) == 0 )
	{
		m_nEffectRowCount = m_pStmt->executeUpdate(strSQL);
	}
	else
	{
		m_pResultSet = m_pStmt->executeQuery(strSQL);
	}

	return ( m_nEffectRowCount > 0 || m_pResultSet != NULL );
}

ResultSet * OracleStatement::fetch(void)
{
	if ( m_pResultSet == NULL )
		return NULL;
	if ( m_pResultSet->next() == ResultSet::END_OF_FETCH )
		return NULL;
	return m_pResultSet;
}

void OracleStatement::closeResultSet(void)
{
	if ( m_pResultSet != NULL )
	{
		m_pStmt->closeResultSet(m_pResultSet);
		m_pResultSet =NULL;
	}
}

RFC_NAMESPACE_END

#endif	// #if (SUPPORT_ORACLE_DB != 0)

