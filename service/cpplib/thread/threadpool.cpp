/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/threadpool.h"

RFC_NAMESPACE_BEGIN

bool ThreadPool::CycleRunningThread::createAndStart(int nStackSize, int nPriority)
{
	return Thread::createAndStart(CycleRunningThread::cycleRunningFunc, nStackSize, nPriority);
}

bool ThreadPool::CycleRunningThread::stop(void)
{
	Thread::stop();

	AutoMutexLock auLock(m_lockTask);
	RunableObject * pRunableObj = m_task.getRunnableObj();
	if ( pRunableObj != NULL )
		pRunableObj->stop();
	return true;
}

bool ThreadPool::CycleRunningThread::waitForTask()
{
	Task aNewTask;
	if ( !m_threadPool.m_taskPool.popTask(aNewTask, m_threadPool.isPoolIdle() ? m_threadPool.m_tvIdleTimeout : TimeValue::g_tvInfiniteTime) )
		return false;

	AutoMutexLock auLock(m_lockTask);
	m_task = aNewTask;
	return true;
}

void ThreadPool::CycleRunningThread::runTask()
{
	RunableObject * pRunableObj = m_task.getRunnableObj();
	if ( pRunableObj == NULL )
		return;
	pRunableObj->start();
	pRunableObj->waitUntilStop();
}

bool ThreadPool::CycleRunningThread::afterStop(void)
{
	return true;
}

void * ThreadPool::CycleRunningThread::cycleRunningFunc(void * arg)
{
	CycleRunningThread * pThread = reinterpret_cast<CycleRunningThread *>(arg);
	ThreadContext * pThreadContext = new ThreadContext;
	pThreadContext->setThreadID(pThread->m_nThreadID);
	pThread->start();

	rfc_uint_32 nMaxUseCount = GlobalConstant::g_uMaxUInt32;// pThread->m_threadPool.m_nThreadMaxUsedCount;
	for ( rfc_uint_32 i = 0; i < nMaxUseCount; ++i )
	{
		if ( pThread->m_threadPool.shouldExit() )
			break;
		if ( !pThread->waitForTask() )
			break;
		pThread->runTask();
		pThread->m_threadPool.onFinishTask();
	} //for ( rfc_uint_32 i = 0; i < nMaxUseCount; ++i )

	pThread->m_threadPool.destroyThread(pThread);
	delete pThreadContext;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void ThreadPool::initPool(rfc_uint_32 nMinThreadCount, rfc_uint_32 nMaxThreadCount,
						  size_t nTaskPoolSize, int nThreadStackSize, time_t nIdleTimeout)
{
	stopPool();
	m_nMinThreadCount = nMinThreadCount;
	m_nMaxThreadCount = nMaxThreadCount;
	m_nThreadStackSize = nThreadStackSize;
	m_tvIdleTimeout.setSecond(nIdleTimeout);

	m_nCurThreadCount = m_nIdleThreadCount = m_nNotFinishTaskCount = 0;
	m_taskPool.setQueueMaxSize(nTaskPoolSize);
	m_taskPool.setActive(true);
	m_nPoolState = RunableObject::STATE_READY;
}

void ThreadPool::startPool(rfc_uint_32 nBuildThreadCount)
{
	m_nPoolState = RunableObject::STATE_READY;
	if ( nBuildThreadCount > m_nMaxThreadCount )
		nBuildThreadCount = m_nMaxThreadCount;
	for ( rfc_uint_32 i = 0; i < nBuildThreadCount; ++i )
		buildThread();
	m_taskPool.setActive(true);
}

void ThreadPool::stopPool()
{
	AutoMutexLock au(m_lockForThread);
	if ( !isPoolStart() )
		return;
	m_nPoolState = RunableObject::STATE_WAITING_STOP;
	m_taskPool.setActive(false);

	typeThreadList::iterator itThread, itThreadEnd = m_listThread.end();
	for ( itThread = m_listThread.begin(); itThread != itThreadEnd; ++itThread )
	{
		CycleRunningThread * pThread = (*itThread);
		pThread->stop();
	}

	m_nNotFinishTaskCount -= m_taskPool.getCurTaskCount();
	m_taskPool.clearAllTask();
	while ( m_nCurThreadCount > 0 )
		m_CondForThreadStop.wait(m_lockForThread);
}

bool ThreadPool::tryPushTask(RunableObject * pTask, bool bAutoDelete, const TimeValue & tvTimeWait)
{
	Task auDelete(pTask, bAutoDelete);
	if ( isPoolStart() && isPoolBusy() )
		buildThread();

	if ( !m_taskPool.pushTask(pTask, bAutoDelete, tvTimeWait) )
		return false;
	auDelete.release();
	AutoMutexLock auLock(m_lockForThread);
	++m_nNotFinishTaskCount;
	return true;
}

void ThreadPool::waitUntilAllTaskFinish(void)
{
	AutoMutexLock auLock(m_lockForFinishTask);
	while ( m_nNotFinishTaskCount > 0 )
		m_CondForFinishTask.wait(m_lockForFinishTask);
}

bool ThreadPool::isPoolStart(void) const
{
	return ( m_nPoolState == RunableObject::STATE_READY || m_nPoolState == RunableObject::STATE_RUNNING
				|| m_nPoolState == RunableObject::STATE_SUSPEND );
}

bool ThreadPool::buildThread(void)
{
	AutoMutexLock auLock(m_lockForThread);
	if ( m_nCurThreadCount >= m_nMaxThreadCount )
		return false;

	std::auto_ptr<CycleRunningThread> auThread( new CycleRunningThread(*this) );
	onBeforeThreadStart(auThread.get());
	m_listThread.push_back(auThread.get());
	auThread->createAndStart(m_nThreadStackSize);
	auThread.release();
	++m_nCurThreadCount;
	++m_nIdleThreadCount;
	return true;
}

void ThreadPool::addIdleThread(void)
{
	AutoMutexLock auLock(m_lockForThread);
	++m_nIdleThreadCount;
}

void ThreadPool::decIdleThread(void)
{
	AutoMutexLock auLock(m_lockForThread);
	--m_nIdleThreadCount;
}

void ThreadPool::onFinishTask(void)
{
	AutoMutexLock auLock(m_lockForFinishTask);
	--m_nNotFinishTaskCount;
	m_CondForFinishTask.notifyOne();
}

void ThreadPool::destroyThread(CycleRunningThread * pThread)
{
	onBeforeThreadDestroy(pThread);
	AutoMutexLock auLock(m_lockForThread);
	--m_nCurThreadCount;
	--m_nIdleThreadCount;
	typeThreadList::iterator itThread, itThreadEnd = m_listThread.end();
	for ( itThread = m_listThread.begin(); itThread != itThreadEnd; ++itThread )
	{
		CycleRunningThread * p = (*itThread);
		if ( p == pThread )
		{
			m_listThread.erase(itThread);
			delete pThread;
			break;
		}
	} //for ( itThread = m_listThread.begin(); itThread != itThreadEnd; ++itThread )
	m_CondForThreadStop.notifyOne();
}

RFC_NAMESPACE_END
