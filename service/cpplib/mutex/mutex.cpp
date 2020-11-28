/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

AutoReadWriteLockProxy::AutoReadWriteLockProxy(const ReadWriteLock & aLock)
: AutoLockProxy<ReadWriteLock>(aLock), m_bReadLocked(false), m_bWriteLocked(false)
{
}

void AutoReadWriteLockProxy::readLock(void)
{
	if ( !m_bReadLocked )
	{
		typeAutoLockProxy::m_lock.readLock();
		typeAutoLockProxy::m_bLocked = m_bReadLocked = true;
	}
}

void AutoReadWriteLockProxy::writeLock(void)
{
	if ( !m_bWriteLocked )
	{
		typeAutoLockProxy::m_lock.writeLock();
		typeAutoLockProxy::m_bLocked = m_bWriteLocked = true;
	}
}

bool AutoReadWriteLockProxy::tryReadLock(void)
{
	if ( !m_bReadLocked )
		m_bReadLocked = typeAutoLockProxy::m_lock.tryReadLock();
	typeAutoLockProxy::m_bLocked = m_bReadLocked;
	return m_bReadLocked;
}

bool AutoReadWriteLockProxy::tryWriteLock(void)
{
	if ( !m_bWriteLocked )
		m_bWriteLocked = typeAutoLockProxy::m_lock.tryWriteLock();
	typeAutoLockProxy::m_bLocked = m_bWriteLocked;
	return m_bWriteLocked;
}

//////////////////////////////////////////////////////////////////////////
AutoTimedLockProxy::AutoTimedLockProxy(const TimedLock & aLock, const TimeValue & tvTimeWait)
: AutoLockProxy<TimedLock>(aLock)
{
	timedWaitLock(tvTimeWait);
}

bool AutoTimedLockProxy::timedWaitLock(const TimeValue & tvTimeWait)
{
	if ( !typeAutoLockProxy::m_bLocked )
		typeAutoLockProxy::m_bLocked = typeAutoLockProxy::m_lock.timedWaitLock(tvTimeWait);
	return typeAutoLockProxy::m_bLocked;
}

bool AutoTimedLockProxy::timedWaitLockUntil(const TimeValue & tvTimeUntil)
{
	if ( !typeAutoLockProxy::m_bLocked )
		typeAutoLockProxy::m_bLocked = typeAutoLockProxy::m_lock.timedWaitLock(tvTimeUntil);
	return typeAutoLockProxy::m_bLocked;
}

void AutoTimedLockProxy::waitUntilLock(void)
{
	if ( !typeAutoLockProxy::m_bLocked )
	{
		typeAutoLockProxy::m_lock.waitUntilLock();
		typeAutoLockProxy::m_bLocked = true;
	}
}

bool AutoTimedLockProxy::tryLock(void)
{
	if ( !typeAutoLockProxy::m_bLocked )
		typeAutoLockProxy::m_bLocked = typeAutoLockProxy::m_lock.tryLock();
	return typeAutoLockProxy::m_bLocked;
}


void delay(time_t nSecond)
{
	return delay(TimeValue(nSecond));
}

void delay(time_t nSecond, long nMicroSecond)
{
	return delay(TimeValue(nSecond, nMicroSecond));
}

void delay(const TimeValue & tvTimeWait)
{
	return delayUntil(TimeValue::getTimeUntil(tvTimeWait));
}

void delayUntil(time_t nTimeUntil)
{
	return delayUntil(TimeValue(nTimeUntil));
}

void delayUntil(const TimeValue & tvTimeUntil)
{
	MutexLock aLock;
	ConditionVariant condVar;
	AutoMutexLock auLock(aLock);
	condVar.timedWaitUntil(aLock, tvTimeUntil);
}

RFC_NAMESPACE_END
