/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/mysqldb.h"

#if (SUPPORT_MYSQL_DB != 0)

#include "stream/streambase.h"
#include "base/formatstream.h"
#include "base/autoarray.h"

RFC_NAMESPACE_BEGIN

SafeDBConnection MysqlEnv::createConnection(const DBEnvParams & dbEnvParam)
{
	static const stdstring g_strDefaultPort = "3306";
	const stdstring & strServer = dbEnvParam.getEnvParam(DBEnvParams::m_strServerKey);
	const stdstring & strPort = dbEnvParam.getEnvParam(DBEnvParams::m_strPortKey, g_strDefaultPort);
	const stdstring & strUser = dbEnvParam.getEnvParam(DBEnvParams::m_strUserKey);
	const stdstring & strPassword = dbEnvParam.getEnvParam(DBEnvParams::m_strPasswordKey);
	const stdstring & strDBName = dbEnvParam.getEnvParam(DBEnvParams::m_strDatabaseKey);
	const stdstring & strCharset = dbEnvParam.getEnvParam(DBEnvParams::m_strCharsetKey, StringFunc::g_strEmptyString);
	unsigned long nFlags = CLIENT_FOUND_ROWS;

	std::auto_ptr<MysqlConnection> auConnection( new MysqlConnection(this) );
	if ( !auConnection->connect(strServer, strPort, strUser, strPassword, strDBName, strCharset, nFlags) )
		auConnection->checkErrorException("connect");
	return SafeDBConnection( auConnection.release() );
}

bool MysqlEnv::encodeString(const stdstring & strSrc, stdstring & strDest) const
{
	size_t nSize = strSrc.size() * 2 + 10;
	AutoArray<char> auBuffer( new char[nSize] );
	memset(auBuffer.get(), 0, nSize);
	mysql_escape_string(auBuffer.get(), strSrc.data(), strSrc.size());
	strDest = auBuffer.get();
	return true;
}

//////////////////////////////////////////////////////////////////////////

MysqlField::MysqlField(const stdstring & strFieldName, int nFieldMaxSize, int nOriginalFieldType, DBFieldType nLocaleFieldType)
: DBField(strFieldName, nFieldMaxSize, nOriginalFieldType, nLocaleFieldType), m_pFieldData(NULL), m_nDataSize(0)
{
}

bool MysqlField::isNULL(void) const
{
	return ( m_pFieldData == NULL );
}

bool MysqlField::asBool(void) const
{
	return ( asInteger32() != 0 );
}

rfc_int_32 MysqlField::asInteger32(void) const
{
	stdstring strData;
	const stdstring & str = asString(strData);
	return StringFunc::stringToInt<rfc_int_32>(str.data(), str.size());
}

rfc_int_64 MysqlField::asInteger64(void) const
{
	stdstring strData;
	const stdstring & str = asString(strData);
	return StringFunc::stringToInt<rfc_int_64>(str.data(), str.size());
}

double MysqlField::asDouble(void) const
{
	stdstring strData;
	return atof( asString(strData).c_str() );
}

bool MysqlField::asDateTime(DateTime & dt) const
{
	static const char * g_pFormat[] = 
	{
		"%Y-%m-%d %H:%M:%S",
		"%Y-%m-%d",
		"%H:%M:%S",

		"%y",
		"%y%m",
		"%y%m%d",
		"%Y%m%d",
		"%y%m%d%H%M",
		"%y%m%d%H%M%S",
		"%Y%m%d%H%M%S",
	};
	static const int g_nFormatCount = sizeof(g_pFormat) / sizeof(g_pFormat[0]);

	int nType = ( static_cast<int>(getLocaleFieldType()) & DBFIELD_TYPE_DATETIME );
	if ( nType != DBFIELD_TYPE_DATETIME )
		throw DBException( FormatString("Field [%] is not DateTime in MysqlField::asDateTime, type:%")
		.arg(getFieldName()).arg(nType >> DBFIELD_SHIFT_BITS).str() );
	if ( m_pFieldData == NULL || m_nDataSize == 0 )
	{
		dt.setNULL();
		return true;
	}

	int nSubType = ( static_cast<int>(getLocaleFieldType()) & ( (1 << DBFIELD_SHIFT_BITS) - 1 ) );
	if ( nSubType < 0 || nSubType >= g_nFormatCount )
		throw DBException( FormatString("Field [%] type error in MysqlField::asDateTime, subtype:%")
		.arg(getFieldName()).arg(nSubType).str() );

	return dt.parseDateTime(g_pFormat[nSubType], reinterpret_cast<const char *>(m_pFieldData), m_nDataSize);
}

