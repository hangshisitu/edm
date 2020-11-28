#include "pthreads.h"
#include "synclock.h"

using namespace std;

Pthreads::Pthreads()
:running_(false)
{
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&cond_, NULL);
}

Pthreads::~Pthreads()
{
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_);
}

void Pthreads::start( int nthreads, int mode )
{
    if (running_)
        return;

    running_ = true;
    threads_.clear();

    for (int i = 0; i < nthreads; ++i)
    {
        pthread_t pid;
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr,mode);

        if (0 == pthread_create(&pid, &attr, &Pthreads::svc, this))
            threads_.push_back(pid);
    }
}

void Pthreads::stop()
{
    SyncLock lock(mutex_);
    running_ = false;
    pthread_cond_broadcast(&cond_);
}

void Pthreads::run( pthread_pf task, void* arg)
{
    SyncLock lock(mutex_);
    thread_arg_t thrarg = {task,arg};
    queue_.push_back(thrarg);
    pthread_cond_signal(&cond_);
}

thread_arg_t Pthreads::take()
{
    SyncLock lock(mutex_);

    while (queue_.empty() && running_)
        pthread_cond_wait(&cond_, &mutex_);

    thread_arg_t task;
    task.thrfunc = NULL;
    task.arg = NULL;

    if(!queue_.empty()){
        task = queue_.front();
        queue_.pop_front();
    }

    return task;
}

void* Pthreads::svc( void* arg)
{
    Pthreads* tp = (Pthreads*)arg;

    if (!tp) 
        return (void*)0;

    while (tp->running_){
        thread_arg_t task = tp->take();

        if (task.thrfunc)
            task.thrfunc(task.arg);
    }

    return (void*)0;
}

void Pthreads::wait()
{
    for (int i=0; i<(int)threads_.size(); i++)
        pthread_join(threads_[i], NULL);
    threads_.clear();
}
