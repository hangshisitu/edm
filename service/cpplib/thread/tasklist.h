/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_TASKLIST_H_201008
#define RFC_TASKLIST_H_201008

#include "thread/runableobject.h"
#include "base/datetime.h"
#include <list>

RFC_NAMESPACE_BEGIN

class Task
{
public:
	Task(void) : m_pRunnableObj(NULL), m_bDeleteObjOnFinish(false) {}
	Task(RunableObject * pRunnable, bool bAutoDelete) : m_pRunnableObj(pRunnable), m_bDeleteObjOnFinish(bAutoDelete) {}
	Task(Task & _Right) : m_pRunnableObj(NULL), m_bDeleteObjOnFinish(false) { *this = _Right; }
	~Task(void);

	Task &					operator = (Task & _Right);
	void					attach(RunableObject * pRunnableObj, bool bDeleteObjOnFinish);
	RunableObject *			release(void);
	RunableObject *			getRunnableObj(void) { return m_pRunnableObj; }

private:
	RunableObject *			m_pRunnableObj;
	bool					m_bDeleteObjOnFinish;
};

class TaskQueue
{
public:
	TaskQueue(size_t nQueueMaxSize = 3000) : m_bActive(true), m_nQueueMaxSize(nQueueMaxSize), m_nCurTaskCount(0),
		m_nPopWaitCount(0), m_nPushWaitCount(0) {}
	~TaskQueue(void) { clearAllTask(); }

	bool				isActive(void) const { AutoMutexLock au(m_lockTaskList); return m_bActive; }
	void				setActive(bool bActive);

	size_t				getQueueMaxSize(void) const { AutoMutexLock au(m_lockTaskList); return m_nQueueMaxSize; }
	void				setQueueMaxSize(size_t nQueueMaxSize) { AutoMutexLock au(m_lockTaskList); m_nQueueMaxSize = nQueueMaxSize; }

	bool				queueIsFull(void) const { AutoMutexLock au(m_lockTaskList); return m_nCurTaskCount >= m_nQueueMaxSize; }
	bool				queueIsEmpty(void) const { AutoMutexLock au(m_lockTaskList); return m_nCurTaskCount == 0; }

	size_t				getCurTaskCount(void) const { AutoMutexLock au(m_lockTaskList); return m_nCurTaskCount; }
	size_t				getPopWaitCount(void) const { AutoMutexLock au(m_lockTaskList); return m_nPopWaitCount; }
	size_t				getPushWaitCount(void) const { AutoMutexLock au(m_lockTaskList); return m_nPushWaitCount; }

	template<class typeOP>
	void				forEachTask(typeOP & op)
	{
		AutoMutexLock au(m_lockTaskList);
		std::for_each(m_listTask.begin(), m_listTask.end(), op);
	}

	void				clearAllTask(void);

	// push a task, if m_listTask is full, will wait tvTimeWait. return whether push a task
	bool				pushTask(RunableObject * pRunnable, bool bAutoDelete, const TimeValue & tvTimeWait);

	// pop a task, if m_listTask is empty, will wait tvTimeWait. return whether pop a task
	bool				popTask(Task & aTask, const TimeValue & tvTimeWait);

protected:
	bool				m_bActive;
	size_t				m_nQueueMaxSize;
	size_t				m_nCurTaskCount;
	size_t				m_nPopWaitCount, m_nPushWaitCount;		// 正在等待pop/push的数量

	typedef std::list<Task*>	typeTaskList;
	typeTaskList		m_listTask;
	MutexLock			m_lockTaskList;
	ConditionVariant	m_condTaskList;
};

RFC_NAMESPACE_END

#endif	//RFC_TASKLIST_H_201008


