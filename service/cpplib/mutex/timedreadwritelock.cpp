/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/timedlock.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

TimedReadWriteLock::TimedReadWriteLock(void) : m_bWriteLocked(false), m_nReadLockedCount(0), m_nWaitingForWriteLock(0)
{

}

bool TimedReadWriteLock::isReadLocked(void) const
{
	return ( m_nReadLockedCount > 0 );
}

bool TimedReadWriteLock::isWriteLocked(void) const
{
	return m_bWriteLocked;
}

bool TimedReadWriteLock::timedWaitReadLock(const TimeValue & tvTimeWait) const throw(MutexException)
{
	if ( tvTimeWait == TimeValue::g_tvZeroTime )
		return timedWaitReadLockUntil(TimeValue::g_tvZeroTime);
	return timedWaitReadLockUntil(TimeValue::getTimeUntil(tvTimeWait));
}

bool TimedReadWriteLock::timedWaitReadLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException)
{
	AutoMutexLock auLock(m_mutexLock);
	if ( (m_bWriteLocked || m_nWaitingForWriteLock > 0) && tvTimeUntil == TimeValue::g_tvZeroTime )
		return false;

	while ( m_bWriteLocked || m_nWaitingForWriteLock > 0 )
	{
		if ( !m_readCondVariant.timedWaitUntil(m_mutexLock, tvTimeUntil) )
			return false;
	} // while ( m_bWriteLocked || m_nWaitingForWriteLock > 0 )

	++m_nReadLockedCount;
	return true;
}

void TimedReadWriteLock::waitUntilReadLock(void) const throw(MutexException)
{
	timedWaitReadLockUntil(TimeValue::g_tvInfiniteTime);
}

bool TimedReadWriteLock::tryReadLock(void) const throw(MutexException)
{
	return timedWaitReadLockUntil(TimeValue::g_tvZeroTime);
}

bool TimedReadWriteLock::timedWaitWriteLock(const TimeValue & tvTimeWait) const throw(MutexException)
{
	if ( tvTimeWait == TimeValue::g_tvZeroTime )
		return timedWaitWriteLockUntil(TimeValue::g_tvZeroTime);
	return timedWaitWriteLockUntil(TimeValue::getTimeUntil(tvTimeWait));
}

bool TimedReadWriteLock::timedWaitWriteLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException)
{
	AutoMutexLock auLock(m_mutexLock);
	if ( (isReadLocked() || isWriteLocked()) && tvTimeUntil == TimeValue::g_tvZeroTime )
		return false;

	while ( isReadLocked() || isWriteLocked() )
	{
		AutoIncrease<int> auIncWaitingCount(m_nWaitingForWriteLock);
		if ( !m_writeCondVariant.timedWaitUntil(m_mutexLock, tvTimeUntil) )
			return false;
	} // while ( isReadLocked() || isWriteLocked() )

	m_bWriteLocked = true;
	return true;
}

void TimedReadWriteLock::waitUntilWriteLock(void) const throw(MutexException)
{
	timedWaitWriteLockUntil(TimeValue::g_tvInfiniteTime);
}

bool TimedReadWriteLock::tryWriteLock(void) const throw(MutexException)
{
	return timedWaitWriteLockUntil(TimeValue::g_tvZeroTime);
}

void TimedReadWriteLock::release(void) const throw(MutexException)
{
	AutoMutexLock auLock(m_mutexLock);
	if ( !isReadLocked() && !isWriteLocked() )
		throw MutexException(EINVAL, "TimedReadWriteLock::release error");

	if ( m_nReadLockedCount > 0 )
	{
		--m_nReadLockedCount;
		if ( m_nReadLockedCount == 0 )
			m_writeCondVariant.notifyOne();
	}
	else if ( m_bWriteLocked )
	{
		m_bWriteLocked = false;
		if ( m_nWaitingForWriteLock > 0 )
			m_writeCondVariant.notifyOne();
		else
			m_readCondVariant.notifyOne();
	}
	else
		throw MutexException(EINVAL, "TimedReadWriteLock::release error");
}

RFC_NAMESPACE_END
