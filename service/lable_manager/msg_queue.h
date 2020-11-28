//星期四 2013年3月28日, 10:02:07 by wEide
#ifndef _MSG_QUEUE_H
#define _MSG_QUEUE_H

#include <deque>
#include <pthread.h>
#include <errno.h>
#include <time.h>

class MsgGuard
{
public:
    MsgGuard(pthread_mutex_t & mutex)
        :mutex_(mutex){ pthread_mutex_lock(&mutex_);}

    ~MsgGuard() {pthread_mutex_unlock(&mutex_);}
private:
    MsgGuard(const MsgGuard&);
    pthread_mutex_t & mutex_;
};

template<typename T>
class Msg_Queue
{
public:
    enum {MQ_INFINITE = -1, MQ_NOTWAIT = 0};

    // >0:最大容量 <0:不限容量 ==0:。。。
    explicit Msg_Queue(int capacity) //capacity = {<0, >0, ==0}
    {
        capacity_ = capacity;
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&not_empty_, NULL);
        pthread_cond_init(&not_full_, NULL);
    }

    ~Msg_Queue()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&not_empty_);
        pthread_cond_destroy(&not_full_);
    }

    int put(const T& x, int seconds = MQ_INFINITE) //timeout
    {
        MsgGuard lock(mutex_);

        while(queue_.size() == capacity_)
        {
            if (seconds < 0){
                pthread_cond_wait(&not_full_, &mutex_);
            }
            else
            {
	            struct timespec abstime;
	            clock_gettime(CLOCK_REALTIME, &abstime);
	            abstime.tv_sec += seconds;
	            if (ETIMEDOUT == pthread_cond_timedwait(&not_full_, &mutex_, &abstime))
	            	return -1;
            }
        }

        queue_.push_back(x);
        pthread_cond_signal(&not_empty_);
        return 0;
    }

    int get(T& x, int seconds = MQ_INFINITE) //timeout
    {
        MsgGuard lock(mutex_);

        while (queue_.empty())
        {
            if (seconds < 0){
                pthread_cond_wait(&not_empty_, &mutex_);
            }
            else
            {
	            struct timespec abstime;
	            clock_gettime(CLOCK_REALTIME, &abstime);
	            abstime.tv_sec += seconds;
	            if (ETIMEDOUT == pthread_cond_timedwait(&not_empty_, &mutex_, &abstime))
	            	return -1;
            }
        }

        x = queue_.front();
        queue_.pop_front();
        pthread_cond_signal(&not_full_);
        return 0;
    }

    bool empty()
    {
        MsgGuard lock(mutex_);
        return queue_.empty();
    }

    bool full()
    {
        MsgGuard lock(mutex_);
        return queue_.size() == capacity_;
    }

    size_t size()
    {
        MsgGuard lock(mutex_);
        return queue_.size();
    }

protected:

private:
    pthread_mutex_t mutex_;
    pthread_cond_t  not_empty_;
    pthread_cond_t  not_full_;
    std::deque<T>   queue_;
    int             capacity_;
};

#endif  //_MSG_QUEUE_H
