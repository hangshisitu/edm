/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/thread.h"

RFC_NAMESPACE_BEGIN

int ThreadFunc::createThread(ThreadEntryFuncPtr func, void * arg, int nStackSize, int nPriority, typeThreaID * pThreadID, bool bDetach)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr); // initialize attr with default attributes
	pthread_attr_setstacksize(&attr, nStackSize);

	if ( nPriority != 0 )
	{
		struct sched_param sched;
		int policy = sched_getscheduler(0);
		if ( policy < 0 )
			throw ThreadException("Error to get the thread scheduler");
		sched_getparam(0, &sched);

		nPriority = sched.sched_priority - nPriority;
		if ( nPriority > sched_get_priority_max(policy) )
			nPriority = sched_get_priority_max(policy);
		if ( nPriority < sched_get_priority_min(policy) )
			nPriority = sched_get_priority_min(policy);
		sched.sched_priority = nPriority;
		pthread_attr_setschedpolicy(&attr, policy);
		pthread_attr_setschedparam(&attr, &sched);

		if ( nPriority != 0 )
			pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	}

	int nErrno = pthread_create(pThreadID, &attr, func, arg);
	pthread_attr_destroy(&attr);
	if ( nErrno == 0 && bDetach )
		nErrno = pthread_detach( *pThreadID );
	return nErrno;
}

//////////////////////////////////////////////////////////////////////////

void * Thread::threadEntryFunc(void * arg)
{
	Thread * pThread = reinterpret_cast<Thread *>(arg);
	ThreadContext * pThreadContext = new ThreadContext;
	ThreadContextHolder::setThreadContext(pThreadContext);
	pThreadContext->setThreadID(pThread->m_nThreadID);
	pThread->start();
	ThreadFunc::threadExit(pThread->getResult());
	delete pThreadContext;
	if ( pThread->getDeleteMeOnFinish() )
		delete pThread;
	return 0;
}

bool Thread::createAndStart(ThreadEntryFuncPtr entryFunc, int nStackSize, int nPriority)
{
	AutoMutexLock au(m_LockState);
	if ( m_nState != RunableObject::STATE_INIT )
		return false;
	if ( !beforeCreate() )
		throw ThreadException("Error Before Create thread");

	int nRetCode = ThreadFunc::createThread(entryFunc, this, nStackSize, nPriority, &m_nThreadID, true);
	if ( nRetCode < 0 )
		throw ThreadException(nRetCode, "Start Thread Error");
	m_ConditionState.wait(m_LockState);
	return true;
}

bool Thread::createAndStart(int nStackSize, int nPriority)
{
	return createAndStart(Thread::threadEntryFunc, nStackSize, nPriority);
}

static ThreadKey g_threadContextHolder;
ThreadContext &	Thread::getThreadContext(void)
{
	ThreadContext * pContext = static_cast<ThreadContext *>(g_threadContextHolder.getData());
	if ( pContext == NULL)
		throw Exception("Thread context not exist");
	return *pContext;
}

void Thread::setThreadContext(ThreadContext * pContext)
{
	g_threadContextHolder.setData(pContext);
}

RFC_NAMESPACE_END

