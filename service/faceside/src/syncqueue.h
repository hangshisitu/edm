#ifndef _SYNCQUEUE_H
#define _SYNCQUEUE_H

#include <deque>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "synclock.h"

template<typename T>
class SyncQueue
{
public:
    enum {MQ_INFINITE = -1, MQ_NOTWAIT = 0};

    explicit SyncQueue(int capacity)
    {
        capacity_ = capacity;
        deactivate_ = 0;
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&not_empty_, NULL);
        pthread_cond_init(&not_full_, NULL);
    }

    ~SyncQueue()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&not_empty_);
        pthread_cond_destroy(&not_full_);
    }

    int put(const T& x, int seconds = MQ_INFINITE) //timeout
    {
        SyncLock lock(mutex_);

        while((int)queue_.size() == capacity_)
        {
            if (seconds < 0){
                pthread_cond_wait(&not_full_, &mutex_);
            }
            else {
	            struct timespec abstime;
	            clock_gettime(CLOCK_REALTIME, &abstime);
	            abstime.tv_sec += seconds;

	            if (ETIMEDOUT == pthread_cond_timedwait(&not_full_, &mutex_, &abstime)){
	            	return -2;
                }
            }

            if (this->deactivate_)
                return -1;
        }

        queue_.push_back(x);
        pthread_cond_signal(&not_empty_);
        return 0;
    }

    int get(T& x, int seconds = MQ_INFINITE) //timeout
    {
        SyncLock lock(mutex_);

        while (queue_.empty())
        {
            if (seconds < 0){
                pthread_cond_wait(&not_empty_, &mutex_);
            }
            else{
	            struct timespec abstime;
	            clock_gettime(CLOCK_REALTIME, &abstime);
	            abstime.tv_sec += seconds;

	            if (ETIMEDOUT == pthread_cond_timedwait(&not_empty_, &mutex_, &abstime)){
	            	return -2;
                }
            }

            if (this->deactivate_)
                return -1;
        }

        x = queue_.front();
        queue_.pop_front();
        pthread_cond_signal(&not_full_);
        return 0;
    }

    int deactivate()
    {
        deactivate_ = 1;
        pthread_cond_broadcast(&not_full_);
        pthread_cond_broadcast(&not_empty_);
        return 0;
    }

    bool empty()
    {
        SyncLock lock(mutex_);
        return queue_.empty();
    }

    bool full()
    {
        SyncLock lock(mutex_);
        return (int)queue_.size() == capacity_;
    }

    size_t size()
    {
        SyncLock lock(mutex_);
        return queue_.size();
    }

protected:

private:
    pthread_mutex_t mutex_;
    pthread_cond_t  not_empty_;
    pthread_cond_t  not_full_;
    std::deque<T>   queue_;
    int             capacity_;
    int             deactivate_;
};

#endif  //_SYNCQUEUE_H
