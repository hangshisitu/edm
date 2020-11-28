/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/timedlock.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

RecursiveLock::RecursiveLock(void) : m_nLockedCount(0), m_nHoldLockThreadID(0)
{

}

void RecursiveLock::lock(void) const throw(MutexException)
{
	typeThreaID nThreadID = pthread_self();
	AutoMutexLock auLock(m_mutexLock);
	if ( m_nLockedCount > 0 && m_nHoldLockThreadID == nThreadID )
		++m_nLockedCount;
	else
	{
		while ( m_nLockedCount > 0 )
			m_condVariant.wait(m_mutexLock);
		m_nLockedCount = 1;
		m_nHoldLockThreadID = nThreadID;
	}
}

bool RecursiveLock::tryLock(void) const throw(MutexException)
{
	AutoMutexTryLock auLock(m_mutexLock);
	if ( !auLock.isLocked() )
		return false;

	typeThreaID nThreadID = pthread_self();	
	if ( m_nLockedCount > 0 && m_nHoldLockThreadID != nThreadID )		// has locked by other thread
		return false;

	// OK. I got the lock
	++m_nLockedCount;
	m_nHoldLockThreadID = nThreadID;
	return true;
}

void RecursiveLock::release(void) const throw(MutexException)
{
	typeThreaID nThreadID = pthread_self();
	AutoMutexLock auLock(m_mutexLock);
	if ( m_nLockedCount <= 0 || m_nHoldLockThreadID != nThreadID )
		throw MutexException(EINVAL, "RecursiveLock::release error");

	--m_nLockedCount;
	if ( m_nLockedCount == 0 )
	{
		m_nHoldLockThreadID = 0;
		m_condVariant.notifyOne();
	}
}

RFC_NAMESPACE_END
