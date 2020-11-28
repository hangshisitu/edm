#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <db.h>
#include <mcheck.h>
#include "conf.h"
#include "writelog.h"
#include "iparea.h"
#include "socket_tool.h"


#define _H_version "scanlog version 1.0."











int TreatOpenLine(char *sLine, char *sEmail, char *sTime, char *sArea, char *sIP, DB *db, DBC *dbc, DBT *dbtKey, 
												DBT *dbtData, int *nOID, int *nBID, int *nLID, char *sTaskID)
{
	char *sTmpB, *sTmpE;
	char *sLine2;
	snprintf(sTime, 9, "%s", sLine+1);
	//sLine2 = strstr(sLine, sTaskID); //Delete By D.wei 2013/08/07
	sLine2 = sLine; //Add .....
	if(!sLine2)
		return 0;
	sTmpB = strstr(sLine2, "&e=");
	if(sTmpB)
	{
		sTmpE = strchr(sTmpB+1, '&');
		if(sTmpE)
			*sTmpE = 0;
		snprintf(sEmail, 100, "%s", sTmpB+3);
		if(sTmpE)
			*sTmpE = '&';	
	}
	
	sTmpB = strstr(sLine2, "&ip=");
//printf("Line= > %s\n", sTmpB);	
	if(sTmpB)
	{
		sTmpE = strchr(sTmpB+1, '&');
		if(sTmpE)
			*sTmpE = 0;
		snprintf(sIP, 20, "%s", sTmpB+4);
		{
			char *sss = strchr(sIP, ']');
			if(sss)
				*sss = 0;	
		}
		if(sTmpE)
			*sTmpE = '&';
        sTmpE = strstr(sLine2, "&oid");
		if(sTmpE)	
		{
			sscanf(sTmpE, "&oid=%d&lid=%d&bid=%d", nOID, nLID, nBID);
		}
		else
		{
			*nOID = 0;
			*nLID = 0;
			*nBID = 0;	
		}
	}
	
	
	
	{
		unsigned char ss[20];
		int nRet;
		sscanf(sIP, "%d.%d.%d.%d", ss, ss+1, ss+2, ss+3);
		memcpy(ss+4, ss, 4);
		dbtKey->data = ss;
		dbtKey->size = 8;
		nRet = dbc->c_get(dbc, dbtKey, dbtData, DB_SET);
		if(nRet == 0)
			snprintf(sArea, 30, "%s", (char*)(dbtData->data));
	}
	//[09:32:33]http://tiaourl.com/test2.php?u=http://139edm.com/logo.gif&ta=334&te=130&c=105&e=13828628930@139.com&ip=221.176.1.62] 
	return 0;
}



int TreatClickLine(char *sLine, char *sEmail, char *sTime, char *sArea, char *sIP, char *sUrl, DB *db, DBC *dbc, 
											DBT *dbtKey, DBT *dbtData, int *nOID, int *nBID, int *nLID, char *sTaskID)
{
	char *sTmpB, *sTmpE;
	char *sLine2;
	
	//sLine2 = strstr(sLine, sTaskID);
	sLine2 = sLine;
	if(!sLine2)
		return 0;
	snprintf(sTime, 9, "%s", sLine+1);
	
	sTmpB = strstr(sLine2, "&e=");
	if(sTmpB)
	{
		sTmpE = strchr(sTmpB+1, '&');
		if(sTmpE)
			*sTmpE = 0;
		snprintf(sEmail, 100, "%s", sTmpB+3);
		if(sTmpE)
			*sTmpE = '&';	
	}
	
	sTmpB = strstr(sLine2, "&ip=");
	if(sTmpB)
	{
		sTmpE = strchr(sTmpB+1, '&');
		if(sTmpE)
			*sTmpE = 0;
		snprintf(sIP, 20, "%s", sTmpB+4);
		{
			char *sss = strchr(sIP, ']');
			if(sss)
				*sss = 0;	
		}
		if(sTmpE)
			*sTmpE = '&';	

        sTmpE = strstr(sLine2, "&oid");

		if(sTmpE)
		{	
			sscanf(sTmpE, "&oid=%d&lid=%d&bid=%d", nOID, nLID, nBID);
		}
		else
		{
			*nOID = 0;
			*nLID = 0;
			*nBID = 0;	
		}
	}
	
	{
		unsigned char ss[20];
		int nRet;
		sscanf(sIP, "%d.%d.%d.%d", ss, ss+1, ss+2, ss+3);
		memcpy(ss+4, ss, 4);
		dbtKey->data = ss;
		dbtKey->size = 8;
		nRet = dbc->c_get(dbc, dbtKey, dbtData, DB_SET);
		if(nRet == 0)
			snprintf(sArea, 30, "%s", (char*)(dbtData->data));
	}
	
	sTmpB = strstr(sLine, "url=");
	if(sTmpB)
	{
		sTmpE = strchr(sTmpB, '&');
		if(sTmpE)
			*sTmpE = 0;
		snprintf(sUrl, 100, "%s", sTmpB+4);
		if(sTmpE)
			*sTmpE = 0;	
	}
	//[12:47:15] req [url=http://www.kouclo.com/activity/120306-1&&u=aHR0cDovL3d3dy5rb3VjbG8uY29tL2FjdGl2aXR5LzEyMDMwNi0x&ta=334&te=130&c=105&e=dxm@catic-co.com&ip=220.231.188.34]
	return 0;
}




int SelectScanFile(char *sScanDir, char *sDate, char *sFileName, int *nFileCount)
{
	struct stat sFileStat;
	DIR* pDir;
	struct dirent *d_c;		
	
	*nFileCount = 0;
	pDir = opendir(sScanDir);	
	while((d_c = readdir(pDir)) && (*nFileCount) < 100)
	{
		struct stat sctFileStat;
		char sNameTmp[100];
				
		if(strcmp(d_c->d_name, ".") == 0 || strcmp(d_c->d_name, "..") == 0 || strstr(d_c->d_name, sDate) == NULL)
			continue;
		
		printf("[[%s]]\n", d_c->d_name);
		
		snprintf(sNameTmp, 100, "%s/%s", sScanDir, d_c->d_name);	
		if(*nFileCount == 0)
		{
			*nFileCount = 1;
			snprintf(sFileName, 100, "%s", sNameTmp);
		}
		else
		{
			int i = 0;
			
			while(i < *nFileCount && *nFileCount < 100)
			{
				if((strlen(sFileName+100*i) < strlen(sNameTmp)) || (strcmp(sFileName+100*i, sNameTmp) > 0 && strlen(sFileName+100*i) == strlen(sNameTmp)) )
				{
					int j = *nFileCount;
					while(j > i)
					{
						snprintf(sFileName+100*j, 100, "%s", sFileName+100*(j-1));
						--j;	
					}
					snprintf(sFileName+100*i, 100, "%s", sNameTmp);
					break;	
				}
				++i;
			}
			if(i == *nFileCount && *nFileCount < 100)
			{	
				snprintf(sFileName+100*(*nFileCount), 100, "%s", sNameTmp);
				(*nFileCount)++;
			}
			else
				(*nFileCount)++;
		}
	}
	
	{
		int jjj = 0;
		while(jjj < *nFileCount)
		{
			printf("<<%s>>\n", sFileName+jjj*100);
			++jjj;	
		}	
	}
	closedir(pDir);
	
	return 0;
}