stdstring & MysqlField::asString(stdstring & strFieldData) const
{
	if ( m_pFieldData == NULL )
		strFieldData.clear();
	else
		strFieldData.assign(reinterpret_cast<const char *>(m_pFieldData), m_nDataSize);
	return strFieldData;
}

size_t MysqlField::readBinary(void * lpBuffer, size_t nSize, size_t nBeginPos) const
{
	if ( m_pFieldData == NULL || nBeginPos >= m_nDataSize )
		return 0;

	size_t nNeedCopySize = m_nDataSize - nBeginPos;
	if ( nNeedCopySize > nSize )
		nNeedCopySize = nSize;
	memcpy(lpBuffer, reinterpret_cast<const char *>(m_pFieldData) + nBeginPos, nNeedCopySize);
	return nNeedCopySize;
}

const void * MysqlField::getFieldData(void) const
{
	return m_pFieldData;
}

size_t MysqlField::getFieldDataSize(void) const
{
	return m_nDataSize;
}

void MysqlField::attachFieldData(void * lpBuffer, size_t nSize)
{
	m_pFieldData = lpBuffer;
	m_nDataSize = ( lpBuffer == NULL ) ? 0 : nSize;
}

void MysqlField::setNULL(void)
{
	attachFieldData(NULL, 0);
}

void MysqlField::afterFetchData(void)
{
}

//////////////////////////////////////////////////////////////////////////

MysqlParam::MysqlParam(const stdstring & strParamName) : DBParam(strParamName), m_lpBuffer(NULL), m_nDataSize(0)
{
}

MysqlParam::~MysqlParam(void)
{
	setNULL();
}

void MysqlParam::setNULL(void)
{
	delete []m_lpBuffer;
	m_lpBuffer = NULL;
	m_nDataSize = 0;
}

void MysqlParam::assignInteger(rfc_int_64 nValue)
{
	assign( StringFunc::intToString(nValue) );
}

void MysqlParam::assignDouble(double dValue)
{
	assign( FormatString("%").arg(dValue).str() );
}

void MysqlParam::assignDateTime(const DateTime & dt)
{
	if ( dt.isNULL() )
		setNULL();
	else
		assign( dt.formatDateTime() );
}

void MysqlParam::assignString(const char * lpString, size_t nSize)
{
	setNULL();
	writeBinary(lpString, nSize);
}

// 检查最后一个字符是汉字并且汉字的第二个字节是反斜杆
static bool isLastCharFakeBackslash(const void * lpBuffer, size_t nSize)
{
	rfc_uint_8 chBackslash = '\\';
	const rfc_uint_8 * lpBegin = reinterpret_cast<const rfc_uint_8 *>(lpBuffer);
	const rfc_uint_8 * lpEnd = lpBegin + nSize;
	if ( (lpBegin + 2 > lpEnd) || *(lpEnd - 1) != chBackslash )
		return false;

	bool bLastCharisFakeBackslash = false;	
	for ( ; lpBegin < lpEnd; ++lpBegin )
	{
		bLastCharisFakeBackslash = false;
		if ( lpBegin + 1 < lpEnd && (*lpBegin) > static_cast<rfc_uint_8>(0x7f) )
		{
			++lpBegin;
			bLastCharisFakeBackslash = ( (*lpBegin) == chBackslash );
		}
	} // for ( ; lpBegin < lpEnd; ++lpBegin )
	return bLastCharisFakeBackslash;
}

