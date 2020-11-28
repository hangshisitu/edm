#include "lock.h"

/* ----- class SyncLock ---*/
SyncLock::SyncLock()
{
#ifdef _WIN32
    ::InitializeCriticalSection(&m_SyncLock);
#else
    pthread_mutex_init(&m_SyncLock, NULL);
#endif
}

SyncLock::~SyncLock()
{
#ifdef _WIN32
    ::DeleteCriticalSection(&m_SyncLock);
#else
    pthread_mutex_destroy(&m_SyncLock);
#endif
}

void SyncLock::Lock()
{
#ifdef _WIN32
    ::EnterCriticalSection(&m_SyncLock);
#else
    pthread_mutex_lock(&m_SyncLock);
#endif
}
void SyncLock::UnLock()
{
#ifdef _WIN32
    ::LeaveCriticalSection(&m_SyncLock);
#else
    pthread_mutex_unlock(&m_SyncLock);
#endif
}

int SyncLock::TryLock()
{
#ifdef _WIN32
    return ::TryEnterCriticalSection(&m_SyncLock)==0?-1:0;
#else
    return pthread_mutex_trylock(&m_SyncLock)==0?0:-1;
#endif
}