int InitialDaBuDai(DABUDAI *pDabudai)
{
	time_t tNow;
	struct tm sctTime;
	memset(pDabudai, 0x0, sizeof(DABUDAI));
	time(&tNow);
	localtime_r(&tNow, &sctTime); 
	pDabudai->nDay = sctTime.tm_mday;
	pDabudai->nMonth = sctTime.tm_mon+1;
	pDabudai->nYear = sctTime.tm_year+1900;
	pDabudai->pLockLog = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pDabudai->pLockLog, NULL);
		
	return 0;
}


int ReadClientSoft(DABUDAI *pDabudai, CONFIGURE *pConf)
{
	FILE *pFile = fopen(pConf->sClientSoftFile, "r");
	char sLine[1024];
	int nLID = 0;
	int nBID = 0;
	int nOID = 0;

    memset(pDabudai->sOID, 0x00, sizeof(pDabudai->sOID));	
    memset(pDabudai->sLID, 0x00, sizeof(pDabudai->sLID));
    memset(pDabudai->sBID, 0x00, sizeof(pDabudai->sBID));

	if(pFile == NULL)
		return 0;
	while(fgets(sLine, 1024, pFile))
	{
		char *sTmp;
		if(TreatConfLine(sLine, &sTmp))
			continue;
		if(strncasecmp(sLine, "lid", 3) == 0 && nLID < 49)
		{
			nLID++;
			snprintf((pDabudai->sLID)[nLID], 20, "%s", sTmp); 	
		}
		else if(strncasecmp(sLine, "oid", 3) == 0 && nOID < 49)
		{
			nOID++;
			snprintf((pDabudai->sOID)[nOID], 20, "%s", sTmp); 	
		}
		else if(strncasecmp(sLine, "bid", 3) == 0 && nBID < 49)
		{
			nBID++;
			snprintf((pDabudai->sBID)[nBID], 20, "%s", sTmp); 	
		}
	}
	
	fclose(pFile);
	return 0;
}




int GetArgument(char *sMsg, char *sID, char *sDate, int *nMode)
{
	char *sTmp, *sTmp2;
	char c = 0;
	
	sTmp = strstr(sMsg, "taskid=");
	if(!sTmp)
		return -1;
	sTmp2 = strchr(sTmp, '&');
	if(!sTmp2)
		sTmp2 = strchr(sTmp, ' ');
	if(!sTmp2)
		return -1;
	c = *sTmp2;
	*sTmp2 = 0;
	sTmp += 7;
	snprintf(sID, 100, "%s", sTmp);
	*sTmp2 = c;
	
	sTmp = strstr(sMsg, "begintime=");
	if(!sTmp)
		return -1;
	sTmp2 = strchr(sTmp, '&');
	if(!sTmp2)
		sTmp2 = strchr(sTmp, ' ');
	if(!sTmp2)
		return -1;
	c = *sTmp2;
	*sTmp2 = 0;
	sTmp += 10;
	snprintf(sDate, 20, "%s", sTmp);
	*sTmp2 = c;
	
	sTmp = strstr(sMsg, "mode=");
	if(!sTmp)
		return -1;
	sTmp2 = strchr(sTmp, '&');
	if(!sTmp2)
		sTmp2 = strchr(sTmp, ' ');
	if(!sTmp2)
		return -1;
	c = *sTmp2;
	*sTmp2 = 0;
	sTmp += 5;
	//printf("((%s))", sTmp);
	if(strcasecmp(sTmp, "head") == 0)
		*nMode = 0;
	else if(strcasecmp(sTmp, "list") == 0)
		*nMode = -1;
	else
		*nMode = 1;
	*sTmp2 = c;
	
	return 0;
}



int WriteData(int nSocket, char *sMsg, int nLength, int nSec)
{
	int nWrite = 0;
	int nFinish = 0;
	
	while(nFinish < nLength)
	{
		int k;
		if(nLength - nFinish > 1024)
			nWrite = 1024;
		else
			nWrite = nLength - nFinish;
		k = SocketWrite(nSocket, nSec, sMsg+nFinish, nWrite);
		if(k != nWrite)
			break;
		nFinish += k;
	}
	return nFinish;
}



/*
int GetClickHead(char *sID, char *sDate, char *sResult, int *nLength, CONFIGURE *pConf)
{
	//struct stat sFileStat;
	//DIR* pDir;
	//struct dirent *d_c;		
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	*nLength = 0;
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	while(nSum < 100)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount)
		{	
			
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			
			while(fgets(sLine, sizeof(sLine), pFile))
			{
				if(strstr(sLine, "url=") == NULL || strstr(sLine, sTaskID) == NULL)
					continue;		
				snprintf(sResult+*nLength, 1024000-*nLength, "%s", sLine);
				*nLength = *nLength + strlen(sResult + *nLength);
				++nSum;
				if(nSum >= 100)
					break;
			}
			fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
	}
	//closedir(pDir);
	
	return 0;
}
*/




int GetClickHead(char *sID, char *sDate, char *sResult, int *nLength, CONFIGURE *pConf, DB *db, DABUDAI *pDabudai)
{
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	DBC *dbc;
	DBT dbtKey, dbtData;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	*nLength = 0;
	
	//Add By D.Wei 2013/08/07
	char dw_corpid[128];
	char dw_taskid[128];
	char dw_tempid[128];
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				//Add By D.Wei 2013/08/07
				snprintf(dw_corpid, sizeof(dw_corpid), "&c=%s&", sID);
				snprintf(dw_taskid, sizeof(dw_taskid), "&ta=%s&", sTmp+1);
				snprintf(dw_tempid, sizeof(dw_tempid), "&te=%s&", sTmp2+1);
				//End by D.wei
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	snprintf(sResult+*nLength, 1024000-*nLength, "email,date,time,area,ip,system,brower,language,url\r\n");
	*nLength = *nLength + strlen(sResult + *nLength);
	
	while(nSum < 100)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount  && i < 99 && nSum < 100)
		{	
			
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			//printf("open file:%s.\n", sFileName+i*100);
			while(pFile && fgets(sLine, sizeof(sLine), pFile))
			{
				char sEmail[100], sTime[20], sArea[100], sIP22[20], sUrl[100];
				int nOID, nBID, nLID;
				sEmail[0] = sTime[0] = sArea[0] = sIP22[0] = sUrl[0] = 0;
				//if(strstr(sLine, "url=") == NULL || strstr(sLine, sTaskID) == NULL)
				if (strstr(sLine, "url=") == NULL || strstr(sLine, dw_taskid) == NULL || strstr(sLine, dw_corpid) == NULL || strstr(sLine, dw_tempid) == NULL )
					continue;		
				TreatClickLine(sLine, sEmail, sTime, sArea, sIP22, sUrl, db, dbc, &dbtKey, &dbtData, &nOID, &nBID, &nLID, sTaskID);
				snprintf(sResult+*nLength, 1024000-*nLength, "%s,%04d-%02d-%02d,%s,%s,%s,%s,%s,%s,%s\r\n", sEmail, nYear, nMonth, nDay, 
									sTime, sArea, sIP22, (pDabudai->sOID)[nOID], (pDabudai->sBID)[nBID], (pDabudai->sLID)[nLID], sUrl);
				*nLength = *nLength + strlen(sResult + *nLength);
				++nSum;
				if(nSum >= 100)
					break;
			}
			if(pFile)
				fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
		free(sFileName);
	}
	//closedir(pDir);
	dbc->c_close(dbc);
	return 0;
}




