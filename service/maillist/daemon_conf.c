#include "daemon_conf.h"

inline void EndLine(char *sLine) 
{
	char *sTmp = strchr(sLine, '\r');
	if(!sTmp)
		sTmp = strchr(sLine, '\n');
	if(sTmp)
		*sTmp = 0;
}


int TreatConfLine(char *sLine, char **sTmp)
{
	char *sTmp2;
	EndLine(sLine);
	sTmp2 = strchr(sLine, '#');
	if(sTmp2)
		*sTmp2 = 0;
	*sTmp = strchr(sLine, '=');
	if(!(*sTmp))
		return -1;
	**sTmp = 0;
	sTmp2 = (*sTmp)-1;
	while(sTmp2 >= sLine)
	{
		if(*sTmp2 == ' ' || *sTmp2 == '\t')
		{
			*sTmp2 = 0;
			sTmp2--;
		}
		else
			break;	
	}
	(*sTmp)++;
	*sTmp = strchr(*sTmp, '\"');
	if(!(*sTmp))
		return -1;
	(*sTmp)++;
	sTmp2 = strchr(*sTmp, '\"');
	if(!sTmp2)
		return -1;
	*sTmp2 = 0;
	
	return 0;
	
}


int InitialConf(CONFIGURE *pConf)
{
	memset(pConf, 0, sizeof(CONFIGURE));
	snprintf(pConf->sBindIp, sizeof(pConf->sBindIp), "0.0.0.0");
	pConf->nBindPort = 5566;
	pConf->nNetTimeOut = 10;
	snprintf(pConf->sBaseDir, sizeof(pConf->sBaseDir), "/opt/iedm");
	snprintf(pConf->sLogName, sizeof(pConf->sLogName), "log/deliver.log.");
	snprintf(pConf->sTaskFileDir, sizeof(pConf->sTaskFileDir), "task");
	snprintf(pConf->sHostName, sizeof(pConf->sHostName), "test");
	snprintf(pConf->sAliasFile, sizeof(pConf->sAliasFile), "alias.txt");
	pConf->nKeepAliveTimeOut = 300;
	pConf->nCheckPeriod = 300;
	pConf->nTaskRemoveTime = 600;
	pConf->nPoolMaxNum = 1000;
	pConf->nMaxLogSize = 1024*1024*100;
	pConf->nCachePort = 5880;
	snprintf(pConf->sCacheIp, sizeof(pConf->sCacheIp), "127.0.0.1");
	pConf->nCachePeriod = 60;
	snprintf(pConf->sTaskBakDir, sizeof(pConf->sTaskBakDir), "backup");
	pConf->nRoundNum = 2;
	snprintf(pConf->sWaterListFile, 100, "waterlist.txt");
	pConf->nWaterLine = 50000;
	pConf->nWaterRound = 10;
	pConf->nWaterAdd = 10000;
	pConf->nWaterNum = 10;
	
	snprintf(pConf->sDayWaterIp, 20, "127.0.0.1");
	pConf->nDayWaterPort = 55659;
	pConf->nDayWaterHour = 22;
	pConf->nDayWaterMin = 30;
	pConf->nDayWaterHour2 = 23;
	pConf->nDayWaterMin2 = 40;
	pConf->nDayWaterTotal = 50000;
	pConf->nDayWaterLine = 15000;
	pConf->nDelayHour = 7;
	pConf->nDelayMin = 0;
	pConf->nDelayHour2 = 6;
	
	pConf->nActiveScore = 5;
	snprintf(pConf->sTrigerNumberDBFile, 100, "trigernumber.db");
	pConf->nTrigerDeliverNum = 3;
	pConf->nTrigerRetryTime = 2;
	pConf->nTrigerRetryPeriod = 600;
	
	pConf->nRoundPeriod = 600;

    //add by D.Wei 2013/12/05
	pConf->enable_water = 0;
    snprintf(pConf->yxfile, sizeof(pConf->yxfile), "yxtask.txt");
    pConf->yxlevel = 1;
	/*
	char sDayWaterIp[20];
	int nDayWaterPort;
	int nDayWaterHour;
	int nDayWaterMin;
	int nDayWaterTotal;
	int nDayWaterHour2;
	int nDayWaterLine;
	int nDayWaterMin2;
	char sWaterListFile[100];
	int nWaterLine;
	int nWaterRound;
	int nWaterAdd;
	char sBindIp[20];
	int nBindPort;
	int nNetTimeOut;
	char sBaseDir[100];
	char sLogName[100];
	char sTaskFileDir[100];
	char sHostName[100];
	char sAliasFile[100];
	int nKeepAliveTimeOut;
	int nCheckPeriod;
		int nTaskRemoveTime;
	int nPoolMaxNum;
	int nMaxLogSize;
	int nCachePort;
	char sCacheIP[20];
	int nCachePriod;
	*/
	return 0;
}


