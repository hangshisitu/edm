/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DBCONNECTION_H_201008
#define RFC_DBCONNECTION_H_201008

#include "database/dbstatement.h"
#include <vector>

RFC_NAMESPACE_BEGIN

class DBConnection
{
public:
	DBConnection(DataBaseEnv * pDBEnv) : m_pDBEnv(pDBEnv) {}
	virtual ~DBConnection(void) {}

	SafeDBStatement			getStatement(const stdstring & strSQL = "", size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize);
	SafeDBStatement			getExecStatement(const stdstring & strSQL, size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize);
	virtual SafeDBStatement	getTableList(const stdstring & strFilter = "") = 0;
	virtual void			close(void) = 0;

	virtual void			beginTransaction(void) = 0;
	virtual void			commitTransaction(void) = 0;
	virtual void			roolbackTransaction(void) = 0;

	virtual bool			ping(void) = 0;
	virtual int				getErrorCode(void) const = 0;
	virtual stdstring		getErrorInfo(void) const = 0;
	virtual	stdstring		getDriverName(void) const = 0;
	virtual	stdstring		getDriverVersion(void) const = 0;

	DataBaseEnv *			getDBEnv(void);
	const DataBaseEnv *		getDBEnv(void) const;

	bool					encodeString(const stdstring & strSrc, stdstring & strDest) const { return getDBEnv()->encodeString(strSrc, strDest); }

protected:
	virtual SafeDBStatement	createStatement(void) = 0;

protected:
	DataBaseEnv *			m_pDBEnv;
};

RFC_NAMESPACE_END

#endif	//RFC_DBCONNECTION_H_201008
