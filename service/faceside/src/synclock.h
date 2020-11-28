#ifndef _SYNCLOCK_H_
#define _SYNCLOCK_H_

#include <pthread.h>

class SyncLock
{
public:
    static void init(pthread_mutex_t & m){
        pthread_mutex_init(&m, NULL);
    }

    SyncLock(pthread_mutex_t & mutex):mutex_(mutex){ 
        pthread_mutex_lock(&mutex_);
    }

    ~SyncLock() {
        pthread_mutex_unlock(&mutex_);
    }

private:
    SyncLock(const SyncLock&);
    pthread_mutex_t & mutex_;
};

class SyncRwLock
{
public:

    enum {RLOCK,WLOCK};

    static void init(pthread_rwlock_t & lock){
        pthread_rwlock_init(&lock, NULL);
    }

    SyncRwLock(pthread_rwlock_t & lock, int type):lock_(lock)
    {
        if (type==RLOCK)
            pthread_rwlock_rdlock(&lock_);
        else
            pthread_rwlock_wrlock(&lock_);
    }

    ~SyncRwLock() {
        pthread_rwlock_unlock(&lock_);
    }

private:
    SyncRwLock(const SyncRwLock&);
    pthread_rwlock_t & lock_;
};

#endif

