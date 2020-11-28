/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MUTEXBASE_H_201008
#define RFC_MUTEXBASE_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"
#include <pthread.h>

RFC_NAMESPACE_BEGIN

typedef pthread_t				typeThreaID;

DECLARE_EXCEPTION(MutexException, Exception)

class NullLock : private NonCopyAble
{
public:
	NullLock(void) {}
	~NullLock(void) {}
	void lock(void) const {}
	bool tryLock(void) const { return true; }
	void release(void) const {}
};

class MutexBase : private NonCopyAble
{
public:
	MutexBase(void) : m_nWaitingLockCount(0) {};
	int					getWaitingLockCount(void) const { return m_nWaitingLockCount; }

protected:
	mutable int			m_nWaitingLockCount;
};

RFC_NAMESPACE_END

#endif	//RFC_MUTEXBASE_H_201008
