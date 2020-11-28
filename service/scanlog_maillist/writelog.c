#include "writelog.h"
#include "conf.h"


int OpenLogFile(CONFIGURE *pConf, DABUDAI *pDa)
{
	char sFileName[200];
	
	pDa->nLogSize = 0;
	while(pDa->nLogNum < 9990)
	{
		char sFileNameTmp[200];
		snprintf(sFileNameTmp, 200, "%s.%04d%02d%02d.%04d", pConf->sLogName, pDa->nYear, pDa->nMonth, pDa->nDay, pDa->nLogNum+1);
		snprintf(sFileName, 200, "%s.%04d%02d%02d.%04d", pConf->sLogName, pDa->nYear, pDa->nMonth, pDa->nDay, pDa->nLogNum);		
		if(access(sFileName, F_OK) == 0 && access(sFileNameTmp, F_OK))
		{
			struct stat sctFileStat;
			stat(sFileName, &sctFileStat);
			if(sctFileStat.st_size < pConf->nMaxLogSize && sctFileStat.st_size >= 0)
			{
				pDa->nLogSize = sctFileStat.st_size;
				break;	
			}
		}
		else if(access(sFileName, F_OK))
			break;
		(pDa->nLogNum)++;
	}
	
	if(pDa->nLogNum >= 9990)
	{
		pDa->nLogNum = 0;
		snprintf(sFileName, 200, "%s.%04d%02d%02d.%04d", pConf->sLogName, pDa->nYear, pDa->nMonth, pDa->nDay, pDa->nLogNum);		
		unlink(sFileName);
	}
	pDa->pFileLog = fopen(sFileName, "a");
	if(pDa->pFileLog == NULL)
		printf("can't open the log file!\n");
	(pDa->nLogNum)++;
	
	return 0;
}


int WriteLog(CONFIGURE *pConf, DABUDAI *pDa, char *sLog, int nSize)
{
	time_t tTime;
	struct tm sctTime;
	char sTime[30];
	
	
	tTime = time(NULL);
	localtime_r(&tTime, &sctTime);
	snprintf(sTime, 30, "[%02d:%02d:%02d] ", sctTime.tm_hour, sctTime.tm_min, sctTime.tm_sec);//[02:23:42] 
	
	pthread_mutex_lock(pDa->pLockLog);
	if(pDa->pFileLog)
	{	
		fwrite(sTime, 11, 1, pDa->pFileLog);
		fwrite(sLog, nSize, 1, pDa->pFileLog);
		
	}
	pDa->nLogSize += (nSize+11);
	/*
	if(pDa->nLogSize > pConf->nMaxLogSize)
	{
		if(pDa->pFileLog)	
			fclose(pDa->pFileLog);
		OpenLogFile(pConf, pDa);
	}
	*/
	pthread_mutex_unlock(pDa->pLockLog);
	return 0;
}



