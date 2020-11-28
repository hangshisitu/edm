/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/threadcontext.h"
#include "base/exception.h"

RFC_NAMESPACE_BEGIN

void ThreadContext::setTempCacheSize(size_t nCacheSize)
{
	if ( nCacheSize > m_nCacheSize )
	{
		m_nCacheSize = nCacheSize;
		m_Cache.reset(NULL);
	}
}

void *	ThreadContext::getTempCacheData(void)
{
	if ( m_Cache.get() == NULL )
		m_Cache.reset(new rfc_uint_8[m_nCacheSize]);
	return m_Cache.get();
}

void ThreadContext::setThreadData(const stdstring & strKey, typeThreadDataAutoPtr & auThreadData)
{
	typeThreadDataMap::iterator it = m_mapThreadData.find(strKey);
	if ( it == m_mapThreadData.end() )
	{
		m_mapThreadData[strKey] = auThreadData.release();
	}
	else
	{
		delete it->second;
		it->second = auThreadData.release();
	}
}

ThreadData * ThreadContext::getThreadData(const stdstring & strKey)
{
	typeThreadDataMap::iterator it = m_mapThreadData.find(strKey);
	if ( it == m_mapThreadData.end() )
		return NULL;
	return it->second;
}

typeThreadDataAutoPtr ThreadContext::swapOutThreadData(const stdstring & strKey)
{
	typeThreadDataAutoPtr auThreadData;
	typeThreadDataMap::iterator it = m_mapThreadData.find(strKey);
	if ( it != m_mapThreadData.end() )
	{
		auThreadData.reset(it->second);
		m_mapThreadData.erase(it);
	}
	return auThreadData;
}

void ThreadContext::removeThreadData(const stdstring & strKey)
{
	swapOutThreadData(strKey);
}

//////////////////////////////////////////////////////////////////////////

ThreadKey::ThreadKey(void) : m_nThreadKey(static_cast<typeThreadKey>(INVALID_HANDLE))
{
	if ( pthread_key_create(&m_nThreadKey, NULL) != 0 )
		m_nThreadKey = static_cast<typeThreadKey>(INVALID_HANDLE);
}

ThreadKey::~ThreadKey(void)
{
	if ( m_nThreadKey != static_cast<typeThreadKey>(INVALID_HANDLE) )
		pthread_key_delete(m_nThreadKey);
}

void * ThreadKey::getData(void)
{
	if ( m_nThreadKey != static_cast<typeThreadKey>(INVALID_HANDLE) )
		return pthread_getspecific(m_nThreadKey);
	return NULL;
}

void ThreadKey::setData(void * pData)
{
	if ( m_nThreadKey != static_cast<typeThreadKey>(INVALID_HANDLE) )
		pthread_setspecific(m_nThreadKey, pData);
}

//////////////////////////////////////////////////////////////////////////

ThreadKey ThreadContextHolder::m_threadDataStorage;

ThreadContext *	ThreadContextHolder::getThreadContext(void)
{
	return static_cast<ThreadContext *>(m_threadDataStorage.getData());
}

void ThreadContextHolder::setThreadContext(ThreadContext * pContext)
{
	m_threadDataStorage.setData(pContext);
}

template<typename typeData>
AutoAllocThreadKey<typeData> AutoAllocThreadContextFunc<typeData>::m_threadSpecificData;

RFC_NAMESPACE_END

