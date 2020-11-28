/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/timedlock.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

TimedLock::TimedLock(void) : m_bLocked(false)
{

}

bool TimedLock::timedWaitLock(const TimeValue & tvTimeWait) const throw(MutexException)
{
	if ( tvTimeWait == TimeValue::g_tvZeroTime )
		return timedWaitLockUntil(TimeValue::g_tvZeroTime);
	return timedWaitLockUntil(TimeValue::getTimeUntil(tvTimeWait));
}

bool TimedLock::timedWaitLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException)
{
	AutoMutexLock auLock(m_mutexLock);
	if ( m_bLocked && tvTimeUntil == TimeValue::g_tvZeroTime )
		return false;

	while ( m_bLocked )
	{
		if ( !m_condVariant.timedWaitUntil(m_mutexLock, tvTimeUntil) )
			return false;
	} // while ( m_bLocked )
	m_bLocked = true;
	return true;
}

void TimedLock::waitUntilLock(void) const throw(MutexException)
{
	timedWaitLockUntil(TimeValue::g_tvInfiniteTime);
}

bool TimedLock::tryLock(void) const throw(MutexException)
{
	return timedWaitLockUntil(TimeValue::g_tvZeroTime);
}

void TimedLock::release(void) const throw(MutexException)
{
	AutoMutexLock auLock(m_mutexLock);
	m_bLocked = false;
	m_condVariant.notifyOne();
}

RFC_NAMESPACE_END
