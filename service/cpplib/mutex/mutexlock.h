/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MUTEXLOCK_H_201008
#define RFC_MUTEXLOCK_H_201008

#include "mutex/mutexbase.h"

RFC_NAMESPACE_BEGIN

class ConditionVariant;

class MutexLock : public MutexBase
{
public:
	MutexLock(void);
	~MutexLock(void);

	void			lock(void) const throw(MutexException);
	bool			tryLock(void) const throw(MutexException);
	void			release(void) const throw(MutexException);

protected:
	friend class ConditionVariant;
	mutable pthread_mutex_t	m_lock;
};

RFC_NAMESPACE_END

#endif	//RFC_MUTEXLOCK_H_201008
