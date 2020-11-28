/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "database/database.h"
#include "database/oracledb.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

static void printQueryResult(SafeDBStatement dbStmt)
{
	if ( dbStmt->getRecordCount() == 0 )
	{
		std::cout << "Empty set" << std::endl;
		return;
	}

	static const int nFieldWidth = 20;
	DBRecordset recordSet = dbStmt->getRecordset();
	std::cout << FormatString("%").argString(StringFunc::g_strEmptyString, FormatFlags::ADJUST_LEFT, nFieldWidth).str();
	size_t i = 0, nColumnCount = recordSet.getColumnCount();
	for ( i = 0 ; i < nColumnCount; ++i )
	{
		const DBField & dbField = recordSet.getField(i);
		std::cout << FormatString("%").argString(dbField.getFieldName(), FormatFlags::ADJUST_LEFT, nFieldWidth).str();
	}
	std::cout << std::endl;

	stdstring strField;
	size_t nFieldIndex = 0;
	while ( recordSet.fetch() )
	{
		strField = FormatString("[%]").arg(nFieldIndex++).str();
		std::cout << FormatString("%").argString(strField, FormatFlags::ADJUST_LEFT, nFieldWidth).str();
		for ( i = 0 ; i < nColumnCount; ++i )
		{
			const DBField & dbField = recordSet.getField(i);
			dbField.asString(strField);
			if ( strField.empty() )
				strField = "NULL";
			std::cout << FormatString("%|").argString(strField, FormatFlags::ADJUST_LEFT, nFieldWidth).str();
		}
		std::cout << std::endl;
	} // while ( recordSet.fetch() )
	std::cout << FormatString("% rows in set").arg(dbStmt->getRecordCount()).str() << std::endl << std::endl;
}

onUnitTest(DataBase)
{
	DataBase::getInstance().initDatebase();

	DBEnvParams mapDBParam;
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strDriverKey, "mysql") );
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strServerKey, "192.168.90.212") );
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strUserKey, "richmail") );
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strPasswordKey, "richmail139") );
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strDatabaseKey, "antispamdb") );
	mapDBParam.insert( DBEnvParams::value_type(DBEnvParams::m_strCharsetKey, "gbk") );

	SafeDBConnection dbConn = DataBase::getInstance().getConnection(mapDBParam);
	printQueryResult( dbConn->getTableList() );
	printQueryResult( dbConn->getExecStatement("select * from as_action", 1, 3) );

	DateTime dt;
	dt.setDateTime(2011, 1, 26, 14, 0, 0);
	stdstring strSQL = "select honeypot_id,email_addr,status,create_time,modify_time from as_honeypot where modify_time > :paramTime";
	SafeDBStatement dbStmt = dbConn->getStatement(strSQL);
	dbStmt->getParam("paramTime").assignDateTime( dt );
	dbStmt->execSQL();
	printQueryResult(dbStmt);
}

#if (SUPPORT_ORACLE_DB != 0)
onUnitTest(OracleDB)
{
	try
	{
		OracleDB oracleDB;
		if ( !oracleDB.connect("p_tdzx02", "n9y3BiBsqne5chZ$tDrFzm", "//192.168.18.121:1521/gzdev") )
		{
			std::cout << "OCCI connect fail: " << std::endl;
			return;
		}

		OracleStatement oracleStmt(&oracleDB);
		if ( !oracleStmt.execSQL("select NAME,ID from test") )
		{
			std::cout << "OCCI execute query fail: " << std::endl;
			return;
		}

		int i = 1;
		ResultSet *	pResultSet = NULL;
		while ( (pResultSet = oracleStmt.fetch()) != NULL )
		{
			int nID = pResultSet->getInt(2);
			std::string strName = pResultSet->getString(1);
			std::cout << "Row " << i << ", ID: " << nID << ", Name: " << strName << std::endl;
			++i;
		}

		if ( !oracleStmt.execSQL("select ID,Name from test where ID > 1") )
		{
			std::cout << "OCCI execute2 query fail: " << std::endl;
			return;
		}
		while ( (pResultSet = oracleStmt.fetch()) != NULL )
		{
			stdstring strID = pResultSet->getString(1);
			std::string strName = pResultSet->getString(2);
			std::cout << "Row " << i << ", strID: " << strID << ", Name: " << strName << std::endl;
			++i;
		}
	}
	catch (SQLException & e)
	{
		std::cout << "Catch SQLException: " << e.what() << std::endl;
	}
	catch ( ... )
	{
		std::cout << "Catch Unknown exception" << std::endl;
	}
}
#endif	// #if (SUPPORT_ORACLE_DB != 0)

RFC_NAMESPACE_END
