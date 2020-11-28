#include "threadpool.h"
#include "log.h"

using namespace std;

class ThrGuard
{
public:
    ThrGuard(pthread_mutex_t & mutex):mutex_(mutex)
    { 
        pthread_mutex_lock(&mutex_);
    }

    ~ThrGuard() 
    {
        pthread_mutex_unlock(&mutex_);
    }

private:
    ThrGuard(const ThrGuard&);
    pthread_mutex_t & mutex_;
};


Thread_Pool::Thread_Pool():running_(false)
{
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&cond_, NULL);
}

Thread_Pool::~Thread_Pool()
{
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_);
}

void Thread_Pool::start( int nthreads, int mode )
{
    if (running_){
        return;
    }

    running_ = true;
    threads_.clear();

    for (int i = 0; i < nthreads; ++i)
    {
        pthread_t pid;
        pthread_attr_t attr = {0};
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, mode==THR_JOIN?PTHREAD_CREATE_JOINABLE:PTHREAD_CREATE_DETACHED);
        switch (pthread_create(&pid, &attr, &Thread_Pool::thread_svc, this))
        {
        case 0:
            threads_.push_back(pid);
            break;
        default:
            printf ("Failed in pthread_create\n");
            break;
        }
    }
}

void Thread_Pool::stop()
{
    ThrGuard lock(mutex_);
    running_ = false;
    pthread_cond_broadcast(&cond_);
}

void Thread_Pool::run( thread_func_pt task, void* param)
{
    ThrGuard lock(mutex_);
    thread_param_t thrparam = {task,param};
    queue_.push_back(thrparam);
    LOG_MSG(("push back thread pool(%d)\n", queue_.size()));
    pthread_cond_signal(&cond_);
}

thread_param_t Thread_Pool::take()
{
    ThrGuard lock(mutex_);

    while (queue_.empty() && running_)
    {
        pthread_cond_wait(&cond_, &mutex_);
    }

    thread_param_t task;
    task.thrfunc = NULL;
    task.param = NULL;
    if(!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        LOG_MSG(("pop front thread pool(%d)\n", queue_.size()));
    }
    return task;
}

void* Thread_Pool::thread_svc( void* param)
{
    Thread_Pool* tp = (Thread_Pool*)param;
    if (tp)
    {
        while (tp->running_)
        {
            thread_param_t task = tp->take();
            if (task.thrfunc){
                task.thrfunc(task.param);
            }
        }
    }

    return (void*)0;
}

void Thread_Pool::wait()
{
    for (int i=0; i<threads_.size(); i++)
    {
        pthread_join(threads_[i], NULL);
    }

    threads_.clear();
}
