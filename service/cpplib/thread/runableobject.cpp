/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/runableobject.h"

RFC_NAMESPACE_BEGIN

bool RunableObject::start(void)
{
	m_nState = STATE_READY;
	m_nResult = -1;
	if ( !beforeStart() )
		return false;
	m_nState = STATE_RUNNING;

	try
	{
		m_nResult = run();
	}
	catch ( ... )
	{
		m_nResult = -1;
	}
	afterStop();
	return true;
}

bool RunableObject::stop(void)
{
	m_nState = STATE_WAITING_STOP;
	return beforeStop();
}

bool AsyncRunableObject::start(void)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState != RunableObject::STATE_INIT )
		return false;
	m_nState = STATE_READY;
	m_ConditionState.notifyOne();
	au.release();

	RunableObject::start();		//run thread
	au.lock();
	RunableObject::m_nState = RunableObject::STATE_DEAD;
	m_ConditionState.notifyOne();
	return true;
}

bool AsyncRunableObject::stop(void)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState == RunableObject::STATE_INIT || RunableObject::m_nState == RunableObject::STATE_WAITING_STOP
		|| RunableObject::m_nState == RunableObject::STATE_DEAD )
		return false;
	RunableObject::stop();
	m_ConditionSleep.notifyOne();
	afterStop();
	return true;
}

int AsyncRunableObject::waitUntilStop(void)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState == RunableObject::STATE_INIT || RunableObject::m_nState == RunableObject::STATE_DEAD )
		return RunableObject::m_nResult;

	while ( RunableObject::m_nState != RunableObject::STATE_DEAD )
		m_ConditionState.wait(m_LockState);
	return RunableObject::m_nResult;
}

int AsyncRunableObject::stopAndWaitResult(void)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState == RunableObject::STATE_INIT || RunableObject::m_nState == RunableObject::STATE_DEAD )
		return RunableObject::m_nResult;
	RunableObject::stop();
	m_ConditionSleep.notifyOne();
	while ( RunableObject::m_nState != RunableObject::STATE_DEAD )
		m_ConditionState.wait(m_LockState);
	return m_nResult;
}

bool AsyncRunableObject::wakeup(void)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState != RunableObject::STATE_SUSPEND )
		return false;
	m_ConditionSleep.notifyOne();
	RunableObject::m_nState = RunableObject::STATE_READY;
	return true;
}

bool AsyncRunableObject::suspend(const TimeValue & timeVal)
{
	AutoMutexLock au(m_LockState);
	if ( RunableObject::m_nState == RunableObject::STATE_SUSPEND )
		return false;
	typeState nOldState = RunableObject::m_nState;
	RunableObject::m_nState = RunableObject::STATE_SUSPEND;
	if ( timeVal == TimeValue::g_tvInfiniteTime )
		m_ConditionSleep.wait(m_LockState);
	else
		m_ConditionSleep.timedWait(m_LockState, timeVal);
	if ( RunableObject::m_nState == RunableObject::STATE_SUSPEND )
		RunableObject::m_nState = nOldState;
	return true;
}

RFC_NAMESPACE_END

