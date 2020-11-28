#ifndef __PTHREADS_H__
#define __PTHREADS_H__

#include <deque>
#include <pthread.h>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::deque;

typedef 
void* (*pthread_pf)(void *arg);

typedef 
struct _thread_arg {
    pthread_pf thrfunc;
    void* arg;
} thread_arg_t;

class Pthreads 
{
public:
    enum{ 
        JOINABLE = PTHREAD_CREATE_JOINABLE,
        DETACHED = PTHREAD_CREATE_DETACHED
    }; 

    explicit Pthreads();
    ~Pthreads();

    void start(int nthreads, int mode=JOINABLE);
    void stop();
    void run(pthread_pf task, void* arg);
    void wait();

private:
    static void* svc(void*);
    thread_arg_t take();

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    vector<pthread_t> threads_;
    deque<thread_arg_t> queue_;

    bool running_;
};

#endif
