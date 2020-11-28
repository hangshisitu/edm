#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <db.h>

#ifndef _H_daemon_conf
#define _H_daemon_conf

typedef struct _CONFIGURE
{
	char sBindIp[20];
	int nBindPort;
		
	char sScanDir[100];
	char sLogName[100];
	int nMaxLogSize;
	char sBackupDir[100];
	
	char sIpAreaFile[100];
	
	char sClientSoftFile[100];
	
	int nOmitCorp[10];
	
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
	DB *dbIpArea;
	
	char sOID[50][20];
	char sLID[50][20];
	char sBID[50][20];
	
}DABUDAI;

inline void EndLine(char *sLine);

int TreatConfLine(char *sLine, char **sTmp);

int ShowConf(CONFIGURE *pConf);

int ReadConf(char *sFileName, CONFIGURE *pConf);

int InitialConf(CONFIGURE *pConf);



#endif