size_t MysqlParam::writeBinary(const void * lpBuffer, size_t nSize)
{
	//方法1 如果最后一个字符是汉字，并且汉字第二个字节是反斜杆\，在其后面加个空格后再调用mysql_escape_string转义
	//方法2 如果最后一个字符是汉字，并且汉字第二个字节是反斜杆\，转义后判断是否出现连续两个反斜杆，去掉最后一个反斜杆
	//方法3 如果最后一个字符是汉字，并且汉字第二个字节是反斜杆\，转义后在其后面再加个空格
	bool bLastCharisFakeBackslash = isLastCharFakeBackslash(lpBuffer, nSize);
	size_t nNeedSize = (nSize + 3) * 2 + m_nDataSize;
	AutoArray<char> auBuffer( new char[nNeedSize] );
	memset(auBuffer.get(), 0, nNeedSize);
	if ( m_lpBuffer != NULL )
	{
		memcpy(auBuffer.get(), m_lpBuffer, m_nDataSize);
		delete []m_lpBuffer;
	}
	m_lpBuffer = auBuffer.release();
	size_t nAddSize = static_cast<size_t>( mysql_escape_string(m_lpBuffer + m_nDataSize, reinterpret_cast<const char *>(lpBuffer), nSize) );
	m_nDataSize += nAddSize;
	// 如果最后一个字符是汉字，并且汉字第二个字节是反斜杆\，转义后在其后面再加个空格
	if ( bLastCharisFakeBackslash )
		m_lpBuffer[m_nDataSize++] = ' ';
	m_lpBuffer[m_nDataSize] = RFC_CHAR_NULL;
	m_lpBuffer[m_nDataSize + 1] = RFC_CHAR_NULL;
	return nSize;
}

const void * MysqlParam::getParamData(void) const
{
	return m_lpBuffer;
}

size_t MysqlParam::getParamDataSize(void) const
{
	return m_nDataSize;
}

void MysqlParam::afterExecSQL(void)
{
	setNULL();
}

//////////////////////////////////////////////////////////////////////////

MysqlStatement::MysqlStatement(MysqlConnection * pDBConnection)
: DBStatement(pDBConnection), m_pResult(NULL), m_nEffectRowCount(0), m_nRecordCount(0), m_nCurPos(-1)
{
}

bool MysqlStatement::prepareSQL(const stdstring & strSQL, size_t nBegin, size_t nCount)
{
	prepareParam(strSQL);
	m_strSQL = strSQL;
	//StringFunc::trimLeft(m_strSQL, StringFunc::g_strSpaceString);
	if ( nBegin != 0 && nCount == GlobalConstant::g_uMaxSize )
		m_strSQL += FormatString(" limit %,-1").arg(nBegin).str();
	else if ( nBegin != 0 && nCount != GlobalConstant::g_uMaxSize )
		m_strSQL += FormatString(" limit %,%").arg(nBegin).arg(nCount).str();
	else if ( nBegin == 0 && nCount != GlobalConstant::g_uMaxSize )
		m_strSQL += FormatString(" limit %").arg(nCount).str();

	return true;
}

