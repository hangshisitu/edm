/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_THREADPOOL_H_201008
#define RFC_THREADPOOL_H_201008

#include "thread/thread.h"
#include "thread/tasklist.h"
#include <list>

RFC_NAMESPACE_BEGIN

class ThreadPool
{
public:
	ThreadPool(const char * lpszPoolName) : m_strPoolName(lpszPoolName), m_nPoolState(RunableObject::STATE_INIT) { initPool(); }
	virtual ~ThreadPool(void) { stopPool(); }

	//@param nMinThreadCount		the thread count at least in pool
	//@param nMaxThreadCount		the thread count at most in pool
	//@param nTaskPoolSize			the task pool size
	//@param nThreadStackSize		every thread stack size (byte)
	//@param nIdleTimeout			while m_nCurThreadCount larger than nMinThreadCount and m_taskPool is empty,
	//								thread will wait nIdleTimeout seconds until has new task, if no task will die
	void					initPool(rfc_uint_32 nMinThreadCount = 5, rfc_uint_32 nMaxThreadCount = 100,
									size_t nTaskPoolSize = 3000, int nThreadStackSize = 64 * 1024, time_t nIdleTimeout = 30);

	//@param nBuildThreadCount:		will build nBuildThreadCount threads after start
	void					startPool(rfc_uint_32 nBuildThreadCount = 0);

	//@param bClearAllTask:			clear all task in pool
	//@param bWaitUntilAllTaskFinish: wait until all task finish
	void					stopPool(void);

	void					waitUntilAllTaskFinish(void);

	bool					tryPushTask(RunableObject * pTask, bool bAutoDelete, const TimeValue & tvTimeWait = TimeValue::g_tvInfiniteTime);

	bool					shouldExit(void) const { return m_nPoolState == RunableObject::STATE_WAITING_STOP || m_nPoolState == RunableObject::STATE_DEAD; }
	const stdstring &		getPoolName(void) const { return m_strPoolName; }

	bool					isTaskPoolEmpty(void) const { return m_taskPool.queueIsEmpty(); }
	bool					isPoolIdle(void) const { return m_nCurThreadCount >= m_nMinThreadCount && m_taskPool.queueIsEmpty(); }
	bool					isPoolBusy(void) const { return m_nIdleThreadCount < m_taskPool.getCurTaskCount(); }
	bool					isPoolFull(void) const { return isPoolBusy() && m_nCurThreadCount >= m_nMaxThreadCount && m_taskPool.queueIsFull();}

protected:
	class CycleRunningThread : private Thread
	{
	public:
		CycleRunningThread(ThreadPool & threadPool) : m_threadPool(threadPool) {}
		virtual bool		createAndStart(int nStackSize, int nPriority = Thread::PRIORITY_NORMAL);

		virtual bool		stop(void);

	protected:
		bool				waitForTask();
		void				runTask();
		virtual int			run(void) { return 0; }
		virtual bool		afterStop(void);

	private:
		ThreadPool &		m_threadPool;
		Task				m_task;
		MutexLock			m_lockTask;
		static void *		cycleRunningFunc(void * arg);
	};

	friend class			CycleRunningThread;
	virtual void			onBeforeThreadStart(CycleRunningThread * pThread) {}
	virtual void			onBeforeThreadDestroy(CycleRunningThread * pThread) {}
	bool					isPoolStart(void) const;
	bool					buildThread(void);
	void					addIdleThread(void);		//thread is idle
	void					decIdleThread(void);		//thread is busy
	void					onFinishTask(void);
	void					destroyThread(CycleRunningThread * pThread);

private:
	const stdstring			m_strPoolName;
	rfc_uint_32				m_nMinThreadCount, m_nMaxThreadCount;
	int						m_nThreadStackSize;
	TimeValue				m_tvIdleTimeout;

	typedef		std::list< CycleRunningThread * >			typeThreadList;
	RunableObject::typeState m_nPoolState;
	rfc_uint_32				m_nCurThreadCount, m_nIdleThreadCount, m_nNotFinishTaskCount;
	MutexLock				m_lockForThread, m_lockForFinishTask;
	ConditionVariant		m_CondForThreadStop, m_CondForFinishTask;
	typeThreadList			m_listThread;
	TaskQueue				m_taskPool;
};

RFC_NAMESPACE_END

#endif	//RFC_THREADPOOL_H_201008

