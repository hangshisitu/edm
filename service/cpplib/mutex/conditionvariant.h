/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_CONDITION_VARIANT_H_201008
#define RFC_CONDITION_VARIANT_H_201008

#include "mutex/mutexbase.h"
#include "base/datetime.h"

RFC_NAMESPACE_BEGIN

class MutexLock;

class ConditionVariant : public MutexBase
{
public:
	ConditionVariant(void);
	~ConditionVariant(void);

	void			notifyOne(void) throw(MutexException);
	void			broadcastAll(void) throw(MutexException);
	void			wait(MutexLock & mutexLock) throw(MutexException);

	//if timeout return false else return true
	bool			timedWait(MutexLock & mutexLock, const TimeValue & tvTimeWait) throw(MutexException);
	bool			timedWaitUntil(MutexLock & mutexLock, const TimeValue & tvTimeUntil) throw(MutexException);

	template<typename typePredicate>
	void			wait(MutexLock & mutexLock, const typePredicate & waitUntilPred) throw(MutexException)
	{
		while ( !waitUntilPred() )
			wait(mutexLock);
	}

	template<typename typePredicate>
	bool			timedWaitUntil(MutexLock & mutexLock, const TimeValue & tvTimeUntil, const typePredicate & waitUntilPred) throw(MutexException)
	{
		while ( !waitUntilPred() )
		{
			if ( !timedWaitUntil(mutexLock, tvTimeUntil) )
				return false;
		} // while ( !waitUntilPred() )
		return true;
	}

	template<typename typePredicate>
	bool			timedWait(MutexLock & mutexLock, const TimeValue & tvTimeWait, const typePredicate & waitUntilPred) throw(MutexException)
	{
		return timedWaitUntil(mutexLock, TimeValue::getTimeUntil(tvTimeWait), waitUntilPred);
	}

protected:
	pthread_cond_t	m_cond;
};

RFC_NAMESPACE_END

#endif	//RFC_CONDITION_VARIANT_H_201008
