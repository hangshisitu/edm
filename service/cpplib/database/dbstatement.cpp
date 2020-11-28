/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/dbstatement.h"
#include "database/dbconnection.h"
#include "base/formatstream.h"
#include "base/deleter.h"

RFC_NAMESPACE_BEGIN

size_t DBRecordset::getColumnCount(void) const
{
	return getDBStatement()->getColumnCount();
}

bool DBRecordset::hasField(const stdstring & strName) const
{
	return getDBStatement()->hasField(strName);
}

DBField & DBRecordset::getField(size_t nIndex)
{
	return getDBStatement()->getField(nIndex);
}

const DBField & DBRecordset::getField(size_t nIndex) const
{
	return getDBStatement()->getField(nIndex);
}

DBField & DBRecordset::getField(const stdstring & strName)
{
	return getDBStatement()->getField(strName);
}

const DBField & DBRecordset::getField(const stdstring & strName) const
{
	return getDBStatement()->getField(strName);
}

bool DBRecordset::fetch(void)
{
	return getDBStatement()->fetch();
}

DBStatement * DBRecordset::getDBStatement(void) throw(DBException)
{
	if ( m_pDBStatement == NULL )
		throw DBException("m_pDBStatement is NULL in DBRecordset::getDBStatement");
	return m_pDBStatement;
}

const DBStatement * DBRecordset::getDBStatement(void) const throw(DBException)
{
	if ( m_pDBStatement == NULL )
		throw DBException("m_pDBStatement is NULL in DBRecordset::getDBStatement");
	return m_pDBStatement;
}

//////////////////////////////////////////////////////////////////////////

bool DBStatement::execSQLDirect(const stdstring & strSQL, size_t nBegin, size_t nCount)
{
	if ( !prepareSQL(strSQL, nBegin, nCount) )
		return false;
	return execSQL();
}

bool DBStatement::execAndCommit(const stdstring & strSQL, size_t nBegin, size_t nCount)
{
	if ( !execSQLDirect(strSQL, nBegin, nCount) )
		return false;
	getDBConnection()->commitTransaction();
	return true;
}

bool DBStatement::hasField(const stdstring & strName) const
{
	typeDBFieldList::const_iterator it, itEnd = m_listField.end();
	for ( it = m_listField.begin(); it != itEnd; ++it )
	{
		const DBField * pField = *it;
		if ( StringFunc::strCaseEqual(strName, pField->getFieldName()) )
			return true;
	} // for ( it = m_listField.begin(); it != itEnd; ++it )
	return false;
}

DBField & DBStatement::getField(size_t nIndex)
{
	if ( nIndex >= m_listField.size() )
		throw DBException( FormatString("DBStatement::getField out of bounds:%(%)").arg(nIndex).arg(m_listField.size()).str() );
	DBField * pField = m_listField[nIndex];
	return *pField;
}

const DBField & DBStatement::getField(size_t nIndex) const
{
	if ( nIndex >= m_listField.size() )
		throw DBException( FormatString("DBStatement::getField out of bounds:%(%)").arg(nIndex).arg(m_listField.size()).str() );
	const DBField * pField = m_listField[nIndex];
	return *pField;
}

DBField & DBStatement::getField(const stdstring & strName)
{
	typeDBFieldList::iterator it, itEnd = m_listField.end();
	for ( it = m_listField.begin(); it != itEnd; ++it )
	{
		DBField * pField = *it;
		if ( StringFunc::strCaseEqual(strName, pField->getFieldName()) )
			return *pField;
	} // for ( it = m_listField.begin(); it != itEnd; ++it )
	throw DBException( FormatString("Field [%] not found in DBStatement::getField").arg(strName).str() );
}

const DBField & DBStatement::getField(const stdstring & strName) const
{
	typeDBFieldList::const_iterator it, itEnd = m_listField.end();
	for ( it = m_listField.begin(); it != itEnd; ++it )
	{
		const DBField * pField = *it;
		if ( StringFunc::strCaseEqual(strName, pField->getFieldName()) )
			return *pField;
	} // for ( it = m_listField.begin(); it != itEnd; ++it )
	throw DBException( FormatString("Field [%] not found in DBStatement::getField").arg(strName).str() );
}

