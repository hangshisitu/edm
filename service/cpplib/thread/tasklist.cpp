/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/tasklist.h"
#include "base/deleter.h"
#include <memory>

RFC_NAMESPACE_BEGIN

Task::~Task(void)
{
	if ( m_bDeleteObjOnFinish )
		delete m_pRunnableObj;
}

Task & Task::operator = (Task & _Right)
{
	attach(_Right.m_pRunnableObj, _Right.m_bDeleteObjOnFinish);
	_Right.m_pRunnableObj = NULL;
	_Right.m_bDeleteObjOnFinish = false;
	return *this;
}

void Task::attach(RunableObject * pRunnableObj, bool bDeleteObjOnFinish)
{
	if ( m_pRunnableObj == pRunnableObj )
		return;
	if ( m_bDeleteObjOnFinish )
		delete m_pRunnableObj;
	m_pRunnableObj = pRunnableObj;
	m_bDeleteObjOnFinish = bDeleteObjOnFinish;
}

RunableObject * Task::release(void)
{
	RunableObject *	pOld = m_pRunnableObj;
	m_pRunnableObj = NULL;
	m_bDeleteObjOnFinish = false;
	return pOld;
}

//////////////////////////////////////////////////////////////////////////

void TaskQueue::setActive(bool bActive)
{
	AutoMutexLock au(m_lockTaskList);
	m_bActive = bActive;
	m_condTaskList.broadcastAll();
}

void TaskQueue::clearAllTask(void)
{
	AutoMutexLock au(m_lockTaskList);
	m_nCurTaskCount = 0;
	std::for_each(m_listTask.begin(), m_listTask.end(), Deleter());
	m_listTask.clear();
	m_condTaskList.broadcastAll();
}

bool TaskQueue::pushTask(RunableObject * pRunnable, bool bAutoDelete, const TimeValue & tvTimeWait)
{
	AutoMutexLock au(m_lockTaskList);
	if ( m_bActive && m_nCurTaskCount >= m_nQueueMaxSize )
	{
		if ( tvTimeWait == TimeValue::g_tvZeroTime )
			return false;

		TimeValue tvTimeUntil = TimeValue::getTimeUntil(tvTimeWait);
		while ( m_bActive && m_nCurTaskCount >= m_nQueueMaxSize )
		{
			AutoIncrease<size_t> auIncreaseCount(m_nPushWaitCount);
			if ( !m_condTaskList.timedWaitUntil(m_lockTaskList, tvTimeUntil) )
				return false;
		}
	} //if ( m_bActive && m_nCurTaskCount >= m_nQueueMaxSize )

	if ( !m_bActive || m_nCurTaskCount >= m_nQueueMaxSize )
		return false;

	m_listTask.push_back( new Task(pRunnable, bAutoDelete) );
	++m_nCurTaskCount;
	if ( m_nPopWaitCount > 0 )
		m_condTaskList.notifyOne();
	return true;
}

bool TaskQueue::popTask(Task & aTask, const TimeValue & tvTimeWait)
{
	AutoMutexLock au(m_lockTaskList);
	if ( m_bActive && m_listTask.empty() )
	{
		if ( tvTimeWait == TimeValue::g_tvZeroTime )
			return false;

		TimeValue tvTimeUntil = TimeValue::getTimeUntil(tvTimeWait);
		while ( m_bActive && m_listTask.empty() )
		{
			AutoIncrease<size_t> auIncreaseCount(m_nPopWaitCount);
			if ( !m_condTaskList.timedWaitUntil(m_lockTaskList, tvTimeUntil) )
				return false;
		}
	} //if ( m_bActive && m_listTask.empty() )

	if ( !m_bActive || m_listTask.empty() )
		return false;

	std::auto_ptr<Task> auTask( m_listTask.front() );	// auto_ptr to delete the object
	m_listTask.pop_front();
	aTask = ( *auTask );
	--m_nCurTaskCount;
	if ( m_nPushWaitCount > 0 )
		m_condTaskList.notifyOne();
	return true;
}

RFC_NAMESPACE_END

