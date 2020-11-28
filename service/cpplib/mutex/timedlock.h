/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_TIMEDLOCK_H_201008
#define RFC_TIMEDLOCK_H_201008

#include "mutex/conditionvariant.h"
#include "mutex/mutexlock.h"

RFC_NAMESPACE_BEGIN

class TimedLock : public MutexBase
{
public:
	TimedLock(void);

	bool						isLocked(void) const { return m_bLocked; }

	bool						timedWaitLock(const TimeValue & tvTimeWait) const throw(MutexException);
	bool						timedWaitLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException);
	void						waitUntilLock(void) const throw(MutexException);
	bool						tryLock(void) const throw(MutexException);
	void						release(void) const throw(MutexException);

protected:
	mutable bool				m_bLocked;		
	mutable MutexLock			m_mutexLock;
	mutable ConditionVariant	m_condVariant;
};

RFC_NAMESPACE_END

#endif	//RFC_TIMEDLOCK_H_201008
