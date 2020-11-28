/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_READWRITELOCK_H_201008
#define RFC_READWRITELOCK_H_201008

#include "mutex/mutexbase.h"

RFC_NAMESPACE_BEGIN

class ReadWriteLock : public MutexBase
{
public:
	ReadWriteLock(void);
	~ReadWriteLock(void);

	void			readLock(void) const throw(MutexException);
	void			writeLock(void) const throw(MutexException);
	bool			tryReadLock(void) const throw(MutexException);
	bool			tryWriteLock(void) const throw(MutexException);
	void			release(void) const throw(MutexException);

protected:
	mutable pthread_rwlock_t	m_lock;
};

RFC_NAMESPACE_END

#endif	//RFC_READWRITELOCK_H_201008
