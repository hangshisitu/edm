/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#include "rfcdefine.h"

#if (SUPPORT_ORACLE_DB != 0)

#ifndef RFC_ORACLEDB_H_201108
#define RFC_ORACLEDB_H_201108

#include "base/string.h"
#include "oracle/occi.h"

RFC_NAMESPACE_BEGIN

using namespace oracle::occi;

class OracleDB
{
public:
	OracleDB(void);
	~OracleDB(void);

	bool					connect(const stdstring & strUser, const stdstring & strPasswd, const stdstring & strConnectDBPath);
	bool					connect(const stdstring & strUser, const stdstring & strPasswd,
								const stdstring & strServer, const stdstring & strDBName, const stdstring & strPort = "1521");
	void					disconnect(void);

	static bool				shouldReConnect(int nErrorCode);

	Environment *			getEnvironment(void) { return m_pEnv; }

	Connection *			getConnection(void) { return m_pConn; }

protected:
	bool					createEnvironment(void);

protected:
	Environment *			m_pEnv;
	Connection *			m_pConn;
};

class OracleStatement
{
public:
	OracleStatement(OracleDB * pOracleDB);
	~OracleStatement(void);

	OracleDB *				getOracleDB(void) { return m_pOracleDB; }

	bool					execSQL(const stdstring & strSQL);

	//ResultSet *			getResultSet(void) { return m_pResultSet; }
	ResultSet *				fetch(void);

	size_t					getEffectRowCount(void) const { return m_nEffectRowCount; }

protected:
	void					closeResultSet(void);

protected:
	OracleDB *				m_pOracleDB;
	Statement *				m_pStmt;
	ResultSet *				m_pResultSet;
	size_t					m_nEffectRowCount;
	//size_t				m_nRecordCount;
};

RFC_NAMESPACE_END

#endif	//RFC_ORACLEDB_H_201108

#endif	// #if (SUPPORT_ORACLE_DB != 0)
