/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_TIMEDRWLOCK_H_201008
#define RFC_TIMEDRWLOCK_H_201008

#include "mutex/conditionvariant.h"
#include "mutex/mutexlock.h"

RFC_NAMESPACE_BEGIN

class TimedReadWriteLock : public MutexBase
{
public:
	TimedReadWriteLock(void);

	bool						isReadLocked(void) const;
	bool						isWriteLocked(void) const;

	bool						timedWaitReadLock(const TimeValue & tvTimeWait) const throw(MutexException);
	bool						timedWaitReadLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException);
	void						waitUntilReadLock(void) const throw(MutexException);
	bool						tryReadLock(void) const throw(MutexException);

	bool						timedWaitWriteLock(const TimeValue & tvTimeWait) const throw(MutexException);
	bool						timedWaitWriteLockUntil(const TimeValue & tvTimeUntil) const throw(MutexException);
	void						waitUntilWriteLock(void) const throw(MutexException);
	bool						tryWriteLock(void) const throw(MutexException);

	void						release(void) const throw(MutexException);

protected:
	mutable bool				m_bWriteLocked;
	mutable int					m_nReadLockedCount, m_nWaitingForWriteLock;
	mutable MutexLock			m_mutexLock;
	mutable ConditionVariant	m_readCondVariant, m_writeCondVariant;
};

RFC_NAMESPACE_END

#endif	//RFC_TIMEDRWLOCK_H_201008
