#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "c_error.h"
#include "daemon_conf.h"
#include "base64qp.h"
#include "tool.h"


/*
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
	int nParamName[20];
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
		
}TaskQueue;

#endif
*/

#ifndef _H_maillist
#define _H_maillist


#define D_MailList_HasDeliver					1
#define D_MailList_Fail								2
#define D_MailList_Success						3
#define D_MailList_Delay							4				

#define D_MailList_NotFound						5		//////////one situation of fail
#define D_MailList_Dns								6		//////////one situation of delay
#define D_MailList_Spam								7		//////////one situation of fail


#define D_MailList_Deliver						100




int PrintMailList(char *sMailboxList, int nMailboxListUsed, char *sOffsetList, int nOffsetListUsed);

int ReadMailList(char *sFileName, MailList *pMailList);

int ReadDomainAlias(char *sFileName, DomainAlias *p);

int PrintAlias(DomainAlias *p);

int FreeDomainAlias(DomainAlias *P);

int FreeMailList(MailList *p);

int FindAliasDomain(char *sAlias, DomainAlias *p);

int ReadMailListWithAlias(char *sFileName, MailList *pMailList, DomainAlias *pDomainAlias);

int AnswerMailboxRequest2(char *sRequest, int nRequestNum, DomainAlias *pAlias, MailList *pList, char **sResult, 
														int *nResultSize, int *nResultLeng);

int AnswerMailboxRequest3(char *sRequest, int nRequestNum, DomainAlias *pAlias, MailList *pList, char **sResult, 
														int *nResultSize, int *nResultLeng, int nResultBegin);



int ChangeMailboxStat(DomainAlias *pAlias, MailList *pMailList, char *sMailbox, int nStat);

int ChangeMailboxStat4(TaskQueue *pBegin, TaskQueue *pEnd, char *sID, char *sMailbox, int nStat);

int ReadMailListWithAliasFast(char *sFileName, MailList *pMail, DomainAlias *pAlias);

int DelOneTask(TaskQueue **pBegin, TaskQueue **pEnd, TaskQueue *pTask, int *nNumber);

int AddTaskQueue(TaskQueue **pBegin, TaskQueue **pEnd, DomainAlias *pAlias, MailList *pMailList, char *sModula, 
											int nModulaSize, char *sID, int *nNumber, char *sFileName, int nPaus);

int AnswerMailboxRequest4(char *sRequest, int nRequestNum, TaskQueue *pBegin, TaskQueue *pEnd, char **sResult, 
													int *nResultSize, int *nResultLeng, int nResultBegin, int nTaskNum, int *nTaskOffset,
													pthread_mutex_t *pLockTask);

int AnswerMailboxRequest5(char *sRequest, int nRequestNum, DABUDAI *pDabudai, char **sResult, int *nResultSize, 
															int *nResultLeng, int nResultBegin, CONFIGURE *pConf, char *sLog, int nLogSize, char *sTag);


int ReadTaskFile(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID);

int PrintMailList2(char *sMailboxList, int nMailboxListUsed, char *sOffsetList, int nOffsetListUsed);

int DelOneTask2(TaskQueue **pBegin, TaskQueue **pEnd, TaskQueue *pTask, int *nNumber, int *nFinishSum);

int AddWaterTask(CONFIGURE* pConf, DABUDAI *pDabudai, int *nn);

int AnswerMailboxRequestWithLevel(char *sRequest, int nRequestNum, DABUDAI *pDabudai, char **sResult, int *nResultSize, 
															int *nResultLeng, int nResultBegin, CONFIGURE *pConf, char *sLog, int nLogSize, char *sTag);

int ChangeMailboxStatWithLevel(TaskQueue *pBegin, TaskQueue *pEnd, char *sID, char *sMailbox, int nStat);

int ReadTaskFileWithLevel(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID, 
														CONFIGURE *pConf, DABUDAI *pDabudai, char *sLog, char *sLogTag);

int DelOldTrigerNum(CONFIGURE *pConf, DABUDAI *pDabudai);

int ChangeMailboxStatTriger(CONFIGURE *pConf, DABUDAI *pDabudai, char *sID, char *sMailbox, int nStat, DBC *dbc, char *cStat);

#endif

