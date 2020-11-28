#include "maillist_daemon.h"
#include "socket_tool.h"
#include "maillist7.h"
#include "c_error.h"
#include "commuteother7.h"
#include "writelog.h"
#include "daemon_conf.h"
#include "yxlist.h"

extern int errno;
//
enum {
	MAX_TASKID_LEN = 64,
	MAX_STOPTASK_NUM = 1000,
	MAX_KEEP_VALIDSOCK = 32,
	MAX_TASK_KEEP_TIME = 300,		//保留300
};
struct TaskState
{
	char _sid[MAX_TASKID_LEN];
	int	_state;
};

struct {
	long _stop_time;
	struct TaskState _task_state;
	//int _valid_socks[MAX_KEEP_VALIDSOCK];
}g_stop_tasks[MAX_STOPTASK_NUM];
int g_stoptask_size = 0;
pthread_mutex_t g_delivery_lock = PTHREAD_MUTEX_INITIALIZER;

int InitialDaBuDai(DABUDAI *pDabudai)
{
    memset(pDabudai, 0, sizeof(DABUDAI));

    pDabudai->nMaxNetSocket = 0;
    pDabudai->nMinNetSocket = 1024;

    {
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
        pDabudai->pLockRWTask = (pthread_rwlock_t*)malloc(sizeof(pthread_rwlock_t));
        if(pthread_rwlock_init(pDabudai->pLockRWTask, &attr))
        {	
           ;//printf("initial write-read lock fail1!!!!\n");
            return -1;
        }	
    }

    pDabudai->pLockTask = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockTask, NULL);
    pDabudai->pLockFile = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockFile, NULL);

    pDabudai->nTaskNum = 0;
    pDabudai->nTaskOffset = 0;

    pDabudai->pLockNet = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockNet, NULL);

    pDabudai->pBegin = NULL;
    pDabudai->pEnd = NULL;

    pDabudai->nEventNum = 0;

    pDabudai->nLogNum = 0;
    pDabudai->nLogSize = 0;
    {
        time_t tTime;
        struct tm sctTime;
        time(&tTime);
        localtime_r(&tTime, &sctTime);
        pDabudai->nYear = 1900 + sctTime.tm_year;
        pDabudai->nMonth = 1 + sctTime.tm_mon;
        pDabudai->nDay =  sctTime.tm_mday;	
    }
    pDabudai->pLockLog = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockLog, NULL);

    pDabudai->tReportCacheTime = time(NULL);

    pDabudai->nCacheSocket = -1;
    pDabudai->nCacheSocket2 = -1;
    //int nCacheFlag2;
    //pthread_mutex_t *pLockCache2;
    pDabudai->nFlagCache2 = 0;
    pDabudai->pLockCache2 = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockCache2, NULL);

    pDabudai->pLockWater = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockWater, NULL);

    pDabudai->pLockTriger = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockTriger, NULL);

    pDabudai->pLockTrigerNum = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pDabudai->pLockTrigerNum, NULL);

    return 0;
}

int FreeDaBuDai(DABUDAI *pDabudai)
{
    free(pDabudai->pLockRWTask);
    free(pDabudai->pLockTask);
    free(pDabudai->pLockFile);
    free(pDabudai->pLockNet);
    free(pDabudai->pLockLog);
    fclose(pDabudai->pFileLog);
    free(pDabudai->pLockCache2);
    return 0;
}



static int BeforeCloseSocket(int nSocket, int nEpollFd, int *nIpList, char *sUsedList, DABUDAI *pDa)
{
    struct epoll_event epTmp;

    epTmp.data.fd = nSocket; 
    epoll_ctl(nEpollFd, EPOLL_CTL_DEL, nSocket, &(epTmp));
    (pDa->nEventNum)--;
    nIpList[nSocket] = 0;
    sUsedList[nSocket] = 0;
    if(nSocket == pDa->nMaxNetSocket)
        (pDa->nMaxNetSocket)--;
    if(nSocket == pDa->nMinNetSocket && pDa->nMinNetSocket < pDa->nMaxNetSocket)
        (pDa->nMinNetSocket)++;

    return 0;
}

//cmd == 4
void *Thread_Report(void *sArg)
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];
    int nDaySend = 0;

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);


    pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    pTask = pDabudai->pBegin;
    nLeng = pDabudai->nTaskNum *25 + 18;
    if(nLeng > nSize)
    {
        sMsg = realloc(sMsg, nLeng + 40);
        nSize = nLeng+40;
    }
    k = pDabudai->nTaskNum *25 + 14;
    k = htonl(k);
    memcpy(sMsg, &k, 4);
    k = 4;
    k = htonl(k);
    memcpy(sMsg+4, &k, 4);
    memset(sMsg+8, 0x0, 8);
    {
        short l = pDabudai->nTaskNum;
        l = htons(l);
        memcpy(sMsg+16, &l, 2);	
    }
    while(pTask)
    {
        MailList *pMailList = pTask->pMailList;
        DomainAlias *pAlias = pTask->pAlias;
        int i = 0;
        int nSend = 0, nSuccess = 0, nSum = 0;

        {
            int *nn = (int*)(pTask->sID);
            if(nn[0] == 0)
            {
                pTask = pTask->pNext;
                continue;
            }
        }
        while(i < pAlias->nDomainNum+1 && nOffset < nLeng)
        {
            pthread_mutex_lock((pMailList+i)->pLock);
            nSend += ((pMailList+i)->nSend);
            nSuccess += ((pMailList+i)->nSuccess);
            nSum += ((pMailList+i)->nSum);
            pthread_mutex_unlock((pMailList+i)->pLock);
            ++i;
        }
        nDaySend += nSend;
        {
            int *nn = (int*)(pTask->sID);
            if(pTask->tFinishTime){
                snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d. this task is finish.\n", 
                ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
            //}
            //else{
                //snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d.\n", 
                //ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }

        }
        memcpy(sMsg+nOffset, pTask->sID, 12);
        nOffset += 12;
        nSum = htonl(nSum);
        memcpy(sMsg+nOffset, &nSum, 4);
        nOffset += 4;
        nSuccess = htonl(nSuccess);
        memcpy(sMsg+nOffset, &nSuccess, 4);
        nOffset += 4;
        nSend = htonl(nSend);
        memcpy(sMsg+nOffset, &nSend, 4);
        nOffset += 4;
        if(pTask->tFinishTime)	
            sMsg[nOffset] = 0;
        else
            sMsg[nOffset] = 1;
        nOffset += 1;
        pTask = pTask->pNext;

    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    if(pDabudai->nCacheSocket < 0)
    {
        char s[2];
        pDabudai->nCacheSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
    }
    if(pDabudai->nCacheSocket < 0)
    {
        snprintf(sLog, sizeof(sLog), "[CACHE][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    }
    else
    {
        k = SendPackage(pDabudai->nCacheSocket, sMsg, nLeng,  pConf->nNetTimeOut);	
        if(k)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            close(pDabudai->nCacheSocket);
            pDabudai->nCacheSocket = -1;
        }
        if(k == 0)
        {
            int l = 0;
            k = ReceivePackage(pDabudai->nCacheSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                close(pDabudai->nCacheSocket);
                pDabudai->nCacheSocket = -1;	
            }
        }
    }

    if(pDabudai->nCacheSocket >= 0)
    {
        close(pDabudai->nCacheSocket);
        pDabudai->nCacheSocket = -1;	
    }
    pthread_rwlock_wrlock(pDabudai->pLockRWTask);
    pthread_mutex_lock(pDabudai->pLockWater);
    nDaySend += (pDabudai->nWaterFinishSum);
    if(nDaySend > pConf->nWaterLine + (pConf->nWaterAdd)*(pDabudai->nWaterOffset) 
        && access(pConf->sWaterListFile, F_OK) == 0 && pDabudai->nWaterRound < pConf->nWaterRound)
    {
        int nn[3];
        nn[0] = 0;
        nn[1] = time(NULL) % 100000;
        (pDabudai->nWaterOffset)++;
        nn[2] = pDabudai->nWaterOffset;
        snprintf(sLog, sizeof(sLog), "[WATER]create water task (%d.%d.%d), day send is:%d, offset is:%d, round is:%d, finish sum is:%d.\n", 
            nn[0], nn[1], nn[2], nDaySend, pDabudai->nWaterOffset, pDabudai->nWaterRound,
            pDabudai->nWaterFinishSum);
        
        //if (pConf->enable_water == 1)
        { //enable_water 2013/12/05 D.Wei   
            AddWaterTask(pConf, pDabudai, nn);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
    }

    pthread_mutex_unlock(pDabudai->pLockWater);
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    free(sMsg);
    return (void*)0;	
}


//cmd ==6
void *Thread_Report2(void *sArg)
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);


    pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    pTask = pDabudai->pBegin;
    while(pTask)
    {
        MailList *pMailList = pTask->pMailList;
        DomainAlias *pAlias = pTask->pAlias;
        int i = 0;
        int k = 0;
        int nSend = 0, nSuccess = 0, nSum = 0, nFail = 0, nDelay = 0;
        int nLeng = 24;

        {
            int *nn = (int*)(pTask->sID);
            if(nn[0] == 0)
            {
                pTask = pTask->pNext;
                continue;
            }
        }
        i = htonl(6);
        memcpy(sMsg+4, &i, 4);
        i = pAlias->nDomainNum+1;
        i = htonl(i);
        memcpy(sMsg+8, &i, 4);
        memcpy(sMsg+12, pTask->sID, 12);
        i = 0;
        //printf(":A%d:D%d::\n", pAlias->nAliasNum, pAlias->nDomainNum);
        while(k < pAlias->nDomainNum+1)
        {
            pthread_mutex_lock((pMailList+k)->pLock);
            nSend = htonl((pMailList+k)->nSend);
            nSuccess = htonl((pMailList+k)->nSuccess);
            nSum = htonl((pMailList+k)->nSum);
            nFail = htonl((pMailList+k)->nFail);
            nDelay = htonl((pMailList+k)->nDelay);
            memcpy(sMsg+nLeng, &nSum, 4);
            memcpy(sMsg+nLeng+4, &nSend, 4);
            memcpy(sMsg+nLeng+8, &nSuccess, 4);
            memcpy(sMsg+nLeng+12, &nDelay, 4);
            memcpy(sMsg+nLeng+16, &nFail, 4);
            if(k < pAlias->nDomainNum)
            {
                snprintf(sMsg+nLeng+20, 50, "%s", pAlias->sDomain+34*k);
                nLeng += (strlen(pAlias->sDomain+34*k) + 21);
                if(strcasecmp(pAlias->sDomain+34*k, "qq.com") == 0)
                {
                    //printf("\nBBBB%d:%d:%d************\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                    pthread_mutex_lock(pDabudai->pLockWater);	
                    pDabudai->nQQDaySend += (ntohl(nSend) - pTask->nQQSendBackup);
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    pTask->nQQSendBackup = ntohl(nSend);
                    //printf("\nEEEEE%d:%d:%d***********\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                }
            }
            else
            {
                snprintf(sMsg+nLeng+20, 50, "other");
                nLeng += (strlen("other") + 21);
            }
            pthread_mutex_unlock((pMailList+k)->pLock);
            {
                int *nn = (int*)(pTask->sID);
                if(k < pAlias->nDomainNum)
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][%s]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d.\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pAlias->sDomain+34*k, (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail);
                else
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][other]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d.\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail);
            }
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            ++k;	
        }
        i = htonl(nLeng);
        memcpy(sMsg, &i, 4);

        if(pDabudai->nCacheSocket2 < 0)
        {
            char s[2];
            pDabudai->nCacheSocket2 = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
        }
        if(pDabudai->nCacheSocket2 < 0)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
        else
        {
            k = SendPackage(pDabudai->nCacheSocket2, sMsg, nLeng,  pConf->nNetTimeOut);	
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                close(pDabudai->nCacheSocket2);
                pDabudai->nCacheSocket2 = -1;
            }
            if(k == 0)
            {
                int l = 0;
                k = ReceivePackage(pDabudai->nCacheSocket2, &sMsg, &nSize, &l, pConf->nNetTimeOut);
                if(k)
                {
                    snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    close(pDabudai->nCacheSocket2);
                    pDabudai->nCacheSocket2 = -1;	
                }
            }
        }
        if(pDabudai->nCacheSocket2 >= 0)
        {
            close(pDabudai->nCacheSocket2);
            pDabudai->nCacheSocket2 = -1;	
        }
        pTask = pTask->pNext;
    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);
    /*
    nLeng = pDabudai->nTaskNum *25 + 18;
    if(nLeng > nSize)
    {
    sMsg = realloc(sMsg, nLeng + 40);
    nSize = nLeng+40;
    }
    k = pDabudai->nTaskNum *25 + 14;
    k = htonl(k);
    memcpy(sMsg, &k, 4);
    k = 4;
    k = htonl(k);
    memcpy(sMsg+4, &k, 4);
    memset(sMsg+8, 0x0, 8);
    {
    short l = pDabudai->nTaskNum;
    l = htons(l);
    memcpy(sMsg+16, &l, 2);	
    }
    while(pTask)
    {
    MailList *pMailList = pTask->pMailList;
    DomainAlias *pAlias = pTask->pAlias;
    int i = 0;
    int nSend = 0, nSuccess = 0, nSum = 0;

    while(i < pAlias->nDomainNum+1 && nOffset < nLeng)
    {
    pthread_mutex_lock((pMailList+i)->pLock);
    nSend += ((pMailList+i)->nSend);
    nSuccess += ((pMailList+i)->nSuccess);
    nSum += ((pMailList+i)->nSum);
    pthread_mutex_unlock((pMailList+i)->pLock);
    ++i;
    }
    {
    int *nn = (int*)(pTask->sID);
    if(pTask->tFinishTime)
    snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d. this task is finish.\n", 
    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
    else
    snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d.\n", 
    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));

    }
    memcpy(sMsg+nOffset, pTask->sID, 12);
    nOffset += 12;
    nSum = htonl(nSum);
    memcpy(sMsg+nOffset, &nSum, 4);
    nOffset += 4;
    nSuccess = htonl(nSuccess);
    memcpy(sMsg+nOffset, &nSuccess, 4);
    nOffset += 4;
    nSend = htonl(nSend);
    memcpy(sMsg+nOffset, &nSend, 4);
    nOffset += 4;
    if(pTask->tFinishTime)	
    sMsg[nOffset] = 0;
    else
    sMsg[nOffset] = 1;
    nOffset += 1;
    pTask = pTask->pNext;	
    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    if(pDabudai->nCacheSocket < 0)
    {
    char s[2];
    pDabudai->nCacheSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
    }
    if(pDabudai->nCacheSocket < 0)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    }
    else
    {
    k = SendPackage(pDabudai->nCacheSocket, sMsg, nLeng,  pConf->nNetTimeOut);	
    if(k)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;
    }
    if(k == 0)
    {
    int l = 0;
    k = ReceivePackage(pDabudai->nCacheSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
    if(k)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;	
    }
    }
    }

    if(pDabudai->nCacheSocket >= 0)
    {
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;	
    }
    */

    pthread_mutex_lock(pDabudai->pLockCache2);
    pDabudai->nFlagCache2 = 0;
    pthread_mutex_unlock(pDabudai->pLockCache2);

    free(sMsg);
    return (void*)0;	
}


//cmd = 7
void *Thread_Report3(void *sArg)
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);


    pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    pTask = pDabudai->pBegin;
    while(pTask)
    {
        MailList *pMailList = pTask->pMailList;
        DomainAlias *pAlias = pTask->pAlias;
        int i = 0;
        int k = 0;
        int nSend = 0, nSuccess = 0, nSum = 0, nFail = 0, nDelay = 0, nNotFound = 0;
        int nLeng = 24;

        {
            int *nn = (int*)(pTask->sID);
            if(nn[0] == 0)
            {
                pTask = pTask->pNext;
                continue;
            }
        }
        i = htonl(7);
        memcpy(sMsg+4, &i, 4);
        i = pAlias->nDomainNum+1;
        i = htonl(i);
        memcpy(sMsg+8, &i, 4);
        memcpy(sMsg+12, pTask->sID, 12);
        i = 0;
        //printf(":A%d:D%d::\n", pAlias->nAliasNum, pAlias->nDomainNum);
        while(k < pAlias->nDomainNum+1)
        {
            pthread_mutex_lock((pMailList+k)->pLock);
            nSend = htonl((pMailList+k)->nSend);
            nSuccess = htonl((pMailList+k)->nSuccess);
            nSum = htonl((pMailList+k)->nSum);
            nFail = htonl((pMailList+k)->nFail);
            nDelay = htonl((pMailList+k)->nDelay);
            nNotFound = htonl((pMailList+k)->nNotFound);
            memcpy(sMsg+nLeng, &nSum, 4);
            memcpy(sMsg+nLeng+4, &nSend, 4);
            memcpy(sMsg+nLeng+8, &nSuccess, 4);
            memcpy(sMsg+nLeng+12, &nDelay, 4);
            memcpy(sMsg+nLeng+16, &nFail, 4);
            memcpy(sMsg+nLeng+20, &nNotFound, 4);

            if(k < pAlias->nDomainNum)
            {
                snprintf(sMsg+nLeng+24, 50, "%s", pAlias->sDomain+34*k);
                nLeng += (strlen(pAlias->sDomain+34*k) + 25);
                if(strcasecmp(pAlias->sDomain+34*k, "qq.com") == 0)
                {
                    //printf("\nBBBB%d:%d:%d************\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                    pthread_mutex_lock(pDabudai->pLockWater);	
                    pDabudai->nQQDaySend += (ntohl(nSend) - pTask->nQQSendBackup);
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    pTask->nQQSendBackup = ntohl(nSend);
                    //printf("\nEEEEE%d:%d:%d***********\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                }
            }
            else
            {
                snprintf(sMsg+nLeng+24, 50, "other");
                nLeng += (strlen("other") + 25);
            }
            pthread_mutex_unlock((pMailList+k)->pLock);
            {
                int *nn = (int*)(pTask->sID);
                if(k < pAlias->nDomainNum)
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][%s]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pAlias->sDomain+34*k, (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail, (pMailList+k)->nNotFound);
                else
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][other]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d.\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail, (pMailList+k)->nNotFound);
            }
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            ++k;	
        }
        i = htonl(nLeng);
        memcpy(sMsg, &i, 4);

        if(pDabudai->nCacheSocket2 < 0)
        {
            char s[2];
            pDabudai->nCacheSocket2 = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
        }
        if(pDabudai->nCacheSocket2 < 0)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
        else
        {
            k = SendPackage(pDabudai->nCacheSocket2, sMsg, nLeng,  pConf->nNetTimeOut);	
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                close(pDabudai->nCacheSocket2);
                pDabudai->nCacheSocket2 = -1;
            }
            if(k == 0)
            {
                int l = 0;
                k = ReceivePackage(pDabudai->nCacheSocket2, &sMsg, &nSize, &l, pConf->nNetTimeOut);
                if(k)
                {
                    snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    close(pDabudai->nCacheSocket2);
                    pDabudai->nCacheSocket2 = -1;	
                }
            }
        }
        if(pDabudai->nCacheSocket2 >= 0)
        {
            close(pDabudai->nCacheSocket2);
            pDabudai->nCacheSocket2 = -1;	
        }
        pTask = pTask->pNext;
    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);
    /*
    nLeng = pDabudai->nTaskNum *25 + 18;
    if(nLeng > nSize)
    {
    sMsg = realloc(sMsg, nLeng + 40);
    nSize = nLeng+40;
    }
    k = pDabudai->nTaskNum *25 + 14;
    k = htonl(k);
    memcpy(sMsg, &k, 4);
    k = 4;
    k = htonl(k);
    memcpy(sMsg+4, &k, 4);
    memset(sMsg+8, 0x0, 8);
    {
    short l = pDabudai->nTaskNum;
    l = htons(l);
    memcpy(sMsg+16, &l, 2);	
    }
    while(pTask)
    {
    MailList *pMailList = pTask->pMailList;
    DomainAlias *pAlias = pTask->pAlias;
    int i = 0;
    int nSend = 0, nSuccess = 0, nSum = 0;

    while(i < pAlias->nDomainNum+1 && nOffset < nLeng)
    {
    pthread_mutex_lock((pMailList+i)->pLock);
    nSend += ((pMailList+i)->nSend);
    nSuccess += ((pMailList+i)->nSuccess);
    nSum += ((pMailList+i)->nSum);
    pthread_mutex_unlock((pMailList+i)->pLock);
    ++i;
    }
    {
    int *nn = (int*)(pTask->sID);
    if(pTask->tFinishTime)
    snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d. this task is finish.\n", 
    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
    else
    snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d.\n", 
    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));

    }
    memcpy(sMsg+nOffset, pTask->sID, 12);
    nOffset += 12;
    nSum = htonl(nSum);
    memcpy(sMsg+nOffset, &nSum, 4);
    nOffset += 4;
    nSuccess = htonl(nSuccess);
    memcpy(sMsg+nOffset, &nSuccess, 4);
    nOffset += 4;
    nSend = htonl(nSend);
    memcpy(sMsg+nOffset, &nSend, 4);
    nOffset += 4;
    if(pTask->tFinishTime)	
    sMsg[nOffset] = 0;
    else
    sMsg[nOffset] = 1;
    nOffset += 1;
    pTask = pTask->pNext;	
    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    if(pDabudai->nCacheSocket < 0)
    {
    char s[2];
    pDabudai->nCacheSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
    }
    if(pDabudai->nCacheSocket < 0)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    }
    else
    {
    k = SendPackage(pDabudai->nCacheSocket, sMsg, nLeng,  pConf->nNetTimeOut);	
    if(k)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;
    }
    if(k == 0)
    {
    int l = 0;
    k = ReceivePackage(pDabudai->nCacheSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
    if(k)
    {
    snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;	
    }
    }
    }

    if(pDabudai->nCacheSocket >= 0)
    {
    close(pDabudai->nCacheSocket);
    pDabudai->nCacheSocket = -1;	
    }
    */

    pthread_mutex_lock(pDabudai->pLockCache2);
    pDabudai->nFlagCache2 = 0;
    pthread_mutex_unlock(pDabudai->pLockCache2);

    free(sMsg);
    return (void*)0;	
}

