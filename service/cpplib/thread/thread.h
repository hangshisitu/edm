/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_THREAD_H_201008
#define RFC_THREAD_H_201008

#include "thread/threadcontext.h"
#include "thread/runableobject.h"

RFC_NAMESPACE_BEGIN

typedef void * (* ThreadEntryFuncPtr)(void * arg);
DECLARE_EXCEPTION(ThreadException, Exception)

class ThreadFunc
{
public:
	static int			createThread(ThreadEntryFuncPtr func, void * arg, int nStackSize, int nPrio, typeThreaID * pThreadID, bool bDetach);

	static inline int	threadJoin(typeThreaID nThreadID, void ** pStatus) { return pthread_join(nThreadID, pStatus); }
	static inline int	threadWaitUntilStop(typeThreaID nThreadID, void ** pStatus) { return threadJoin(nThreadID, pStatus); }

	static inline int	threadDetach(typeThreaID nThreadID) { return pthread_detach(nThreadID); }

	static inline int	threadExit(int nValue) { pthread_exit(&nValue); return nValue; }

	static void			threadYield(void) { pthread_yield(); }

	static typeThreaID	getSelfThreadID(void) { return pthread_self(); }
	static bool			threadEqaul(typeThreaID id1, typeThreaID id2) { return pthread_equal(id1, id2) != 0; }
};

class Thread : public AsyncRunableObject, private NonCopyAble
{
public:
	enum ThreadPriority
	{
		PRIORITY_HIGHEST		=	7,
		PRIORITY_HIGH			=	1,
		PRIORITY_NORMAL			=	0,
		PRIORITY_LOW			=	-1,
		PRIORITY_LOWEST			=	-7,
	};

	Thread(void) : m_bDeleteMeOnFinish(false), m_nThreadID(0) {}
	virtual ~Thread(void) {}

	bool				createAndStart(ThreadEntryFuncPtr entryFunc, int nStackSize = 64 * 1024, int nPriority = PRIORITY_NORMAL);
	virtual bool		createAndStart(int nStackSize = 64 * 1024, int nPriority = PRIORITY_NORMAL);

	typeThreaID			getThreadID(void) const { return m_nThreadID; }

	ThreadContext &		getThreadContext(void);
	void				setThreadContext(ThreadContext * pContext);

	void				setDeleteMeOnFinish(bool bDeleteMeOnFinish) { m_bDeleteMeOnFinish = bDeleteMeOnFinish; }
	bool				getDeleteMeOnFinish(void) const { return m_bDeleteMeOnFinish; }

protected:
	virtual bool		beforeCreate(void) { return true; }
	virtual bool		start(void) { return AsyncRunableObject::start(); }
	bool				m_bDeleteMeOnFinish;
	typeThreaID			m_nThreadID;

private:
	static void *		threadEntryFunc(void * arg);
};

RFC_NAMESPACE_END

#endif	//RFC_THREAD_H_201008

