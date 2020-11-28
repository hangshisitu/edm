/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/spinlock.h"

#if (SUPPORT_POSIX_SPINLOCK != 0)

RFC_NAMESPACE_BEGIN

SpinLock::SpinLock(void)
{
	int nRet = pthread_spin_init(&m_lock, 0);
	if ( nRet != 0 )
		throw MutexException(nRet, "SpinLock::pthread_spin_init error");
}

SpinLock::~SpinLock(void)
{
	pthread_spin_destroy(&m_lock);
}

void SpinLock::lock(void) const throw(MutexException)
{
	int nRet = pthread_spin_lock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "SpinLock::lock error");
}

bool SpinLock::tryLock(void) const throw(MutexException)
{
	int nRet = pthread_spin_trylock(&m_lock);
	if ( nRet == 0 )
		return true;
	else if ( nRet != EBUSY )
		throw MutexException(nRet, "SpinLock::tryLock error");
	return false;
}

void SpinLock::release(void) const throw(MutexException)
{
	int nRet = pthread_spin_unlock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "SpinLock::release error");
}

RFC_NAMESPACE_END

#endif	// #if (SUPPORT_POSIX_SPINLOCK != 0)
