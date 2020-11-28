/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/mutexlock.h"

RFC_NAMESPACE_BEGIN

MutexLock::MutexLock(void)
{
	int nRet = pthread_mutex_init(&m_lock, NULL);
	if ( nRet != 0 )
		throw MutexException(nRet, "MutexLock::pthread_mutex_init error");
}

MutexLock::~MutexLock(void)
{
	pthread_mutex_destroy(&m_lock);
}

void MutexLock::lock(void) const throw(MutexException)
{
	int nRet = pthread_mutex_lock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "MutexLock::lock error");
}

bool MutexLock::tryLock(void) const throw(MutexException)
{
	int nRet = pthread_mutex_trylock(&m_lock);
	if ( nRet == 0 )
		return true;
	else if ( nRet != EBUSY )
		throw MutexException(nRet, "MutexLock::tryLock error");
	return false;
}

void MutexLock::release(void) const throw(MutexException)
{
	int nRet = pthread_mutex_unlock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "MutexLock::release error");
}

RFC_NAMESPACE_END