bool MysqlStatement::execSQL(void)
{
	if ( m_strSQL.empty() )
		throw DBException("prepared sql is empty in MysqlStatement::execSQL");
	MysqlConnection * pMysqlConn = reinterpret_cast<MysqlConnection *>( m_pDBConnection );
	if ( pMysqlConn == NULL )
		throw DBException("mysql connection is NULL in MysqlStatement::execSQL");

	MYSQL * pMysqlHandle = pMysqlConn->getHandle();
	if ( pMysqlHandle == NULL )
		throw DBException("mysql handle is NULL in MysqlStatement::execSQL");

	m_nEffectRowCount = m_nRecordCount = 0;
	m_nCurPos = -1;
	clearFieldList();

	stdstring strRealSQL(m_strSQL);
	size_t i = 0, nParamCount = getParamCount();
	for ( i = 0; i < nParamCount; ++i )
		replaceParam(strRealSQL, getParam(i));

	if ( mysql_query(pMysqlHandle, strRealSQL.c_str()) != 0 )
	{
		pMysqlConn->checkErrorException(strRealSQL);
	}
	else if ( StringFunc::strnCasecmp(m_strSQL.data(), "select ", 7) == 0 ||
			  StringFunc::strnCasecmp(m_strSQL.data(), "show ", 5) == 0 ||
			  StringFunc::strnCasecmp(m_strSQL.data(), "desc ", 5) == 0 )
	{
		if ( m_pResult != NULL )
		{
			mysql_free_result(m_pResult);
			m_pResult = NULL;
		}
		m_pResult = mysql_store_result(pMysqlHandle);
		if ( m_pResult == NULL )
			pMysqlConn->checkErrorException("m_pResult == NULL");

		buildResult();
		m_nRecordCount = static_cast<size_t>( mysql_num_rows(m_pResult) );
	}
	else
	{
		m_nEffectRowCount = static_cast<size_t>( mysql_affected_rows(pMysqlHandle) );
	}

	return true;
}

bool MysqlStatement::close(void)
{
	if ( m_pDBConnection == NULL )
		return true;

	clearFieldList();
	clearParamList();
	m_strSQL.clear();
	if ( m_pResult != NULL )
	{
		mysql_free_result(reinterpret_cast<MYSQL_RES *>(m_pResult));
		m_pResult = NULL;
	}
	m_nRecordCount = m_nEffectRowCount = 0;
	m_nCurPos = -1;
	m_pDBConnection = NULL;
	return true;
}

bool MysqlStatement::fetch(void)
{
	MysqlConnection * pMysqlConn = reinterpret_cast<MysqlConnection *>( m_pDBConnection );
	if ( pMysqlConn == NULL )
		throw DBException("mysql connection is NULL in MysqlStatement::fetch");

	size_t nFieldCount = getFieldList().size();
	if ( m_pResult == NULL || m_nRecordCount == 0 || nFieldCount == 0 )
		return false;

	MYSQL_ROW pMysqlRow = mysql_fetch_row(m_pResult);
	unsigned long * pFieldLength = mysql_fetch_lengths(m_pResult);
	if ( pMysqlRow == NULL || pFieldLength == NULL )
		return false;

	for ( size_t i = 0; i < nFieldCount; ++i )
		getField(i).attachFieldData(pMysqlRow[i], static_cast<size_t>(pFieldLength[i]));
	++m_nCurPos;
	return true;
}

bool MysqlStatement::seek(off_t nOffset, seek_position nWhence)
{
	off_t nNewPos = StreamBase::getNewSeekPosition(m_nCurPos, static_cast<off_t>(m_nRecordCount), nOffset, nWhence);
	if ( nNewPos < 0 || nNewPos >= static_cast<off_t>(m_nRecordCount) )
		return false;

	mysql_data_seek(m_pResult, static_cast<my_ulonglong>(nNewPos));
	m_nCurPos = nNewPos;
	return fetch();
}

