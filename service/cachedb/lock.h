#ifndef __LOCK_H__
#define __LOCK_H__

#ifdef _WIN32
#   include <windows.h>
#else
#   include <pthread.h>
#endif

template < class T>
class AutoGuard
{
public:
    AutoGuard(T & t):t_lock(t) { t_lock.Lock();}
    ~AutoGuard() { t_lock.UnLock();}

    void Lock() {return t_lock.Lock();}
    void Unlock() {return t_lock.UnLock();};

private:
    AutoGuard(const AutoGuard&);
    T &t_lock;
};

enum {RLOCK,WLOCK};
template < class T>
class RwGuard
{
public:
    RwGuard(T & t, int type):t_lock(t) { 
    	switch(type){
    	case RLOCK:t_lock.RLock();
    		break;
    	case WLOCK:t_lock.WLock();
    	    break;
    	default:break;
    	}
    }
    ~RwGuard() { t_lock.UnLock();}

    void RLock() {return t_lock.RLock();}
    void WLock() {return t_lock.WLock();}
    void Unlock() {return t_lock.UnLock();};

private:
    RwGuard(const RwGuard&);
    T &t_lock;
};

class CLock
{
public:
    CLock(void) {};
    virtual ~CLock() {};

    virtual void Lock() = 0;
    virtual void UnLock() = 0;

private:
    CLock(const CLock&) {};
};

#ifdef _WIN32
#   define SYNCLOCK_TYPE    CRITICAL_SECTION
#else
#   define SYNCLOCK_TYPE    pthread_mutex_t
#endif

class SyncLock //:public CLock
{
public:
    SyncLock(void);
    ~SyncLock();

    void Lock();
    void UnLock();
    int  TryLock();

private:
    SyncLock(const SyncLock&);
    SYNCLOCK_TYPE m_SyncLock;
};

class RwLock
{
public:
    RwLock(void);
    ~RwLock();
    
    int RLock();
    int WLock();
    int UnLock();
private:
    pthread_rwlock_t _lock;
};

#endif
