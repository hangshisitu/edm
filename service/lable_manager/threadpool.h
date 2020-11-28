#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <deque>
#include <pthread.h>
#include <vector>
#include <string>

typedef void* (*thread_func_pt)(void *param);

typedef struct _thread_param
{
    thread_func_pt thrfunc;
    void*          param;
} thread_param_t;

class Thread_Pool
{
public:
    enum{ THR_JOIN=0x01,THR_DETACH}; //PTHREAD_CREATE_JOINABLE,PTHREAD_CREATE_DETACHED

    explicit Thread_Pool();
    ~Thread_Pool();

    void start(int nthreads, int mode=THR_JOIN);
    void stop();
    void run(thread_func_pt pt, void* param);
    void wait();

private:
    static void* thread_svc(void*);
    thread_param_t take();

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    std::vector<pthread_t> threads_;
    std::deque<thread_param_t> queue_;
    bool running_;
};
#endif
