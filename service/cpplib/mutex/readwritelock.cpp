/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "mutex/readwritelock.h"

RFC_NAMESPACE_BEGIN

ReadWriteLock::ReadWriteLock(void)
{
	int nRet = pthread_rwlock_init(&m_lock, NULL);
	if ( nRet != 0 )
		throw MutexException(nRet, "ReadWriteLock::pthread_rwlock_init error");
}

ReadWriteLock::~ReadWriteLock(void)
{
	pthread_rwlock_destroy(&m_lock);
}

void ReadWriteLock::readLock(void) const throw(MutexException)
{
	int nRet = pthread_rwlock_rdlock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "ReadWriteLock::readLock error");
}

void ReadWriteLock::writeLock(void) const throw(MutexException)
{
	int nRet = pthread_rwlock_wrlock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "ReadWriteLock::writeLock error");
}

bool ReadWriteLock::tryReadLock(void) const throw(MutexException)
{
	int nRet = pthread_rwlock_tryrdlock(&m_lock);
	if ( nRet == 0 )
		return true;
	else if ( nRet != EBUSY )
		throw MutexException(nRet, "MutexLock::tryLock error");
	return false;
}

bool ReadWriteLock::tryWriteLock(void) const throw(MutexException)
{
	int nRet = pthread_rwlock_trywrlock(&m_lock);
	if ( nRet == 0 )
		return true;
	else if ( nRet != EBUSY )
		throw MutexException(nRet, "MutexLock::tryLock error");
	return false;
}

void ReadWriteLock::release(void) const throw(MutexException)
{
	int nRet = pthread_rwlock_unlock(&m_lock);
	if ( nRet != 0 )
		throw MutexException(nRet, "ReadWriteLock::release error");
}

RFC_NAMESPACE_END