void MysqlStatement::replaceParam(stdstring & strSQL, const DBParam & dbParam)
{
	const stdstring & strParamName = dbParam.getParamName();
	stdstring strSwap;
	bool bQuoteMark = false;
	const char * const pEnd = strSQL.data() + strSQL.size();
	for ( const char * p = strSQL.data(); p < pEnd; ++p )
	{
		char ch = (*p);
		if ( bQuoteMark && ch == '\\' )
			++p;
		else if ( bQuoteMark )
			bQuoteMark = ( ch != '\'' );
		else if ( ch == '\'' )
			bQuoteMark = true;
		else if ( DBParam::isBeginSymbolOfParam(ch) )
		{
			const char * pParamEnd = p + 1 + strParamName.size();
			if ( pParamEnd <= pEnd &&
				 !DBParam::isValidParamChar(*pParamEnd) &&
				 StringFunc::strnCasecmp(p + 1, strParamName.data(), strParamName.size()) == 0 )
			{
				if ( dbParam.getParamData() == NULL )
					strSwap += "NULL";
				else
				{
					strSwap += '\'';
					strSwap.append(reinterpret_cast<const char *>(dbParam.getParamData()), dbParam.getParamDataSize());
					strSwap += '\'';
				}
				p += strParamName.size();
				continue;
			}
		} // else if ( DBParam::isBeginSymbolOfParam(ch) )

		strSwap += ch;
		if ( bQuoteMark && ch == '\\' )
			strSwap += (*p);
	} // for ( const char * p = strSQL.data(); p < pEnd; ++p )

	strSwap.swap(strSQL);
}

void MysqlStatement::buildResult(void)
{
	if ( m_pResult == NULL )
		throw DBException("m_pResult is NULL in MysqlStatement::buildResult");

	clearFieldList();
	size_t nFieldCount = static_cast<size_t>( mysql_num_fields(m_pResult) );
	MYSQL_FIELD * pMysqlFieldList = mysql_fetch_fields(m_pResult);
	stdstring strFieldName;
	int nFieldMaxSize = 0, nMysqlType = 0;
	DBField::DBFieldType nLocaleType = DBField::DBFIELD_TYPE_UNKNOWN;
	for ( size_t i = 0; i < nFieldCount; ++i )
	{
		strFieldName = pMysqlFieldList[i].name;
		nFieldMaxSize = static_cast<int>( pMysqlFieldList[i].max_length ) + 1;
		nMysqlType = static_cast<int>( pMysqlFieldList[i].type );

		if ( nMysqlType == FIELD_TYPE_TINY || nMysqlType == FIELD_TYPE_SHORT )
			nLocaleType = DBField::DBFIELD_TYPE_SHORT;
		else if ( nMysqlType == FIELD_TYPE_LONG || nMysqlType == FIELD_TYPE_INT24 || nMysqlType == FIELD_TYPE_YEAR )
			nLocaleType = DBField::DBFIELD_TYPE_INTEGER;
		else if ( nMysqlType == FIELD_TYPE_LONGLONG || nMysqlType == FIELD_TYPE_DECIMAL )
			nLocaleType = DBField::DBFIELD_TYPE_INT64;
		else if ( nMysqlType == FIELD_TYPE_FLOAT || nMysqlType == FIELD_TYPE_DOUBLE )
			nLocaleType = DBField::DBFIELD_TYPE_DECIMAL;
		else if ( nMysqlType == FIELD_TYPE_TIMESTAMP )
			nLocaleType = DBField::DBFIELD_TYPE_TIMESTAMP;
		else if ( nMysqlType == FIELD_TYPE_DATE )
			nLocaleType = DBField::DBFIELD_TYPE_DATE;
		else if ( nMysqlType == FIELD_TYPE_TIME )
			nLocaleType = DBField::DBFIELD_TYPE_TIME;
		else if ( nMysqlType == FIELD_TYPE_DATETIME )
			nLocaleType = DBField::DBFIELD_TYPE_DATETIME;
		else if ( nMysqlType == FIELD_TYPE_TINY_BLOB || nMysqlType == FIELD_TYPE_MEDIUM_BLOB
			|| nMysqlType == FIELD_TYPE_LONG_BLOB || nMysqlType == FIELD_TYPE_BLOB )
			nLocaleType = DBField::DBFIELD_TYPE_BLOB;
		else
			nLocaleType = DBField::DBFIELD_TYPE_VARSTRING;

		typeAutoDBField auField( new MysqlField(strFieldName, nFieldMaxSize, nMysqlType, nLocaleType) );
		addField(auField);
	} // for ( size_t i = 0; i < nFieldCount; ++i )
}

