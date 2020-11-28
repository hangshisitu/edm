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
#include "conf.h"
#include "writelog.h"
#include "iparea.h"
#include "socket_tool.h"


#define _H_version "findyouallovertheworld version 1.0."





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



int GetArgument(char *sMsg, char *sID, int *nMode)
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
	
	sTmp = strstr(sMsg, "mode=");
	if(sTmp)
	{
		sTmp2 = strchr(sTmp, '&');
		if(!sTmp2)
			sTmp2 = strchr(sTmp, ' ');
		if(!sTmp2)
			return -1;
		c = *sTmp2;
		*sTmp2 = 0;
		sTmp += 5;
		if(strncmp(sTmp, "head", 4) == 0)
			*nMode = 1;
		//snprintf(sID, 100, "%s", sTmp);
		*sTmp2 = c;	
	}
	
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


int FindFile(char *sTaskID, int *nNumber, int *nYear, int *nMonth, int *nDay, CONFIGURE *pConf)
{
	*nNumber = 0;
	
	while(*nNumber < pConf->nScanNum)
	{
		char sFileName[100];
		struct stat sctFileStat;
		
		snprintf(sFileName, 100, "%s/%s.task", (pConf->sTaskDir)[*nNumber], sTaskID);
		printf("<<<<%s>>>>%d\n", sFileName, *nNumber);
		if(stat(sFileName, &sctFileStat) == 0)
		{
			struct tm sctTime;
			localtime_r(&(sctFileStat.st_mtime), &sctTime);
			*nYear = sctTime.tm_year+1900;
			*nMonth = sctTime.tm_mon+1;
			*nDay = sctTime.tm_mday;
			return 0;	
		}
		snprintf(sFileName, 100, "%s/%s.task", (pConf->sBackupDir)[*nNumber], sTaskID);
		printf("<<<<%s>>>>%d\n", sFileName, *nNumber);
		if(stat(sFileName, &sctFileStat) == 0)
		{
			struct tm sctTime;
			localtime_r(&(sctFileStat.st_mtime), &sctTime);
			*nYear = sctTime.tm_year+1900;
			*nMonth = sctTime.tm_mon+1;
			*nDay = sctTime.tm_mday;
			return 0;	
		}
		
		(*nNumber)++;	
	}
	return 0;
}



int ReadMailBox(char *sTaskID, DB *db, int nNumber, CONFIGURE *pConf)
{
	DBT dbtKey, dbtData;
	char sLine[1024];
	char sFileName[100];
	int nFound = 0;
	FILE *pFile;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	snprintf(sFileName, 100, "%s/%s.task", (pConf->sTaskDir)[nNumber], sTaskID);
	if(access(sFileName, F_OK) == 0)
	{
		pFile = fopen(sFileName, "r");	
	}
	else
	{
		snprintf(sFileName, 100, "%s/%s.task", (pConf->sBackupDir)[nNumber], sTaskID);
		pFile = fopen(sFileName, "r");	
	}
	printf("open<<<%s>>>\n", sFileName);
	
	while(pFile && fgets(sLine, sizeof(sLine), pFile))
	{
		if(strstr(sLine, "<$MAILLIST>"))	
		{
			nFound = 1;
			break;	
		}
	}
	
	if(nFound)
	{
		while(fgets(sLine, sizeof(sLine), pFile))
		{
			char *sTmp;
			if(strchr(sLine, '@') == NULL)
				continue;
			sTmp = strchr(sLine, ',');
			if(sTmp)
				*sTmp = 0;
			else
			{
				sTmp = strchr(sLine, '\r');
				if(!sTmp)
					sTmp = strchr(sLine, '\n');
				if(sTmp)
					*sTmp = 0;
			}
			sTmp = sLine;
			while(*sTmp && (*sTmp == ' ' || *sTmp == '\t'))
				sTmp++;
			dbtKey.data = sTmp;
			dbtKey.size = strlen(sTmp)+1;
			
			db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
		}	
	}
	
	if(pFile)
		fclose(pFile);
	
	return 0;
}


