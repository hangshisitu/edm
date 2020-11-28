#include "tool.h"


#ifndef _H_task
#define _H_task

typedef struct _MailList
{
	char *sMailboxList;
	char *sOffsetList;
	int nMailboxListSize;
	int nMailboxListUsed;
	int nOffsetListSize;
	int nOffsetListUsed;
	int nParamName[30];
	char sParamLine[1024];
	pthread_mutex_t *pLock;
	char sDomain[30];
	int nCount;
	int nSum;
	int nSend;
	int nSuccess;
	int nFail;
	int nDelay;
	int nRound;
	int nNowPos;
	int nTime;
	char sID[12];
	int nNotFound;
	int nActive;
	int nNormal;
	int nSilent;
	int nFailNotFound;
	int nFailDns;
	int nFailSpam;
	
	
	
}MailList;


typedef struct _DomainAlias
{
	char *sAlias;
	int nAliasNum;
	char *sDomain;
	int nDomainNum;
}DomainAlias;


typedef struct _TaskQueue
{
	MailList *pMailList;
	DomainAlias *pAlias;
	char sID[12];
	char *sModula;
	int nModulaSize;
	int nTotalCount;
	int nTotalSuccess;
	int nTotalFail;
	int nTotalDelay;
	int nTotalSend;
	time_t tBeginTime;
	time_t tEndTime;
	struct _TaskQueue *pPrev;
	struct _TaskQueue *pNext;
	pthread_mutex_t *pLock;
	time_t tFinishTime;
	time_t tPauseTime;
	char sFileName[200];
	int nQQSendBackup;
	
}TaskQueue;

#endif






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
	int nCacheSocket2;
	int nFlagCache2;
	pthread_mutex_t *pLockCache2;
	
	int nWaterRound;
	int nWaterOffset;
	int nWaterFinishSum;
	pthread_mutex_t *pLockWater;
	
	int nQQDaySend;
	int nDayWaterMode;
	char sDayWaterFrom[513];
	int nDayWaterFrom;
	int nDayWaterFlag;
	
	
	DB *dbList; // from  D_PostPackage
	//DB *dbList2;
	DB *dbNumber;
	pthread_mutex_t *pLockTriger;
	pthread_mutex_t *pLockTrigerNum;
	
	DomainAlias *pDomainAlias;
	
	
}DABUDAI;
#endif



#ifndef _H_c_error
#define _H_c_error




#define D_NetReadIntFail							1
#define D_NetReadMsgFail							2
#define D_NetWriteFail								3
#define D_PackLengError								4						
#define D_JavaPackage									5
#define D_TransNotFinish							6
#define D_TransHasFinish							7
#define D_OpenFileError								8
#define D_StopPackage									9
#define D_RestPackage									10
#define D_PausPackage									11
#define D_ShowPackage									12
#define D_StatPackage									13
#define D_PostPackage									14







#endif

