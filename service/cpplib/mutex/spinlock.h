/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "rfcdefine.h"

#if (SUPPORT_POSIX_SPINLOCK != 0)

#ifndef RFC_SPINLOCK_H_201108
#define RFC_SPINLOCK_H_201108

#include "mutex/mutexbase.h"

RFC_NAMESPACE_BEGIN

//SpinLock 是一种轻量级的同步方法，一种非阻塞锁。当 lock 操作被阻塞时，并不是把自己挂到一个等待队列，而是死循环 CPU 空转等待其他线程释放锁。
//也就是说，等待过程中spin lock还会消耗cpu资源。适合临界区计算非常简单的场合

class SpinLock : public MutexBase
{
public:
	SpinLock(void);
	~SpinLock(void);

	void			lock(void) const throw(MutexException);
	bool			tryLock(void) const throw(MutexException);
	void			release(void) const throw(MutexException);

protected:
	mutable pthread_spinlock_t	m_lock;
};

RFC_NAMESPACE_END

#endif	//RFC_SPINLOCK_H_201108

#endif	// #if (SUPPORT_POSIX_SPINLOCK != 0)