int GetOpenHead(char *sID, char *sDate, char *sResult, int *nLength, CONFIGURE *pConf, DB *db, DABUDAI *pDabudai)
{
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	DBC *dbc;
	DBT dbtKey, dbtData;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	*nLength = 0;
	
	//Add By D.Wei 2013/08/07
	char dw_corpid[128];
	char dw_taskid[128];
	char dw_tempid[128];
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				
				//Add By D.Wei 2013/08/07
				snprintf(dw_corpid, sizeof(dw_corpid), "&c=%s&", sID);
				snprintf(dw_taskid, sizeof(dw_taskid), "&ta=%s&", sTmp+1);
				snprintf(dw_tempid, sizeof(dw_tempid), "&te=%s&", sTmp2+1);
				//End by D.wei
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	snprintf(sResult+*nLength, 1024000-*nLength, "email,date,time,area,ip,system,brower,language\r\n");
	*nLength = *nLength + strlen(sResult + *nLength);
	
	while(nSum < 100)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount && i < 99 && nSum < 100)
		{	
			
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			//printf("open file:%s.\n", sFileName+i*100);
			while(pFile && fgets(sLine, sizeof(sLine), pFile))
			{
				char sEmail[100], sTime[20], sArea[100], sIP22[20];
				int nOID = 0, nBID = 0, nLID = 0;
				sEmail[0] = sTime[0] = sArea[0] = sIP22[0] = 0;
				//if(strstr(sLine, "test2.php") == NULL || strstr(sLine, sTaskID) == NULL) //Delete By D.Wei
				if (strstr(sLine, "test2.php") == NULL || strstr(sLine, dw_taskid) == NULL || strstr(sLine, dw_corpid) == NULL || strstr(sLine, dw_tempid) == NULL ){
					continue;
				}
				TreatOpenLine(sLine, sEmail, sTime, sArea, sIP22, db, dbc, &dbtKey, &dbtData, &nOID, &nBID, &nLID, sTaskID);
				snprintf(sResult+*nLength, 1024000-*nLength, "%s,%04d-%02d-%02d,%s,%s,%s,%s,%s,%s\r\n", 
							sEmail, nYear, nMonth, nDay, sTime, sArea, sIP22, (pDabudai->sOID)[nOID], (pDabudai->sBID)[nBID], (pDabudai->sLID)[nLID]);
				*nLength = *nLength + strlen(sResult + *nLength);
				++nSum;
				if(nSum >= 100)
					break;
			}
			if(pFile)
				fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		free(sFileName);
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
	}
	//closedir(pDir);
	dbc->c_close(dbc);
	return 0;
}



int GetOpenDetail(char *sID, char *sDate, CONFIGURE *pConf, DB *db, FILE *pOut, DABUDAI *pDabudai)
{
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	DBC *dbc;
	DBT dbtKey, dbtData;
	DB *db2 = OpenDB(NULL, NULL, 64, 512, NULL);
	DBC *dbc2;
	iconv_t ICONV;
	
	ICONV = iconv_open("gbk", "utf-8");
	
	db2->cursor(db2, NULL, &dbc2, 0);
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	//Add By D.Wei 2013/08/07
	char dw_corpid[128];
	char dw_taskid[128];
	char dw_tempid[128];
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				
				//Add By D.Wei 2013/08/07
				snprintf(dw_corpid, sizeof(dw_corpid), "&c=%s&", sID);
				snprintf(dw_taskid, sizeof(dw_taskid), "&ta=%s&", sTmp+1);
				snprintf(dw_tempid, sizeof(dw_tempid), "&te=%s&", sTmp2+1);
				//End by D.wei
				
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	//snprintf(sResult+*nLength, 1024000-*nLength, "email,time,area,ip,url\r\n");
	//*nLength = *nLength + strlen(sResult + *nLength);
	
	while(1)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount && i < 99)
		{	
			char sDate2[20];
			snprintf(sDate2, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			printf("open file:%s.\n", sFileName+i*100);
			while(pFile && fgets(sLine, sizeof(sLine), pFile))
			{
				char sEmail[100], sTime[20], sArea[30], sIP22[20];
				char sMsg[80];
				int n = 1;
				int nRet = 0;
				int nOID, nBID, nLID;
				
				sEmail[0] = sTime[0] = sArea[0] = sIP22[0] = 0;
				//if(strstr(sLine, "test2.php") == NULL || strstr(sLine, sTaskID) == NULL)
				if (strstr(sLine, "test2.php") == NULL || strstr(sLine, dw_taskid) == NULL || strstr(sLine, dw_corpid) == NULL || strstr(sLine, dw_tempid) == NULL )
					continue;		
				TreatOpenLine(sLine, sEmail, sTime, sArea, sIP22, db, dbc, &dbtKey, &dbtData, &nOID, &nBID, &nLID, sTaskID);
				memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 9);
				memcpy(sMsg+20, &n, 4);
				{
					char sss[30];
					char *s1 = sArea, *s2 = sss;
					size_t inleft = strlen(sArea)+1, outleft = 30;
					iconv(ICONV, &s1, &inleft, &s2, &outleft);	
					//printf("%d:%d\n", strlen(sArea), strlen(sss));
					memcpy(sMsg+24, sss, 30);
				}
				memcpy(sMsg+54, &nOID, 4);
				memcpy(sMsg+58, &nBID, 4);
				memcpy(sMsg+62, &nLID, 4);
				memcpy(sMsg+66, sIP22, 20);
				//snprintf(sMsg+74, 500, "%s:", sUrl);
				dbtKey.data = sEmail;
				dbtKey.size = strlen(sEmail)+1;
				dbtData.data = sMsg;
				dbtData.size = 92;
				nRet = db2->put(db2, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
				if(nRet)
				{
					nRet = dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_SET);
					if(nRet == 0)
					{
						int ll = 0;
						int *kk = (int*)(dbtData.data+20);
						(*kk)++;
						//memcpy(&n, dbtData.data+18, 4);
						//memcpy(&n, sMsg+20, 4);
						//n++;
						//memcpy(dbtData.data+18, &n, 4);
						//ll = strlen(dbtData.data+74);
						//snprintf(dbtData.data+74+ll, 500-ll, "%s:", sUrl);
						//memcpy(sMsg+20, &n, 4);
						//dbtData.data = sMsg;
						//dbtData.size = 544;
						dbc2->c_put(dbc2, &dbtKey, &dbtData, DB_CURRENT);	
					}
				}
				/*
				snprintf(sResult+*nLength, 1024000-*nLength, "%s,%s,%s,%s,%s\r\n", sEmail, sTime, sArea, sIP22, sUrl);
				*nLength = *nLength + strlen(sResult + *nLength);
				*/
				++nSum;
			}
			if(pFile)
				fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		free(sFileName);
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
	}
	//closedir(pDir);
	
	dbc2->c_close(dbc2);
	db2->cursor(db2, NULL, &dbc2, 0);
	fprintf(pOut, "email,date,time,count,area,ip,system,brower,language\r\n");
	while( dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		/*int *n, *n2;
		n = (int*)(dbtData.data);
		n2 = (int*)(dbtData.data+44);*/
		int *n = (int*)(dbtData.data+54);
		fprintf(pOut, "%s,%s,%s,%d,%s,%s,%s,%s,%s\r\n", dbtKey.data, dbtData.data, dbtData.data+11, *((int*)(dbtData.data+20)), 
																						dbtData.data+24, dbtData.data+66/*54*/, (pDabudai->sOID)[n[0]], 
																						(pDabudai->sBID)[n[1]], (pDabudai->sLID)[n[2]]);
	}
				/*memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 7);
				memcpy(sMsg+18, &n, 4);
				memcpy(sMsg+22, sArea, 30);
				memcpy(sMsg+52, sIP22, 20);
				snprintf(sMsg+72, 500, "%s:", sUrl);*/
	
	
	dbc->c_close(dbc);
	dbc2->c_close(dbc2);
	CloseDB(db2);
	
	iconv_close(ICONV);
	
	return 0;
}


