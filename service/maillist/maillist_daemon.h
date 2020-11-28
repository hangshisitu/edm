#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <mcheck.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "maillist7.h"
#include "c_error.h"


/*
#ifndef _H_dabudai
#define _H_dabudai
typedef struct _DABUDAI
{
	int nMaxNetSocket;														/////epoll队列中最大的有效socket
	int nMinNetSocket;														/////epoll队列中最小的有效socket
	
	pthread_rwlock_t *pLockRWTask;
	pthread_mutex_t *pLockTask;
	pthread_mutex_t *pLockFile;
	
	int nTaskNum;
	int nTaskOffset;
	pthread_mutex_t *pLockNet;
	
	TaskQueue *pBegin;
	TaskQueue *pEnd;
	
	int nEventNum;
	
	int nDay;
	int nMonth;
	int nYear;
	FILE *pFileLog;
	pthread_mutex_t *pLockLog;
	int nLogSize;
	int nLogNum;
	
	time_t tReportCacheTime;
	int nCacheSocket;
}DABUDAI;
#endif
*/


#ifndef _H_maillist_daemon
#define _H_maillist_daemon


#define _H_Version		"iedm system, version:0.1.\n"


static int InitialDaBuDai(DABUDAI *pDabudai);

static int FreeDaBuDai(DABUDAI *pDabudai);



#endif


