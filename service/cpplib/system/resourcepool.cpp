/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/resourcepool.h"

RFC_NAMESPACE_BEGIN

const size_t ResourcePool::npos = static_cast<size_t>( -1 );
static const size_t POOL_FLAG_HAS_APPLIED = static_cast<size_t>( -1 );
static const size_t POOL_FLAG_ENDING = static_cast<size_t>( -2 );	//the last one

ResourcePool::ResourcePool(size_t nSize) : m_bHasLockedAll(false), m_lpnResourcePool(NULL), m_nFreeCount(0),
	m_nTotalCount(0), m_nNextAvailableIndex(POOL_FLAG_ENDING)
{
	if ( nSize > 0 )
		resize(nSize);
}

size_t ResourcePool::getIndexWithoutLock(void)
{
	if ( m_bHasLockedAll || m_lpnResourcePool == NULL || m_nFreeCount == 0 || m_nNextAvailableIndex == POOL_FLAG_ENDING )
		return ResourcePool::npos;

	size_t nRetIndex = m_nNextAvailableIndex;
	m_nNextAvailableIndex = m_lpnResourcePool[nRetIndex];
	m_lpnResourcePool[nRetIndex] = POOL_FLAG_HAS_APPLIED;
	--m_nFreeCount;
	return nRetIndex;
}

void ResourcePool::clearWithoutLock(bool bLockAll)
{
	m_bHasLockedAll = bLockAll;
	delete []m_lpnResourcePool;
	m_lpnResourcePool = NULL;
	m_nFreeCount = m_nTotalCount = 0;
	m_nNextAvailableIndex = POOL_FLAG_ENDING;
}

bool ResourcePool::resizeWithoutLock(bool bLockAll, size_t nSize)
{
	clearWithoutLock(bLockAll);

	if ( nSize > 0 )
	{
		m_lpnResourcePool = new size_t[nSize];
		if ( m_lpnResourcePool == NULL )
			return false;

		for ( size_t i = 0; i < nSize; ++i )
			m_lpnResourcePool[i] = i + 1;
		m_lpnResourcePool[nSize - 1] = POOL_FLAG_ENDING;

		m_nFreeCount = m_nTotalCount = nSize;
		m_nNextAvailableIndex = 0;
	} //if ( nSize > 0 )
	return true;
}

bool ResourcePool::resize(size_t nSize)
{
	AutoMutexLock auLock(m_lockForPool);
	while ( m_nFreeCount < m_nTotalCount )
		m_condRelease.wait(m_lockForPool);

	return resizeWithoutLock(m_bHasLockedAll, nSize);
}

size_t ResourcePool::apply(void)
{
	return apply(TimeValue::g_tvInfiniteTime);
}

size_t ResourcePool::apply(const TimeValue & tvTimeWait)
{
	TimeValue tvTimeUntil = TimeValue::getTimeUntil(tvTimeWait);

	AutoMutexLock auLock(m_lockForPool);
	while ( !m_bHasLockedAll && m_lpnResourcePool != NULL )
	{
		size_t nRetIndex = getIndexWithoutLock();
		if ( nRetIndex != ResourcePool::npos )
			return nRetIndex;
		if ( !m_condRelease.timedWaitUntil(m_lockForPool, tvTimeUntil) )
			return ResourcePool::npos;
	} //while ( !m_bHasLockedAll && m_lpnResourcePool != NULL )
	return ResourcePool::npos;
}

void ResourcePool::release(size_t nIndex, bool bResult)
{
	AutoMutexLock auLock(m_lockForPool);
	if ( nIndex < m_nTotalCount && m_lpnResourcePool[nIndex] == POOL_FLAG_HAS_APPLIED )
	{
		m_lpnResourcePool[nIndex] = m_nNextAvailableIndex;
		m_nNextAvailableIndex = nIndex;
		++m_nFreeCount;
		m_condRelease.notifyOne();
	}
}

void ResourcePool::lockAll(void)
{
	AutoMutexLock auLock(m_lockForPool);
	m_bHasLockedAll = true;
	while ( m_nFreeCount < m_nTotalCount )
		m_condRelease.wait(m_lockForPool);
}

void ResourcePool::lockAllAndResize(size_t nSize)
{
	AutoMutexLock auLock(m_lockForPool);
	while ( m_nFreeCount < m_nTotalCount )
		m_condRelease.wait(m_lockForPool);
	resizeWithoutLock(true, nSize);
}

void ResourcePool::releaseAll(void)
{
	AutoMutexLock auLock(m_lockForPool);
	m_bHasLockedAll = false;
}

RFC_NAMESPACE_END