typeAutoDBParam MysqlStatement::createParam(const stdstring & strName)
{
	return typeAutoDBParam( new MysqlParam(strName) );
}

//////////////////////////////////////////////////////////////////////////

bool MysqlConnection::connect(const stdstring & strServer, const stdstring & strPort, const stdstring & strUser,
							 const stdstring & strPassword, const stdstring & strDBName, const stdstring & strCharset, unsigned long nFlags)
{
	close();
	m_pMysql = new MYSQL;
	if ( mysql_init(m_pMysql) == NULL )
		throw DBException(SQLERR_INIT_ERROR, "MYSQL init error");
	if ( !strCharset.empty() )
		mysql_options(m_pMysql, MYSQL_SET_CHARSET_NAME, strCharset.c_str());

	unsigned int uPort = StringFunc::stringToInt<unsigned int>(strPort.data(), strPort.size());
	if ( mysql_real_connect(m_pMysql, strServer.c_str(), strUser.c_str(), strPassword.c_str(), strDBName.c_str(), uPort, NULL, nFlags) == NULL )
	{
		fprintf(stderr, "err:%s\n", mysql_error(m_pMysql));
		return false;
		checkErrorException("mysql_real_connect");
	}
	return true;
}

SafeDBStatement MysqlConnection::getTableList(const stdstring & strFilter)
{
	SafeDBStatement stmt( createStatement() );
	stdstring strSQL = "show tables";
	if ( !strFilter.empty() )
	{
		stdstring strEncodeFilter;
		encodeString(strFilter, strEncodeFilter);
		strSQL += " like '";
		strSQL += strEncodeFilter;
		strSQL += '\'';
	}

	stmt->execSQLDirect(strSQL);
	return stmt;
}

void MysqlConnection::close(void)
{
	if ( m_pMysql != NULL )
	{
		mysql_close(m_pMysql);
		delete m_pMysql;
		m_pMysql = NULL;
	}
}

void MysqlConnection::beginTransaction(void)
{
}

void MysqlConnection::commitTransaction(void)
{
	if ( mysql_commit(m_pMysql) != 0 )
		checkErrorException("mysql_commit");
}

void MysqlConnection::roolbackTransaction(void)
{
	if ( mysql_rollback(m_pMysql) != 0 )
		checkErrorException("mysql_rollback");
}

bool MysqlConnection::ping(void)
{
	return ( mysql_ping(m_pMysql) == 0 );
}

int MysqlConnection::getErrorCode(void) const
{
	return static_cast<int>( mysql_errno(m_pMysql) );
}

stdstring MysqlConnection::getErrorInfo(void) const
{
	return mysql_error(m_pMysql);
}

stdstring MysqlConnection::getDriverName(void) const
{
	static const stdstring g_strDriverName("mysql");
	return g_strDriverName;
}

stdstring MysqlConnection::getDriverVersion(void) const
{
	static const stdstring g_strDriverVersion = StringFunc::intToString(MYSQL_VERSION_ID);
	return g_strDriverVersion;
}

MYSQL * MysqlConnection::getHandle(void)
{
	if ( m_pMysql == NULL )
		throw DBException(SQLERR_INIT_ERROR, "MYSQL connection not init");
	return m_pMysql;
}

void MysqlConnection::checkErrorException(const stdstring & strDebugInfo) const
{
	int nErrno = getErrorCode();
	if ( nErrno != 0 )
		throw DBException(SQLERR_ERROR, FormatString("MYSQL error:% (%), debug info:%")
		.arg(nErrno).arg(getErrorInfo()).arg(strDebugInfo).str() );
}

SafeDBStatement MysqlConnection::createStatement(void)
{
	return SafeDBStatement( new MysqlStatement(this) );
}

RFC_NAMESPACE_END

#endif	// #if (SUPPORT_MYSQL_DB != 0)

