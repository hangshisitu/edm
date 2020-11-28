/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/conditionvariant.h"
#include "mutex/mutexlock.h"

RFC_NAMESPACE_BEGIN

ConditionVariant::ConditionVariant(void)
{
	int nRet = pthread_cond_init(&m_cond, NULL);
	if ( nRet != 0 )
		throw MutexException(nRet, "MutexLock::pthread_cond_init error");
}

ConditionVariant::~ConditionVariant(void)
{
	pthread_cond_destroy(&m_cond);
}

void ConditionVariant::notifyOne(void) throw(MutexException)
{
	int nRet = pthread_cond_signal(&m_cond);
	if ( nRet != 0 )
		throw MutexException(nRet, "ConditionVariant::notifyOne error");
}

void ConditionVariant::broadcastAll(void) throw(MutexException)
{
	int nRet = pthread_cond_broadcast(&m_cond);
	if ( nRet != 0 )
		throw MutexException(nRet, "ConditionVariant::broadcastAll error");
}

void ConditionVariant::wait(MutexLock & mutexLock) throw(MutexException)
{
	AutoIncrease<int> auIncWaitingCount(m_nWaitingLockCount);
	int nRet = pthread_cond_wait(&m_cond, &mutexLock.m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "ConditionVariant::wait error");
}

bool ConditionVariant::timedWait(MutexLock & mutexLock, const TimeValue & tvTimeWait) throw(MutexException)
{
	return timedWaitUntil(mutexLock, TimeValue::getTimeUntil(tvTimeWait));
}

bool ConditionVariant::timedWaitUntil(MutexLock & mutexLock, const TimeValue & tvTimeUntil) throw(MutexException)
{
	if ( tvTimeUntil == TimeValue::g_tvInfiniteTime )
	{
		wait(mutexLock);
		return true;
	}

	AutoIncrease<int> auIncWaitingCount(m_nWaitingLockCount);
	struct timespec abstime;
	abstime.tv_sec = tvTimeUntil.getSecond();
	abstime.tv_nsec = tvTimeUntil.getMicroSecond() * 1000;
	int nRet = pthread_cond_timedwait(&m_cond, &mutexLock.m_lock, &abstime);
	if ( nRet == 0 )
		return true;
	else if ( nRet != ETIMEDOUT )
		throw MutexException(nRet, "ConditionVariant::timedWaitUntil error");
	return false;
}

RFC_NAMESPACE_END