int GetOpenList(char *sID, char *sDate, CONFIGURE *pConf, DB *db, FILE *pOut, DABUDAI *pDabudai)
{
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	DBC *dbc;
	DBT dbtKey, dbtData;
	//DB *db2 = OpenDB(NULL, NULL, 64, 512, NULL);
	//DBC *dbc2;
	iconv_t ICONV;
	
	ICONV = iconv_open("gbk", "utf-8");
	
	//db2->cursor(db2, NULL, &dbc2, 0);
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	//Add By D.Wei 2013/08/07
	char dw_corpid[128];
	char dw_taskid[128];
	char dw_tempid[128];
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				//Add By D.Wei 2013/08/07
				snprintf(dw_corpid, sizeof(dw_corpid), "&c=%s&", sID);
				snprintf(dw_taskid, sizeof(dw_taskid), "&ta=%s&", sTmp+1);
				snprintf(dw_tempid, sizeof(dw_tempid), "&te=%s&", sTmp2+1);
				//End by D.wei
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	//snprintf(sResult+*nLength, 1024000-*nLength, "email,time,area,ip,url\r\n");
	//*nLength = *nLength + strlen(sResult + *nLength);
	
	
	fprintf(pOut, "email,date,time,area,ip,system,brower,language\r\n");
	while(1)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount && i < 99)
		{	
			char sDate2[20];
			snprintf(sDate2, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			printf("open file:%s.\n", sFileName+i*100);
			while(pFile && fgets(sLine, sizeof(sLine), pFile))
			{
				char sEmail[100], sTime[20], sArea[30], sIP22[20];
				char sMsg[80];
				int n = 1;
				int nRet = 0;
				int nOID, nBID, nLID;
				
				sEmail[0] = sTime[0] = sArea[0] = sIP22[0] = 0;
				//if(strstr(sLine, "test2.php") == NULL || strstr(sLine, sTaskID) == NULL)
				if (strstr(sLine, "test2.php") == NULL || strstr(sLine, dw_taskid) == NULL || strstr(sLine, dw_corpid) == NULL || strstr(sLine, dw_tempid) == NULL )
					continue;		
				TreatOpenLine(sLine, sEmail, sTime, sArea, sIP22, db, dbc, &dbtKey, &dbtData, &nOID, &nBID, &nLID, sTaskID);
				fprintf(pOut, "%s,%s,%s,%s,%s,%s,%s,%s\r\n", sEmail, sDate2, sTime, sArea, sIP22, (pDabudai->sOID)[nOID],
																								(pDabudai->sBID)[nBID], (pDabudai->sLID)[nLID]);
				//fprintf(pOut, "email,date,time,count,area,ip,system,brower,language\r\n");
				//fprintf(pOut, "%s,%s,%s,%d,%s,%s,%s,%s,%s\r\n", dbtKey.data, dbtData.data, dbtData.data+11, *((int*)(dbtData.data+20)), 
					//																	dbtData.data+24, dbtData.data+66/*54*/, (pDabudai->sOID)[n[0]], 
					//																	(pDabudai->sBID)[n[1]], (pDabudai->sLID)[n[2]]);
				/*	
				memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 9);
				memcpy(sMsg+20, &n, 4);
				{
					char sss[30];
					char *s1 = sArea, *s2 = sss;
					size_t inleft = strlen(sArea)+1, outleft = 30;
					iconv(ICONV, &s1, &inleft, &s2, &outleft);	
					//printf("%d:%d\n", strlen(sArea), strlen(sss));
					memcpy(sMsg+24, sss, 30);
				}
				memcpy(sMsg+54, &nOID, 4);
				memcpy(sMsg+58, &nBID, 4);
				memcpy(sMsg+62, &nLID, 4);
				memcpy(sMsg+66, sIP22, 20);
				//snprintf(sMsg+74, 500, "%s:", sUrl);
				dbtKey.data = sEmail;
				dbtKey.size = strlen(sEmail)+1;
				dbtData.data = sMsg;
				dbtData.size = 92;
				nRet = db2->put(db2, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
				if(nRet)
				{
					nRet = dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_SET);
					if(nRet == 0)
					{
						int ll = 0;
						int *kk = (int*)(dbtData.data+20);
						(*kk)++;
						//memcpy(&n, dbtData.data+18, 4);
						//memcpy(&n, sMsg+20, 4);
						//n++;
						//memcpy(dbtData.data+18, &n, 4);
						//ll = strlen(dbtData.data+74);
						//snprintf(dbtData.data+74+ll, 500-ll, "%s:", sUrl);
						//memcpy(sMsg+20, &n, 4);
						//dbtData.data = sMsg;
						//dbtData.size = 544;
						dbc2->c_put(dbc2, &dbtKey, &dbtData, DB_CURRENT);	
					}
				}
				*/
				/*
				snprintf(sResult+*nLength, 1024000-*nLength, "%s,%s,%s,%s,%s\r\n", sEmail, sTime, sArea, sIP22, sUrl);
				*nLength = *nLength + strlen(sResult + *nLength);
				*/
				++nSum;
			}
			if(pFile)
				fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		free(sFileName);
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
	}
	//closedir(pDir);
	
	//dbc2->c_close(dbc2);
	//db2->cursor(db2, NULL, &dbc2, 0);
	//	fprintf(pOut, "email,date,time,count,area,ip,system,brower,language\r\n");
	//while( dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_NEXT) == 0)
	//{
		/*int *n, *n2;
		n = (int*)(dbtData.data);
		n2 = (int*)(dbtData.data+44);*/
		//int *n = (int*)(dbtData.data+54);
		//fprintf(pOut, "%s,%s,%s,%d,%s,%s,%s,%s,%s\r\n", dbtKey.data, dbtData.data, dbtData.data+11, *((int*)(dbtData.data+20)), 
			//																			dbtData.data+24, dbtData.data+66/*54*/, (pDabudai->sOID)[n[0]], 
				//																		(pDabudai->sBID)[n[1]], (pDabudai->sLID)[n[2]]);
	//}
				/*memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 7);
				memcpy(sMsg+18, &n, 4);
				memcpy(sMsg+22, sArea, 30);
				memcpy(sMsg+52, sIP22, 20);
				snprintf(sMsg+72, 500, "%s:", sUrl);*/
	
	
	dbc->c_close(dbc);
	//dbc2->c_close(dbc2);
	//CloseDB(db2);
	
	iconv_close(ICONV);
	
	return 0;
}



