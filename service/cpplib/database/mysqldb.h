/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "rfcdefine.h"

#if (SUPPORT_MYSQL_DB != 0)

#ifndef RFC_MYSQLDB_H_201008
#define RFC_MYSQLDB_H_201008

#include "database/dbconnection.h"
#include "mysql/mysql.h"
#include <vector>

RFC_NAMESPACE_BEGIN

class MysqlEnv : public DataBaseEnv
{
public:
	virtual ~MysqlEnv(void) {}
	virtual SafeDBConnection	createConnection(const DBEnvParams & dbEnvParam);
	virtual bool				encodeString(const stdstring & strSrc, stdstring & strDest) const;
};

class MysqlField : public DBField
{
public:
	MysqlField(void) : m_pFieldData(NULL), m_nDataSize(0) {}
	MysqlField(const stdstring & strFieldName, int nFieldMaxSize, int nOriginalFieldType, DBFieldType nLocaleFieldType);
	virtual ~MysqlField(void) {}

	virtual bool		isNULL(void) const;
	virtual bool		asBool(void) const;
	virtual	rfc_int_32	asInteger32(void) const;
	virtual rfc_int_64	asInteger64(void) const;
	virtual	double		asDouble(void) const;
	virtual	bool		asDateTime(DateTime & dt) const;
	virtual stdstring &	asString(stdstring & strFieldData) const;
	virtual size_t		readBinary(void * lpBuffer, size_t nSize, size_t nBeginPos) const;

	virtual const void * getFieldData(void) const;
	virtual size_t		getFieldDataSize(void) const;
	virtual void		attachFieldData(void * lpBuffer, size_t nSize);
	virtual void		setNULL(void);
	virtual void		afterFetchData(void);

protected:
	void *				m_pFieldData;
	size_t				m_nDataSize;
};

class MysqlParam : public DBParam
{
public:
	MysqlParam(void) : m_lpBuffer(NULL), m_nDataSize(0) {}
	MysqlParam(const stdstring & strParamName);
	virtual ~MysqlParam(void);

	virtual void		setNULL(void);
	virtual void		assignInteger(rfc_int_64 nValue);
	virtual void		assignDouble(double dValue);
	virtual void		assignDateTime(const DateTime & dt);
	virtual void		assignString(const char * lpString, size_t nSize);
	virtual size_t		writeBinary(const void * lpBuffer, size_t nSize);

	virtual const void * getParamData(void) const;
	virtual size_t		getParamDataSize(void) const;
	virtual void		afterExecSQL(void);

protected:
	char *				m_lpBuffer;
	size_t				m_nDataSize;				// m_lpBuffer保存数据的大小
};

class MysqlConnection;
class MysqlStatement : public DBStatement
{
public:
	MysqlStatement(MysqlConnection * pDBConnection);
	virtual ~MysqlStatement(void) { close();}

	virtual bool			prepareSQL(const stdstring & strSQL, size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize);
	virtual bool			execSQL(void);
	virtual bool			close(void);

	virtual bool			fetch(void);
	virtual off_t			tell(void) const { return m_nCurPos; }
	virtual bool			seek(off_t nOffset, seek_position nWhence = seek_begin);

	virtual size_t			getEffectRowCount(void) { return m_nEffectRowCount; }
	virtual size_t			getRecordCount(void) { return m_nRecordCount; }

protected:
	virtual typeAutoDBParam	createParam(const stdstring & strName);

	void					replaceParam(stdstring & strSQL, const DBParam & dbParam);
	void					buildResult(void);

protected:
	stdstring				m_strSQL;
	MYSQL_RES *				m_pResult;
	size_t					m_nEffectRowCount;
	size_t					m_nRecordCount;
	off_t					m_nCurPos;
};

class MysqlConnection : public DBConnection
{
public:
	MysqlConnection(MysqlEnv * pDBEnv) : DBConnection(pDBEnv), m_pMysql(NULL) {}
	virtual ~MysqlConnection(void) { close(); }

	bool					connect(const stdstring & strServer, const stdstring & strPort, const stdstring & strUser,
								const stdstring & strPassword, const stdstring & strDBName, const stdstring & strCharset, unsigned long nFlags);

	virtual SafeDBStatement	getTableList(const stdstring & strFilter = "");
	virtual void			close(void);

	virtual void			beginTransaction(void);
	virtual void			commitTransaction(void);
	virtual void			roolbackTransaction(void);

	virtual bool			ping(void);
	virtual int				getErrorCode(void) const;
	virtual stdstring		getErrorInfo(void) const;
	virtual stdstring		getDriverName(void) const;
	virtual stdstring		getDriverVersion(void) const;

	MYSQL *					getHandle(void);
	void					checkErrorException(const stdstring & strDebugInfo) const;

protected:
	virtual SafeDBStatement	createStatement(void);

protected:
	MYSQL *					m_pMysql;
};

RFC_NAMESPACE_END

#endif	//RFC_MYSQLDB_H_201008

#endif	// #if (SUPPORT_MYSQL_DB != 0)