int ReadDaemonConf(char *sFileName, CONFIGURE *pConf)
{
	FILE *pIn = fopen(sFileName, "r");
	char sLine[1024];
	
	pConf->tReadTime = time(NULL);
	while(fgets(sLine, 1024, pIn))
	{
		char *sTmp;
		
		if(TreatConfLine(sLine, &sTmp))
			continue;
		if(strcasecmp(sLine, "deliver_bind_ip") == 0)
		{
			if (strlen(sLine) < 1){
			    snprintf(pConf->sBindIp, sizeof(pConf->sBindIp), "0.0.0.0");
			}
			else{
			    snprintf(pConf->sBindIp, sizeof(pConf->sBindIp), "%s", sTmp);
		    }
		}	
		else if(strcasecmp(sLine, "deliver_bind_port") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nBindPort = n;
		}
		else if(strcasecmp(sLine, "net_time_out") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 1000)
				pConf->nNetTimeOut = n;	
		}
		else if(strcasecmp(sLine, "base_dir") == 0)	
		{
			snprintf(pConf->sBaseDir, sizeof(pConf->sBaseDir), "%s", sTmp);
		}	
		else if(strcasecmp(sLine, "log_name") == 0)	
		{
			snprintf(pConf->sLogName, sizeof(pConf->sLogName), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "task_file_dir") == 0)	
		{
			snprintf(pConf->sTaskFileDir, sizeof(pConf->sTaskFileDir), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "host_name") == 0)	
		{
			snprintf(pConf->sHostName, sizeof(pConf->sHostName), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "domain_alias_file") == 0)	
		{
			snprintf(pConf->sAliasFile, sizeof(pConf->sAliasFile), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "keep_alive_time_out") == 0)
		{
			int n = atoi(sTmp);
			if(n > 10)
				pConf->nKeepAliveTimeOut = n;		
		}
		else if(strcasecmp(sLine, "check_period") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nCheckPeriod = n;	
          // printf("nCheckPeriod = %d\n",pConf->nCheckPeriod );
		}
		else if(strcasecmp(sLine, "task_remove_time") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nTaskRemoveTime = n;	
		}
		else if(strcasecmp(sLine, "pool_max_number") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nPoolMaxNum = n;	
		}
		else if(strcasecmp(sLine, "max_log_size") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nMaxLogSize = n*1024;	
		}
		else if(strcasecmp(sLine, "cache_ip") == 0)	
		{
			snprintf(pConf->sCacheIp, sizeof(pConf->sCacheIp), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "cache_port") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nCachePort = n;
		}
		else if(strcasecmp(sLine, "cache_period") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nCachePeriod = n;
		}
		else if(strcasecmp(sLine, "task_backup_dir") == 0)
		{
			snprintf(pConf->sTaskBakDir, sizeof(pConf->sTaskBakDir), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "round_number") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nRoundNum = n;
		}
		else if(strcasecmp(sLine, "water_list_file") == 0)
		{
			snprintf(pConf->sWaterListFile, sizeof(pConf->sWaterListFile), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "water_line") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nWaterLine = n;
		}
		else if(strcasecmp(sLine, "water_round") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nWaterRound = n;
		}
		else if(strcasecmp(sLine, "water_add") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nWaterAdd = n;
		}
		else if(strcasecmp(sLine, "water_num") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nWaterNum = n;
		}
		else if(strcasecmp(sLine, "day_water_port") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nDayWaterPort = n;
		}
		else if(strcasecmp(sLine, "day_water_line") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nDayWaterLine = n;
		}
		else if(strcasecmp(sLine, "day_water_total") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nDayWaterTotal = n;
		}
		else if(strcasecmp(sLine, "day_water_ip") == 0)
		{
			snprintf(pConf->sDayWaterIp, sizeof(pConf->sDayWaterIp), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "day_water_begin") == 0)	
		{
			int n1, n2;// = atoi(sTmp);
			if(sscanf(sTmp, "%d:%d", &n1, &n2) == 2)
			{
				pConf->nDayWaterHour = n1;
				pConf->nDayWaterMin = n2;
			}
		}
		else if(strcasecmp(sLine, "day_water_end") == 0)	
		{
			int n1, n2;// = atoi(sTmp);
			if(sscanf(sTmp, "%d:%d", &n1, &n2) == 2)
			{
				pConf->nDayWaterHour2 = n1;
				pConf->nDayWaterMin2 = n2;
			}
		}
		else if(strcasecmp(sLine, "day_water_from") == 0)
		{
			//if(pConf->nDayWaterFrom < 4)
			{
				snprintf(pConf->sDayWaterFrom, sizeof(pConf->sDayWaterFrom)-1, "%s", sTmp);
				//snprintf(pConf->sDayWaterFrom + 50*(pConf->nDayWaterFrom), 50, "%s", sTmp);
				//i(pConf->nDayWaterFrom)++;	
			}
		}
		else if(strcasecmp(sLine, "day_task_delay") == 0)	
		{
			int n1, n2;// = atoi(sTmp);
			if(sscanf(sTmp, "%d:%d", &n1, &n2) == 2)
			{
				pConf->nDelayHour = n1;
				pConf->nDelayMin = n2;
			}
		}
		else if(strcasecmp(sLine, "task_delay_hour") == 0)
		{
			int n = 0;
			sscanf(sTmp, "%d", &n);
			if(n > 0)
				pConf->nDelayHour2 = n;	
		}
		else if(strcasecmp(sLine, "pipe") == 0)
		{
			snprintf(pConf->sPipe, sizeof(pConf->sPipe), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "check_mailbox_port") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nCheckMailBoxPort = n;
		}
		else if(strcasecmp(sLine, "check_mailbox_ip") == 0)
		{
			snprintf(pConf->sCheckMailBoxIp, 20, "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "active_score") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nActiveScore = n;
		}
		else if(strcasecmp(sLine, "triger_number_db") ==0 )
		{
			snprintf(pConf->sTrigerNumberDBFile, 100, "%s", sTmp);
		}
		else if(strcasecmp(sLine, "triger_deliver_number") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nTrigerDeliverNum = n;
		}
		else if(strcasecmp(sLine, "triger_retry_time") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60)
				pConf->nTrigerRetryTime = n;
		}
		else if(strcasecmp(sLine, "triger_retry_period") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 6000)
				pConf->nTrigerRetryPeriod = n;
		}
		else if(strcasecmp(sLine, "round_period") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 6000)
				pConf->nRoundPeriod = n;
		}
		else if(strcasecmp(sLine, "enable_water") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 )
				pConf->enable_water = 1;
		}
		else if(strcasecmp(sLine, "yxfile") == 0)
		{
            snprintf(pConf->yxfile, sizeof(pConf->yxfile), "%s", sTmp);
		}

		else if(strcasecmp(sLine, "yxlevel") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 )
				pConf->yxlevel = n;
		}

		/*
		else if(strcasecmp(sLine, "") == 0)	
		{
			
		}
		*/
		/*
		char sDayWaterIp[20];
	int nDayWaterPort;
	int nDayWaterHour;
	int nDayWaterMin;
	int nDayWaterTotal;
	int nDayWaterLine;
	int nDayWaterHour2;
	int nDayWaterLine;
	int nDayWaterMin2;
	
		char sWaterListFile[100];
	int nWaterLine;
	int nWaterRound;
	int nWaterAdd;
	int nCachePort;
	char sCacheIP[20];
	int nCachePriod;
	char sBindIp[20];
	int nBindPort;
	int nNetTimeOut;
	char sBaseDir[100];
	char sLogName[100];
	char sTaskFileDir[100];
	char sHostName[100];
	char sAliasFile[100];
	int nKeepAliveTimeOut;
	int nCheckPeriod;
	int nTaskRemoveTime;
	int nPoolMaxNum;
	int nMaxLogSize;
	*/
		
	}
	
	
	fclose(pIn);
	return 0;
	
}

int ShowDaemonConf(CONFIGURE *pConf)
{
	printf("bind ip is:%s.\n", pConf->sBindIp);
	printf("bind port is:%d.\n", pConf->nBindPort);
	printf("net time out is:%d.\n", pConf->nNetTimeOut);
	printf("base dir is:%s.\n", pConf->sBaseDir);
	printf("log name is:%s.\n", pConf->sLogName);
	printf("task file dir is:%s.\n", pConf->sTaskFileDir);
	printf("host name is:%s.\n", pConf->sHostName);
	printf("alias file is:%s.\n", pConf->sAliasFile);
	/*
	char sBindIp[20];
	int nBindPort;
	int nNetTimeOut;
	char sBaseDir[100];
	char sLogName[100];
	char sTaskFileDir[100];
	char sHostName[100];
	char sAliasFile[100];
	*/
	return 0;
}


/*
int main(int argc, char *argv[])
{
	CONFIGURE *pConf = (CONFIGURE*)malloc(sizeof(CONFIGURE));
	
	InitialConf(pConf);
	ReadDaemonConf(argv[1], pConf);
	ShowDaemonConf(pConf);
	
	return 0;
}
*/