//| len | cmd(8) | domain_num | task_id | vect{sum | send | success | dely | fail| ..}
void *Thread_Report8(void *sArg)//////////命令字8
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];
    int nSocket;

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);


    pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    pTask = pDabudai->pBegin;
    while(pTask)
    {
        MailList *pMailList = pTask->pMailList;
        DomainAlias *pAlias = pTask->pAlias;
        int i = 0;
        int k = 0;
        int nSend = 0, nSuccess = 0, nSum = 0, nFail = 0, nDelay = 0, nNotFound = 0, nFailNotFound = 0, nFailDns = 0, nFailSpam = 0;
        int nLeng = 24;

        {
            int *nn = (int*)(pTask->sID);
            if(nn[0] == 0)
            {
                pTask = pTask->pNext;
                continue;
            }
        }
		
        i = htonl(8);
        memcpy(sMsg+4, &i, 4);
        i = pAlias->nDomainNum+1;
        i = htonl(i);
        memcpy(sMsg+8, &i, 4);
        memcpy(sMsg+12, pTask->sID, 12);
        i = 0;
        //printf(":A%d:D%d::\n", pAlias->nAliasNum, pAlias->nDomainNum);
        while(k < pAlias->nDomainNum+1)
        {
            pthread_mutex_lock((pMailList+k)->pLock);
            nSend = htonl((pMailList+k)->nSend);
            nSuccess = htonl((pMailList+k)->nSuccess);
            nSum = htonl((pMailList+k)->nSum);
            nFail = htonl((pMailList+k)->nFail);
            nDelay = htonl((pMailList+k)->nDelay);
            nNotFound = htonl((pMailList+k)->nNotFound);
            nFailNotFound = htonl((pMailList+k)->nFailNotFound);
            nFailDns = htonl((pMailList+k)->nFailDns);
            nFailSpam = htonl((pMailList+k)->nFailSpam);
            memcpy(sMsg+nLeng, &nSum, 4);
            memcpy(sMsg+nLeng+4, &nSend, 4);
            memcpy(sMsg+nLeng+8, &nSuccess, 4);
            memcpy(sMsg+nLeng+12, &nDelay, 4);
            memcpy(sMsg+nLeng+16, &nFail, 4);
            memcpy(sMsg+nLeng+20, &nNotFound, 4);
            memcpy(sMsg+nLeng+24, &nFailNotFound, 4);
            memcpy(sMsg+nLeng+28, &nFailDns, 4);
            memcpy(sMsg+nLeng+32, &nFailSpam, 4);
            if(k < pAlias->nDomainNum)
            {
                snprintf(sMsg+nLeng+36, 50, "%s", pAlias->sDomain+34*k);
                nLeng += (strlen(pAlias->sDomain+34*k) + 37);
                if(strcasecmp(pAlias->sDomain+34*k, "qq.com") == 0)
                {
                    //printf("\nBBBB%d:%d:%d************\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                    pthread_mutex_lock(pDabudai->pLockWater);	
                    pDabudai->nQQDaySend += (ntohl(nSend) - pTask->nQQSendBackup);
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    pTask->nQQSendBackup = ntohl(nSend);
                    //printf("\nEEEEE%d:%d:%d***********\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
                }
            }
            else
            {
                snprintf(sMsg+nLeng+36, 50, "other");
                nLeng += (strlen("other") + 37);
            }
            pthread_mutex_unlock((pMailList+k)->pLock);
            {
                int *nn = (int*)(pTask->sID);
                if(k < pAlias->nDomainNum)
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][%s]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d, failnotfound:%d, faildns:%d, failspam:%d.\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pAlias->sDomain+34*k, (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail, 
                    (pMailList+k)->nNotFound, (pMailList+k)->nFailNotFound, (pMailList+k)->nFailDns, (pMailList+k)->nFailSpam);
                else
                    snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][other]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d, failnotfound:%d, faildns:%d, failspam:%d.\n", 
                    ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), (pMailList+k)->nSum,
                    (pMailList+k)->nSend, (pMailList+k)->nSuccess, (pMailList+k)->nDelay, (pMailList+k)->nFail, 
                    (pMailList+k)->nNotFound, (pMailList+k)->nFailNotFound, (pMailList+k)->nFailDns, (pMailList+k)->nFailSpam);
            }
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            ++k;	
        }
        i = htonl(nLeng - 4);
        memcpy(sMsg, &i, 4);


        {
            char s[2];
            nSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
        }
        if(nSocket < 0)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
        else
        {
            k = SendPackage(nSocket, sMsg, nLeng,  pConf->nNetTimeOut);	
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                close(nSocket);
                nSocket = -1;
            }
            if(k == 0)
            {
                int l = 0;
                k = ReceivePackage(nSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
                if(k)
                {
                    snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    close(nSocket);
                    nSocket = -1;	
                }
            }
        }
        if(nSocket >= 0)
        {
            close(nSocket);
            nSocket = -1;	
        }
        pTask = pTask->pNext;
    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    /*
    pthread_mutex_lock(pDabudai->pLockCache2);
    pDabudai->nFlagCache2 = 0;
    pthread_mutex_unlock(pDabudai->pLockCache2);
    */
    free(sMsg);
    return (void*)0;	
}

//Triger从db中获取
//| len | cmd(8) | domain_num | task_id | vect{sum | send | success | dely | fail| ..}
void *Thread_ReportTriger2(void *sArg)//////////命令字8
{
	DABUDAI *pDabudai;
	CONFIGURE *pConf;
	char *sMsg = (char*)malloc(10240);
	int nSize = 10240;
	int nOffset = 18;
	TaskQueue *pTask;
	int k = 0;
	int nLeng;
	char sLog[1024];
	DBT dbtKey, dbtData;
	DBC *dbc;
	int nSocket = -1;


	pthread_detach(pthread_self());

	memcpy(&pConf, sArg, sizeof(CONFIGURE*));
	memcpy(&pDabudai, sArg + sizeof(CONFIGURE*), sizeof(DABUDAI*));
	free(sArg);

	pDabudai->dbNumber->cursor(pDabudai->dbNumber, NULL, &dbc, 0);
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));

	pthread_mutex_lock(pDabudai->pLockTrigerNum);
	while (dbc->c_get(dbc, &dbtKey, &dbtData, DB_PREV) == 0)
	{
		//MailList *pMailList = pTask->pMailList;
		DomainAlias *pAlias = pDabudai->pDomainAlias;
		int i = 0;
		int k = 0;
		int nSend = 0, nSuccess = 0, nSum = 0, nFail = 0, nDelay = 0, nNotFound = 0, nFailNotFound = 0, nFailDns = 0, nFailSpam = 0;
		int nLeng = 24;
		int *nData = (int*)(dbtData.data);
		int nID2[3];
		memcpy(nID2, dbtKey.data, 12);
		nID2[2] = htonl(1);
		pthread_mutex_unlock(pDabudai->pLockTrigerNum);

		i = htonl(8);
		memcpy(sMsg + 4, &i, 4);
		i = pAlias->nDomainNum + 1;
		i = htonl(i);
		memcpy(sMsg + 8, &i, 4);
		memcpy(sMsg + 12, nID2, 12);
		//memcpy(sMsg+12, dbtKey.data, 12);
		i = 0;
		//printf(":A%d:D%d::\n", pAlias->nAliasNum, pAlias->nDomainNum);
		while (k < pAlias->nDomainNum + 1)
		{
			//pthread_mutex_lock((pMailList+k)->pLock);
			nSend = htonl(nData[3 + 4 * k]);
			nSuccess = htonl(nData[4 * k]);
			nSum = htonl(nData[3 + 4 * k]);
			nFail = htonl(nData[2 + 4 * k]);
			nDelay = htonl(nData[1 + 4 * k]);
			nNotFound = htonl(0);
			nFailNotFound = htonl(0);
			nFailDns = htonl(0);
			nFailSpam = htonl(0);
			memcpy(sMsg + nLeng, &nSum, 4);
			memcpy(sMsg + nLeng + 4, &nSend, 4);
			memcpy(sMsg + nLeng + 8, &nSuccess, 4);
			memcpy(sMsg + nLeng + 12, &nDelay, 4);
			memcpy(sMsg + nLeng + 16, &nFail, 4);
			memcpy(sMsg + nLeng + 20, &nNotFound, 4);
			memcpy(sMsg + nLeng + 24, &nFailNotFound, 4);
			memcpy(sMsg + nLeng + 28, &nFailDns, 4);
			memcpy(sMsg + nLeng + 32, &nFailSpam, 4);
			if (k < pAlias->nDomainNum)
			{
				snprintf(sMsg + nLeng + 36, 50, "%s", pAlias->sDomain + 34 * k);
				nLeng += (strlen(pAlias->sDomain + 34 * k) + 37);
				/*
				if(strcasecmp(pAlias->sDomain+34*k, "qq.com") == 0)
				{
				//printf("\nBBBB%d:%d:%d************\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
				pthread_mutex_lock(pDabudai->pLockWater);
				pDabudai->nQQDaySend += (ntohl(nSend) - pTask->nQQSendBackup);
				pthread_mutex_unlock(pDabudai->pLockWater);
				pTask->nQQSendBackup = ntohl(nSend);
				//printf("\nEEEEE%d:%d:%d***********\n", pDabudai->nQQDaySend, pTask->nQQSendBackup, ntohl(nSend));
				}*/
			}
			else
			{
				snprintf(sMsg + nLeng + 36, 50, "other");
				nLeng += (strlen("other") + 37);
			}
			//pthread_mutex_unlock((pMailList+k)->pLock);
			{
				int *nn = (int*)(dbtKey.data);
				if (k < pAlias->nDomainNum)
					snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][%s]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d, failnotfound:%d, faildns:%d, failspam:%d.\n",
						ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pAlias->sDomain + 34 * k, nData[4 * k + 3],
						nData[4 * k + 3], nData[4 * k], nData[4 * k + 1], nData[4 * k + 2],
						0, 0, 0, 0);
				else
					snprintf(sLog, sizeof(sLog), "[CACHE2][%d.%d.%d][other]sum is:%d, send is:%d, success is:%d, delay is:%d, fail is:%d, not found:%d, failnotfound:%d, faildns:%d, failspam:%d.\n",
						ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nData[4 * k + 3],
						nData[4 * k + 3], nData[4 * k], nData[4 * k + 1], nData[4 * k + 2],
						0, 0, 0, 0);
			}
			//WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			++k;
		}
		i = htonl(nLeng);
		memcpy(sMsg, &i, 4);

		//if(pDabudai->nCacheSocket2 < 0)
		{
			char s[2];
			nSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);
		}
		if (nSocket < 0)
		{
			snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		}
		else
		{
			k = SendPackage(nSocket, sMsg, nLeng, pConf->nNetTimeOut);
			if (k)
			{
				snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				nSocket = -1;
			}
			if (k == 0)
			{
				int l = 0;
				k = ReceivePackage(nSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
				if (k)
				{
					snprintf(sLog, sizeof(sLog), "[CACHE2][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
					WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					close(nSocket);
					nSocket = -1;
				}
			}
		}
		if (nSocket >= 0)
		{
			close(nSocket);
			nSocket = -1;
		}
		pthread_mutex_lock(pDabudai->pLockTrigerNum);
	}
	pthread_mutex_unlock(pDabudai->pLockTrigerNum);
	dbc->c_close(dbc);

	/*
	pthread_mutex_lock(pDabudai->pLockCache2);
	pDabudai->nFlagCache2 = 0;
	pthread_mutex_unlock(pDabudai->pLockCache2);
	*/
	free(sMsg);
	return (void*)0;
}

//|len | cmd(9) | 填充 8 bytes | task_num(2) | task_info|
void *Thread_Report9(void *sArg)////////命令字9
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];
    int nDaySend = 0;
    int nSocket = -1;
    int nNum = 0;

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);


    pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    pTask = pDabudai->pBegin;
    nLeng = pDabudai->nTaskNum * (25+24) + 18;
    if(nLeng > nSize)
    {
        sMsg = realloc(sMsg, nLeng + 40);
        nSize = nLeng+40;
    }
	
    // Comment by weide@richinfo.cn at 2013/04/15
    k = pDabudai->nTaskNum * (25+24) + 14;
    k = htonl(k);
    memcpy(sMsg, &k, 4);
    k = 9;
    k = htonl(k);
    memcpy(sMsg+4, &k, 4); //命令字 4 bytes
    memset(sMsg+8, 0x0, 8); // 填充 8 bytes 
    {
        short l = pDabudai->nTaskNum;
        l = htons(l);
        memcpy(sMsg+16, &l, 2);	//任务数 2 bytes
    }

    //遍历任务
    while(pTask)
    {
        MailList *pMailList = pTask->pMailList;
        DomainAlias *pAlias = pTask->pAlias;
        int i = 0;

        int nSend = 0, nSuccess = 0, nSum = 0, nFail = 0, nDelay = 0, nNotFound = 0, nFailNotFound = 0, nFailDns = 0, 
            nFailSpam = 0;

        {
            int *nn = (int*)(pTask->sID);
            if(nn[0] == 0) //过滤无效的任务ID
            {
                pTask = pTask->pNext;
                continue;
            }
        }
        nNum++;

        //统计各个域投递情况
        while(i < pAlias->nDomainNum+1 && nOffset < nLeng)
        {
            pthread_mutex_lock((pMailList+i)->pLock);
            nSend += ((pMailList+i)->nSend);
            nSuccess += ((pMailList+i)->nSuccess);
            nSum += ((pMailList+i)->nSum);
            nFail += ((pMailList+i)->nFail);
            nDelay += ((pMailList+i)->nDelay);
            nNotFound += ((pMailList+i)->nNotFound);
            nFailNotFound += ((pMailList+i)->nFailNotFound);
            nFailDns += ((pMailList+i)->nFailDns);
            nFailSpam += ((pMailList+i)->nFailSpam);
            pthread_mutex_unlock((pMailList+i)->pLock);
            ++i;
        }
        nDaySend += nSend;
        {
            int *nn = (int*)(pTask->sID);
            if(pTask->tFinishTime){
                snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d, delay:%d, fail:%d, notfound:%d, failnotfound:%d, faildns:%d, failspam:%d. this task is finish.\n", 
                ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess, nDelay, nFail, nNotFound, nFailNotFound, nFailDns, nFailSpam);
            //else
                //snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d, delay:%d, fail:%d, notfound:%d, failnotfound:%d, faildns:%d, failspam:%d.\n", 
                //ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), nSum, nSend, nSuccess, nDelay, nFail, nNotFound, nFailNotFound, nFailDns, nFailSpam);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }

        }

        memcpy(sMsg+nOffset, pTask->sID, 12); //任务ID 12 bytes
        nOffset += 12;
        nSum = htonl(nSum);
        memcpy(sMsg+nOffset, &nSum, 4); //
        nOffset += 4;
        nSuccess = htonl(nSuccess);
        memcpy(sMsg+nOffset, &nSuccess, 4);
        nOffset += 4;
        nSend = htonl(nSend);
        memcpy(sMsg+nOffset, &nSend, 4);
        nOffset += 4;
        nFail = htonl(nFail);
        memcpy(sMsg+nOffset, &nFail, 4);
        nOffset += 4;
        nDelay = htonl(nDelay);
        memcpy(sMsg+nOffset, &nDelay, 4);
        nOffset += 4;
        nNotFound = htonl(nNotFound);
        memcpy(sMsg+nOffset, &nNotFound, 4);
        nOffset += 4;
        nFailNotFound = htonl(nFailNotFound);
        memcpy(sMsg+nOffset, &nFailNotFound, 4);
        nOffset += 4;
        nFailDns = htonl(nFailDns);
        memcpy(sMsg+nOffset, &nFailDns, 4);
        nOffset += 4;
        nFailSpam = htonl(nFailSpam);
        memcpy(sMsg+nOffset, &nFailSpam, 4);
        nOffset += 4;
        if(pTask->tFinishTime)	
            sMsg[nOffset] = 0;
        else
            sMsg[nOffset] = 1;
        nOffset += 1;
        pTask = pTask->pNext;

    }
    pthread_rwlock_unlock(pDabudai->pLockRWTask);
    {
        short l = nNum;
        l = htons(l);
        memcpy(sMsg+16, &l, 2);	
    }
    //if(pDabudai->nCacheSocket < 0)

    snprintf(sLog, sizeof(sLog), "[CACHE]send %d task report to cachedb.\n", nNum);
    //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    {
        char s[2];
        nSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, s, 2);	
    }
    if(nSocket < 0)
    {
        snprintf(sLog, sizeof(sLog), "[CACHE][WARN]can not connect cache(%s:%d).\n", pConf->sCacheIp, pConf->nCachePort);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
    }
    else
    {
        k = SendPackage(nSocket, sMsg, nLeng,  pConf->nNetTimeOut);	
        if(k)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            close(nSocket);
            nSocket = -1;
        }
        if(k == 0)
        {
            int l = 0;
            k = ReceivePackage(nSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                close(nSocket);
                nSocket = -1;	
            }
        }
    }

    if(nSocket >= 0)
    {
        close(nSocket);
        nSocket = -1;	
    }

    pthread_rwlock_wrlock(pDabudai->pLockRWTask);
    pthread_mutex_lock(pDabudai->pLockWater);
    nDaySend += (pDabudai->nWaterFinishSum);
    if(nDaySend > pConf->nWaterLine + (pConf->nWaterAdd)*(pDabudai->nWaterOffset) 
        && access(pConf->sWaterListFile, F_OK) == 0 && pDabudai->nWaterRound < pConf->nWaterRound)
    {
        int nn[3];
        nn[0] = 0;
        nn[1] = time(NULL) % 100000;
        (pDabudai->nWaterOffset)++;
        nn[2] = pDabudai->nWaterOffset;
        snprintf(sLog, sizeof(sLog), "[WATER]create water task (%d.%d.%d), day send is:%d, offset is:%d, round is:%d, finish sum is:%d.\n", 
            nn[0], nn[1], nn[2], nDaySend, pDabudai->nWaterOffset, pDabudai->nWaterRound,
            pDabudai->nWaterFinishSum);

        //if (pConf->enable_water == 1)
        {
        	//enable_water 2013/12/05 D.Wei
        	
        	AddWaterTask(pConf, pDabudai, nn);
        	WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
    }

    pthread_mutex_unlock(pDabudai->pLockWater);
    pthread_rwlock_unlock(pDabudai->pLockRWTask);

    free(sMsg);
    return (void*)0;	
}

