/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_RECURSIVELOCK_H_201008
#define RFC_RECURSIVELOCK_H_201008

#include "mutex/conditionvariant.h"
#include "mutex/mutexlock.h"

RFC_NAMESPACE_BEGIN

class RecursiveLock : public MutexBase
{
public:
	RecursiveLock(void);

	bool						isLocked(void) const { return m_nLockedCount > 0; }

	void						lock(void) const throw(MutexException);
	bool						tryLock(void) const throw(MutexException);
	void						release(void) const throw(MutexException);

protected:
	mutable int					m_nLockedCount;
	mutable typeThreaID			m_nHoldLockThreadID;
	mutable MutexLock			m_mutexLock;
	mutable ConditionVariant	m_condVariant;
};

RFC_NAMESPACE_END

#endif	//RFC_RECURSIVELOCK_H_201008
