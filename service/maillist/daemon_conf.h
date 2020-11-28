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
	
	int nNetTimeOut;
	int nKeepAliveTimeOut;
	int nCheckPeriod;
	
	char sBaseDir[100];
	char sLogName[100];
	char sTaskFileDir[100];
	char sHostName[100];
	char sAliasFile[100];
	char sTaskBakDir[100];
	
	int nTaskRemoveTime;
	int nPoolMaxNum;
	
	int nMaxLogSize;
	
	int nCachePort;
	char sCacheIp[20];
	int nCachePeriod;
	
	int nRoundNum;
	time_t tReadTime;
	
	char sWaterListFile[100];
	int nWaterLine;
	int nWaterRound;
	int nWaterAdd;
	int nWaterNum;
	
	char sDayWaterIp[20];
	int nDayWaterPort;
	int nDayWaterHour;
	int nDayWaterMin;
	int nDayWaterTotal;
	int nDayWaterLine;
	int nDayWaterHour2;
	//int nDayWaterLine;
	int nDayWaterMin2;
	char sDayWaterFrom[513];
	int nDayWaterFrom;
	int nDelayHour;
	int nDelayMin;
	int nDelayHour2;
	
	char sPipe[100];
	
	char sCheckMailBoxIp[20];
	int nCheckMailBoxPort;
	
	int nActiveScore;
	
	char sTrigerNumberDBFile[100];
	int nTrigerDeliverNum;
	int nTrigerRetryTime;
	int nTrigerRetryPeriod;
	
	int nRoundPeriod;
	
	//add by D.Wei 2013/12/05
	int enable_water; //1 = yes, 0 = no
    char yxfile[256];
    int yxlevel;
	
}CONFIGURE;

inline void EndLine(char *sLine);

int TreatConfLine(char *sLine, char **sTmp);

int ShowDaemonConf(CONFIGURE *pConf);

int ReadDaemonConf(char *sFileName, CONFIGURE *pConf);

int InitialConf(CONFIGURE *pConf);



#endif