int GetClickDetail(char *sID, char *sDate, CONFIGURE *pConf, DB *db, FILE *pOut, DABUDAI *pDabudai)
{
	char sTaskID[100];
	int nSum = 0;
	char sDate3[20];
	char *sFileName;
	int nFileCount;
	int i = 0;
	int nYear, nMonth, nDay;
	FILE *pFile;
	char sLine[1024];
	DBC *dbc;
	DBT dbtKey, dbtData;
	DB *db2 = OpenDB(NULL, NULL, 64, 512, NULL);
	DBC *dbc2;
	iconv_t ICONV;
	
	ICONV = iconv_open("gbk", "utf-8");
	
	db2->cursor(db2, NULL, &dbc2, 0);
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	memcpy(sDate3, sDate, 4);
	sDate3[4] = '-';
	memcpy(sDate3+5, sDate+4, 2);
	sDate3[7] = '-';
	memcpy(sDate3+8, sDate+6, 2);
	sDate3[10] = 0;
	
	//Add By D.Wei 2013/08/07
	char dw_corpid[128];
	char dw_taskid[128];
	char dw_tempid[128];	
	
	sscanf(sDate3, "%d-%d-%d", &nYear, &nMonth, &nDay);
	
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				//Add By D.Wei 2013/08/07
				snprintf(dw_corpid, sizeof(dw_corpid), "&c=%s&", sID);
				snprintf(dw_taskid, sizeof(dw_taskid), "&ta=%s&", sTmp+1);
				snprintf(dw_tempid, sizeof(dw_tempid), "&te=%s&", sTmp2+1);
				//End by D.wei
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	
	//snprintf(sResult+*nLength, 1024000-*nLength, "email,time,area,ip,url\r\n");
	//*nLength = *nLength + strlen(sResult + *nLength);
	
	while(1)
	{
		sFileName = (char*)malloc(10000);
		SelectScanFile(pConf->sScanDir, sDate3, sFileName, &nFileCount);
		i = 0;
		while(i < nFileCount && i < 99)
		{	
			char sDate2[20];
			snprintf(sDate2, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			{
				char *ss = strstr(sFileName+i*100, "log.");
				if(!ss)
				{
					++i;
					continue;
				}
				sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
			}
			//snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
			//if(memcmp(sDate2, sDate, 8) < 0)
			//{
				//continue;	
			//}
			
			pFile = fopen(sFileName+i*100, "r");
			
			//printf("open file:%s.\n", sFileName+i*100);
			while(pFile && fgets(sLine, sizeof(sLine), pFile))
			{
				char sEmail[100], sTime[20], sArea[30], sIP22[20], sUrl[100];
				char sMsg[580];
				int n = 1;
				int nRet = 0;
				int nOID, nBID, nLID;
				
				sEmail[0] = sTime[0] = sArea[0] = sIP22[0] = sUrl[0] = 0;
				//if(strstr(sLine, "url=") == NULL || strstr(sLine, sTaskID) == NULL)
				if (strstr(sLine, "url=") == NULL || strstr(sLine, dw_taskid) == NULL || strstr(sLine, dw_corpid) == NULL || strstr(sLine, dw_tempid) == NULL )
					continue;		
				TreatClickLine(sLine, sEmail, sTime, sArea, sIP22, sUrl, db, dbc, &dbtKey, &dbtData, &nOID, &nBID, &nLID, sTaskID);
                printf("TreatClickLine end\n");
				memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 9);
				memcpy(sMsg+20, &n, 4);
				{
					char sss[30];
					char *s1 = sArea, *s2 = sss;
					size_t inleft = strlen(sArea)+1, outleft = 30;
					iconv(ICONV, &s1, &inleft, &s2, &outleft);	
					//printf("%d:%d\n", strlen(sArea), strlen(sss));
					memcpy(sMsg+24, sss, 30);
				}
				memcpy(sMsg+54, sIP22, 20);
				memcpy(sMsg+74, &nOID, 4);
				memcpy(sMsg+78, &nBID, 4);
				memcpy(sMsg+82, &nLID, 4);
				snprintf(sMsg+86, 500, "%s:", sUrl);
				dbtKey.data = sEmail;
				dbtKey.size = strlen(sEmail)+1;
				dbtData.data = sMsg;
				dbtData.size = 592;
				nRet = db2->put(db2, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);	
				if(nRet)
				{
					nRet = dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_SET);
					if(nRet == 0)
					{
						int ll = 0;
						int *kk = (int*)(dbtData.data+20);
						(*kk)++;
						//memcpy(&n, dbtData.data+18, 4);
						//memcpy(&n, sMsg+20, 4);
						//n++;
						//memcpy(dbtData.data+18, &n, 4);
						ll = strlen(dbtData.data+86);
						snprintf(dbtData.data+86+ll, 500-ll, "%s:", sUrl);
						//memcpy(sMsg+20, &n, 4);
						//dbtData.data = sMsg;
						//dbtData.size = 544;
						dbc2->c_put(dbc2, &dbtKey, &dbtData, DB_CURRENT);	
					}
				}
				/*
				snprintf(sResult+*nLength, 1024000-*nLength, "%s,%s,%s,%s,%s\r\n", sEmail, sTime, sArea, sIP22, sUrl);
				*nLength = *nLength + strlen(sResult + *nLength);
				*/
				++nSum;
			}
			if(pFile)
				fclose(pFile);
			++i;
		}
		nDay++;
		if(nDay > 31)
		{
			nDay = 1;
			nMonth++;
		}
		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;	
		}
		{
			time_t tTime = time(NULL);
			struct tm sctTime;
			localtime_r(&tTime, &sctTime);
			if(nYear > sctTime.tm_year+1900)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth > sctTime.tm_mon+1)
				break;
			if(nYear == sctTime.tm_year+1900 && nMonth == sctTime.tm_mon+1 && nDay > sctTime.tm_mday)
				break;	
		}	
		free(sFileName);
		snprintf(sDate3, 100, "%04d-%02d-%02d", nYear, nMonth, nDay);
	}
	//closedir(pDir);
	
	dbc2->c_close(dbc2);
	db2->cursor(db2, NULL, &dbc2, 0);
