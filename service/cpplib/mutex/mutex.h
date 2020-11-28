/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MUTEX_H_201008
#define RFC_MUTEX_H_201008

#include "mutex/conditionvariant.h"
#include "mutex/mutexlock.h"
#include "mutex/spinlock.h"
#include "mutex/readwritelock.h"
#include "mutex/recursivelock.h"
#include "mutex/timedlock.h"
#include "mutex/timedreadwritelock.h"

RFC_NAMESPACE_BEGIN

#if (SUPPORT_POSIX_SPINLOCK == 0)
typedef MutexLock	SpinLock;		// 如果不支持spinlock,使用MutexLock当作spinlock
#endif

template<typename typeLock>
class AutoLockProxy
{
public:
	~AutoLockProxy(void)
	{
		try
		{
			release();
		}
		catch ( ... )
		{
		}
	}
	bool			isLocked(void) const { return m_bLocked; }
	void			release(void)
	{
		if ( m_bLocked )
		{
			m_lock.release();
			m_bLocked = false;
		}
	}

protected:
	AutoLockProxy(const typeLock & aLock) : m_bLocked(false), m_lock(aLock) {}
	bool					m_bLocked;
	const typeLock  &		m_lock;
};

template<typename typeLock>
class AutoMutexLockProxy : public AutoLockProxy<typeLock>
{
public:
	typedef AutoLockProxy<typeLock>		typeAutoLockProxy;
	AutoMutexLockProxy(const typeLock & aLock, bool bRetryLock = false) : typeAutoLockProxy(aLock)
	{
		if ( bRetryLock )
			tryLock();
		else
			lock();
	}

	void			lock(void)
	{
		if ( !typeAutoLockProxy::m_bLocked )
		{
			typeAutoLockProxy::m_lock.lock();
			typeAutoLockProxy::m_bLocked = true;
		}
	}

	bool			tryLock(void)
	{
		if ( !typeAutoLockProxy::m_bLocked )
			typeAutoLockProxy::m_bLocked = typeAutoLockProxy::m_lock.tryLock();
		return typeAutoLockProxy::m_bLocked;
	}	
};

template<typename typeLock>
class AutoMutexTryLockProxy : public AutoMutexLockProxy<typeLock>
{
public:
	AutoMutexTryLockProxy(const typeLock & aLock) : AutoMutexLockProxy<typeLock>(aLock, true) {}
};

//////////////////////////////////////////////////////////////////////////

class AutoReadWriteLockProxy : public AutoLockProxy<ReadWriteLock>
{
public:
	typedef AutoLockProxy<ReadWriteLock>		typeAutoLockProxy;
	bool			isReadLocked(void) const { return m_bReadLocked; }
	bool			isWriteLocked(void) const { return m_bWriteLocked; }

	void			readLock(void);
	void			writeLock(void);
	bool			tryReadLock(void);
	bool			tryWriteLock(void);

protected:
	AutoReadWriteLockProxy(const ReadWriteLock & aLock);
	bool			m_bReadLocked, m_bWriteLocked;
};

class AutoReadLockProxy : public AutoReadWriteLockProxy
{
public:
	AutoReadLockProxy(const ReadWriteLock & aLock) : AutoReadWriteLockProxy(aLock) { AutoReadWriteLockProxy::readLock(); }
};

class AutoWriteLockProxy : public AutoReadWriteLockProxy
{
public:
	AutoWriteLockProxy(const ReadWriteLock & aLock) : AutoReadWriteLockProxy(aLock) { AutoReadWriteLockProxy::writeLock(); }
};

class AutoTryReadLockProxy : public AutoReadWriteLockProxy
{
public:
	AutoTryReadLockProxy(const ReadWriteLock & aLock) : AutoReadWriteLockProxy(aLock) { AutoReadWriteLockProxy::tryReadLock(); }
};

class AutoTryWriteLockProxy : public AutoReadWriteLockProxy
{
public:
	AutoTryWriteLockProxy(const ReadWriteLock & aLock) : AutoReadWriteLockProxy(aLock) { AutoReadWriteLockProxy::tryWriteLock(); }
};

//////////////////////////////////////////////////////////////////////////

class AutoTimedLockProxy : public AutoLockProxy<TimedLock>
{
public:
	typedef AutoLockProxy<TimedLock>		typeAutoLockProxy;
	AutoTimedLockProxy(const TimedLock & aLock, const TimeValue & tvTimeWait);

	bool						timedWaitLock(const TimeValue & tvTimeWait);
	bool						timedWaitLockUntil(const TimeValue & tvTimeUntil);
	void						waitUntilLock(void);
	bool						tryLock(void);
};

//////////////////////////////////////////////////////////////////////////

typedef AutoMutexLockProxy<NullLock>				AutoNullLock;
typedef AutoMutexLockProxy<MutexLock>				AutoMutexLock;
typedef AutoMutexLockProxy<SpinLock>				AutoSpinLock;

typedef AutoMutexTryLockProxy<NullLock>				AutoNullTryLock;
typedef AutoMutexTryLockProxy<MutexLock>			AutoMutexTryLock;
typedef AutoMutexTryLockProxy<SpinLock>				AutoSpinTryLock;

typedef AutoReadLockProxy							AutoReadLock;
typedef AutoWriteLockProxy							AutoWriteLock;

typedef AutoTryReadLockProxy						AutoTryReadLock;
typedef AutoTryWriteLockProxy						AutoTryWriteLock;

typedef AutoTimedLockProxy							AutoTimedLock;

void delay(time_t nSecond);
void delay(time_t nSecond, long nMicroSecond);
void delay(const TimeValue & tvTimeWait);

void delayUntil(time_t nTimeUntil);
void delayUntil(const TimeValue & tvTimeUntil);

RFC_NAMESPACE_END

#endif	//RFC_MUTEX_H_201008
