/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_THREADSPECIFICDATA_H_201008
#define RFC_THREADSPECIFICDATA_H_201008

#include "base/autoarray.h"
#include "base/globalfunc.h"
#include <pthread.h>
#include <map>
#include <memory>

RFC_NAMESPACE_BEGIN

typedef pthread_t				typeThreaID;
typedef pthread_key_t			typeThreadKey;

// 线程内保存的数据可通过继承ThreadData来实现
class ThreadData
{
public:
	ThreadData(void) {}
	~ThreadData(void) {}
};

typedef std::auto_ptr<ThreadData>	typeThreadDataAutoPtr;

class ThreadContext : private NonCopyAble	//thread-specific-data
{
public:
	ThreadContext(void) : m_nThreadID(0), m_nCacheSize(8 * 1024), m_nSN(0), m_Cache(NULL), m_nRandSeed(time(NULL)) {}

	inline typeThreaID			getThreadID(void) const { return m_nThreadID; }
	inline void					setThreadID(typeThreaID nID) { m_nThreadID = nID; }

	void						setTempCacheSize(size_t nCacheSize);
	inline size_t				getTempCacheSize(void) const { return m_nCacheSize; }
	void *						getTempCacheData(void);
	void *						getTempCacheData(size_t nCacheSize)
	{
		setTempCacheSize(nCacheSize);
		return getTempCacheData();
	}

	size_t						genSN(void) { return m_nSN++; }

	rfc_uint_32					rand(void) { return GlobalFunc::threadSafeRand( &m_nRandSeed ); }

	void						setThreadData(const stdstring & strKey, typeThreadDataAutoPtr & auThreadData);
	ThreadData *				getThreadData(const stdstring & strKey);
	typeThreadDataAutoPtr		swapOutThreadData(const stdstring & strKey);
	void						removeThreadData(const stdstring & strKey);

protected:
	typeThreaID				m_nThreadID;
	size_t					m_nCacheSize;
	size_t					m_nSN;
	AutoArray<rfc_uint_8>	m_Cache;
	rfc_uint_32				m_nRandSeed;

	typedef std::map<stdstring, ThreadData*>	typeThreadDataMap;
	typeThreadDataMap		m_mapThreadData;
};

class ThreadKey		//thread-specific-data storage
{
public:
	ThreadKey(void);
	~ThreadKey(void);

	void *						getData(void);
	void						setData(void * pData);

private:
	typeThreadKey				m_nThreadKey;
};

class ThreadContextHolder
{
public:
	static	ThreadContext *	getThreadContext(void);
	static	void			setThreadContext(ThreadContext * pData);

private:
	static	ThreadKey		m_threadDataStorage;
};

template<typename typeData>
class AutoAllocThreadKey	//auto allocate thread-specific-data storage
{
public:
	AutoAllocThreadKey(void) : m_nThreadKey(static_cast<typeThreadKey>(INVALID_HANDLE))
	{
		if ( pthread_key_create(&m_nThreadKey, &AutoAllocThreadKey::destructor) != 0 )
			m_nThreadKey = static_cast<typeThreadKey>(INVALID_HANDLE);
	}

	~AutoAllocThreadKey(void)
	{
		if ( m_nThreadKey != static_cast<typeThreadKey>(INVALID_HANDLE) )
			pthread_key_delete(m_nThreadKey);
	}

	typeData *					getData(void)
	{
		if ( m_nThreadKey == static_cast<typeThreadKey>(INVALID_HANDLE) )
			return NULL;

		typeData * pData = static_cast<typeData*>( pthread_getspecific(m_nThreadKey) );
		if ( pData != NULL )
			return pData;
		pData = new typeData();
		pthread_setspecific(m_nThreadKey, pData);
		return pData;
	}

private:
	static void destructor(void * pData)
	{
		typeData * pObj = static_cast<typeData*>(pData);
		delete pObj;
	}

	typeThreadKey				m_nThreadKey;
};

template<typename typeData>
class AutoAllocThreadContextFunc
{
public:
	static	typeData *			getThreadSpecificData(void) { return m_threadSpecificData.getData(); }

private:
	static	AutoAllocThreadKey<typeData>		m_threadSpecificData;
};

RFC_NAMESPACE_END

#endif	//RFC_THREADSPECIFICDATA_H_201008