int ScanDeliverLog(char *sTaskID, DB *db, int nNumber, CONFIGURE *pConf, DABUDAI *pDabudai, int nYear, int nMonth, int nDay)
{
	char sLine[10240];
	int nFinish = 0;
	DBT dbtKey, dbtData;
	DBC *dbc;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	
	while(1)
	{
		if(nFinish)
			break;
		if(nYear > pDabudai->nYear)
			break;	
		else if(nYear == pDabudai->nYear && nMonth > pDabudai->nMonth)
			break;	
		else if(nYear == pDabudai->nYear && nMonth == pDabudai->nMonth && nDay > pDabudai->nDay)
			break;
		
		{
			FILE *pFile;
			char sFileName[100];
			int n = 0;
			char sTag[50];
			char sTag2[50];
			char sDay[20];
			char sTime[20];
			char sData[250];
			
			snprintf(sTag, 50, "[%s]", sTaskID);
			snprintf(sTag2, 50, "<%s>", sTaskID);
			snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);
			snprintf(sDay, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			while((pFile = fopen(sFileName, "r")))
			{
				while(fgets(sLine, 10240, pFile))
				{
					char *sTmp, *sTmp2, *sTmp3;
					int nStat;
					sTmp = strstr(sLine, "][");
					if(!sTmp)
						continue;
					sTmp += 2;
					if(strncmp(sTmp, "SYSTEM", 6) == 0 && strstr(sTmp, "delete task "))
					{
						if(strstr(sTmp, sTag2))
						{
							nFinish = 1;
							break;	
						}
					}
					if(strncmp(sTmp, "REPORT", 6))
						continue;	
					sTmp2 = strchr(sTmp, '\r');
					if(!sTmp2)
						sTmp2 = strchr(sTmp, '\n');
					if(sTmp2)
						*sTmp2 = 0;
					sTmp2 = strchr(sLine, ']');
					if(!sTmp2)
						continue;
					*sTmp2 = 0;
					snprintf(sTime, 20, "%s", sLine+1);
					sTmp = strstr(sTmp, sTag);
					while(sTmp)
					{
						char c = 0;
						int nRet = 0;
						char *sTmp4;
						sTmp2 = sTmp - 2;
						if(*sTmp2 == 's')
						{
							c = 's';
						}	
						else if(*sTmp2 == 'd')
						{
							c = 'd';
						}
						else
						{
							c = 'f';
						}
						sTmp2 = strchr(sTmp, ']');
						if(!sTmp2)
							break;
						sTmp2++;
						sTmp3 = strchr(sTmp2, '[');
						if(sTmp3)
							*sTmp3 = 0;
						sTmp4 = strchr(sTmp2, '(');
						if(sTmp4)
						{
							*sTmp4 = 0;
							sTmp4++;
						}
						dbtKey.data = sTmp2;
						dbtKey.size = strlen(sTmp2)+1;
						
						nRet = dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET);
						if(nRet == 0 && dbtData.size == 250)
						{	
							char *s = (char*)(dbtData.data+40);
							if(*s != 's')
							{
								memcpy(dbtData.data, sDay, 20);
								memcpy(dbtData.data+20, sTime, 20);
								memcpy(dbtData.data+40, &c, 1);
								if(sTmp4)
									snprintf(dbtData.data+50, 200, "(%s", sTmp4);
								else
									memset(dbtData.data+50, 0, 10);
								dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);
							}
						}
						else if(nRet == 0)
						{
							memcpy(sData, sDay, 20);
							memcpy(sData+20, sTime, 20);
							memcpy(sData+40, &c, 1);
							if(sTmp4)
								snprintf(sData+50, 200, "(%s", sTmp4);
							else
								memset(sData+50, 0, 10);
							dbtData.size = 250;
							dbtData.data = sData;
							dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);	
						}
						if(sTmp3)
						{
							*sTmp3 = '[';
							sTmp = strstr(sTmp3, sTag);	
						}
						else
							sTmp = NULL;
					}
					
				}
				fclose(pFile);
				++n;
				if(n > 9999 || nFinish)
					break;
				snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);	
			}
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
	}
	
	dbc->c_close(dbc);
	return 0;
}




