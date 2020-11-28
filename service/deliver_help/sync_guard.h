
#ifndef _SYNC_GUARD_H
#define _SYNC_GUARD_H

#include <pthread.h>

class Sync_Guard
{
public:
    Sync_Guard(pthread_mutex_t & mutex)
        :mutex_(mutex){ pthread_mutex_lock(&mutex_);}
    ~Sync_Guard() {pthread_mutex_unlock(&mutex_);}
private:
    Sync_Guard(const Sync_Guard&);
    pthread_mutex_t & mutex_;
};

#endif