DBParam & DBStatement::getParam(size_t nIndex)
{
	if ( nIndex >= m_listParam.size() )
		throw DBException( FormatString("DBStatement::getParam out of bounds:%(%)").arg(nIndex).arg(m_listParam.size()).str() );
	DBParam * pParam = m_listParam[nIndex];
	return *pParam;
}

const DBParam & DBStatement::getParam(size_t nIndex) const
{
	if ( nIndex >= m_listParam.size() )
		throw DBException( FormatString("DBStatement::getParam out of bounds:%(%)").arg(nIndex).arg(m_listParam.size()).str() );
	const DBParam * pParam = m_listParam[nIndex];
	return *pParam;
}

DBParam & DBStatement::getParam(const stdstring & strName)
{
	typeDBParamList::iterator it, itEnd = m_listParam.end();
	for ( it = m_listParam.begin(); it != itEnd; ++it )
	{
		DBParam * pParam = *it;
		if ( StringFunc::strCaseEqual(strName, pParam->getParamName()) )
			return *pParam;
	} // for ( it = m_listParam.begin(); it != itEnd; ++it )
	throw DBException( FormatString("Param [%] not found in DBStatement::getParam").arg(strName).str() );
}

const DBParam & DBStatement::getParam(const stdstring & strName) const
{
	typeDBParamList::const_iterator it, itEnd = m_listParam.end();
	for ( it = m_listParam.begin(); it != itEnd; ++it )
	{
		const DBParam * pParam = *it;
		if ( StringFunc::strCaseEqual(strName, pParam->getParamName()) )
			return *pParam;
	} // for ( it = m_listParam.begin(); it != itEnd; ++it )
	throw DBException( FormatString("Param [%] not found in DBStatement::getParam").arg(strName).str() );
}

void DBStatement::prepareParam(const stdstring & strSQL)
{
	clearParamList();
	if ( strSQL.find(':') == stdstring::npos )	// param not exist
		return;

	const char * pBeginParamName = NULL;
	bool bQuoteMark = false;
	const char * const pEnd = strSQL.data() + strSQL.size();
	for ( const char * p = strSQL.data(); p < pEnd; ++p )
	{
		char ch = (*p);
		if ( bQuoteMark && ch == '\\' )
			++p;
		else if ( bQuoteMark )
			bQuoteMark = ( ch != '\'' );
		else if ( pBeginParamName != NULL )
		{
			if ( !DBParam::isValidParamChar(ch) )	// end of param name
			{
				stdstring strName(pBeginParamName, p - pBeginParamName);
				if ( !strName.empty() )
					m_listParam.push_back( createParam(strName).release() );
				pBeginParamName = DBParam::isBeginSymbolOfParam(ch) ? p + 1 : NULL;
			}
		}
		else if ( ch == '\'' )
			bQuoteMark = true;
		else if ( DBParam::isBeginSymbolOfParam(ch) )
			pBeginParamName = p + 1;
	} // for ( const char * p = strSQL.data(); p < pEnd; ++p )

	if ( pBeginParamName != NULL )
	{
		stdstring strName(pBeginParamName, pEnd - pBeginParamName);
		if ( !strName.empty() )
			m_listParam.push_back( createParam(strName).release() );
	}
}

DBConnection * DBStatement::getDBConnection(void)
{
	if ( m_pDBConnection == NULL )
		throw DBException("m_pDBConnection is NULL in DBStatement::getDBConnection");
	return m_pDBConnection;
}

const DBConnection * DBStatement::getDBConnection(void) const
{
	if ( m_pDBConnection == NULL )
		throw DBException("m_pDBConnection is NULL in DBStatement::getDBConnection");
	return m_pDBConnection;
}

void DBStatement::clearParamList(void)
{
	std::for_each(m_listParam.begin(), m_listParam.end(), Deleter());
	m_listParam.clear();
}

void DBStatement::clearFieldList(void)
{
	std::for_each(m_listField.begin(), m_listField.end(), Deleter());
	m_listField.clear();
}

void DBStatement::addField( typeAutoDBField & auField )
{
	m_listField.push_back( auField.release() );
}

void DBStatement::addParam( typeAutoDBParam & auParam )
{
	m_listParam.push_back( auParam.release() );
}

void DBStatement::addParam( const stdstring & strName )
{
	typeAutoDBParam auParam( createParam(strName) );
	m_listParam.push_back( auParam.release() );
}

RFC_NAMESPACE_END
