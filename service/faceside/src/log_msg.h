#ifndef _LOG_MSG_H_
#define _LOG_MSG_H_

#include "logger.h"
#include "synclock.h"

extern pthread_mutex_t loglock;

#define LOG_MSG(x) do{\
    SyncLock lock(loglock);\
    Logger::instance()->write x ;\
}while(0)

#endif
