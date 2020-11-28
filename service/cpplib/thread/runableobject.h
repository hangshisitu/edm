/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_RUNABLEOBJECT_H_201008
#define RFC_RUNABLEOBJECT_H_201008

#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

//继承AsyncRunableObject实现run()来满足不同线程业务

class RunableObject
{
public:
	enum typeState
	{
		STATE_INIT,				//线程还没初始化
		STATE_READY,			//就绪状态
		STATE_RUNNING,			//线程正在执行
		STATE_SUSPEND,			//线程已挂起
		STATE_WAITING_STOP,		//正等待结束
		STATE_DEAD,				//线程已结束
	};

	RunableObject(void) : m_nState(STATE_INIT), m_nResult(0) {}
	virtual ~RunableObject(void) {}

	//static const char * getStateString(typeState nState);

	typeState		getState(void) const { return m_nState; }
	int				getResult(void) const { return m_nResult; }

	virtual int		run(void) = 0;
	virtual bool	start(void);
	virtual bool	stop(void);
	virtual int		waitUntilStop(void) { return getResult(); }

	//to support pthread_create
	void	operator()(void) { start(); }

protected:
	virtual bool	beforeStart(void) { return true; }
	virtual bool	beforeStop(void) { return true; }
	virtual bool	afterStop(void) { return true; }

	bool			shouldExit(void) const { return (m_nState == STATE_WAITING_STOP || m_nState == STATE_DEAD); }
	void			setState(typeState nState) { m_nState = nState; }
	typeState		m_nState;
	int				m_nResult;
};

class AsyncRunableObject : public RunableObject
{
public:
	AsyncRunableObject(void) : RunableObject() {}
	virtual ~AsyncRunableObject(void) { stopAndWaitResult(); }

	virtual bool	start(void);
	virtual bool	stop(void);			//don't wait result
	virtual int		stopAndWaitResult(void);
	virtual int		waitUntilStop(void);

	bool			wakeup(void);

protected:
	//! suspend any seconds
	bool			suspend(const TimeValue & timeVal);

	MutexLock		m_LockState;
	ConditionVariant	m_ConditionState;
	ConditionVariant	m_ConditionSleep;
};

RFC_NAMESPACE_END

#endif	//RFC_RUNABLEOBJECT_H_201008