//Triger从db中获取
//|len | cmd(9) | 填充 8 bytes | task_num(2) | task_info|
void *Thread_ReportTriger(void *sArg)////////命令字9
{
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char *sMsg = (char*)malloc(10240);
    int nSize = 10240;
    int nOffset = 18;
    TaskQueue *pTask;
    int k = 0;
    int nLeng;
    char sLog[1024];
    int nDaySend = 0;
    int nNumber = 0;
    int nSocket;
    DBT dbtKey, dbtData;
    DBC *dbc;
    int nDay;

    {
        time_t tTime;
        struct tm sctTime;
        tTime = time(NULL);
        localtime_r(&tTime, &sctTime);
        nDay = sctTime.tm_mday;
    }

    pthread_detach(pthread_self());

    memcpy(&pConf, sArg, sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+sizeof(CONFIGURE*), sizeof(DABUDAI*));
    free(sArg);

    pthread_mutex_lock(pDabudai->pLockTrigerNum);	
    pDabudai->dbNumber->sync(pDabudai->dbNumber, 0);
    pthread_mutex_unlock(pDabudai->pLockTrigerNum);	

    pDabudai->dbNumber->cursor(pDabudai->dbNumber, NULL, &dbc, 0);
    memset(&dbtKey, 0x0, sizeof(DBT));
    memset(&dbtData, 0x0, sizeof(DBT));


    //pthread_rwlock_rdlock(pDabudai->pLockRWTask);
    //pTask = pDabudai->pBegin;
    //nLeng = pDabudai->nTaskNum * (25+24) + 18;
    //nLeng = 18;

    pthread_mutex_lock(pDabudai->pLockTrigerNum);	
    {
        while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_PREV) == 0)
        {
            int nSuccess = 0, nDelay = 0, nSend = 0, nFail = 0;
            int *nn  = (int*)(dbtData.data);
            int *nID = (int*)(dbtKey.data);
            int i = 0;
            int nID2[3];
            memcpy(nID2, nID, 12);
            nID2[2] = htonl(1);
            //nLeng += (25+24);
            nNumber++;
            if(nOffset + 25 + 24 > nSize)
            {
                sMsg = realloc(sMsg, nSize + 400);
                nSize = nSize+400;
            }
            memcpy(sMsg+nOffset, nID2, 12);
            //memcpy(sMsg+nOffset, dbtKey.data, 12);
            nOffset += 12;
            while(i < pDabudai->pDomainAlias->nDomainNum+1)
            {
                nSend += (nn[3+4*i]);
                nSuccess += (nn[0+4*i]);
                nDelay += (nn[1+4*i]);
                nFail += (nn[2+4*i]);
                i++;
            }
            nSend = htonl(nSend);
            nSuccess = htonl(nSuccess);
            nDelay = htonl(nDelay);
            nFail = htonl(nFail);
            memcpy(sMsg+nOffset, &nSend, 4);
            nOffset += 4;
            memcpy(sMsg+nOffset, &nSuccess, 4);
            nOffset += 4;
            memcpy(sMsg+nOffset, &nSend, 4);
            nOffset += 4;
            memcpy(sMsg+nOffset, &nDelay, 4);
            nOffset += 4;
            memcpy(sMsg+nOffset, &nFail, 4);
            nOffset += 4;
            memset(sMsg+nOffset, 0, 17);
            if(nDay == (ntohl(nID[1]))%100)
            {
                *(sMsg+nOffset+16) = 1;
                nOffset += 17;
                snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d, delay:%d, fail:%d, notfound:0, failnotfound:0, faildns:0, failspam:0.\n", 
                    ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), ntohl(nSend), ntohl(nSend), ntohl(nSuccess), ntohl(nDelay), ntohl(nFail));
            }
            else
            {	
                *(sMsg+nOffset+16) = 0;
                nOffset += 17;
                snprintf(sLog, sizeof(sLog), "[CACHE][%d.%d.%d]task sum is:%d, send:%d, success:%d, delay:%d, fail:%d, notfound:0, failnotfound:0, faildns:0, failspam:0. task is finish.\n", 
                    ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), ntohl(nSend), ntohl(nSend), ntohl(nSuccess), ntohl(nDelay), ntohl(nFail));
            }
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            //nOffset = nOffset + 25 + 24;
        }
    }
    pthread_mutex_unlock(pDabudai->pLockTrigerNum);	
    dbc->c_close(dbc);

    k = nOffset - 4;
    k = htonl(k);
    memcpy(sMsg, &k , 4);
    k = htonl(9);
    memcpy(sMsg+4, &k ,4);
    memset(sMsg+8, 0x0, 8);
    {
        short l = nNumber;
        l = htons(l);
        memcpy(sMsg+16, &l, 2);	
    }

    nSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, pConf->nNetTimeOut, NULL, 0);
    if(nSocket >= 0)
    {
        k = SendPackage(nSocket, sMsg, nOffset,  pConf->nNetTimeOut);	
        if(k)
        {
            snprintf(sLog, sizeof(sLog), "[CACHE][WARN]write error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
        if(k == 0)
        {
            int l = 0;
            k = ReceivePackage(nSocket, &sMsg, &nSize, &l, pConf->nNetTimeOut);
            if(k)
            {
                snprintf(sLog, sizeof(sLog), "[CACHE][WARN]read error:%d(%s:%d).\n", k, pConf->sCacheIp, pConf->nCachePort);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }
        }
        close(nSocket);	
    }
    free(sMsg);
    return (void*)0;	


}

/*
rep:
|task_num(4)| stop_taskid'\0' | stop_taskid'\0' |
rsp:
|task_num(4)| stop_taskid'\0' | stop_taskid'\0' |
*/

int make_stop_delivery_notify(char* notify_msg, int buf_len)
{
	int nindex = 0, ntask_len = 0, ntask_size = 0, notify_size = 0;
	if (g_stoptask_size > 0) {
		long now_time = time(NULL);
		pthread_mutex_lock(&g_delivery_lock);
		char* taskids = notify_msg + sizeof(int);
		for (nindex = 0; nindex < g_stoptask_size; ++nindex) {
			if (D_StopPackage == g_stop_tasks[nindex]._task_state._state 
			&&  now_time - MAX_TASK_KEEP_TIME < g_stop_tasks[nindex]._stop_time) {
				ntask_len = strlen(g_stop_tasks[nindex]._task_state._sid);
				if ((taskids + ntask_len) >= (notify_msg + buf_len)) {
					break;
				}
				strcpy(taskids, g_stop_tasks[nindex]._task_state._sid);
				taskids += (ntask_len + 1);
				++ntask_size;
				g_stop_tasks[nindex]._task_state._state = -1;   //标记已处理
			}
		}//for 
		if (ntask_size > 0) {
			ntask_size = htonl(ntask_size);
			memcpy(notify_msg, &ntask_size, sizeof(int));
			notify_size = (taskids - notify_msg);
		}
		pthread_mutex_unlock(&g_delivery_lock);
	}

	return notify_size;
}

void *Thread_Session(void *sArg)
{
    int nSocket;
    int nEpollFd;
    struct epoll_event epTmp;
    char *sMsg;
    int nMsgSize;
    int k = 0;
    unsigned int *nIpList;
    char sTag[30];
    char *sUsedList;
    int nFail = 0;
    int nCommand;
    int nMsgLeng = 0;
    DABUDAI *pDabudai;
    CONFIGURE *pConf;
    char sLog[2048];
    int nSessionNum;

    //printf("thread up!!\n");

    pthread_detach(pthread_self());
    memcpy(&nEpollFd, sArg, 4);
    memcpy(&nSocket, sArg+4, 4);
    memcpy(&nIpList, sArg+8, sizeof(int*));
    memcpy(&sUsedList, sArg+8+sizeof(int*), sizeof(char*));
    memcpy(&pConf, sArg+8+2*sizeof(int*), sizeof(CONFIGURE*));
    memcpy(&pDabudai, sArg+8+3*sizeof(int*), sizeof(DABUDAI*));
    memcpy(&nSessionNum, sArg+8+4*sizeof(int*), sizeof(int));
    free(sArg);

    {
        unsigned char *s = (unsigned char *)(nIpList+nSocket);
        snprintf(sTag, sizeof(sTag), "%d.%d.%d.%d.%05d", s[0], s[1], s[2], s[3], nSessionNum);	
    }
	enum {MAX_MSG_LEN = 4000};
    sMsg = (char*)malloc(MAX_MSG_LEN);
    nMsgSize = MAX_MSG_LEN;

    k = ReceivePackage(nSocket, &sMsg, &nMsgSize, &nMsgLeng, pConf->nNetTimeOut);

    //free(sMsg);
    //sMsg = (char*)malloc(nMsgSize);
    //printf("%d\n", __LINE__);
    //fflush(NULL);


    if(k == D_JavaPackage)
    {
        /**********************
        ADD TREAT JAVA MESSAGE!
        **********************/
        DomainAlias *pAlias = NULL;
        MailList *pMailList = NULL;
        char sID[12];
        char *sModula = NULL;
        char sFileName[200];

        snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]this connection is for sending new task file.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));

        k = ReadTaskFromJava(nSocket, sFileName, 200, pDabudai->pLockFile, pConf->nNetTimeOut, pConf->sTaskFileDir, 
            pConf->sTaskBakDir);
		//k >0 异常
        if(k)  
        {
            if(k == D_TransHasFinish)
            {
                snprintf(sMsg, 5, "HH\n");	
                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]task <%s> has been accepted before, will not accept again.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }	
            else if(k == D_TransNotFinish)
            {
                snprintf(sMsg, 5, "DD\n");
                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]task <%s> has been accepted before, will not accept again.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
            }
            else if(k == D_OpenFileError)
            {
                snprintf(sMsg, 5, "OO\n");
                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK][WARN]can't open file for task <%s>.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
            }
            else if(k == D_PackLengError)
            {
                snprintf(sMsg, 5, "LL\n");
                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]package length is wrong.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));			
            }
            else if(k == D_NetReadMsgFail)
            {
                snprintf(sMsg, 5, "NN\n");
                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]net error while reading task <%s>.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
            }
            SocketWrite(nSocket, 5, sMsg, 3);
        }
        else
        {
            {
                DomainAlias *pAlias;
                MailList *pMailList;
                char sID[12];
                char *sModula = NULL;
                pAlias = (DomainAlias*)malloc(sizeof(DomainAlias));	
                memset(pAlias, 0x0, sizeof(DomainAlias));
                ReadDomainAlias(pConf->sAliasFile, pAlias);
                pMailList = (MailList*)malloc(sizeof(MailList) * (pAlias->nDomainNum+1));
                memset(pMailList, 0x0, sizeof(MailList)*(pAlias->nDomainNum+1));	

                snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]success receive file for task <%s>.\n", sTag, sFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                {
                    int i = 0;
                    MailList *p = pMailList;
                    while(i < pAlias->nDomainNum + 1)
                    {
                        p->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
                        pthread_mutex_init(p->pLock, NULL);
                        ++i;
                        ++p;	
                    }	
                }
				//------------------------------------------------
				//从任务文件获取邮件列表
				//------------------------------------------------
                ReadTaskFileWithLevel(sFileName, pMailList, pAlias, &sModula, sID, pConf, pDabudai, sLog, sTag);
                /*
                {
                int i = 0;
                while(i < 12)
                {
               ;//printf("0X%02x, ", *(sID+i));
                ++i;
                }
               ;//printf("\n");
                }
                */
                //printf("MMMMMMM[%s]MMMMMM\n", sModula);
                /*{
                int i = 0;
                DomainAlias *p = pAlias;
                MailList *p2 = pMailList;
                while(i < p->nDomainNum+1)
                {
                MailList *pp = p2+i;
                if(i < p->nDomainNum)
               ;//printf("<<%s>>:\n", p->sDomain + 34*i);
                else
               ;//printf("<<other>>:\n");
                PrintMailList(pp->sMailboxList, pp->nMailboxListUsed, pp->sOffsetList, pp->nOffsetListUsed);
               ;//printf("\n\n");
                ++i;
                }
                }*/
                if(sModula)
                {
                    char *s = strstr(sModula, "$MAIL_FROM$");

                    pthread_mutex_lock(pDabudai->pLockWater);
                    if(s)
                    {
                        //snprintf(pDabudai->sDayWaterFrom + 50*(pDabudai->nDayWaterFrom%4), 50, "%s", s+11);
                        snprintf(pDabudai->sDayWaterFrom , sizeof(pDabudai->sDayWaterFrom)-1, "%s", s+11);
                        //s = strchr(pDabudai->sDayWaterFrom + 50*(pDabudai->nDayWaterFrom%4), '\r');
                        s = strchr(pDabudai->sDayWaterFrom, '\r');
                        if(!s)
                            //s = strchr(pDabudai->sDayWaterFrom + 50*(pDabudai->nDayWaterFrom%4), '\n');
                            s = strchr(pDabudai->sDayWaterFrom , '\n');
                        if(s)
                            *s = 0;
                        pDabudai->nDayWaterFrom = (pDabudai->nDayWaterFrom+1)%4;
                    }
                    pthread_mutex_unlock(pDabudai->pLockWater);
                }
                if(pAlias && pMailList && sModula)
                {
                    int kk = 0;
                    pthread_mutex_lock(pDabudai->pLockWater);
                    kk = pDabudai->nDayWaterMode;
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    pthread_rwlock_wrlock(pDabudai->pLockRWTask);
					//------------------------------------------------
					//-----------------存库单个邮件任务----源头----------
					//------------------------------------------------
                    AddTaskQueue(&(pDabudai->pBegin), &(pDabudai->pEnd), pAlias, pMailList, sModula, strlen(sModula)+1, 
                        sID, &(pDabudai->nTaskNum), sFileName, kk);
                    if(kk == 0)
                        snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]add task <%s> into queue successfully.\n", sTag, sFileName);
                    else
                        snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]add task <%s> into queue successfully. in day water mode, will delay.\n", sTag, sFileName);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 5, "OK\n");
                    pthread_rwlock_unlock(pDabudai->pLockRWTask);
                }
                else
                {
                    int i = 0;
                    while(i < pAlias->nDomainNum+1)
                    {
                        FreeMailList(pMailList+i);
                        ++i;	
                    }
                    free(sModula);
                    free(pAlias);
                    free(pMailList);
                    unlink(sFileName);
                    snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]the format of task file <%s> is wrong.\n", sTag, sFileName);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 5, "BA\n");	
                }
            }


            SocketWrite(nSocket, 5, sMsg, 3);
            //ReadTaskDetail(sFileName, sAliasFile, sID, &sModula, &pAlias, &pMailList);	
        }
        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;	
    }
    else if(k == D_StopPackage)
    {
        int nn = 0;
        TaskQueue *pTaskUsed;

        snprintf(sLog, sizeof(sLog), "[%s][STOPTASK]this connection is for stop task.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        nn = SocketReadLine(nSocket, pConf->nNetTimeOut, sMsg, nMsgSize);
        if(nn > 0 && strchr(sMsg, '\n'))
        {
            int nID[3];
			char sID[50] = {0};
            sscanf(sMsg, " %d.%d.%d", nID, nID+1, nID+2);
            snprintf(sID, 50, "%d.%d.%d", nID[0], nID[1], nID[2]);
            nID[0] = htonl(nID[0]);
            nID[1] = htonl(nID[1]);
            nID[2] = htonl(nID[2]);
            //pthread_rwlock_wrlock(pDabudai->pLockRWTask);
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            pTaskUsed = pDabudai->pBegin;
            while(pTaskUsed)
            {
                if(memcmp(pTaskUsed->sID, nID, 12) == 0)
                    break;
                pTaskUsed = pTaskUsed->pNext;	
            }
            if(pTaskUsed)
            {
                pthread_mutex_lock(pTaskUsed->pLock);
                if(pTaskUsed->tFinishTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][STOPTASK]task [%s] has stop.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    snprintf(sMsg, 100, "500 task [%s] has stop.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else
                {
                    int ii = 0;
                    pTaskUsed->tFinishTime = time(NULL);
                    while(ii < (pTaskUsed->pAlias->nDomainNum) + 1)
                    {
                        ((pTaskUsed->pMailList)+ii)->nRound = 1000;
                        ++ii;
                    }
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    {
                        char ssss[200];
                        char *sTmp = strrchr(pTaskUsed->sFileName, '/');
                        int k = 0;
                        ssss[0] = 0;
                        if(sTmp && access(pTaskUsed->sFileName, F_OK) == 0)
                        {
                            snprintf(ssss, 200, "%s/%s", pConf->sTaskBakDir, sTmp+1);
                            pthread_mutex_lock(pDabudai->pLockFile);
                            k = rename(pTaskUsed->sFileName, ssss);	
                            pthread_mutex_unlock(pDabudai->pLockFile);
                        }
                        else
                            k = 1;
                        if(k)
                        {
                            snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]move task file %s to file %s fail.\n", pTaskUsed->sFileName, ssss);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        }
                        else
                        {
                            snprintf(sLog, sizeof(sLog), "[SYSTEM]move task file %s to file %s.\n", pTaskUsed->sFileName, ssss);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                        }	
                    }
                    snprintf(sMsg, 100, "200 stop task [%s] successfully.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
					//----------------接收暂停处理成功----------------------
					long now_time = time(NULL);
					pthread_mutex_lock(&g_delivery_lock);
					int nindex = 0;
					if (g_stoptask_size < MAX_STOPTASK_NUM - 1) {
						nindex = g_stoptask_size;
						++g_stoptask_size;
					}
					else {
						int noldest = 0;
						for (nindex = 0; nindex < MAX_STOPTASK_NUM; ++nindex) {
							if (now_time - g_stop_tasks[nindex]._stop_time > MAX_TASK_KEEP_TIME) {
								break;
							}
							if (noldest != nindex && g_stop_tasks[nindex]._stop_time < g_stop_tasks[noldest]._stop_time) {
								noldest = nindex;
							}
						}
						if (nindex == MAX_STOPTASK_NUM) {
							nindex = noldest;
						}
					}
					g_stop_tasks[nindex]._task_state._state = D_StopPackage;
					strncpy(g_stop_tasks[nindex]._task_state._sid, sID,  MAX_TASKID_LEN - 1);
					g_stop_tasks[nindex]._stop_time = now_time;
					
					pthread_mutex_unlock(&g_delivery_lock);
		 			//----------------------------------------------------
                    snprintf(sLog, sizeof(sLog), "[%s][STOPTASK]stop task [%s] size [%d] successfully.\n", sTag, sID, g_stoptask_size);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		}

            }
            else
            {
                snprintf(sLog, sizeof(sLog), "[%s][STOPTASK]can not find task [%s] to stop.\n", sTag, sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                snprintf(sMsg, 100, "500 can not find task [%s].\r\n", sID);
                SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
            }
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
        }
        else
        {
            snprintf(sLog, sizeof(sLog), "[%s][STOPTASK]read error or format error.\n", sTag);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        }	
        //snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]task <%s> has been accepted before, will not accept again.\n", sTag, sFileName);
        //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;		
    }
    else if(k == D_RestPackage)
    {
        int nn = 0;
        TaskQueue *pTaskUsed;

        snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]this connection is for restart task.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        nn = SocketReadLine(nSocket, pConf->nNetTimeOut, sMsg, nMsgSize);
        if(nn > 0 && strchr(sMsg, '\n'))
        {
            int nID[3];
            char sID[50];
            sscanf(sMsg, " %d.%d.%d", nID, nID+1, nID+2);
            snprintf(sID, 50, "%d.%d.%d", nID[0], nID[1], nID[2]);
            nID[0] = htonl(nID[0]);
            nID[1] = htonl(nID[1]);
            nID[2] = htonl(nID[2]);
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            pTaskUsed = pDabudai->pBegin;
            while(pTaskUsed)
            {
                if(memcmp(pTaskUsed->sID, nID, 12) == 0)
                    break;
                pTaskUsed = pTaskUsed->pNext;	
            }
            if(pTaskUsed)
            {
                //printf("RESTART:::%d::\n\n", pTaskUsed->tPauseTime);
                pthread_mutex_lock(pTaskUsed->pLock);
                if(pTaskUsed->tFinishTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]task [%s] has finished.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "500 task [%s] has finish.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else if(pTaskUsed->tPauseTime == 0)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]task [%s] is not in pause stat now.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    snprintf(sMsg, 100, "500 task [%s] is not in pause stat now.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));	
                }
                else
                {
                    pTaskUsed->tPauseTime = 0;
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]restart task [%s].\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "200 task [%s] restart successfully.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }	
            }
            else
            {
                snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]can not find task [%s] to restart.\n", sTag, sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                snprintf(sMsg, 100, "500 can not find task [%s]\r\n", sID);
                SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
            }
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
        }
        else
        {
            snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]read error or format error.\n", sTag);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        }

        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;	
    }
    else if(k == D_PausPackage)
    {
        int nn = 0;
        TaskQueue *pTaskUsed;

        snprintf(sLog, sizeof(sLog), "[%s][PAUSTASK]this connection is for pause task.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        nn = SocketReadLine(nSocket, pConf->nNetTimeOut, sMsg, nMsgSize);
        if(nn > 0 && strchr(sMsg, '\n'))
        {
            int nID[3];
            char sID[50];
            sscanf(sMsg, " %d.%d.%d", nID, nID+1, nID+2);
            snprintf(sID, 50, "%d.%d.%d", nID[0], nID[1], nID[2]);
            nID[0] = htonl(nID[0]);
            nID[1] = htonl(nID[1]);
            nID[2] = htonl(nID[2]);
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            pTaskUsed = pDabudai->pBegin;
            while(pTaskUsed)
            {
                if(memcmp(pTaskUsed->sID, nID, 12) == 0)
                    break;
                pTaskUsed = pTaskUsed->pNext;	
            }
            if(pTaskUsed)
            {
                //printf("PAUSE:::%d::\n\n", pTaskUsed->tPauseTime);
                pthread_mutex_lock(pTaskUsed->pLock);
                if(pTaskUsed->tFinishTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][PAUSETASK]task [%s] has finished.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "500 task [%s] has finish.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else if(pTaskUsed->tPauseTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][PAUSETASK]task [%s] has been paused before.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "500 task [%s] has been paused before.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else
                {
                    pTaskUsed->tPauseTime = time(NULL);
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][PAUSETASK]pause task [%s].\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "200 pause task [%s] successfully.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }	
            }
            else
            {
                snprintf(sLog, sizeof(sLog), "[%s][PAUSETASK]can not find task [%s] to pause.\n", sTag, sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                snprintf(sMsg, 100, "500 can not find task [%s].\r\n", sID);
                SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
            }
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
        }
        else
        {
            snprintf(sLog, sizeof(sLog), "[%s][PAUSTASK]read error or format error.\n", sTag);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        }

        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;	
    }
    else if(k == D_ShowPackage)
    {
        int nn = 0;
        TaskQueue *pTaskUsed;

        snprintf(sLog, sizeof(sLog), "[%s][SHOWTASK]this connection is for show task.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        nn = SocketReadLine(nSocket, pConf->nNetTimeOut, sMsg, nMsgSize);
        if(nn > 0 && strchr(sMsg, '\n'))
        {
            int *nID;
            int nOffset = 4;
            snprintf(sMsg, 100, "200 ");
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            pTaskUsed = pDabudai->pBegin;
            snprintf(sMsg+nOffset, 100, "%d tasks:", pDabudai->nTaskNum);
            nOffset = nOffset + strlen(sMsg+nOffset);
            while(pTaskUsed)
            {
                if(nOffset + 400 > nMsgSize)
                {
                    nMsgSize += 1024;
                    sMsg = (char*)realloc(sMsg, nMsgSize);	
                }
                nID = (int*)(pTaskUsed->sID);
                pthread_mutex_lock(pTaskUsed->pLock);
                //printf("RESTART:::%d::\n\n", pTaskUsed->tPauseTime);
                if(pTaskUsed->tPauseTime)
                    snprintf(sMsg+nOffset, 380, "[%d.%d.%d](pause),", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]));
                else if(pTaskUsed->tFinishTime)
                    snprintf(sMsg+nOffset, 380, "[%d.%d.%d](finish),", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]));
                else
                    snprintf(sMsg+nOffset, 380, "[%d.%d.%d](process),", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]));
                pthread_mutex_unlock(pTaskUsed->pLock);
                nOffset = nOffset + strlen(sMsg+nOffset);
                pTaskUsed = pTaskUsed->pNext;	
            }
            snprintf(sMsg+nOffset, nMsgSize-nOffset, "\r\n");
            nOffset += 2;
            SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, nOffset);
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
        }
        else
        {
            snprintf(sLog, sizeof(sLog), "[%s][RESTTASK]read error or format error.\n", sTag);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        }

        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;		
    }
    else if(k == D_StatPackage)
    {
        int nn = 0;
        TaskQueue *pTaskUsed;

        snprintf(sLog, sizeof(sLog), "[%s][STATTASK]this connection is for stat task.\n", sTag);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        nn = SocketReadLine(nSocket, pConf->nNetTimeOut, sMsg, nMsgSize);
        if(nn > 0 && strchr(sMsg, '\n'))
        {
            int nID[3];
            char sID[50];
            sscanf(sMsg, " %d.%d.%d", nID, nID+1, nID+2);
            snprintf(sID, 50, "%d.%d.%d", nID[0], nID[1], nID[2]);
            nID[0] = htonl(nID[0]);
            nID[1] = htonl(nID[1]);
            nID[2] = htonl(nID[2]);
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            pTaskUsed = pDabudai->pBegin;
            while(pTaskUsed)
            {
                if(memcmp(pTaskUsed->sID, nID, 12) == 0)
                    break;
                pTaskUsed = pTaskUsed->pNext;	
            }
            if(pTaskUsed)
            {
                pthread_mutex_lock(pTaskUsed->pLock);
                if(pTaskUsed->tFinishTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][STATTASK]task [%s] is finished.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "227 task [%s] is finish.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else if(pTaskUsed->tPauseTime)
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    snprintf(sLog, sizeof(sLog), "[%s][STATTASK]task [%s] is paused.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "226 task [%s] is paused.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }
                else
                {
                    pthread_mutex_unlock(pTaskUsed->pLock);
                    //pTaskUsed->tPauseTime = time(NULL);
                    snprintf(sLog, sizeof(sLog), "[%s][STATTASK]task [%s] is proccessing.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "224 task [%s] is proccessing.\r\n", sID);
                    SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
                }	

            }
            else
            {
                char sName[100];

                snprintf(sName, 100, "%s/%s.task", pConf->sTaskBakDir, sID);
                if(access(sName, F_OK) == 0)
                {
                    snprintf(sLog, sizeof(sLog), "[%s][STATTASK]task [%s] has finish, and has delete from queue.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "227 task [%s] is finish.\r\n", sID);	
                }	
                else
                {
                    snprintf(sLog, sizeof(sLog), "[%s][STATTASK]can not find task [%s] to stat.\n", sTag, sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    snprintf(sMsg, 100, "500 can not find task [%s].\r\n", sID);
                }
                SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
            }
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
        }
        else
        {
            snprintf(sLog, sizeof(sLog), "[%s][PAUSTASK]read error or format error.\n", sTag);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        }

        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;	
    }
    else if(k == D_PostPackage)
    {
        char *sTmp;
        int k1, k2;
        int nLeng = 0;
        char *sModular = NULL, *sModularEnd;
        char *sParam = NULL, *sParamEnd;
        char *sMail = NULL, *sMailEnd;
        //char *sTmp;
        int nID1 = 0, nID2, nID3;

        //free(sMsg);
        //sMsg = (char*)malloc(nMsgSize);
        //printf("%d\n", __LINE__);
        //fflush(NULL);
        //free(sMsg);
        //sMsg = (char*)malloc(102400);
        //nMsgSize = 102400;
        ///ReceivePost(&sMsg, &nMsgSize, &nLeng, nSocket, pConf->nNetTimeOut);
        k = SocketRead2(nSocket, pConf->nNetTimeOut, (char*)(&nLeng), 4);
        //printf("POST%d\n", nLeng);
        if(k != 4 || nLeng < 0 || nLeng >10240000)
        {
            sMsg[0] = 0;	
        }	
        else 
        {
            char ss[10];
            //free(sMsg);
            //sMsg = (char*)malloc(nMsgSize);
            //printf("%d\n", __LINE__);
            //fflush(NULL);
            if(nLeng+10 > nMsgSize)
            {
                free(sMsg);
                sMsg = (char*)malloc(nLeng+10);
                nMsgSize = nLeng+10;
            }
            k = SocketRead2(nSocket, pConf->nNetTimeOut, sMsg, nLeng);
            sMsg[nLeng] = 0;	
            snprintf(ss, 10, "OK\n");
            SocketWrite(nSocket, pConf->nNetTimeOut, ss, 3);
        }

        {
            sTmp = strstr(sMsg, "<$ID>");
            if(sTmp)
                sTmp = strchr(sTmp, '\n');
            if(sTmp)
            {
                sTmp++;
                sscanf(sTmp, "%d.%d.%d", &nID1, &nID2, &nID3);
                //printf("ID:%d:%d:%d\n", nID1, nID2, nID3);	
            }
        }
        {
            sTmp = strstr(sMsg, "<$MODULA>");
            if(sTmp)
                sTmp = strchr(sTmp, '\n');
            if(sTmp)
                sModular = sTmp+1;
            if(sModular)
                sModularEnd = strstr(sModular, "</$MODULA>");	
            if(sModularEnd == NULL)
                sModular = NULL;
            //printf("MODULAR:%s\n", sModular);	

        }
        {
            sTmp = strstr(sMsg, "<$MAILLIST>");
            if(sTmp)
                sTmp = strchr(sTmp, '\n');
            if(sTmp)
            {
                sParam = sTmp+1;
                sParamEnd = strchr(sParam, '\n');
                if(sParamEnd == NULL)
                    sParam = NULL;
                if(*(sParamEnd-1) == '\r')
                    *(sParamEnd-1) = 0;
                sTmp = strchr(sParam, '\n');
                if(sTmp)
                {
                    sMail = sTmp+1;
                    sMailEnd = strchr(sMail, '\n');
                    if(sMailEnd == NULL)
                        sMail = NULL;	
                    if(*(sMailEnd-1) == '\r')
                        *(sMailEnd-1) = 0;
                }
            }
            //printf();	
        }

        if(sMail && sModular && nID1 && sParam)
        {
            char *s = (char*)malloc(nMsgSize);	
            char *s2 = (char*)malloc(12);
            int n = 0;
            int nOffset = 6+8;
            char *sTmp1, *sTmp2;
            DBT dbtKey, dbtData;
            time_t tTime; 

            tTime = time(NULL);
            s[0] = 0;
            s[1] = 0;
            memcpy(s+2, &tTime, sizeof(time_t));
            *sMailEnd = 0;
            *sParamEnd = 0;

            sTmp1 = strchr(sParam, ',');
            sTmp2 = strchr(sMail, ',');
            //printf("PARAM:%s,MAILLIST:%s\n", sParam, sMail);
            if(sTmp2)
                *sTmp2 = 0;
            snprintf(s+nOffset, 100, "%s", sMail);
            nOffset += (strlen(sMail)+1);
            //printf("OFFSET:%d.\n", nOffset);
            while(sTmp1 && sTmp2 )
            {
                char *sTmp11, *sTmp22;
                sTmp1++;
                sTmp2++;
                sTmp11 = strchr(sTmp1, ',');
                sTmp22 = strchr(sTmp2, ',');
                if(sTmp11)
                    *sTmp11 = 0;
                if(sTmp22)
                    *sTmp22 = 0;

                snprintf(s+nOffset, nMsgSize - nOffset, "$%s$%s", sTmp1, sTmp2);
                nOffset += (strlen(sTmp1)+strlen(sTmp2)+3);
                //printf("OFFSET:%d, %s,%s.\n", nOffset, sTmp1, sTmp2);				
                if(sTmp11)
                    sTmp1 = sTmp11;
                else
                    sTmp1 = NULL;
                if(sTmp22)
                    sTmp2 = sTmp22;
                else
                    sTmp2 = NULL;	
            }
            //printf("OFFSET:%d.\n", nOffset);
            nOffset = nOffset - 14;
            memcpy(s+10, &nOffset, 4);
            nOffset += 14;
            nID1 = htonl(nID1);
            memcpy(s2, &nID1, 4);
            nID2 = htonl(nID2+1000000000);
            memcpy(s2+4, &nID2, 4);
            nID3 = htonl(nID3);
            memcpy(s2+8, &nID3, 4);

            *sModularEnd = 0;

            //printf("MODULAR:%s\n", sModular);

            n = strlen(sModular)+1;
            //printf("OFFSET:%d.\n", nOffset);
            memcpy(s+nOffset, &n, 4);
            nOffset += 4;
            //printf("OFFSET:%d.\n", nOffset);
            memcpy(s+nOffset, sModular, n);
            nOffset += n;
            //printf("OFFSET:%d.\n", nOffset);

            memset(&dbtKey, 0x0, sizeof(DBT));
            memset(&dbtData, 0x0, sizeof(DBT));
            dbtKey.data = s2;
            dbtKey.size = 12;
            dbtData.data = s;
            dbtData.size = nOffset;
            pthread_mutex_lock(pDabudai->pLockTriger);
            //printf("ADDTTTTTTTTTTTTTTTTTT,send:%d,stat:%d\n", *s, *(s+1));
            int kkk = pDabudai->dbList->put(pDabudai->dbList, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
            pthread_mutex_unlock(pDabudai->pLockTriger);
            if(kkk == 0)
            {
                snprintf(sLog, sizeof(sLog), "[%s][ADDTRIGER]add task %d.%d.%d successful.\n", sTag, ntohl(nID1), ntohl(nID2), ntohl(nID3));
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }
            else 
            {
                snprintf(sLog, sizeof(sLog), "[%s][ADDTRIGER]add task %d.%d.%d fail.\n", sTag, ntohl(nID1), ntohl(nID2), ntohl(nID3));
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
            }

            //printf("data size:%d, mailsize:%d", nOffset, ((int*)(s+10))[0]);
            free(s);
            free(s2);
        }

        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return(void*)0;	
    }
    else if(k)
    {
        //printf("receive fail at line <%d>, error code is:%d!\n", __LINE__, k);
        snprintf(sLog, sizeof(sLog), "[%s]receive fail, error code is:%d, will close this connection.\n", sTag, k);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return (void*)0;
    }	
    //printf("receive success::%d::\n", nMsgLeng);

    /*{
    int i = 0;
    unsigned char *ss = sMsg;
   ;//printf("\n");
    while(i < 12)
    {
   ;//printf("[%d]0x%02x, ", i, ss[i]);	
    ++i;
    }
   ;//printf("\n");
    }*/
    
    do {
		char notify_msg[MAX_MSG_LEN] = {0};
		int notify_msg_size = make_stop_delivery_notify(notify_msg + 12, MAX_MSG_LEN - 12);
		if (notify_msg_size > 0) {
			snprintf(sLog, sizeof(sLog), "notify  size [%d] info [%s] .\n", notify_msg_size, notify_msg + 16);
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			//包体长度
        		int k = 8 + notify_msg_size;
        		k = htonl(k);
        		memcpy(notify_msg, &k, 4);
			//状态码 
			k = 4;
			k = htonl(k);
			memcpy(notify_msg + 4, &k, 4);	
			//内容长度
        		k = htonl(notify_msg_size);
        		memcpy(notify_msg + 8, &k, 4);
        
       			k = SendPackage(nSocket, notify_msg, 12 + notify_msg_size, pConf->nNetTimeOut);
        		if(k) {
            			snprintf(sLog, sizeof(sLog), "[noitfy] net write fail, error code is:%d.\n", k);
            			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            			pthread_mutex_lock(pDabudai->pLockNet);
            			BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
            			pthread_mutex_unlock(pDabudai->pLockNet);
            			close(nSocket);
            			return (void*)0;
        		}
		}//if_notify
   } while (0);
    memcpy(&nCommand, sMsg+8, 4);
	//发送机器向管理机器报告它的能力
    if(nCommand == 1 || ntohl(nCommand) == 1) //请求邮箱
    {
        int nDomainNum = 0;
        char *sResult = (char*)malloc(10240);
        int nResultSize = 10240;
        int nResultLeng = 0;
        memcpy(&nDomainNum, sMsg+12, 4); //域的个数,偏移量12 bytes
        nDomainNum = ntohl(nDomainNum);
        memset(sResult, 0x0, 13);
        //k = AnswerMailboxRequest3(sMsg+16, nDomainNum, pAlias, pMailList, &sResult, &nResultSize, &nResultLeng, 17);
        snprintf(sLog, sizeof(sLog), "[%s][REQUEST]this connection is for request mailbox.\n", sTag);
        //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        pthread_rwlock_rdlock(pDabudai->pLockRWTask);
        //k = AnswerMailboxRequest4(sMsg+16, nDomainNum, pDabudai->pBegin, pDabudai->pEnd, &sResult, &nResultSize, 
        //													&nResultLeng, 17, pDabudai->nTaskNum, &(pDabudai->nTaskOffset), pDabudai->pLockTask);
        k = AnswerMailboxRequestWithLevel(sMsg+16,//偏移量16 ,直接指向域内容 domain=xxx&cnt=xxx\0
            nDomainNum, //域的个数
            pDabudai,  //参数
            &sResult,  //输出缓冲
            &nResultSize, //内存大小
            &nResultLeng, //最终的长度
            17, //17不知如何来 ,看参数名称 result_begin,应该是偏移量一类的东西。
            pConf, //参数
            sLog,  //日志输出缓冲
            sizeof(sLog),  
            sTag); //日志类型标签

        pthread_rwlock_unlock(pDabudai->pLockRWTask);

        {
            DBC *dbc;
            DBT dbtKey, dbtData;
            DBT dbtKey2, dbtData2;
            DBC *dbcNum;
            int i = 0;
            time_t tNow;
            int nLogOffset;
            int nGet = 0;

            snprintf(sLog, sizeof(sLog), "[%s][TRIGER]", sTag);
            nLogOffset = strlen(sLog);
            int nHeadOffset = nLogOffset;
            tNow = time(NULL);
            pDabudai->dbList->cursor(pDabudai->dbList, NULL, &dbc, 0);
            pDabudai->dbNumber->cursor(pDabudai->dbNumber, NULL, &dbcNum, 0);
            memset(&dbtKey, 0x0, sizeof(DBT));
            memset(&dbtData, 0x0, sizeof(DBT));
            memset(&dbtKey2, 0x0, sizeof(DBT));
            memset(&dbtData2, 0x0, sizeof(DBT));
            pthread_mutex_lock(pDabudai->pLockTriger);
            while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_PREV) == 0) //遍历数据库 从尾-头
            {
                char *s = (char*)(dbtData.data);
                time_t *t = (time_t*)(s+2);
                int *n = (int*)(s+10);
                int ll;
                int *nID = (int*)(dbtKey.data);


                //printf("TTTTTTTTTTTTTTTTRIGER:%d.%d.%d,now:%ld,t:%ld,send:%d, stat:%d, %s, period:%d, retry:%d.\n", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), tNow, *t, *s, *(s+1), s+14, pConf->nTrigerRetryPeriod, pConf->nTrigerRetryTime);
                if(*t <= tNow && *s < pConf->nTrigerRetryTime)
                {

                    snprintf(sLog+nLogOffset, 1024-nLogOffset, "[%d.%d.%d]%s,", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), s+14);
                    nLogOffset += strlen(sLog+nLogOffset);

                    k++;
                    if(nResultSize-nResultLeng < 1024)
                    {
                        nResultSize += 2048;
                        sResult = (char*)realloc(sResult, nResultSize);	
                    }	
                    ll = htonl(*n + 12);
                   ;//printf("trigger:[%d.%d.%d]%s, %d\n", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), s+14, *n);
                    memcpy(sResult+nResultLeng, &ll, 4);
                    memcpy(sResult+nResultLeng+4, dbtKey.data, 12);
                    memcpy(sResult+nResultLeng+16, dbtData.data+14, *n);
                    nResultLeng += (*n + 16);
                    *t = tNow + pConf->nTrigerRetryPeriod;

                    if(*s == 0)
                    {
                        {
                            int n[3];
                            int nWhich;
                            memcpy(n, nID, 12);
                            n[1] = htonl(ntohl(n[1]));
                            n[2] = n[0];
                            dbtKey2.data = n;
                            dbtKey2.size = 12;
                            {
                                char *sDomain = strchr(s+14, '@');
                                if(!sDomain)
                                    nWhich = pDabudai->pDomainAlias->nDomainNum;
                                else
                                {
                                    sDomain++;
                                    nWhich = FindAliasDomain(sDomain, pDabudai->pDomainAlias);
                                    if(nWhich < 0)
                                        nWhich = pDabudai->pDomainAlias->nDomainNum;	
                                }		
                            }
                            pthread_mutex_lock(pDabudai->pLockTrigerNum);
                            if(dbcNum->c_get(dbcNum, &dbtKey2, &dbtData2, DB_SET) == 0)
                            {
                                int *nn = (int*)(dbtData2.data+16*nWhich);
                                (nn[3])++;
                                //(nn[3]) += nGet;
                               ;//printf("DBDB         %d  %d  %d  %d\n\n", nn[0], nn[1], nn[2], nn[3]);
                                if(dbcNum->c_put(dbcNum, &dbtKey2, &dbtData2, DB_CURRENT))
                                   ;//printf("cursor put error!!\n");
                            }
                            else
                            {
                                int nn[4*(pDabudai->pDomainAlias->nDomainNum+1)];
                                //int nn[4];
                                memset(nn, 0, 16*(pDabudai->pDomainAlias->nDomainNum+1));
                                nn[3+nWhich*4] = 1;
                                dbtData2.data = nn;
                                dbtData2.size = 16*(pDabudai->pDomainAlias->nDomainNum+1);
                                if(pDabudai->dbNumber->put(pDabudai->dbNumber, NULL, &dbtKey2, &dbtData2, DB_NOOVERWRITE))
                                   ;//printf("put error!!\n");	
                            }
                            pthread_mutex_unlock(pDabudai->pLockTrigerNum);	
                        }
                        //printf("get:%d\n", nGet);	
                    }

                    *s = *s+1;
                    //printf("TTTTTTTTTTTTTTTTRIGER:%d.%d.%d,now:%ld,t:%ld,send:%d, stat:%d, %s.\n", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), tNow, *t, *s, *(s+1), s+14);
                    dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);
                    i++;
                }
                else if(*t <= tNow && *s >= pConf->nTrigerRetryTime)
                {
                    pDabudai->dbList->del(pDabudai->dbList, NULL, &dbtKey, 0);
                   ;//printf("delete from trigger:[%d.%d.%d]%s.\n", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), s+14);
                }
                /*
                else
                {
                ;//printf("TTTTTTTTTTTTTTTTTKKKKKKKKKKKKKKKt:%ld now:%ld, time:%d, conf:%d.\n", *t, tNow, *s, pConf->nTrigerRetryTime);	
                }
                */
                if(i >= pConf->nTrigerDeliverNum)
                {	
                    //printf("TTTTTTTTEEND\n");
                    break; 
                }
                ///////////YYYYYYYYYYYYYYYYYYYY
            }
            pthread_mutex_unlock(pDabudai->pLockTriger);

            if (nHeadOffset < nLogOffset)
            {
                snprintf(sLog+nLogOffset, 1024-nLogOffset, "\n");
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }


            dbc->c_close(dbc);
            dbcNum->c_close(dbcNum);

        }
        k = htonl(k);
        memcpy(sResult+13, &k, 4);
        k = 1;
        k = htonl(k);
        memcpy(sResult+8, &k, 4);
        k = nResultLeng-4;
        k = htonl(k);
        memcpy(sResult, &k, 4);
        k = 1;
        k = htonl(k);
        memcpy(sResult+4, &k, 4);
        k = SendPackage(nSocket, sResult, nResultLeng, pConf->nNetTimeOut);

        free(sResult);	

        if(k)
        {
            //printf("write fail at line <%d>, error code is:%d!\n", __LINE__, k);
            snprintf(sLog, sizeof(sLog), "[%s][REQUEST]net write fail, error code is:%d, will close this connection.\n", sTag, k);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            pthread_mutex_lock(pDabudai->pLockNet);
            BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
            pthread_mutex_unlock(pDabudai->pLockNet);
            close(nSocket);
            free(sMsg);
            return (void*)0;
        }	
    }
    //发送机器向管理机器请求模板
	else if(nCommand == 2 || ntohl(nCommand) == 2)
    {
        char *sEml = NULL;
        int nLeng;
        char sID[12];
        int *nn = (int*)(sID);

        memcpy(sID, sMsg+12, 12);
        snprintf(sLog, sizeof(sLog), "[%s][MODULAR]this connection is for modular (%d.%d.%d).\n", sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]));
        //WriteLog(pConf, pDabudai, sLog, strlen(sLog));


        if(ntohl(nn[1]) < 1000000000)
        {
            pthread_rwlock_rdlock(pDabudai->pLockRWTask);
            {
                TaskQueue *p = pDabudai->pBegin;
                while(p)
                {
                    if(memcmp(sID, p->sID, 12) == 0)
                        break;
                    p = p->pNext;	
                }
                if(p)
                {
                    sEml = (char*)malloc(p->nModulaSize+50);
                    memcpy(sEml+29, p->sModula, p->nModulaSize);
                    nLeng = 29+(p->nModulaSize);
                    snprintf(sLog, sizeof(sLog), "[%s][MODULAR]find modular, size is:%d.\n", sTag, p->nModulaSize);
                    //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                }
                else
                {
                    snprintf(sLog, sizeof(sLog), "[%s][MODULAR]can't find modular.\n", sTag);
                    //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    sEml = (char*)malloc(50);
                    *(sEml+29) = 0;	
                    nLeng = 30;
                }
            }
            pthread_rwlock_unlock(pDabudai->pLockRWTask);

            k = nLeng-4;
            k = htonl(k);
            memcpy(sEml, &k, 4);
            memset(sEml+4, 0x0, 4);
            k = 1;
            k = htonl(k);
            memcpy(sEml+8, &k, 4);
            memset(sEml+12, 0, 1);
            memcpy(sEml+13, sID, 12);
            k = nLeng-29;
            k = htonl(k);
            memcpy(sEml+25, &k, 4);
            k = 2;
            k = htonl(k);
            memcpy(sEml+4, &k, 4);
        }
        else
        {
            DBC *dbc;
            DBT dbtKey, dbtData;

            pDabudai->dbList->cursor(pDabudai->dbList, NULL, &dbc, 0);
            memset(&dbtKey, 0x0, sizeof(DBT));
            memset(&dbtData, 0x0, sizeof(DBT));	
            dbtKey.data = sID;
            dbtKey.size = 12;
            pthread_mutex_lock(pDabudai->pLockTriger);
            if(dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET) == 0 && dbtData.size > 14)
            {
                int *k1, *k2;
                k1 = (int*)(dbtData.data+10);
                if(dbtData.size > 18 + (*k1) )
                {

                    k2 = (int*)(dbtData.data+14+(*k1));
                    sEml = (char*)malloc(*k2 + 50);
                    memcpy(sEml+29, dbtData.data+(*k1)+18, *k2);
                    nLeng = 29 + (*k2);
                    snprintf(sLog, sizeof(sLog), "[%s][MODULAR]find modular, size is:%d.\n", sTag, *k2);
                    //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                }
                else
                {
                    sEml = (char*)malloc(50);
                    *(sEml+29) = 0;	
                    nLeng = 30;	
                }
            }
            else
            {
                snprintf(sLog, sizeof(sLog), "[%s][MODULAR]can't find modular.\n", sTag);
                //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                sEml = (char*)malloc(50);
                *(sEml+29) = 0;	
                nLeng = 30;
            }
            pthread_mutex_unlock(pDabudai->pLockTriger);
            dbc->c_close(dbc);
            k = nLeng-4;
            k = htonl(k);
            memcpy(sEml, &k, 4);
            memset(sEml+4, 0x0, 4);
            k = 1;
            k = htonl(k);
            memcpy(sEml+8, &k, 4);
            memset(sEml+12, 0, 1);
            memcpy(sEml+13, sID, 12);
            k = nLeng-29;
            k = htonl(k);
            memcpy(sEml+25, &k, 4);
            k = 2;
            k = htonl(k);
            memcpy(sEml+4, &k, 4);
        }
        k = SendPackage(nSocket, sEml, nLeng, pConf->nNetTimeOut);
        free(sEml);
        if(k)
        {
            //printf("write fail at line <%d>, error code is:%d!\n", __LINE__, k);
            snprintf(sLog, sizeof(sLog), "[%s][MODULAR]net write fail, error code is:%d, will close this connection.\n", sTag, k);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            pthread_mutex_lock(pDabudai->pLockNet);
            BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
            pthread_mutex_unlock(pDabudai->pLockNet);
            close(nSocket);
            free(sMsg);
            return (void*)0;
        }	

    }
    //发送机器向管理机器报告发送的情况
	else if(nCommand == 3 || ntohl(nCommand) == 3)
    {
        int nNumber;
        int i = 0;
        int nOffset = 16;
        int nLogOffset;
        DBC *dbc;
        DBC *dbcNum;

        pDabudai->dbList->cursor(pDabudai->dbList, NULL, &dbc, 0);
        pDabudai->dbNumber->cursor(pDabudai->dbNumber, NULL, &dbcNum, 0);

        memcpy(&nNumber, sMsg+12, 4);
        nNumber = ntohl(nNumber);
        snprintf(sLog, sizeof(sLog), "[%s][REPORT]this connection is for report, number is:%d, length is:%d.\n", 
            sTag, nNumber, nMsgLeng);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        /*
        if(nNumber > 0)
        {
        FILE *p = fopen("/tmp/ttt.txt", "w");
        fwrite(sMsg, nMsgLeng, 1, p);
        fclose(p);	
        }
        */
        snprintf(sLog, sizeof(sLog), "[%s][REPORT]", sTag);
        nLogOffset = strlen(sLog);
        int nHeadoffset = nLogOffset;
        pthread_rwlock_rdlock(pDabudai->pLockRWTask);
        while(i < nNumber && nOffset < nMsgLeng)
        {
            //printf("[%d][%c]<%s>\n", i, *(sMsg+nOffset+12), sMsg+nOffset+13);
            int *nnID = (int*)(sMsg+nOffset);
            if(*(sMsg+nOffset+12) == 's')
            {
                char sID[100];
                sID[0] = 0;
                if(nLogOffset > sizeof(sLog)-300)
                {
                    if(nLogOffset <= sizeof(sLog)-2)
                    {	
                        sLog[nLogOffset] = '\n';	
                        sLog[nLogOffset+1] = 0;
                        WriteLog(pConf, pDabudai, sLog, nLogOffset+1);
                    }
                    else
                    {
                        sLog[sizeof(sLog)-2] = '\n';
                        sLog[sizeof(sLog)-1] = 0;	
                        WriteLog(pConf, pDabudai, sLog, sizeof(sLog)-1);
                    }
                    {
                        snprintf(sLog, sizeof(sLog), "[%s][REPORT]", sTag);
                    }
                    nLogOffset = strlen(sLog);
                }
                if(nLogOffset < sizeof(sLog))	
                {

                    char sReturn[200];
                    char *sTmp;
                    snprintf(sReturn, sizeof(sReturn), "%s", sMsg+nOffset+strlen(sMsg+nOffset+13)+14);
                    sTmp = strchr(sReturn, '\r');
                    if(!sTmp)
                        sTmp = strchr(sReturn, '\n');
                    if(sTmp)
                        *sTmp = 0;
                    snprintf(sID, 100, "[%d.%d.%d]", ntohl(nnID[0]), ntohl(nnID[1]), ntohl(nnID[2]));
                    snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[s]%s%s(%s)", sID, sMsg+nOffset+13, sReturn);													
                    nLogOffset += (3+strlen(sMsg+nOffset+13)+strlen(sID)+strlen(sReturn)+2);
                }
                if(ntohl(nnID[1]) < 1000000000)
                {
                    if(ChangeMailboxStatWithLevel(pDabudai->pBegin, pDabudai->pEnd, sMsg+nOffset, sMsg+nOffset+13, D_MailList_Success) < 0)
                    {
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;
                        //printf("LLLLLLLLLLLLLLLLL<<%s>>LLLLLLLLLLLLL\n", sMsg+nOffset+13);
                    }
                }
                else
                {
                    int k;
                    char cStat;
                    int nID[3];
                    DBT dbtKey, dbtData;
                    int nStat = D_MailList_Success;
                    int nWhich;
                    pthread_mutex_lock(pDabudai->pLockTriger);
                    if((k = ChangeMailboxStatTriger(pConf, pDabudai, sMsg+nOffset, sMsg+nOffset+13, D_MailList_Success, dbc, &cStat)))//////////UUUUUUUUUUUUUUUUUUUUUUU
                    {
                       ;//printf("EEEEE%dEEEEE\n", k);
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;	
                    }	
                    pthread_mutex_unlock(pDabudai->pLockTriger);
                    memset(&dbtKey, 0x0, sizeof(DBT));
                    memset(&dbtData, 0x0, sizeof(DBT));
                    memcpy(nID, sMsg+nOffset, 12);
                    nID[1] = htonl( ntohl(nID[1]));
                    nID[2] = nID[0];
                    dbtKey.data = nID;
                    dbtKey.size = 12;
                    {
                        char *s = strchr(sMsg+nOffset+13, '@');
                        if(!s)
                            nWhich = pDabudai->pDomainAlias->nDomainNum;
                        else
                        {
                            s++;
                            nWhich = FindAliasDomain(s, pDabudai->pDomainAlias);
                            if(nWhich < 0)
                                nWhich = pDabudai->pDomainAlias->nDomainNum;	
                        }	
                    }
                    pthread_mutex_lock(pDabudai->pLockTrigerNum);
                    if(dbcNum->c_get(dbcNum, &dbtKey, &dbtData, DB_SET) == 0)
                    {
                        int *n = (int*)(dbtData.data+16*nWhich);
                        if(nStat == D_MailList_Success)
                            (n[0])++;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            (n[1])++;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            (n[2])++;
                       ;//printf("DB22222222222AAAAAAAA  FAIL  %s %d %d %d %d, %d\n", sMsg+nOffset+13, n[0], n[1], n[2], n[3], cStat);
                        if((cStat == D_MailList_Delay || cStat == D_MailList_Dns)&& n[1] > 0)
                            (n[1])--;
                        else if( (cStat == D_MailList_Fail || cStat == D_MailList_NotFound || cStat == D_MailList_Spam) && n[2] > 0)
                            (n[2])--;
                       ;//printf("DB222222222222DB  FAIL  %s  %d %d  %d  %d, %d\n", sMsg+nOffset+13, n[0], n[1], n[2], n[3], cStat);
                        dbcNum->c_put(dbcNum, &dbtKey, &dbtData, DB_CURRENT);
                    }
                    else
                    {
                        int n[4*(pDabudai->pDomainAlias->nDomainNum+1)];
                        memset(n, 0, 16*(pDabudai->pDomainAlias->nDomainNum+1));
                        if(nStat == D_MailList_Success)
                            n[0+4*nWhich] = 1;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            n[1+4*nWhich] = 1;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            n[2+4*nWhich] = 1;
                        dbtData.data = n;
                        dbtData.size = 16*(pDabudai->pDomainAlias->nDomainNum+1);
                       ;//printf("DB222222222222DB    %d %d  %d  %d\n", n[0], n[1], n[2], n[3]);
                        pDabudai->dbNumber->put(pDabudai->dbNumber, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
                    }
                    pthread_mutex_unlock(pDabudai->pLockTrigerNum);
                }
            }
            else if(*(sMsg+nOffset+12) == 'd')
            {
                char sID[100];
                char sReturn[200];
                int nStat = D_MailList_Delay;
                sID[0] = 0;
                if(nLogOffset > sizeof(sLog)-200)
                {
                    if(nLogOffset <= sizeof(sLog)-2)
                    {	
                        sLog[nLogOffset] = '\n';	
                        sLog[nLogOffset+1] = 0;
                        WriteLog(pConf, pDabudai, sLog, nLogOffset+1);
                    }
                    else
                    {
                        sLog[sizeof(sLog)-2] = '\n';
                        sLog[sizeof(sLog)-1] = 0;	
                        WriteLog(pConf, pDabudai, sLog, sizeof(sLog)-1);
                    }
                    {
                        snprintf(sLog, sizeof(sLog), "[%s][REPORT]", sTag);
                    }
                    nLogOffset = strlen(sLog);
                }
                if(nLogOffset < sizeof(sLog))
                {	
                    int *nnID = (int*)(sMsg+nOffset);
                    //char sReturn[200];
                    char *sTmp;
                    snprintf(sReturn, sizeof(sReturn), "%s", sMsg+nOffset+strlen(sMsg+nOffset+13)+14);
                    sTmp = strchr(sReturn, '\r');
                    if(!sTmp)
                        sTmp = strchr(sReturn, '\n');
                    if(sTmp)
                        *sTmp = 0;
                    snprintf(sID, 100, "[%d.%d.%d]", ntohl(nnID[0]), ntohl(nnID[1]), ntohl(nnID[2]));
                    snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[d]%s%s(%s)", sID, sMsg+nOffset+13, sReturn);
                    nLogOffset += (3+strlen(sMsg+nOffset+13)+strlen(sID)+strlen(sReturn)+2);
                }
                if( strstr(sReturn, "Domain not exist") == NULL && (strstr(sReturn, "user doesn") || strstr(sReturn, "not exist") || strstr(sReturn, " unknown") || strstr(sReturn, "not found") 
                    || strstr(sReturn, "such user") || strstr(sReturn, "nvalid recipient") || strstr(sReturn, " Unknown") || 
                    strstr(sReturn, "unavailable") || strstr(sReturn, "is disable") || strstr(sReturn, "ddress rejected") ||
                    strstr(sReturn, "such recipient") || strstr(sReturn, "no mailbox") || strstr(sReturn, "ccess denied")))
                {	
                    if(strstr(sReturn, ":5"))
                        nStat = D_MailList_NotFound;
                }
                else if(strstr(sReturn, "step 1--") || strstr(sReturn, "Domain not exist"))
                    nStat = D_MailList_Dns;
                else if(strstr(sReturn, "Spam") || strstr(sReturn, "spam"))
                    nStat = D_MailList_Spam;
                if(ntohl(nnID[1]) < 1000000000)
                {	
                    if(ChangeMailboxStatWithLevel(pDabudai->pBegin, pDabudai->pEnd, sMsg+nOffset, sMsg+nOffset+13, nStat) < 0)
                    {		
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;
                        //printf("LLLLLLLLLLLLLLLLL<<%s>>LLLLLLLLLLLLL\n", sMsg+nOffset+13);
                    }
                }
                else
                {
                    int k;
                    char cStat;
                    int nID[3];
                    DBT dbtKey, dbtData;
                    int nWhich;
                    pthread_mutex_lock(pDabudai->pLockTriger);
                    if((k = ChangeMailboxStatTriger(pConf, pDabudai, sMsg+nOffset, sMsg+nOffset+13, nStat, dbc, &cStat)))//////////UUUUUUUUUUUUUUUUUUUUUUU
                    {
                       ;//printf("EEEEE%dEEEEE\n", k);
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;	
                    }	
                    {
                        char *s = strchr(sMsg+nOffset+13, '@');
                        if(!s)
                            nWhich = pDabudai->pDomainAlias->nDomainNum;
                        else
                        {
                            s++;
                            nWhich = FindAliasDomain(s, pDabudai->pDomainAlias);
                            if(nWhich < 0)
                                nWhich = pDabudai->pDomainAlias->nDomainNum;	
                        }	
                    }
                    pthread_mutex_unlock(pDabudai->pLockTriger);
                    memset(&dbtKey, 0x0, sizeof(DBT));
                    memset(&dbtData, 0x0, sizeof(DBT));
                    memcpy(nID, sMsg+nOffset, 12);
                    nID[1] = htonl( ntohl(nID[1])  );
                    nID[2] = nID[0];
                    dbtKey.data = nID;
                    dbtKey.size = 12;
                    pthread_mutex_lock(pDabudai->pLockTrigerNum);
                    if(dbcNum->c_get(dbcNum, &dbtKey, &dbtData, DB_SET) == 0)
                    {
                        int *n = (int*)(dbtData.data+16*nWhich);
                        if(nStat == D_MailList_Success)
                            (n[0])++;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            (n[1])++;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            (n[2])++;
                        if((cStat == D_MailList_Delay || cStat == D_MailList_Dns)&& n[1] > 0)
                            (n[1])--;
                        else if( (cStat == D_MailList_Fail || cStat == D_MailList_NotFound || cStat == D_MailList_Spam) && n[2] > 0)
                            (n[2])--;
                       ;//printf("DB222222222222DB    %d %d  %d  %d\n", n[0], n[1], n[2], n[3]);
                        dbcNum->c_put(dbcNum, &dbtKey, &dbtData, DB_CURRENT);
                    }
                    else
                    {
                        int n[4*(pDabudai->pDomainAlias->nDomainNum+1)];
                        memset(n, 0, 16*(pDabudai->pDomainAlias->nDomainNum+1));
                        if(nStat == D_MailList_Success)
                            n[0+4*nWhich] = 1;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            n[1+4*nWhich] = 1;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            n[2+4*nWhich] = 1;
                        dbtData.data = n;
                        dbtData.size = 16*(pDabudai->pDomainAlias->nDomainNum+1);
                       ;//printf("DB222222222222DB    %d %d  %d  %d\n", n[0], n[1], n[2], n[3]);
                        pDabudai->dbNumber->put(pDabudai->dbNumber, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
                    }
                    pthread_mutex_unlock(pDabudai->pLockTrigerNum);
                }
            }
            else
            {
                char sID[100];
                char sReturn[200];
                int nStat = D_MailList_Fail;
                sID[0] = 0;
                if(nLogOffset > sizeof(sLog)-200)
                {
                    if(nLogOffset <= sizeof(sLog)-2)
                    {	
                        sLog[nLogOffset] = '\n';	
                        sLog[nLogOffset+1] = 0;
                        WriteLog(pConf, pDabudai, sLog, nLogOffset+1);
                    }
                    else
                    {
                        sLog[sizeof(sLog)-2] = '\n';
                        sLog[sizeof(sLog)-1] = 0;	
                        WriteLog(pConf, pDabudai, sLog, sizeof(sLog)-1);
                    }
                    {
                        snprintf(sLog, sizeof(sLog), "[%s][REPORT]", sTag);
                    }
                    nLogOffset = strlen(sLog);
                }
                if(nLogOffset < sizeof(sLog))	
                {
                    int *nnID = (int*)(sMsg+nOffset);
                    //char sReturn[200];
                    char *sTmp;
                    snprintf(sReturn, sizeof(sReturn), "%s", sMsg+nOffset+strlen(sMsg+nOffset+13)+14);
                    sTmp = strchr(sReturn, '\r');
                    if(!sTmp)
                        sTmp = strchr(sReturn, '\n');
                    if(sTmp)
                        *sTmp = 0;
                    snprintf(sID, 100, "[%d.%d.%d]", ntohl(nnID[0]), ntohl(nnID[1]), ntohl(nnID[2]));
                    snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[f]%s%s(%s)", sID, sMsg+nOffset+13, sReturn);
                    nLogOffset += (3+strlen(sMsg+nOffset+13)+strlen(sID)+strlen(sReturn)+2);
                }
                if(strstr(sReturn, "Domain not exist") == NULL && (strstr(sReturn, "user doesn") || strstr(sReturn, "not exist") || strstr(sReturn, " unknown") || strstr(sReturn, "not found") 
                    || strstr(sReturn, "such user") || strstr(sReturn, "nvalid recipient") || strstr(sReturn, " Unknown") || 
                    strstr(sReturn, "unavailable") || strstr(sReturn, "is disable") || strstr(sReturn, "ddress rejected") ||
                    strstr(sReturn, "such recipient") || strstr(sReturn, "no mailbox") || strstr(sReturn, "ccess denied")))
                {	
                    if(strstr(sReturn, ":5"))
                        nStat = D_MailList_NotFound;
                }
                else if(strstr(sReturn, "step 1--") || strstr(sReturn, "Domain not exist"))
                    nStat = D_MailList_Dns;
                else if(strstr(sReturn, "Spam") || strstr(sReturn, "spam"))
                    nStat = D_MailList_Spam;
                if(ntohl(nnID[1]) < 1000000000)
                {	
                    if(ChangeMailboxStatWithLevel(pDabudai->pBegin, pDabudai->pEnd, sMsg+nOffset, sMsg+nOffset+13, nStat) < 0)
                    {
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;
                        //printf("LLLLLLLLLLLLLLLLL<<%s>>LLLLLLLLLLLLL\n", sMsg+nOffset+13);
                    }
                }
                else
                {
                    int k;
                    char cStat;
                    int nID[3];
                    DBT dbtKey, dbtData;
                    int nWhich;

                    pthread_mutex_lock(pDabudai->pLockTriger);
                    if((k = ChangeMailboxStatTriger(pConf, pDabudai, sMsg+nOffset, sMsg+nOffset+13, nStat, dbc, &cStat)))//////////UUUUUUUUUUUUUUUUUUUUUUU
                    {
                       ;//printf("EEEEE%dEEEEE\n", k);
                        if(nLogOffset < sizeof(sLog))	
                            snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "[NOT FIND]");
                        nLogOffset += 10;	
                    }
                    pthread_mutex_unlock(pDabudai->pLockTriger);	
                    memcpy(nID, sMsg+nOffset, 12);
                    nID[1] = htonl( ntohl(nID[1])  );
                    nID[2] = nID[0];

                    memset(&dbtKey, 0x0, sizeof(DBT));
                    memset(&dbtData, 0x0, sizeof(DBT));
                    dbtKey.data = nID;
                    dbtKey.size = 12;
                   ;//printf("%d,,,,%d\n", nStat, cStat);
                    {
                        char *s = strchr(sMsg+nOffset+13, '@');
                        if(!s)
                            nWhich = pDabudai->pDomainAlias->nDomainNum;
                        else
                        {
                            s++;
                            nWhich = FindAliasDomain(s, pDabudai->pDomainAlias);
                            if(nWhich < 0)
                                nWhich = pDabudai->pDomainAlias->nDomainNum;	
                        }	
                    }
                    pthread_mutex_lock(pDabudai->pLockTrigerNum);
                    if(dbcNum->c_get(dbcNum, &dbtKey, &dbtData, DB_SET) == 0)
                    {
                        int *n = (int*)(dbtData.data+16*nWhich);
                        if(nStat == D_MailList_Success)
                            (n[0])++;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            (n[1])++;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            (n[2])++;
                       ;//printf("DB22222222222AAAAAAAA  FAIL  %s %d %d %d %d, %d\n", sMsg+nOffset+13, n[0], n[1], n[2], n[3], cStat);
                        if((cStat == D_MailList_Delay || cStat == D_MailList_Dns)&& n[1] > 0)
                            (n[1])--;
                        else if( (cStat == D_MailList_Fail || cStat == D_MailList_NotFound || cStat == D_MailList_Spam) && n[2] > 0)
                            (n[2])--;
                       ;//printf("DB222222222222DB  FAIL  %s  %d %d  %d  %d, %d\n", sMsg+nOffset+13, n[0], n[1], n[2], n[3], cStat);
                        dbcNum->c_put(dbcNum, &dbtKey, &dbtData, DB_CURRENT);
                    }
                    else
                    {
                        int n[4*(pDabudai->pDomainAlias->nDomainNum+1)];
                        memset(n, 0, 16*(pDabudai->pDomainAlias->nDomainNum+1));
                        if(nStat == D_MailList_Success)
                            n[0+4*nWhich] = 1;
                        else if(nStat == D_MailList_Delay || nStat == D_MailList_Dns )
                            n[1+4*nWhich] = 1;
                        else if(nStat == D_MailList_Fail || nStat == D_MailList_NotFound || nStat == D_MailList_Spam)
                            n[2+4*nWhich] = 1;
                        dbtData.data = n;
                        dbtData.size = 16*(pDabudai->pDomainAlias->nDomainNum+1);
                       ;//printf("DB222222222222DB    %d %d  %d  %d\n", n[0], n[1], n[2], n[3]);
                        pDabudai->dbNumber->put(pDabudai->dbNumber, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
                    }
                    pthread_mutex_unlock(pDabudai->pLockTrigerNum);

                }
            }

            ++i;
            nOffset += (strlen(sMsg+nOffset+12)+13);
            nOffset = nOffset+(strlen(sMsg+nOffset))+1;
			//---------------------停止任务删除-------------------
		#if 0
			//由于无法确定执行任务的具体机器，只能瞎等，且不保证完整删除。
			//另外，由于需要通过延迟广播时长确保发送机都能接收，因此，这里不执行删除。
		if (g_stoptask_size > 0) {
			char task_strid[MAX_TASKID_LEN] = { 0 };
			snprintf(task_strid, MAX_TASKID_LEN - 1, "%d.%d.%d", ntohl(nnID[0]), ntohl(nnID[1]), ntohl(nnID[2]));	
			pthread_mutex_lock(&g_delivery_lock);
			int nindex = 0;
			for (; nindex < g_stoptask_size; ++nindex) {
				if (g_stop_tasks[nindex]._task_state._state == -1
				    && 0 == strncmp(g_stop_tasks[nindex]._task_state._sid, task_strid, strlen(task_strid))) {
					break;
				}
			}
			if (nindex < g_stoptask_size) {
				char xx_log[128] = {0};
				snprintf(xx_log, sizeof(xx_log), "[DEL]stop task [%s] successfully.\n", task_strid);
			        WriteLog(pConf, pDabudai, xx_log, strlen(xx_log));
				if (nindex < g_stoptask_size - 1) {
					memmove(&g_stop_tasks[nindex], &g_stop_tasks[nindex + 1], (g_stoptask_size - nindex) * sizeof(g_stop_tasks[0]));
				}	
				--g_stoptask_size;
			}
			pthread_mutex_unlock(&g_delivery_lock);
		}
		#endif
			//---------------------------------------------------
        }
        dbc->c_close(dbc);
        dbcNum->c_close(dbcNum);
        
        if (nHeadoffset < nLogOffset)
        {
            if(nLogOffset < sizeof(sLog))	
                snprintf(sLog+nLogOffset, sizeof(sLog)-nLogOffset, "\n");
            nLogOffset += 1;
            if(nLogOffset < sizeof(sLog))
                WriteLog(pConf, pDabudai, sLog, nLogOffset);
            else {
                sLog[sizeof(sLog)-2] = '\n';
                WriteLog(pConf, pDabudai, sLog, sizeof(sLog)-1);
            }
        }

        pthread_rwlock_unlock(pDabudai->pLockRWTask);
	//包体长度
        k = 8;
        k = htonl(k);
        memcpy(sMsg, &k, 4);
	//状态码 
	k = 3;
	k = htonl(k);
	memcpy(sMsg + 4, &k, 4);	
	//内容长度
        k = htonl(0);
        memcpy(sMsg + 8, &k, 4);
        
        k = SendPackage(nSocket, sMsg, 12, pConf->nNetTimeOut);

        if(k)
        {
            snprintf(sLog, sizeof(sLog), "[%s][REPORT]net write fail, error code is:%d.\n", sTag, k);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            //printf("receive fail at line <%d>, error code is:%d!\n", __LINE__, k);
            pthread_mutex_lock(pDabudai->pLockNet);
            BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
            pthread_mutex_unlock(pDabudai->pLockNet);
            close(nSocket);
            free(sMsg);
            return (void*)0;
        }	

    }//command == 3
    else
    {
        snprintf(sLog, sizeof(sLog), "[%s]command num error(%d:%d), will close this connection.\n", sTag, nCommand, ntohl(nCommand));
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        //printf("command num error(%d:%d), at line %d.\n", nCommand, ntohl(nCommand), __LINE__);
        pthread_mutex_lock(pDabudai->pLockNet);
        BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
        pthread_mutex_unlock(pDabudai->pLockNet);
        close(nSocket);
        free(sMsg);
        return (void*)0;
    }

    free(sMsg);
    {
        int nIsClose = 0;

        pthread_mutex_lock(pDabudai->pLockNet);
        //printf("NOW HAS :%d:\n", nEventNum);
        if(sUsedList[nSocket] == 2)
        {
            BeforeCloseSocket(nSocket, nEpollFd, nIpList, sUsedList, pDabudai);
            //printf("close 666666.\n");
            nIsClose = 1;
        }
        else
        {	
            epTmp.data.fd = nSocket;
            epTmp.events = EPOLLIN | EPOLLET;
            sUsedList[nSocket] = 0;
            epoll_ctl(nEpollFd, EPOLL_CTL_MOD, nSocket, &(epTmp));
        }
        pthread_mutex_unlock(pDabudai->pLockNet);
        if(nIsClose)
            close(nSocket);
    }

    //printf("\nSUCCESS!\n");
    return (void*)0;

}


//从任务文件恢复
int RunRecover(CONFIGURE *pConf, DABUDAI *pDabudai)
{
    time_t tBeginTime = time(NULL);

    {
        struct stat sFileStat;
        DIR* pDir;
        struct dirent *d_c;		
		//获取目录下的任务文件
        pDir = opendir(pConf->sTaskFileDir);
        while(d_c = readdir(pDir))
        {
            char sFileName[200];
            struct stat sctFileStat;
            if(strcmp(d_c->d_name, ".") == 0 || strcmp(d_c->d_name, "..") == 0)
                continue;
            snprintf(sFileName, 200, "%s/%s", pConf->sTaskFileDir, d_c->d_name);
            stat(sFileName, &sctFileStat);
            if(S_ISREG(sctFileStat.st_mode) == 0)
                continue;

            {
                DomainAlias *pAlias;
                MailList *pMailList;
                char sID[12];
                char *sModula = NULL;
                pAlias = (DomainAlias*)malloc(sizeof(DomainAlias));	
                memset(pAlias, 0x0, sizeof(DomainAlias));
                ReadDomainAlias(pConf->sAliasFile, pAlias);
                pMailList = (MailList*)malloc(sizeof(MailList) * (pAlias->nDomainNum+1));
                memset(pMailList, 0x0, sizeof(MailList)*(pAlias->nDomainNum+1));	

               printf("begin to read task file <%s>.\n", sFileName);
                {
                    int i = 0;
                    MailList *p = pMailList;
                    while(i < pAlias->nDomainNum + 1)
                    {
                        p->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
                        pthread_mutex_init(p->pLock, NULL);
                        ++i;
                        ++p;	
                    }	
                }
                ReadTaskFileWithLevel(sFileName, pMailList, pAlias, &sModula, sID, pConf, pDabudai, NULL, 0);

                if(pAlias && pMailList && sModula)
                {
                    int kk = 0;
                    pthread_mutex_lock(pDabudai->pLockWater);
                    kk = pDabudai->nDayWaterMode;
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    pthread_rwlock_wrlock(pDabudai->pLockRWTask);
                    AddTaskQueue(&(pDabudai->pBegin), &(pDabudai->pEnd), pAlias, pMailList, sModula, strlen(sModula)+1, 
                        sID, &(pDabudai->nTaskNum), sFileName, kk);
                    pthread_rwlock_unlock(pDabudai->pLockRWTask);
                   printf("add task <%s> into queue successfully.\n", sFileName);
                    if(tBeginTime > sctFileStat.st_mtime)
                    {
                        tBeginTime = sctFileStat.st_mtime;	
                    }
                }
                else
                {
                    int i = 0;
                    while(i < pAlias->nDomainNum+1)
                    {
                        FreeMailList(pMailList+i);
                        ++i;	
                    }
                    free(sModula);
                    free(pAlias);
                    free(pMailList);
                    unlink(sFileName);
                   printf("the format of task file <%s> is wrong.\n", sFileName);

                }
            }
        }
        closedir(pDir);
    }

    {
        int nYear, nMonth, nDay, nHour, nMin, nSec;
        struct tm sctTime;
        char sLogName[100];
        int nLogNum = 0;
        char sLine[4048];
        time_t tNow = time(NULL);
        tBeginTime = tBeginTime - 600;// 即10分钟之前
        //while(tBeginTime < tNow)
        {
            localtime_r(&tBeginTime, &sctTime);
            nYear = sctTime.tm_year+1900;
            nMonth = sctTime.tm_mon+1;
            nDay = sctTime.tm_mday;
            nHour = sctTime.tm_hour;
            nMin = sctTime.tm_min;
            nSec = sctTime.tm_sec;

            while(nLogNum < 9999)
            {
                FILE *pFile;
                int nH = 0, nM = 0, nS = 0;

                snprintf(sLogName, sizeof(sLogName), "%s.%04d%02d%02d.%04d", pConf->sLogName, nYear, nMonth, nDay, nLogNum);
                if(access(sLogName, F_OK))
                    break;
                pFile = fopen(sLogName, "r");
                fgets(sLine, sizeof(sLine), pFile);
                fclose(pFile);
                sscanf(sLine, "[%d:%d:%d]",  &nH, &nM, &nS);//第一行日志显示的时间

                if(nH > nHour || (nH == nHour && nM > nMin)) //日志时间大于开始时间
                {
                    if(nLogNum)
                        nLogNum--;
                    break;
                }
                nLogNum++;
            }
            snprintf(sLogName, sizeof(sLogName), "%s.%04d%02d%02d.%04d", pConf->sLogName, nYear, nMonth, nDay, nLogNum);
            if(access(sLogName, F_OK) )
            {
                if(nLogNum)
                    nLogNum--;	
            }
        }

        while(tBeginTime < tNow+3600*25) //
        {
            while(1)
            {	
                FILE *pFile;
                snprintf(sLogName, sizeof(sLogName), "%s.%04d%02d%02d.%04d", pConf->sLogName, nYear, nMonth, nDay, nLogNum);
                if(access(sLogName, F_OK))
                    break;
               printf("read log %s to recover.\n", sLogName);
                pFile = fopen(sLogName, "r");
                while(fgets(sLine, sizeof(sLine), pFile))
                {
                    char *sTmp, *sTmp2;
                    int nStat;
                    sTmp = strstr(sLine, "][");
                    if(!sTmp)
                        continue;
                    sTmp += 2;
                    if(strncmp(sTmp, "REPORT", 6))
                        continue;	
                    sTmp2 = strchr(sLine, '\r');
                    if(!sTmp2)
                        sTmp2 = strchr(sLine, '\n');
                    if(sTmp2)
                        *sTmp2 = 0;
                    sTmp = strchr(sTmp, ']');
                    if(!sTmp)
                        continue;
                    sTmp++;
                    while(sTmp)
                    {
                        char sID[12];
                        int n[3];

                        if(strncmp(sTmp, "[s]", 3) == 0)
                            nStat = D_MailList_Success;
                        else if(strncmp(sTmp, "[d]", 3) == 0)
                            nStat = D_MailList_Delay;
                        else if(strncmp(sTmp, "[f]", 3) == 0)
                            nStat = D_MailList_Fail;
                        else
                            break;					
                        sTmp += 3;
                        if(sscanf(sTmp, "[%d.%d.%d]", n, n+1, n+2) != 3)
                            break;
                        n[0] = htonl(n[0]);
                        n[1] = htonl(n[1]);
                        n[2] = htonl(n[2]);
                        memcpy(sID, n, 12);
                        sTmp = strchr(sTmp, ']');
                        if(!sTmp)
                            break;
                        sTmp++;
                        sTmp2 = strchr(sTmp, '[');
                        if(sTmp2)
                            *sTmp2 = 0;
                        {
                            char *sTmp3 = strchr(sTmp, '(');
                            char *sTmp4;
                            if(sTmp3)
                                *sTmp3 = 0;
                            sTmp4 = strchr(sTmp3+1, ')');
                            if(sTmp4)
                                *sTmp4 = 0;
                            if(sTmp3 && sTmp4)	
                            {
                                if(strstr(sTmp3+1, "user doesn") || strstr(sTmp3+1, "not exist") || strstr(sTmp3+1, " unknown") || strstr(sTmp3+1, "not found") 
                                    || strstr(sTmp3+1, "such user") || strstr(sTmp3+1, "nvalid recipient") || strstr(sTmp3+1, " Unknown") || 
                                    strstr(sTmp3+1, "unavailable") || strstr(sTmp3+1, "is disable") || strstr(sTmp3+1, "ddress rejected") ||
                                    strstr(sTmp3+1, "such recipient") || strstr(sTmp3+1, "no mailbox") || strstr(sTmp3+1, "ccess denied"))
                                {	
                                    if(strstr(sTmp3+1, ":5"))
                                        nStat = D_MailList_NotFound;
                                }
                                else if(strstr(sTmp3+1, "step 1--"))
                                    nStat = D_MailList_Dns;
                                else if(strstr(sTmp3+1, "Spam") || strstr(sTmp3+1, "spam"))
                                    nStat = D_MailList_Spam;	
                            }
                            ChangeMailboxStatWithLevel(pDabudai->pBegin, pDabudai->pEnd, sID, sTmp, nStat);
                            if(sTmp3)
                                *sTmp3 = '(';
                            if(sTmp4)
                                *sTmp4 = ')';
                        }
                        if(sTmp2)
                            *sTmp2 = '[';
                        sTmp = sTmp2;
                    }
                    //[09:50:03] [192.168.14.151.00338][REPORT][s][123456.123456.1011]backwardvoldemortfell@126.com[s][12345
                    //6.123456.1011]lack1556@126.com
                }
                fclose(pFile);
                nLogNum++;
            }
            
            nLogNum = 0;
            tBeginTime = tBeginTime + 3600*24;
            localtime_r(&tBeginTime, &sctTime);
            nYear = sctTime.tm_year+1900;
            nMonth = sctTime.tm_mon+1;
            nDay = sctTime.tm_mday;
            nHour = sctTime.tm_hour;
            nMin = sctTime.tm_min;
            nSec = sctTime.tm_sec;
        }
    }

//    {
//        TaskQueue *pTask = pDabudai->pBegin;
//        while(pTask)
//        {
//            DomainAlias *pAlias = pTask->pAlias;
//            MailList *pMail = pTask->pMailList;
//            int i = 0;
//            while(i < pAlias->nDomainNum+1)
//            {
//                PrintMailList2((pMail+i)->sMailboxList, (pMail+i)->nMailboxListUsed, (pMail+i)->sOffsetList, (pMail+i)->nOffsetListUsed);
//                ++i;
//            }
//            pTask = pTask->pNext;
//        }
//    }

    {
        TaskQueue *pTask = pDabudai->pBegin;
        while(pTask)
        {
            DomainAlias *pAlias = pTask->pAlias;
            MailList *pMail = pTask->pMailList;
            int i = 0;
            while(i < pAlias->nDomainNum + 1)
            {
                (pMail+i)->nSend = (pMail+i)->nSuccess + (pMail+i)->nDelay + (pMail+i)->nFail;
                ++i;	
            }	
            pTask = pTask->pNext;
        }	
    }


    return 0;
}

//add by weide
int check_inet_addr(const char* cp)
{
    if (!cp)
        return -1;

    int i=0, dot=0;
    char buf[4] = {0,0,0,0};
    while (*cp != '\0')
    {
        if (isdigit(*cp)){
            if (i > 3)
                return -1;
            buf[i++] = *cp;
        }
        else if (*cp == '.'){
            if (++dot > 3)
                return -1;
            if ( i<1 ||atoi(buf) > 255)
                return -1;
            i = buf[1] = buf[2] = 0;
        }
        else{
            return -1;
        }

        cp++;
    }

    return  (dot!=3 || i<1 || atoi(buf)>255) ? -1:0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in sctClientAddr; 
    struct sockaddr_in sctServerAddr; 
    int nListenSocket;  ///监听的句柄
    struct epoll_event epListen, epEvent[1024], epTmp;
    int sockopt_mode = 1;
    int epfd = -1;  ////epoll的文件句柄
    unsigned int *nIpList;  ////存放连接的IP的信息，为0说明该socket没有使用
    char *sUsedList;    ////存放连接的状态的信息，为0，说明不在会话中，为1，在会话中，为2，在会话中，但是会话结束后要被关闭
    int *nTimeList; /////存放连接的上一次使用时间的信息

    int nPollWaitCount = 0; /////epoll轮询的次数，次数足够多，检查一次队列，去除超时的。	
    char sLog[1024];
    int nSessionNum = 0;
    CONFIGURE *pConf;
    CONFIGURE *pConf1 = (CONFIGURE*)malloc(sizeof(CONFIGURE));
    CONFIGURE *pConf2 = (CONFIGURE*)malloc(sizeof(CONFIGURE));
    DABUDAI *pDabudai= (DABUDAI*)malloc(sizeof(DABUDAI));
    char sConfFile[100];
    pConf = pConf1;

    sConfFile[0] = 0;
    {
        int nRecover = 0;
        int nShowVersion = 0;
        int ch;

        while((ch = getopt(argc, argv, "c:vr")) != -1)
        {
            switch(ch)
            {
            case 'c':
                snprintf(sConfFile, 100, "%s", optarg);	
                break;
            case 'v':
                nShowVersion = 1;	
                break;
            case 'r':
                nRecover = 1;
                break;
            }	
        }
        //printf("configure file is:%s\n", sConfFile);
        if(nShowVersion)
        {
           ;//printf("%s\n", _H_Version);
            return 0;	
        }	
        InitialConf(pConf1);
        InitialConf(pConf2);
        if(sConfFile[0])
            ReadDaemonConf(sConfFile, pConf1);
        else
           ;//printf("[warn]no configure file, will run under default configure.\n");
        if(InitialDaBuDai(pDabudai))
            return -1;

        pDabudai->dbList = OpenDB(NULL, NULL, 64, 256, NULL);
        //pDabudai->dbList2 = OpenDB(NULL, NULL, 64, 512, NULL);
        pDabudai->dbNumber = OpenDB(pConf->sTrigerNumberDBFile, NULL, 64, 512, NULL);
        pDabudai->pDomainAlias = (DomainAlias*)malloc(sizeof(DomainAlias));	
        memset(pDabudai->pDomainAlias, 0x0, sizeof(DomainAlias));
        ReadDomainAlias(pConf->sAliasFile, pDabudai->pDomainAlias);


        OpenLogFile(pConf, pDabudai);
        snprintf(sLog, sizeof(sLog), "[SYSTEM]deliver system start!\n");
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        if(nRecover)
        {
            snprintf(sLog, sizeof(sLog), "[SYSTEM]maybe quit abnormally, will recover according to the task file and log.\n");	
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            RunRecover(pConf, pDabudai);
            snprintf(sLog, sizeof(sLog), "[SYSTEM]recover completely, has %d tasks in list.\n", pDabudai->nTaskNum);	
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            fflush(NULL);
        }
    }


    nIpList = (unsigned int*)malloc(sizeof(int)*(pConf->nPoolMaxNum + 30));
    memset(nIpList, 0x0, (pConf->nPoolMaxNum + 30)*sizeof(int));
    sUsedList = (char*)malloc((pConf->nPoolMaxNum + 30));
    memset(sUsedList, 0x0, (pConf->nPoolMaxNum + 30));
    nTimeList = (int*)malloc((pConf->nPoolMaxNum + 30)*sizeof(int));
    memset(nTimeList, 0x0, sizeof(int)*(pConf->nPoolMaxNum + 30));

    memcpy(pDabudai->sDayWaterFrom, pConf-> sDayWaterFrom, sizeof(pConf->sDayWaterFrom));

    /***忽略信号，不然children跟pipe的信号会打断通讯，pipe甚至会导致进程退出**/
    {	
        int nPid = fork();
        if(nPid < 0)
        {
           ;//printf("creat daemon fail(%s)!\n", strerror(errno));
            return -1;	
        }
        if(nPid > 0)
            return 0;
        setsid();
        signal(SIGINT, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        signal(SIGUSR2, SIG_IGN);
        umask(0);
        setpgrp();
    }
    /***END   忽略信号**********************/

    {
        char s[3];
        pDabudai->nCacheSocket = -1;
        //pDabudai->nCacheSocket = TcpConnect(pConf->sCacheIp, pConf->nCachePort, NULL, 5, s, 3);
    }

    mtrace();

    epfd = epoll_create(pConf->nPoolMaxNum);


    /*********绑定IP端口，设置为非阻塞*************/
    memset(&sctServerAddr,0x0,sizeof(sctServerAddr));
    sctServerAddr.sin_addr.s_addr = inet_addr(pConf->sBindIp);
    sctServerAddr.sin_port = htons(pConf->nBindPort);
    nListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&sockopt_mode, sizeof(sockopt_mode));
    sockopt_mode = 1;
    ioctl(nListenSocket, FIONBIO, &sockopt_mode);
    if( check_inet_addr(pConf->sBindIp) != 0 || bind(nListenSocket, (struct sockaddr *) &sctServerAddr, sizeof(sctServerAddr)) < 0)
    {
        snprintf(sLog, sizeof(sLog), "[SYSTEM][ERROR]can't bind, ip:%s, port:%d.\n", pConf->sBindIp, pConf->nBindPort);
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
       ;//printf("can't bind the socket!\n");
        return -1;
    }
    /*****END   绑定IP端口*************/
    snprintf(sLog, sizeof(sLog), "[SYSTEM]bind ip:%s, port:%d.\n", pConf->sBindIp, pConf->nBindPort);
    WriteLog(pConf, pDabudai, sLog, strlen(sLog));

    epListen.data.fd = nListenSocket; 
    epListen.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, nListenSocket, &epListen); 
    pDabudai->nEventNum = 1;

    if( listen(nListenSocket, 100) < 0)
    {
       ;//printf("can't listen the socket(%s)!\n", strerror(errno));
        snprintf(sLog, sizeof(sLog), "[SYSTEM][ERROR]can't listen the socket.\n");
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        return -3;
    }


    while(1)
    {
        int nFdNum = 0;
        int i = 0;
        time_t tNow = time(NULL);

        nPollWaitCount++;
        fflush(NULL);
		//定时报告状态
        {
            if(tNow - pDabudai->tReportCacheTime > pConf->nCachePeriod)
            {
                yx_reload_list(pConf->yxfile,pConf, pDabudai); //add by wEide 2014/05/23

                char *sArg = (char*)malloc(sizeof(CONFIGURE*)+sizeof(DABUDAI*));
                pthread_t nThreadId;
                pDabudai->tReportCacheTime = tNow;
                memcpy(sArg, &pConf, sizeof(CONFIGURE*));
                memcpy(sArg+sizeof(CONFIGURE*), &pDabudai, sizeof(DABUDAI*));
				//cmd = 9 -> cache
                if(pthread_create(&nThreadId, NULL, Thread_Report9, sArg) != 0)
                {
                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to send report.\n");
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    free(sArg);
                }
                sArg = (char*)malloc(sizeof(CONFIGURE*)+sizeof(DABUDAI*));
                memcpy(sArg, &pConf, sizeof(CONFIGURE*));
                memcpy(sArg+sizeof(CONFIGURE*), &pDabudai, sizeof(DABUDAI*));
				//cmd = 9 -> cache
                if(pthread_create(&nThreadId, NULL, Thread_ReportTriger, sArg) != 0)
                {
                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to send report.\n");
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    free(sArg);
                }
                sArg = (char*)malloc(sizeof(CONFIGURE*)+sizeof(DABUDAI*));
                memcpy(sArg, &pConf, sizeof(CONFIGURE*));
                memcpy(sArg+sizeof(CONFIGURE*), &pDabudai, sizeof(DABUDAI*));
				//从db 中获取数据，cmd = 8 -> cache
                if(pthread_create(&nThreadId, NULL, Thread_ReportTriger2, sArg) != 0)
                {
                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to send report.\n");
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    free(sArg);
                }
                //pthread_mutex_lock(pDabudai->pLockCache2);
                //if(pDabudai->nFlagCache2 == 0)
                {
                    pDabudai->nFlagCache2 = 1;
                    sArg = (char*)malloc(sizeof(CONFIGURE*)+sizeof(DABUDAI*));
                    memcpy(sArg, &pConf, sizeof(CONFIGURE*));
                    memcpy(sArg+sizeof(CONFIGURE*), &pDabudai, sizeof(DABUDAI*));
					//cmd = 8 -> cache
                    if(pthread_create(&nThreadId, NULL, Thread_Report8, sArg) != 0)
                    {
                        snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to send report2.\n");
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                        free(sArg);
                        pDabudai->nFlagCache2 = 0;
                    }
                }
                //pthread_mutex_unlock(pDabudai->pLockCache2);
            }	
        }

        if(nPollWaitCount >= pConf->nCheckPeriod)				////////轮询次数已经很多次，检查连接，把超时的close
        {
            int j = pDabudai->nMinNetSocket;
            struct tm sctTime;

            if(sConfFile[0])
            {
                struct stat sctFile;
                stat(sConfFile, &sctFile);
                if(sctFile.st_mtime > pConf->tReadTime)
                {
                    snprintf(sLog, sizeof(sLog), "[SYSTEM]detect conf file change, will read again.\n");
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    //printf("BEFORE\n");
                    //ShowDaemonConf(pConf);
                    if(pConf == pConf1)
                    {
                        ReadDaemonConf(sConfFile, pConf2);
                        pConf = pConf2;	
                    }
                    else
                    {
                        ReadDaemonConf(sConfFile, pConf1);
                        pConf = pConf1;
                    }	
                    //printf("AFTER\n");
                    //ShowDaemonConf(pConf);
                }	
            }

            localtime_r(&tNow, &sctTime);
            if(pDabudai->nDay != sctTime.tm_mday )
            {
                pthread_mutex_lock(pDabudai->pLockWater);
                pDabudai->nWaterRound = 0;
                pDabudai->nWaterOffset = 0;
                pDabudai->nWaterFinishSum = 0;
                pDabudai->nQQDaySend = 0;
                pDabudai->nDayWaterFlag = 0;
                pthread_mutex_unlock(pDabudai->pLockWater);
                pthread_mutex_lock(pDabudai->pLockTrigerNum);
                DelOldTrigerNum(pConf, pDabudai);
                pthread_mutex_unlock(pDabudai->pLockTrigerNum);
            }	
            pthread_mutex_lock(pDabudai->pLockLog);
            if(pDabudai->nDay != sctTime.tm_mday )
            {
                pDabudai->nDay = sctTime.tm_mday;
                pDabudai->nMonth = sctTime.tm_mon+1;
                pDabudai->nYear = sctTime.tm_year+1900;
                pDabudai->nLogNum = 0;
                fclose(pDabudai->pFileLog);
                OpenLogFile(pConf, pDabudai);	

            }
            else if(pDabudai->nLogSize > pConf->nMaxLogSize)	
            {
                fclose(pDabudai->pFileLog);
                OpenLogFile(pConf, pDabudai);
            }
            pthread_mutex_unlock(pDabudai->pLockLog);

            pthread_mutex_lock(pDabudai->pLockWater);
            //printf("\nbbbbbbbbbbb WaterMode:%d.*******************\n", pDabudai->nDayWaterMode);
            if(pDabudai->nDayWaterMode == 0 && (sctTime.tm_hour > pConf->nDayWaterHour || (sctTime.tm_hour == pConf->nDayWaterHour && sctTime.tm_min >= pConf->nDayWaterMin)))
            {	
                pDabudai->nDayWaterMode = 1;
                if(pDabudai->nDayWaterFlag == 0)
                    pDabudai->nDayWaterFlag = 1;
            }
            //printf("\nbbbbbaaaaa WaterMode:%d.***%d,%d,%d,%d****************\n", pDabudai->nDayWaterMode, sctTime.tm_hour, sctTime.tm_min, pConf->nDayWaterHour, pConf->nDayWaterMin);
            //if(pDabudai->nDayWaterMode == 1 && (sctTime.tm_hour > pConf->nDelayHour || (sctTime.tm_hour == pConf->nDelayHour && sctTime.tm_min >= pConf->nDelayMin)))
            //pDabudai->nDayWaterMode = 0;
            if(pDabudai->nDayWaterMode)
            {
                if(sctTime.tm_hour < pConf->nDayWaterHour)
                {
                    if(sctTime.tm_hour + 24 - pConf->nDayWaterHour >= pConf->nDelayHour2)
                        pDabudai->nDayWaterMode = 0;	
                }	
                else if(sctTime.tm_hour - pConf->nDayWaterHour >= pConf->nDelayHour2)
                    pDabudai->nDayWaterMode = 0;

            }
            //printf("\naaaaaa WaterMode:%d.***********%d********\n", pDabudai->nDayWaterMode, pConf->nDelayHour2);
            pthread_mutex_unlock(pDabudai->pLockWater);

            if(pDabudai->nDayWaterFlag == 1)
            {
                int kk = 0, kkk = 1;
                pDabudai->nDayWaterFlag = 2;
                pthread_mutex_lock(pDabudai->pLockWater);
               ;//printf("\nADDDDDDDDDDDD  %d, %d, %d.\n", pDabudai->nQQDaySend, pConf->nDayWaterLine, pConf->nDayWaterTotal);
                kk = pDabudai->nQQDaySend;
                pthread_mutex_unlock(pDabudai->pLockWater);

                pthread_rwlock_rdlock(pDabudai->pLockRWTask);
                kkk = pDabudai->nTaskNum;
                pthread_rwlock_unlock(pDabudai->pLockRWTask);

                if((kk >= pConf->nDayWaterLine || pConf->nDayWaterLine < 100) &&  kk < pConf->nDayWaterTotal)
                {
                    char s[600];
                    int i = 0, nOffset = 0;

                    snprintf(sLog, sizeof(sLog), "[WATER]qq send %d, over the line(%d).\n", kk, pConf->nDayWaterLine);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));

                    kk = pConf->nDayWaterTotal - kk;
                    snprintf(s+nOffset, 100, "$NUM$%d", kk);
                    nOffset += strlen(s+nOffset);
                    snprintf(s+nOffset, 100, "$PIPE$%s", pConf->sPipe);
                    nOffset += strlen(s+nOffset);
                    pthread_mutex_lock(pDabudai->pLockWater);
                    while(i < 4)
                    {
                        //snprintf(s+nOffset, 100, "$FROM%d$%s", i, pDabudai->sDayWaterFrom+50*i);
                        snprintf(s+nOffset, 100, "$FROM%d$%s", i, pDabudai->sDayWaterFrom+100*i);
                        nOffset += strlen(s+nOffset);
                        i++;	
                    }
                    snprintf(s+nOffset, 3, "\r\n");	
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    //printf("\nWATER******%s********\n", s);
                    snprintf(sLog, sizeof(sLog), "[WATER]send day water pipe:%s.\n", s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    kk = TcpConnect(pConf->sDayWaterIp, pConf->nDayWaterPort, NULL, 2, NULL, 0);
                    if(kk >= 0)
                    {
                        SocketWrite(kk, 2, s, strlen(s));
                        SocketReadLine(kk, 1, s, 100);
                        close(kk);
                    }
                }
                else
                {
                    snprintf(sLog, sizeof(sLog), "[WATER]qq send %d, not over the line(%d).\n", kk, pConf->nDayWaterLine);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                }
            }
            if(pDabudai->nDayWaterFlag == 2 && (sctTime.tm_hour > pConf->nDayWaterHour2 || (sctTime.tm_hour == pConf->nDayWaterHour2 && sctTime.tm_min >= pConf->nDayWaterMin2) ))
            {
                pDabudai->nDayWaterFlag = 3;	
            }

            nPollWaitCount = 0;
            pthread_mutex_lock(pDabudai->pLockNet);
            while(j <= pDabudai->nMaxNetSocket)
            {
                if(nIpList[j] && sUsedList[j] == 0 && tNow-nTimeList[j] > pConf->nKeepAliveTimeOut)
                {
                    unsigned char *s = (char*)(nIpList+j);
                    snprintf(sLog, sizeof(sLog), "[SYSTEM]ip <%d.%d.%d.%d> has no data transfer in %d seconds, will be close.\n", 
                        s[0], s[1], s[2], s[3], pConf->nKeepAliveTimeOut);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    BeforeCloseSocket(j, epfd, nIpList, sUsedList, pDabudai);
                    close(j);
                    //printf("close time out!!!!!\n");
                }
                ++j;	
            }	
            pthread_mutex_unlock(pDabudai->pLockNet);
            {
                int i = 0;
                time_t tNowNow;
                tNowNow = time(NULL);
                ////////////////DEBUG
                TaskQueue *p = pDabudai->pBegin;
                pthread_rwlock_wrlock(pDabudai->pLockRWTask);
                //printf("TTTTTTTTAAAAASK:%d:", pDabudai->nTaskNum);
                if (pDabudai->nTaskNum > 0) {
			snprintf(sLog, sizeof(sLog), "[SYSTEM]now has %d tasks in queue.\n", pDabudai->nTaskNum);
                	WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		}
                while(i < pDabudai->nTaskNum && p)
                {
                    int *iii = (int*)(p->sID);
                    int nSuccess = 0, nFail = 0, nDelay = 0;
                    {
                        int jj = 0;
                        while(jj < (p->pAlias)->nDomainNum + 1)
                        {
                            nSuccess += ((p->pMailList + jj)->nSuccess);
                            nDelay += ((p->pMailList + jj)->nDelay);
                            nFail += ((p->pMailList + jj)->nFail);
                            ++jj;	
                        }	
                    }
                    //printf("TTT[%d]<%d.%d.%d>[T:%d,S:%d,F:%d,D:%d],", i, ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]), 
                    //p->nTotalCount, nSuccess, nFail, nDelay);
                    ++i;

                    p = p->pNext;	
                }
                //printf("\n");
                //////////////DEBUG
                p = pDabudai->pBegin;
                i = 0;
                while(p)
                {
                    int nDelete = 1;
                    MailList *pM = p->pMailList;
                    DomainAlias *pA = p->pAlias;
                    int jj = 0;
                    int nMode = 0;
                    pthread_mutex_lock(pDabudai->pLockWater);
                    nMode = pDabudai->nDayWaterMode;
                    pthread_mutex_unlock(pDabudai->pLockWater);
                    if(p->tPauseTime == 10 && nMode == 0)
                    {
                        int *iii = (int*)(p->sID);
                        p->tPauseTime = 0;
                        snprintf(sLog, sizeof(sLog), "[SYSTEM]task <%d.%d.%d> will be restart, for the delay time is over.\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    }
                    if(0 && pDabudai->nDayWaterFlag == 3)
                    {
                        int *iii = (int*)(p->sID);
                        if(ntohl(iii[0]) < 100 && ntohl(iii[0]) > 0)
                        {
                            p->tFinishTime = time(NULL);	
                            snprintf(sLog, sizeof(sLog), "[SYSTEM]water task <%d.%d.%d> be stop, for the end time come.\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                            {
                                char ssss[200];
                                char *sTmp = strrchr(p->sFileName, '/');
                                int k = 0;
                                ssss[0] = 0;
                                if(sTmp && access(p->sFileName, F_OK) == 0)
                                {
                                    snprintf(ssss, 200, "%s/%s", pConf->sTaskBakDir, sTmp+1);
                                    pthread_mutex_lock(pDabudai->pLockFile);
                                    k = rename(p->sFileName, ssss);	
                                    pthread_mutex_unlock(pDabudai->pLockFile);
                                }
                                else
                                    k = 1;
                                if(k)
                                {
                                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]move task file %s to file %s fail.\n", p->sFileName, ssss);
                                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                                }
                                else
                                {
                                    snprintf(sLog, sizeof(sLog), "[SYSTEM]move task file %s to file %s.\n", p->sFileName, ssss);
                                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                                }		
                            }
                        }	
                    }
                    if(p->tFinishTime && p->tFinishTime + pConf->nTaskRemoveTime < tNowNow)
                    {
                        int *iii = (int*)(p->sID);
                        int kkk = 0;
                        //printf("TTTTTTTTAAASSdelete<%d.%d.%d>\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                        snprintf(sLog, sizeof(sLog), "[SYSTEM]delete task <%d.%d.%d> from queue.\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        pthread_mutex_lock(pDabudai->pLockWater);
                        kkk = pDabudai->nWaterFinishSum;
                        pthread_mutex_unlock(pDabudai->pLockWater);
                        DelOneTask2(&(pDabudai->pBegin), &(pDabudai->pEnd), p, &(pDabudai->nTaskNum), &kkk);
                        pthread_mutex_lock(pDabudai->pLockWater);
                        pDabudai->nWaterFinishSum = kkk;
                        pthread_mutex_unlock(pDabudai->pLockWater);
                        p = pDabudai->pBegin;
                    }	
                    else if(p->tFinishTime == 0)
                    {		
                        while(jj < pA->nDomainNum + 1)
                        {
                            //printf("rrrrr<%d>", pM->nRound);
                            if(pM->nRound < pConf->nRoundNum && pM->nCount)
                                nDelete = 0;
                            pM++;
                            jj++;	
                        } 
                        if(nDelete == 0)
                            p = p->pNext;
                        else
                        {
                            int *iii = (int*)(p->sID);
                            //printf("TTTTTTTTTFFFFINISH<%d.%d.%d>\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                            snprintf(sLog, sizeof(sLog), "[SYSTEM]task <%d.%d.%d> has finished.\n", ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                            p->tFinishTime = tNowNow;
                            {
                                char ssss[200];
                                char *sTmp = strrchr(p->sFileName, '/');
                                int k = 0;
                                ssss[0] = 0;
                                if(sTmp && access(p->sFileName, F_OK) == 0)
                                {
                                    snprintf(ssss, 200, "%s/%s", pConf->sTaskBakDir, sTmp+1);
                                    pthread_mutex_lock(pDabudai->pLockFile);
                                    k = rename(p->sFileName, ssss);	
                                    pthread_mutex_unlock(pDabudai->pLockFile);
                                }
                                else
                                    k = 1;
                                if(k)
                                {
                                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]move task file %s to file %s fail.\n", p->sFileName, ssss);
                                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                                }
                                else
                                {
                                    snprintf(sLog, sizeof(sLog), "[SYSTEM]move task file %s to file %s.\n", p->sFileName, ssss);
                                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                                }	
                            }	
                            p = p->pNext;
                        }
                    }
                    else
                        p = p->pNext;	
                }
                ////////////////DEBUG
                p = pDabudai->pBegin;
                i = 0;
                //printf("TTTTTTTTAAAAASK:%d:", pDabudai->nTaskNum);
                while(i < pDabudai->nTaskNum && p)
                {
                    int *iii = (int*)(p->sID);
                    //printf("TTT[%d]<%d.%d.%d>", i, ntohl(iii[0]), ntohl(iii[1]), ntohl(iii[2]));
                    ++i;
                    p = p->pNext;	
                }
                //printf("\n");
                //////////////DEBUG
                pthread_rwlock_unlock(pDabudai->pLockRWTask);
            }
            if(pDabudai->nDayWaterFlag == 3)
                pDabudai->nDayWaterFlag = 4;	
        }
        //sleep(1);	
        nFdNum = epoll_wait(epfd, epEvent, pConf->nPoolMaxNum, 1000);

        while(i < nFdNum)
        {
            if(epEvent[i].data.fd == nListenSocket)						////////有新连接
            {
                while(1)
                {
                    int nConnectSocket;
                    int nTmp = sizeof(sctClientAddr);

                    nConnectSocket = accept(nListenSocket, (struct sockaddr *)&sctClientAddr, &nTmp);	
                    if(nConnectSocket < 0)
                    {
                        break;
                    }
                    else if(pDabudai->nEventNum < pConf->nPoolMaxNum && nConnectSocket < 1020)
                    {
                        int nSum = 0;
                        nTmp = 1;
                        ioctl(nConnectSocket, FIONBIO, &nTmp);

                        epTmp.data.fd = nConnectSocket; 
                        epTmp.events = EPOLLIN | EPOLLET;

                        pthread_mutex_lock(pDabudai->pLockNet);
                        epoll_ctl(epfd, EPOLL_CTL_ADD, nConnectSocket, &epTmp); 
                        (pDabudai->nEventNum)++;
                        if(nConnectSocket > pDabudai->nMaxNetSocket)
                            pDabudai->nMaxNetSocket = nConnectSocket;
                        if(nConnectSocket < pDabudai->nMinNetSocket)
                            pDabudai->nMinNetSocket = nConnectSocket;
                        nIpList[nConnectSocket] = sctClientAddr.sin_addr.s_addr;
                        nTimeList[nConnectSocket] = tNow;
                        /*
                        {															//////////一个IP允许保持一个长连接，故有新连接时检查是否已经有连接，close掉旧的
                        int i = nConnectSocket-1;
                        while(i >= pDabudai->nMinNetSocket)
                        {
                        if(nIpList[i] == nIpList[nConnectSocket])
                        {
                        if(sUsedList[i] == 0)
                        {
                        unsigned char *s = (unsigned char*)(nIpList+i);
                        //printf("delete old connection 111111111.\n");	
                        snprintf(sLog, sizeof(sLog), "[SYSTEM]ip <%d.%d.%d.%d> has two connections, close the old one.\n",
                        s[0], s[1], s[2], s[3]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        BeforeCloseSocket(i, nEpollFd, nIpList, sUsedList, pDabudai);
                        close(i);
                        }
                        else			//////该连接现在正在处理会话中，不关闭，做记号，等处理会话的线程退出时关闭连接
                        {
                        sUsedList[i] = 2;	
                        }
                        }
                        --i;	
                        }
                        i = nConnectSocket+1;
                        while(i <= pDabudai->nMaxNetSocket)
                        {
                        if(nIpList[i] == nIpList[nConnectSocket])
                        {
                        if(sUsedList[i] == 0)
                        {
                        unsigned char *s;
                        s = (unsigned char*)(nIpList+i);
                        //printf("delete old connection 222222222.\n");	
                        snprintf(sLog, sizeof(sLog), "[SYSTEM]ip <%d.%d.%d.%d> has two connections, will close the old one.\n",
                        s[0], s[1], s[2], s[3]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        BeforeCloseSocket(i, nEpollFd, nIpList, sUsedList, pDabudai);
                        close(i);
                        }
                        else			//////该连接现在正在处理会话中，不关闭，做记号，等处理会话的线程退出时关闭连接
                        {
                        sUsedList[i] = 2;	
                        }
                        }
                        ++i;
                        }	
                        }*/																		//////////
                        {																		//////////print debug message
                            unsigned char s[4];
                            unsigned char *ss = (unsigned char*)(nIpList+nConnectSocket);
                            s[0] = ((nIpList[nConnectSocket]) & 0xff000000) >> 24;
                            s[1] = ((nIpList[nConnectSocket]) & 0x00ff0000) >> 16;
                            s[2] = ((nIpList[nConnectSocket]) & 0x0000ff00) >> 8;
                            s[3] = (nIpList[nConnectSocket]) & 0x000000ff;
                            //printf("new session:<%d>[%d][%d.%d.%d.%d]\n", pDabudai->nEventNum, nSum, s[3], s[2], s[1], s[0]);
                            snprintf(sLog, sizeof(sLog), "[SYSTEM]new connection from ip <%d.%d.%d.%d>.\n",
                                ss[0], ss[1], ss[2], ss[3]);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        }
                        //nIpList[nConnectSocket] = sctClientAddr.sin_addr.s_addr;
                        pthread_mutex_unlock(pDabudai->pLockNet);
                    }
                    else																																			////连接已经太多，不能再加了。默认一个进程打开1024个句柄
                    {
                        snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]has too many connections(max:%d), close the new.\n",
                            pConf->nPoolMaxNum);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        //printf("too many connect. close the new.\n");
                        close(nConnectSocket);	
                    }
                }	
            }
            else if((epEvent[i].events) & EPOLLIN) 																			/////旧的连接有数据可以读取
            {
	//	fprintf(stderr, "in here....\n");
                int nSocket = epEvent[i].data.fd;
                char *sArg = (char*)malloc(8+sizeof(int*)+3*sizeof(char*)+sizeof(int));
                pthread_t nThreadId;
                epEvent[i].events = 0;
                pthread_mutex_lock(pDabudai->pLockNet);
                epoll_ctl(epfd, EPOLL_CTL_MOD, nSocket, &(epEvent[i]));
                if(sUsedList[nSocket] == 0)
                    sUsedList[nSocket] = 1;
                pthread_mutex_unlock(pDabudai->pLockNet);
                nTimeList[nSocket] = tNow;																						////修改该连接的使用时间
                nSessionNum = (nSessionNum+1)%10000;
                memcpy(sArg, &epfd, 4);
                memcpy(sArg+4, &nSocket, 4);
                memcpy(sArg+8, &nIpList, sizeof(int*));
                memcpy(sArg+8+sizeof(int*), &sUsedList, sizeof(char*));
                memcpy(sArg+8+2*sizeof(int*), &pConf, sizeof(char*));
                memcpy(sArg+8+3*sizeof(int*), &pDabudai, sizeof(char*));
                memcpy(sArg+8+4*sizeof(int*), &nSessionNum, sizeof(int));
                if(pthread_create(&nThreadId, NULL, Thread_Session, sArg) != 0)
                {																																					/////如果线程创建失败，close掉socket
                    unsigned char *s = (unsigned char *)(nIpList+nSocket);
                    free(sArg);
                    //printf("can't gen thread!\n");
                    snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to treat the quest from:<%d.%d.%d.%d>.\n",
                        s[0], s[1], s[2], s[3] );
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));

                    pthread_mutex_lock(pDabudai->pLockNet);
                    BeforeCloseSocket(nSocket, epfd, nIpList, sUsedList, pDabudai);
                    pthread_mutex_unlock(pDabudai->pLockNet);
                    close(nSocket);
                }
            }

            ++i;	
        }

    }

    sleep(10);

    epTmp.data.fd = nListenSocket; 
    epoll_ctl(epfd, EPOLL_CTL_DEL, nListenSocket, &(epTmp));

    close(nListenSocket);
    close(epfd);
    free(nIpList);
    {
        TaskQueue *pp = pDabudai->pBegin;
        while(pp)
        {
            TaskQueue *ppp = pp->pNext;
            int kkk;
            pthread_mutex_lock(pDabudai->pLockWater);
            kkk = pDabudai->nWaterFinishSum;
            pthread_mutex_unlock(pDabudai->pLockWater);
            pthread_rwlock_wrlock(pDabudai->pLockRWTask);
            DelOneTask2(&(pDabudai->pBegin), &(pDabudai->pEnd), pp, &(pDabudai->nTaskNum), &kkk);
            pthread_rwlock_unlock(pDabudai->pLockRWTask);
            pthread_mutex_lock(pDabudai->pLockWater);
            pDabudai->nWaterFinishSum = kkk;
            pthread_mutex_unlock(pDabudai->pLockWater);

            pp = ppp;	
        }	
    }
    free(sUsedList);
    free(nTimeList);
    free(pConf1);
    free(pConf2);
    FreeDaBuDai(pDabudai);
    free(pDabudai);
    return 0;

}
