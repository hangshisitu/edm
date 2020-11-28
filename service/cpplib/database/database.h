/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DATABASE_H_201008
#define RFC_DATABASE_H_201008

#include "database/dbconnection.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

class DataBase
{
public:
	static DataBase &		getInstance(void);

	bool					initDatebase(void);
	void					initDatebaseEx(void) throw(DBException);

	SafeDBConnection		getConnection(const DBEnvParams & dbEnvParam) throw(DBException);

protected:
	DataBase(void) {}
	~DataBase(void);
	DataBaseEnv	*			getDBEnv(const stdstring & strDriver) throw(DBException);

protected:
	typedef std::map<stdstring, DataBaseEnv *, IgnoreCaseStringCompare>	typeDBEnvMap;

	//ReadWriteLock			m_lock;
	typeDBEnvMap			m_mapNameToEnv;
};

RFC_NAMESPACE_END

#endif	//RFC_DATABASE_H_201008
