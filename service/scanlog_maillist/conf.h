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
	
	int nScanNum;	
	char sLogDir[50][100];
	char sTaskDir[50][100];
	char sBackupDir[50][100];
	
	char sLogName[100];
	int nMaxLogSize;
	char sSaveDir[100];
	
	char sIpAreaFile[100];
	
	
	
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
	
}DABUDAI;

inline void EndLine(char *sLine);

int TreatConfLine(char *sLine, char **sTmp);

int ShowConf(CONFIGURE *pConf);

int ReadConf(char *sFileName, CONFIGURE *pConf);

int InitialConf(CONFIGURE *pConf);



#endif
