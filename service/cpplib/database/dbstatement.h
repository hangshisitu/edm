/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DBSTATEMENT_H_201008
#define RFC_DBSTATEMENT_H_201008

#include "database/dbfield.h"
#include <vector>

RFC_NAMESPACE_BEGIN

class DBStatement;
class DBConnection;

class DBRecordset
{
public:
	size_t					getColumnCount(void) const;
	bool					hasField(const stdstring & strName) const;

	DBField &				getField(size_t nIndex);
	const DBField &			getField(size_t nIndex) const;

	DBField &				getField(const stdstring & strName);
	const DBField &			getField(const stdstring & strName) const;

	/*DBField &				operator[](size_t nIndex) { return getField(nIndex); }
	const DBField &			operator[](size_t nIndex) const { return getField(nIndex); }
	DBField &				operator[](const stdstring & strName) { return getField(strName); }
	const DBField &			operator[](const stdstring & strName) const { return getField(strName); }*/

	bool					fetch(void);

protected:
	friend class DBStatement;
	explicit DBRecordset(DBStatement * pStatement) : m_pDBStatement(pStatement) {}

	DBStatement *			getDBStatement(void) throw(DBException);
	const DBStatement *		getDBStatement(void) const throw(DBException);

private:
	DBStatement *			m_pDBStatement;
};

class DBStatement : private NonCopyAble
{
public:
	explicit DBStatement(DBConnection * pDBConnection) : m_pDBConnection(pDBConnection) {}
	virtual ~DBStatement(void) {}

	virtual bool			prepareSQL(const stdstring & strSQL, size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize) = 0;
	virtual bool			execSQL(void) = 0;
	virtual bool			close(void) = 0;

	virtual bool			execSQLDirect(const stdstring & strSQL, size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize);
	bool					execAndCommit(const stdstring & strSQL = "", size_t nBegin = 0, size_t nCount = GlobalConstant::g_uMaxSize);

	virtual bool			fetch(void) = 0;
	virtual off_t			tell(void) const = 0;
	virtual bool			seek(off_t nOffset, seek_position nWhence = seek_begin) = 0;

	virtual size_t			getEffectRowCount(void) = 0;
	virtual size_t			getRecordCount(void) = 0;

	bool					moveNext(void) { return fetch(); }
	bool					movePrior(void) { return seek(-1, seek_cur); }
	bool					moveFirst(void) { return seek(0, seek_begin); }
	bool					moveLast(void) { return seek(-1, seek_end); }
	bool					skipRow(size_t nRowCount) { return seek(static_cast<off_t>(nRowCount), seek_cur); }

	typeDBFieldList &		getFieldList(void) { return m_listField; }
	const typeDBFieldList &	getFieldList(void) const { return m_listField; }

	typeDBParamList &		getParamList(void) { return m_listParam; }
	const typeDBParamList &	getParamList(void) const { return m_listParam; }

	size_t					getColumnCount(void) const { return m_listField.size(); }
	size_t					getParamCount(void) const { return m_listParam.size(); }

	bool					hasField(const stdstring & strName) const;
	DBField &				getField(size_t nIndex);
	const DBField &			getField(size_t nIndex) const;

	DBField &				getField(const stdstring & strName);	
	const DBField &			getField(const stdstring & strName) const;

	DBParam &				getParam(size_t nIndex);
	const DBParam &			getParam(size_t nIndex) const;

	DBParam &				getParam(const stdstring & strName);	
	const DBParam &			getParam(const stdstring & strName) const;

	/*DBField &				operator[](size_t nIndex) { return getField(nIndex); }
	const DBField &			operator[](size_t nIndex) const { return getField(nIndex); }
	DBField &				operator[](const stdstring & strName) { return getField(strName); }
	const DBField &			operator[](const stdstring & strName) const { return getField(strName); }*/

	DBRecordset				getRecordset(void) { return DBRecordset(this); }

protected:
	virtual typeAutoDBParam	createParam(const stdstring & strName) = 0;

	void					prepareParam(const stdstring & strSQL);
	DBConnection *			getDBConnection(void);
	const DBConnection *	getDBConnection(void) const;
	void					clearFieldList(void);
	void					clearParamList(void);
	void					addField( typeAutoDBField & auField );
	void					addParam( typeAutoDBParam & auParam );
	void					addParam( const stdstring & strName );

	DBConnection *			m_pDBConnection;
	typeDBFieldList			m_listField;
	typeDBParamList			m_listParam;
};

RFC_NAMESPACE_END

#endif	//RFC_DBSTATEMENT_H_201008