//     printf("test point 1\n");
	fprintf(pOut, "email,date,time,count,area,ip,system,brower,language,url\r\n");
	while( dbc2->c_get(dbc2, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		/*int *n, *n2;
		n = (int*)(dbtData.data);
		n2 = (int*)(dbtData.data+44);*/
		int *n = (int*)(dbtData.data+74);
//        printf("test point 2\n");
//printf("dbtKey.data = %s\n", dbtKey.data);
//printf("dbtData.data = %s\n", dbtData.data);
//printf("dbtData.data+11 = %s\n", dbtData.data+11);
//printf("*((int*)(dbtData.data+20)) = %d\n", *((int*)(dbtData.data+20)));

//printf("dbtData.data+24 = %s\n", dbtData.data+24);
//printf("(pDabudai->sOID)[n[0]] = %s\n", (pDabudai->sOID)[n[0]]);
//printf("OK 311\n");
//printf("(pDabudai->sBID)[n[1]] = %s\n", (pDabudai->sBID)[n[1]]);
//printf("OK 211\n");
//printf("(pDabudai->sLID)[n[2]] = %s\n", (pDabudai->sLID)[n[2]]);
//printf("OK 111\n");
//printf("dbtData.data+86 = %s\n", dbtData.data+86);
int oid = abs(n[0]) % (sizeof(pDabudai->sOID)/sizeof(pDabudai->sOID[0]));
int bid = abs(n[1]) % (sizeof(pDabudai->sBID)/sizeof(pDabudai->sBID[0]));
int lid = abs(n[2]) % (sizeof(pDabudai->sLID)/sizeof(pDabudai->sLID[0]));
		fprintf(pOut, "%s,%s,%s,%d,%s,%s,%s,%s,%s,%s\r\n", dbtKey.data, dbtData.data, dbtData.data+11, *((int*)(dbtData.data+20)), 
																						dbtData.data+24, dbtData.data+54, (pDabudai->sOID)[oid], (pDabudai->sBID)[bid],
																						(pDabudai->sLID)[lid], dbtData.data+86/*74*/);
	}
  // printf("test point 3\n");
				/*memcpy(sMsg, sDate2, 11);
				memcpy(sMsg+11, sTime, 7);
				memcpy(sMsg+18, &n, 4);
				memcpy(sMsg+22, sArea, 30);
				memcpy(sMsg+52, sIP22, 20);
				snprintf(sMsg+72, 500, "%s:", sUrl);*/
	
	
	dbc->c_close(dbc);
	dbc2->c_close(dbc2);
	CloseDB(db2);
	
	iconv_close(ICONV);
	
	return 0;
}






/*
int GetOpenHead(char *sID, char *sDate, char *sResult, int *nLength, CONFIGURE *pConf)
{
	struct stat sFileStat;
	DIR* pDir;
	struct dirent *d_c;		
	char sTaskID[100];
	int nSum = 0;
	*nLength = 0;
	
	
	//printf("old ID:%s::\n", sID);
	{
		char *sTmp;
		sTmp = strchr(sID, '.');
		if(sTmp)
		{
			char *sTmp2 = strchr(sTmp+1, '.');
			if(sTmp2)
			{
				*sTmp = *sTmp2 = 0;
				//printf("ddddddd   %d  %d  %d  dddddd\n", sID, sTmp+1, sTmp2+1);
				snprintf(sTaskID, 100, "ta=%s&te=%s&c=%s&", sTmp+1, sTmp2+1, sID);
				*sTmp = *sTmp2 = '.';	
			}	
		}
			
	}
	//printf("\nID:::%s::\n", sTaskID);
	pDir = opendir(pConf->sScanDir);	
	while(d_c = readdir(pDir))
	{
		char sFileName[200];
		struct stat sctFileStat;
		FILE *pFile;
		int nYear, nMonth, nDay;
		char sDate2[20];
		char sLine[1024];
		
		if(nSum >= 100)
			break;
		if(strcmp(d_c->d_name, ".") == 0 || strcmp(d_c->d_name, "..") == 0)
			continue;
		snprintf(sFileName, 200, "%s/%s", pConf->sScanDir, d_c->d_name);
		stat(sFileName, &sctFileStat);
		if(S_ISREG(sctFileStat.st_mode) == 0)
			continue;
		{
			char *ss = strstr(d_c->d_name, "log.");
			if(!ss)
				continue;
			sscanf(ss, "log.%d-%d-%d", &nYear, &nMonth, &nDay);		
		}
		snprintf(sDate2, 20, "%04d%02d%02d", nYear, nMonth, nDay);
		if(memcmp(sDate2, sDate, 8) < 0)
		{
			continue;	
		}
		
		pFile = fopen(sFileName, "r");
		//printf("\nopen file:%s.\n", sFileName);
		while(fgets(sLine, sizeof(sLine), pFile))
		{
			if(strstr(sLine, "test2.php") == NULL || strstr(sLine, sTaskID) == NULL)
				continue;	
			//printf("LINE::%s:\n", sLine);	
			snprintf(sResult+*nLength, 1024000-*nLength, "%s", sLine);
			*nLength = *nLength + strlen(sResult + *nLength);
			++nSum;
			//printf("SUM LENG:%d::%d\n", *nLength, nSum);
			if(nSum >= 100)
				break;
		}
		fclose(pFile);
	}
	closedir(pDir);
	
	return 0;
}
*/

