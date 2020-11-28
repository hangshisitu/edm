#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifndef _H_daemon_conf
#define _H_daemon_conf

typedef struct _CONFIGURE
{
	char sBindIp[20];
	int nBindPort;
	char sBackIp[20];
	int nBackPort;
	
	char sLogName[100];
	int nMaxLogSize;
	char sTaskFileDir[200];
	char sTaskBakDir[200];
	char sCallBackIp[20];
	int nCallBackPort;
	char sCallBackUrl[200];
	char sWebBaseDir[200];
	time_t tReadTime;
	char sReplyTo[100];
	char sReturnPath[100];
	
	int nPipeNum;
	char sPipeName[50][50];
	char sPipeIp[50][20];
	int nPipePort[50];
	
	char sAddAdDomain[1024];
	int nAddAdDomainOffset;
	
}CONFIGURE;


typedef struct _DABUDAI
{
	int nDay;
	int nMonth;
	int nYear;
	FILE *pFileLog;
	pthread_mutex_t *pLockLog;
	int nLogSize;
	int nLogNum;
	
}DABUDAI;

inline void EndLine(char *sLine);

int TreatConfLine(char *sLine, char **sTmp);

int ShowConf(CONFIGURE *pConf);

int ReadConf(char *sFileName, CONFIGURE *pConf);

int InitialConf(CONFIGURE *pConf);



#endif