int PrintDeliver(DB *db, char cStat, int nSocket, char *sDir, int n)
{
	DBC *dbc;
	DBT dbtKey, dbtData;
	FILE *pFile;
	char sFileName[100];
	
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	db->cursor(db, NULL, &dbc, 0);
	snprintf(sFileName, 100, "%s/%d", sDir, n);
	pFile = fopen(sFileName, "w");
	fprintf(pFile, "mailbox,date,time,return\n");
	while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		char *s;
		if(dbtData.size < 250)
		{
			if(cStat == 'u')
			{
				fprintf(pFile, "%s,0000-00-00,00:00:00,(null)\n", dbtKey.data);	
			}
			continue;
		}
		s = (char*)(dbtData.data)+40;
		if(cStat == 'o')
		{
			//printf("\n000000000000\n");
			if(strstr((char*)(dbtData.data+50), "user doesn") || strstr((char*)(dbtData.data+50), "not exist") || strstr((char*)(dbtData.data+50), " unknown") || strstr((char*)(dbtData.data+50), "not found") 
										|| strstr((char*)(dbtData.data+50), "such user") || strstr((char*)(dbtData.data+50), "nvalid recipient") || strstr((char*)(dbtData.data+50), " Unknown") || 
										strstr((char*)(dbtData.data+50), "unavailable") || strstr((char*)(dbtData.data+50), "is disable") || strstr((char*)(dbtData.data+50), "ddress rejected") ||
										strstr((char*)(dbtData.data+50), "such recipient") || strstr((char*)(dbtData.data+50), "no mailbox") || strstr((char*)(dbtData.data+50), "ccess denied"))
			{
				fprintf(pFile, "%s,%s,%s,%s\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50));
				//printf("\n11111111111\n");
			}	
			continue;	
		}
		if(cStat == 'p')
		{
			if(strstr((char*)(dbtData.data+50), "spam") || strstr((char*)(dbtData.data+50), "Spam"))
			{
				fprintf(pFile, "%s,%s,%s,%s\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50));
			}	
			continue;	
		}
		if(cStat == 'n')
		{
			if(strstr((char*)(dbtData.data+50), "step 1--") )
			{
				fprintf(pFile, "%s,%s,%s,%s\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50));
			}
			continue;		
		}
		if(*s != cStat)
			continue;
		fprintf(pFile, "%s,%s,%s,%s\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50));
	}
	dbc->c_close(dbc);
	fclose(pFile);
	
	{
		struct stat sctStat;
		char *sss = (char*)malloc(1024*1024);
		int nFinish = 0, nRead = 0;;	
		
		stat(sFileName, &sctStat);
		pFile = fopen(sFileName, "r");
		
		snprintf(sss, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=gbk\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
		SocketWrite(nSocket, 5, sss, strlen(sss));
		
		while(nFinish < sctStat.st_size)
		{
			if(sctStat.st_size - nFinish < 1024*1024)
				nRead = sctStat.st_size - nFinish;
			else
				nRead = 1024*1024;
			fread(sss, nRead, 1, pFile);
			WriteData(nSocket, sss, nRead, 5);
			nFinish += nRead;
		}
		fclose(pFile);
		free(sss);
	}
	
	
	return 0;
}



int ScanDeliverLog3(char *sTaskID, DB *db, int nNumber, CONFIGURE *pConf, DABUDAI *pDabudai, int nYear, int nMonth, int nDay)
{
	char sLine[10240];
	int nFinish = 0;
	DBT dbtKey, dbtData;
	DBC *dbc;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	
	while(1)
	{
		if(nFinish)
			break;
		if(nYear > pDabudai->nYear)
			break;	
		else if(nYear == pDabudai->nYear && nMonth > pDabudai->nMonth)
			break;	
		else if(nYear == pDabudai->nYear && nMonth == pDabudai->nMonth && nDay > pDabudai->nDay)
			break;
		
		{
			FILE *pFile;
			char sFileName[100];
			int n = 0;
			char sTag[50];
			char sTag2[50];
			char sDay[20];
			char sTime[20];
			char sData[270];
			
			snprintf(sTag, 50, "[%s]", sTaskID);
			snprintf(sTag2, 50, "<%s>", sTaskID);
			snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);
			snprintf(sDay, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			while((pFile = fopen(sFileName, "r")))
			{
				while(fgets(sLine, 10240, pFile))
				{
					char *sTmp, *sTmp2, *sTmp3;
					int nStat;
					char sDeliverIp[20];
					sDeliverIp[0] = 0;
					{
						char *s1, *s2;
						s1 = strstr(sLine, "] [");
						if(s1)
						{
							int n1, n2, n3, n4;
							s1 += 3;
							sscanf(s1, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
							snprintf(sDeliverIp, 20, "%d.%d.%d.%d", n1, n2, n3, n4);
						}
						//[17:52:24] [72.46.140.211.06535]	
					}
					sTmp = strstr(sLine, "][");
					if(!sTmp)
						continue;
					sTmp += 2;
					if(strncmp(sTmp, "SYSTEM", 6) == 0 && strstr(sTmp, "delete task "))
					{
						if(strstr(sTmp, sTag2))
						{
							nFinish = 1;
							break;	
						}
					}
					if(strncmp(sTmp, "REPORT", 6))
						continue;	
					sTmp2 = strchr(sTmp, '\r');
					if(!sTmp2)
						sTmp2 = strchr(sTmp, '\n');
					if(sTmp2)
						*sTmp2 = 0;
					sTmp2 = strchr(sLine, ']');
					if(!sTmp2)
						continue;
					*sTmp2 = 0;
					snprintf(sTime, 20, "%s", sLine+1);
					sTmp = strstr(sTmp, sTag);
					while(sTmp)
					{
						char c = 0;
						int nRet = 0;
						char *sTmp4;
						sTmp2 = sTmp - 2;
						if(*sTmp2 == 's')
						{
							c = 's';
						}	
						else if(*sTmp2 == 'd')
						{
							c = 'd';
						}
						else
						{
							c = 'f';
						}
						sTmp2 = strchr(sTmp, ']');
						if(!sTmp2)
							break;
						sTmp2++;
						sTmp3 = strchr(sTmp2, '[');
						if(sTmp3)
							*sTmp3 = 0;
						sTmp4 = strchr(sTmp2, '(');
						if(sTmp4)
						{
							*sTmp4 = 0;
							sTmp4++;
						}
						dbtKey.data = sTmp2;
						dbtKey.size = strlen(sTmp2)+1;
						
						nRet = dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET);
						if(nRet == 0 && dbtData.size == 270)
						{	
							char *s = (char*)(dbtData.data+40);
							if(*s != 's')
							{
								memcpy(dbtData.data, sDay, 20);
								memcpy(dbtData.data+20, sTime, 20);
								memcpy(dbtData.data+40, &c, 1);
								if(sTmp4)
									snprintf(dbtData.data+50, 200, "(%s", sTmp4);
								else
									memset(dbtData.data+50, 0, 10);
								memcpy(dbtData.data+250, sDeliverIp, 20);
								dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);
							}
						}
						else if(nRet == 0)
						{
							memcpy(sData, sDay, 20);
							memcpy(sData+20, sTime, 20);
							memcpy(sData+40, &c, 1);
							if(sTmp4)
								snprintf(sData+50, 200, "(%s", sTmp4);
							else
								memset(sData+50, 0, 10);
							memcpy(sData+250, sDeliverIp, 20);
							dbtData.size = 270;
							dbtData.data = sData;
							dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);	
						}
						if(sTmp3)
						{
							*sTmp3 = '[';
							sTmp = strstr(sTmp3, sTag);	
						}
						else
							sTmp = NULL;
					}
					
				}
				fclose(pFile);
				++n;
				if(n > 9999 || nFinish)
					break;
				snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);	
			}
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
	}
	
	dbc->c_close(dbc);
	return 0;
}




int PrintDeliver3(DB *db, char cStat, int nSocket, char *sDir, int n)
{
	DBC *dbc;
	DBT dbtKey, dbtData;
	FILE *pFile;
	char sFileName[100];
	
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	db->cursor(db, NULL, &dbc, 0);
	snprintf(sFileName, 100, "%s/%d", sDir, n);
	pFile = fopen(sFileName, "w");
	fprintf(pFile, "mailbox,date,time,return,deliverip\n");
	while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		char *s;
		if(dbtData.size < 250)
		{
			if(cStat == 'u')
			{
				fprintf(pFile, "%s,0000-00-00,00:00:00,(null)\n", dbtKey.data);	
			}
			continue;
		}
		s = (char*)(dbtData.data)+40;
		if(*s != cStat)
			continue;
		fprintf(pFile, "%s,%s,%s,%s,%s\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50), (char*)(dbtData.data+250));
	}
	dbc->c_close(dbc);
	fclose(pFile);
	
	{
		struct stat sctStat;
		char *sss = (char*)malloc(1024*1024);
		int nFinish = 0, nRead = 0;;	
		
		stat(sFileName, &sctStat);
		pFile = fopen(sFileName, "r");
		
		snprintf(sss, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=gbk\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
		SocketWrite(nSocket, 5, sss, strlen(sss));
		
		while(nFinish < sctStat.st_size)
		{
			if(sctStat.st_size - nFinish < 1024*1024)
				nRead = sctStat.st_size - nFinish;
			else
				nRead = 1024*1024;
			fread(sss, nRead, 1, pFile);
			WriteData(nSocket, sss, nRead, 5);
			nFinish += nRead;
		}
		fclose(pFile);
		free(sss);
	}
	
	
	return 0;
}



int ScanDeliverLog2(char *sTaskID, DB *db, int nNumber, CONFIGURE *pConf, DABUDAI *pDabudai, int nYear, int nMonth, int nDay)
{
	char sLine[10240];
	int nFinish = 0;
	DBT dbtKey, dbtData;
	//DBC *dbc;
	int nSum = 0;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	//db->cursor(db, NULL, &dbc, 0);
	
	
	while(nSum < 100)
	{
		if(nFinish)
			break;
		if(nYear > pDabudai->nYear)
			break;	
		else if(nYear == pDabudai->nYear && nMonth > pDabudai->nMonth)
			break;	
		else if(nYear == pDabudai->nYear && nMonth == pDabudai->nMonth && nDay > pDabudai->nDay)
			break;
		
		{
			FILE *pFile;
			char sFileName[100];
			int n = 0;
			char sTag[50];
			char sTag2[50];
			char sDay[20];
			char sTime[20];
			char sData[250];
			
			snprintf(sTag, 50, "[%s]", sTaskID);
			snprintf(sTag2, 50, "<%s>", sTaskID);
			snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);
			snprintf(sDay, 20, "%04d-%02d-%02d", nYear, nMonth, nDay);
			while((pFile = fopen(sFileName, "r")) && nSum < 100)
			{
				while(fgets(sLine, 10240, pFile) && nSum < 100)
				{
					char *sTmp, *sTmp2, *sTmp3;
					int nStat;
					sTmp = strstr(sLine, "][");
					if(!sTmp)
						continue;
					sTmp += 2;
					if(strncmp(sTmp, "SYSTEM", 6) == 0 && strstr(sTmp, "delete task "))
					{
						if(strstr(sTmp, sTag2))
						{
							nFinish = 1;
							break;	
						}
					}
					if(nSum >= 100)
					{
						nFinish = 1;
						break;
					}
					if(strncmp(sTmp, "REPORT", 6))
						continue;	
					sTmp2 = strchr(sTmp, '\r');
					if(!sTmp2)
						sTmp2 = strchr(sTmp, '\n');
					if(sTmp2)
						*sTmp2 = 0;
					sTmp2 = strchr(sLine, ']');
					if(!sTmp2)
						continue;
					*sTmp2 = 0;
					snprintf(sTime, 20, "%s", sLine+1);
					sTmp = strstr(sTmp, sTag);
					while(sTmp && nSum < 100)
					{
						char c = 0;
						int nRet = 0;
						char *sTmp4;
						sTmp2 = sTmp - 2;
						
						nSum++;
						if(*sTmp2 == 's')
						{
							c = 's';
						}	
						else if(*sTmp2 == 'd')
						{
							c = 'd';
						}
						else
						{
							c = 'f';
						}
						sTmp2 = strchr(sTmp, ']');
						if(!sTmp2)
							break;
						sTmp2++;
						sTmp3 = strchr(sTmp2, '[');
						if(sTmp3)
							*sTmp3 = 0;
						sTmp4 = strchr(sTmp2, '(');
						if(sTmp4)
						{
							*sTmp4 = 0;
							sTmp4++;
						}
						dbtKey.data = sTmp2;
						dbtKey.size = strlen(sTmp2)+1;
						
						//nRet = dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET);
						/*
						if(nRet == 0 && dbtData.size == 250)
						{	
							char *s = (char*)(dbtData.data+40);
							if(*s != 's')
							{
								memcpy(dbtData.data, sDay, 20);
								memcpy(dbtData.data+20, sTime, 20);
								memcpy(dbtData.data+40, &c, 1);
								if(sTmp4)
									snprintf(dbtData.data+50, 200, "(%s", sTmp4);
								else
									memset(dbtData.data+50, 0, 10);
								dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);
							}
						}
						*/
						//else if(nRet == 0)
						{
							memcpy(sData, sDay, 20);
							memcpy(sData+20, sTime, 20);
							memcpy(sData+40, &c, 1);
							if(sTmp4)
								snprintf(sData+50, 200, "(%s", sTmp4);
							else
								memset(sData+50, 0, 10);
							dbtData.size = 250;
							dbtData.data = sData;
							db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
						}
						
						if(sTmp3)
						{
							*sTmp3 = '[';
							sTmp = strstr(sTmp3, sTag);	
						}
						else
							sTmp = NULL;
					}
					
				}
				fclose(pFile);
				++n;
				if(n > 9999 || nFinish)
					break;
				snprintf(sFileName, 100, "%s/deliver.log.%04d%02d%02d.%04d", (pConf->sLogDir)[nNumber], nYear, nMonth, nDay, n);	
			}
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
	}
	
	//dbc->c_close(dbc);
	return 0;
}




int PrintDeliver2(DB *db, char cStat, int nSocket, char *sDir, int n)
{
	DBC *dbc;
	DBT dbtKey, dbtData;
	FILE *pFile;
	char sFileName[100];
	
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	db->cursor(db, NULL, &dbc, 0);
	snprintf(sFileName, 100, "%s/%d", sDir, n);
	pFile = fopen(sFileName, "w");
	fprintf(pFile, "mailbox,date,time,return\r\n");
	while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		char *s;
		/*
		if(dbtData.size < 250)
		{
			if(cStat == 'u')
			{
				fprintf(pFile, "%s,0000-00-00,00:00:00,(null)\r\n", dbtKey.data);	
			}
			continue;
		}
		s = (char*)(dbtData.data)+40;
		if(*s != cStat)
			continue;
		*/
		fprintf(pFile, "%s,%s,%s,%s\r\n", dbtKey.data, (char*)(dbtData.data), (char*)(dbtData.data+20), (char*)(dbtData.data+50));
	}
	dbc->c_close(dbc);
	fclose(pFile);
	
	{
		struct stat sctStat;
		char *sss = (char*)malloc(1024*1024);
		int nFinish = 0, nRead = 0;;	
		
		stat(sFileName, &sctStat);
		pFile = fopen(sFileName, "r");
		
		snprintf(sss, 1024, "HTTP/1.0 200 OK\r\nContent-Type: text/plain; charset=gbk\r\nVary: Accept-Encoding\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", sctStat.st_size);
		SocketWrite(nSocket, 5, sss, strlen(sss));
		
		while(nFinish < sctStat.st_size)
		{
			if(sctStat.st_size - nFinish < 1024*1024)
				nRead = sctStat.st_size - nFinish;
			else
				nRead = 1024*1024;
			fread(sss, nRead, 1, pFile);
			WriteData(nSocket, sss, nRead, 5);
			nFinish += nRead;
		}
		fclose(pFile);
		free(sss);
	}
	
	
	return 0;
}




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
	DB *db = OpenDB(NULL, NULL, 64, 512, NULL);
	
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
		CloseDB(db);
		return (void*)0;
	}
	
	
	
	{
		char *sTmp;
		char sID[100];
		int nStat = 0;
		char sDate[20];
		int nMode = 0;
		
		sTmp = strstr(sMsg, "GET");
		if(sTmp)
		{
			sTmp = strchr(sTmp, '/');	
		}
		if(!sTmp)
		{
			snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find GET or \"/\".\n");
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			close(nSocket);
			free(sMsg);
			CloseDB(db);
			return (void*)0;	
		}	
		printf("<<<<%s>>>>\n", sTmp);
		if(strncasecmp(sTmp+1, "success", 7) == 0 && strstr(sTmp+1, "success2") == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 's', nSocket, pConf->sSaveDir, n);
				}
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 's', nSocket, pConf->sSaveDir, n);	
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;		
			}
		}	
		else if(strncasecmp(sTmp+1, "success2", 8) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog3(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver3(db, 's', nSocket, pConf->sSaveDir, n);
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;		
			}
		}	
		else if(strncasecmp(sTmp+1, "delay", 5) == 0 && strstr(sTmp+1, "delay2") == NULL)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'd', nSocket, pConf->sSaveDir, n);
				}
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 's', nSocket, pConf->sSaveDir, n);	
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
				
		}
		else if(strncasecmp(sTmp+1, "delay2", 6) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog3(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver3(db, 'd', nSocket, pConf->sSaveDir, n);
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;		
			}
		}	
		else if(strncasecmp(sTmp+1, "fail", 4) == 0 && strstr(sTmp+1, "fail2") == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'f', nSocket, pConf->sSaveDir, n);
				}
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 's', nSocket, pConf->sSaveDir, n);		
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			
		}
		else if(strncasecmp(sTmp+1, "fail2", 5) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog3(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver3(db, 'f', nSocket, pConf->sSaveDir, n);
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;		
			}
		}	
		else if(strncasecmp(sTmp+1, "undo", 4) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'u', nSocket, pConf->sSaveDir, n);
				}
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 's', nSocket, pConf->sSaveDir, n);		
				}
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			
		}
		else if(strncasecmp(sTmp+1, "notfound", 8) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'o', nSocket, pConf->sSaveDir, n);
				}
				/*
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 'o', nSocket, pConf->sSaveDir, n);		
				}
				*/
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			
		}
		else if(strncasecmp(sTmp+1, "spam", 4) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'p', nSocket, pConf->sSaveDir, n);
				}
				/*
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 'o', nSocket, pConf->sSaveDir, n);		
				}
				*/
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			
		}
		else if(strncasecmp(sTmp+1, "dns", 3) == 0)
		{
			if(GetArgument(sMsg, sID, &nMode) >= 0)
			{	
				int nNumber = 0, nYear = 0, nMonth = 0, nDay = 0;
				FindFile(sID, &nNumber, &nYear, &nMonth, &nDay, pConf);
				if(nMode == 0)
				{
					ReadMailBox(sID, db, nNumber, pConf);
					ScanDeliverLog(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver(db, 'n', nSocket, pConf->sSaveDir, n);
				}
				/*
				else
				{
					ScanDeliverLog2(sID, db, nNumber, pConf, pDabudai, nYear, nMonth, nDay);
					PrintDeliver2(db, 'o', nSocket, pConf->sSaveDir, n);		
				}
				*/
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			else
			{
				snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]get argument fail.\n");
				WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				close(nSocket);
				free(sMsg);
				CloseDB(db);
				return (void*)0;	
			}
			
		}
		else
		{
			snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can recognize command.\n");
			WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			close(nSocket);
			free(sMsg);
			CloseDB(db);
			return (void*)0;		
		}
	}
	
	close(nSocket);
	free(sMsg);
	CloseDB(db);
	
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
	
	InitialDaBuDai(pDabudai);
	InitialConf(pConf);
	
	//pDabudai->dbIpArea = OpenDB(NULL, NULL, 64, 512, CompareIp);
	//ReadIpArea(pDabudai->dbIpArea, pConf->sIpAreaFile);
	
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
			fflush(NULL);
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
	}
	
	
	
	
	return 0;
}