void *Thread_Session(void *sArg)
{
	int nIp;
	int nSocket;
	CONFIGURE *pConf;
	DABUDAI *pDabudai;
	char sLog[1024];
	int nLogOffset;
	int n;
	char *sMsg = (char*)malloc(10240);
	int k = 0;
	int nSuccess = 0;
	
	
	pthread_detach(pthread_self());
	memcpy(&nIp, sArg, sizeof(nIp));
	memcpy(&pConf, sArg+4, sizeof(char*));
	memcpy(&pDabudai, sArg+4+sizeof(char*), sizeof(char*));
	memcpy(&nSocket, sArg+4+2*sizeof(char*), sizeof(nSocket));
	memcpy(&n, sArg+8+2*sizeof(char*), 4);
	free(sArg);
	
	{
		unsigned char *s = (unsigned char*)(&nIp);
		snprintf(sLog, 100, "[%d.%d.%d.%d.%04d]", s[0], s[1], s[2], s[3], n);
		nLogOffset = strlen(sLog);
	}
	
	k = SocketReadDoubleLine(nSocket, 3, sMsg, 10240);
	if(k <= 0)
	{
		snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]receive fail!\n");
		WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		close(nSocket);
		free(sMsg);
		return (void*)0;
	}
	
	//printf("receive ::%s::", sMsg);
	
	{
		char *sTmp;
		char sID[100];
		int nMode = 0;
		char sDate[20];
		
		sTmp = strstr(sMsg, "GET");
		if(sTmp)
		{
			sTmp = strchr(sTmp, '/');	
		}
		if(!sTmp)
		{
			//printf("\n%d\n", __LINE__);
			snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find GET or \"/\".\n");
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			close(nSocket);
			free(sMsg);
			return (void*)0;	
		}	
		if(strncasecmp(sTmp+1, "open", 4) == 0)
		{
			
			//printf("\n%d\n", __LINE__);
			if(GetArgument(sMsg, sID, sDate, &nMode) >= 0)
			{
				{	
					int n = atoi(sID);
					int j = 0;
					while(j < 10)
					{
						if(n == (pConf->nOmitCorp)[j])
						{
							close(nSocket);
							free(sMsg);
							return (void*)0;	
						}
						j++;	
					}
				}
				//printf("\n%d, mode:%d\n", __LINE__, nMode);
				//snprintf(sResult, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nVary: Accept-Encoding\r\nContent-Length: 110\r\nConnection: close\r\n");
				if(nMode == 0)
				{
					int nLength = 0;
					char *sResult = (char*)malloc(1024000);
					//printf("\n%d\n", __LINE__);
					GetOpenHead(sID, sDate, sResult, &nLength, pConf, pDabudai->dbIpArea, pDabudai);
					//printf("<<<<%d:%s>>>>", nLength, sResult);
					snprintf(sMsg, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", nLength);
					k = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
					if(k == strlen(sMsg) && WriteData(nSocket, sResult, nLength, 5) == nLength)
					{
						snprintf(sLog+nLogOffset, 10240-nLogOffset, "success to send back %d bytes.\n", nLength);
						WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					}
					else
					{
						snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to send back %d bytes.\n", nLength);
						WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					}
					free(sResult);
				}
				else if(nMode < 0)
				{
					char sName[100];
					FILE *pOut;
					struct stat sctStat;
					char *sss = (char*)malloc(1024*1024);
					int nFinish = 0, nRead = 0;;
									
					snprintf(sName, 100, "%s/%d.list", pConf->sBackupDir, n);
					pOut = fopen(sName, "w");
					snprintf(sLog+nLogOffset, 10240-nLogOffset, "write to file:%s.\n", sName);
					WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					GetOpenList(sID, sDate, pConf, pDabudai->dbIpArea, pOut, pDabudai);	
					fclose(pOut);
					stat(sName, &sctStat);
					pOut = fopen(sName, "r");
					snprintf(sMsg, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
					SocketWrite(nSocket, 5, sMsg, strlen(sMsg));		
					while(nFinish < sctStat.st_size)
					{
						if(sctStat.st_size - nFinish < 1024*1024)
							nRead = sctStat.st_size - nFinish;
						else
							nRead = 1024*1024;
						fread(sss, nRead, 1, pOut);
						WriteData(nSocket, sss, nRead, 5);
						nFinish += nRead;
					}
					fclose(pOut);
					free(sss);	
				}
				else
				{
					char sName[100];
					FILE *pOut;
					struct stat sctStat;
					char *sss = (char*)malloc(1024*1024);
					int nFinish = 0, nRead = 0;;
									
					snprintf(sName, 100, "%s/%d.detail", pConf->sBackupDir, n);
					pOut = fopen(sName, "w");
					snprintf(sLog+nLogOffset, 10240-nLogOffset, "write to file:%s.\n", sName);
					WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					GetOpenDetail(sID, sDate, pConf, pDabudai->dbIpArea, pOut, pDabudai);	
					fclose(pOut);
					stat(sName, &sctStat);
					pOut = fopen(sName, "r");
					snprintf(sMsg, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=gbk\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
					SocketWrite(nSocket, 5, sMsg, strlen(sMsg));		
					while(nFinish < sctStat.st_size)
					{
						if(sctStat.st_size - nFinish < 1024*1024)
							nRead = sctStat.st_size - nFinish;
						else
							nRead = 1024*1024;
						fread(sss, nRead, 1, pOut);
						WriteData(nSocket, sss, nRead, 5);
						nFinish += nRead;
					}
					fclose(pOut);
					free(sss);
				}
				/*
				else
				{
					GetOpenDetail(sID, sDate, sResult, &nLength);
				}
				*/
				
				close(nSocket);
				free(sMsg);
				return (void*)0;
			}
			else
			{
				//printf("\n%d\n", __LINE__);
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				return (void*)0;		
			}
		}	
		else if(strncasecmp(sTmp+1, "click", 5) == 0)
		{
			//printf("\n%d\n", __LINE__);
			if(GetArgument(sMsg, sID, sDate, &nMode) >= 0)
			{	
				{	
					int n = atoi(sID);
					int j = 0;
					while(j < 10)
					{
						if(n == (pConf->nOmitCorp)[j])
						{
							close(nSocket);
							free(sMsg);
							return (void*)0;	
						}
						j++;	
					}
				}
				
				if(nMode == 0)
				{
					char *sResult = (char*)malloc(1024000);
					int nLength = 0;
					GetClickHead(sID, sDate, sResult, &nLength, pConf, pDabudai->dbIpArea, pDabudai);
					snprintf(sMsg, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", nLength);
					k = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
					if(k == strlen(sMsg) && WriteData(nSocket, sResult, nLength, 5) == nLength) 
					{
						snprintf(sLog+nLogOffset, 10240-nLogOffset, "success to send back %d bytes.\n", nLength);
						WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					}
					else
					{
						snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to send back %d bytes.\n", nLength);
						WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					}
					free(sResult);
				}
				/*else
				{
					FILE *pOut = fopen("/tmp/abcderfg", "w");
					GetClickDetail(sID, sDate, pConf, pDabudai->dbIpArea, pOut);	
					fclose(pOut);
					///pOut = fopen();
				}*/
				else
				{
					char sName[100];
					FILE *pOut;
					struct stat sctStat;
					char *sss = (char*)malloc(1024*1024);
					int nFinish = 0, nRead = 0;;
										
					snprintf(sName, 100, "%s/%d.detail", pConf->sBackupDir, n);
					pOut = fopen(sName, "w");
					snprintf(sLog+nLogOffset, 10240-nLogOffset, "write to file:%s.\n", sName);
					WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					GetClickDetail(sID, sDate, pConf, pDabudai->dbIpArea, pOut, pDabudai);	
					fclose(pOut);
					stat(sName, &sctStat);
					snprintf(sMsg, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=gbk\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
					SocketWrite(nSocket, 5, sMsg, strlen(sMsg));	
					pOut = fopen(sName, "r");
					while(nFinish < sctStat.st_size)
					{
						if(sctStat.st_size - nFinish < 1024*1024)
							nRead = sctStat.st_size - nFinish;
						else
							nRead = 1024*1024;
						fread(sss, nRead, 1, pOut);
						WriteData(nSocket, sss, nRead, 5);
						nFinish += nRead;
					}
					fclose(pOut);
					free(sss);
				}
				
				close(nSocket);
				free(sMsg);
				return (void*)0;
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				return (void*)0;	
			}
			
		}
		else
		{
			snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can recognize command.\n");
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			close(nSocket);
			free(sMsg);
			return (void*)0;		
		}
	}
	
	
	return (void*)0;	
}




int main(int argc, char *argv[])
{
	DABUDAI *pDabudai = (DABUDAI*)malloc(sizeof(DABUDAI));
	CONFIGURE *pConf = (CONFIGURE*)malloc(sizeof(CONFIGURE));
	char sLog[1024];
	struct sockaddr_in sctClientAddr; 
	struct sockaddr_in sctServerAddr; 
	int nListenSocket, nConnectSocket;	
	int nNum = 0;
	char sConfFile[100];
	int nTmpTmpTmp = 0;
	
	
	//mtrace();
	
	InitialDaBuDai(pDabudai);
	InitialConf(pConf);
	
	pDabudai->dbIpArea = OpenDB(NULL, NULL, 64, 512, CompareIp);
	ReadIpArea(pDabudai->dbIpArea, pConf->sIpAreaFile);
	
	
	{
		int nShowVersion = 0;
		int ch;
		
		sConfFile[0] = 0;
		while((ch = getopt(argc, argv, "c:v")) != -1)
		{
			switch(ch)
			{
				case 'c':
				snprintf(sConfFile, 100, "%s", optarg);	
				break;
				case 'v':
				nShowVersion = 1;	
				break;
			}	
		}
		printf("configure file is:%s\n", sConfFile);
		if(nShowVersion)
		{
			printf("%s\n", _H_version);
			return 0;	
		}	
		if(sConfFile[0])
		{
			ReadConf(sConfFile, pConf);
		}
		else
			printf("[warn]no configure file, will run under default configure.\n");
				
		OpenLogFile(pConf, pDabudai);
		snprintf(sLog, sizeof(sLog), "[SYSTEM]sideside system start!\n");
		WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		fflush(NULL);
	}
	ReadClientSoft(pDabudai, pConf);
	
	
	{	
		int nPid = fork();
		if(nPid < 0)
		{
			printf("creat daemon fail(%s)!\n", strerror(errno));
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
	
	{
		int nTmp = 1;
		memset(&sctServerAddr,0x0,sizeof(sctServerAddr));
		sctServerAddr.sin_addr.s_addr = inet_addr(pConf->sBindIp);
		sctServerAddr.sin_port = htons(pConf->nBindPort);
		nListenSocket = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&nTmp, sizeof(nTmp));
		if( bind(nListenSocket, (struct sockaddr *) &sctServerAddr, sizeof(sctServerAddr)) < 0)
		{
			snprintf(sLog, sizeof(sLog), "[SYSTEM][ERROR]can't bind, ip:%s, port:%d.\n", pConf->sBindIp, pConf->nBindPort);
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			printf("can't bind the socket!\n");
			return -1;
		}
	}
	snprintf(sLog, sizeof(sLog), "[SYSTEM]bind ip:%s, port:%d.\n", pConf->sBindIp, pConf->nBindPort);
	WriteLog(pConf, pDabudai, sLog, strlen(sLog));
	if( listen(nListenSocket, 100) < 0)
	{
		printf("can't listen the socket(%s)!\n", strerror(errno));
		snprintf(sLog, sizeof(sLog), "[SYSTEM][ERROR]can't listen the socket.\n");
		WriteLog(pConf, pDabudai, sLog, strlen(sLog));
		return -3;
	}
	
	while(1)
	{
		time_t tNow = time(NULL);
		int nTmp;
		int nIp;
		
		fflush(NULL);
		{
			struct tm sctTime;
			
			localtime_r(&tNow, &sctTime);
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
		}	
		nTmp = sizeof(sctClientAddr);
		nConnectSocket = accept(nListenSocket, (struct sockaddr *)&sctClientAddr, &nTmp);	
		if(nConnectSocket < 0)
			continue;
			
		nTmp = 1;	
		ioctl(nConnectSocket, FIONBIO, &nTmp);
		
		{
			char *sArg = (char*)malloc(sizeof(nIp)+sizeof(CONFIGURE*)+sizeof(DABUDAI)+sizeof(nConnectSocket)+sizeof(int));
			pthread_t nThreadId;
			nIp = sctClientAddr.sin_addr.s_addr;
			nNum = (nNum+1)%10000;
			memcpy(sArg, &nIp, sizeof(nIp));
			memcpy(sArg+4, &pConf, sizeof(char*));
			memcpy(sArg+4+sizeof(char*), &pDabudai, sizeof(char*));
			memcpy(sArg+4+2*sizeof(char*), &nConnectSocket, sizeof(nConnectSocket));
			memcpy(sArg+8+2*sizeof(char*), &nNum, 4);
			
			if(pthread_create(&nThreadId, NULL, Thread_Session, sArg) != 0)
			{
				unsigned char *s = (unsigned char*)(&nIp);
				free(sArg);
				close(nConnectSocket);
				snprintf(sLog, sizeof(sLog), "[SYSTEM][WARN]can't generate thread to treat the quest from:<%d.%d.%d.%d>.\n",
																								s[0], s[1], s[2], s[3] );
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
			}	
		}
		
		/*
		nTmpTmpTmp++;
		
		if(nTmpTmpTmp > 3)
		{
			sleep(10);
			fclose(pDabudai->pFileLog);
			CloseDB(pDabudai->dbIpArea);	
			return 0;
		}
		*/
	}
	
	
	
	
	return 0;
}