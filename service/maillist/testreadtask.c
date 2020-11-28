#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <db.h>
#include "maillist7.h"
#include "c_error.h"


DB* OpenDB(char *sFileName, char *sDbName, int nPageSize, int nCacheSize, int (*Compare)(DB *, const DBT *, const DBT *))
{
	DB *db;
	int nRet = 0;
	int nFlag;
	
	nRet = db_create(&db, NULL, 0);
	if(nRet)
		return NULL;
	if(Compare)
	{
		nRet = db->set_bt_compare(db, Compare);
		if(nRet)
		{
			db->close(db, 0);
			printf("[[%s]]\n", db_strerror(nRet));	
			return NULL;
		}
	}
	nRet = db->set_pagesize(db, nPageSize*1024);
	if(nRet)
	{
		db->close(db, 0);
		printf("[[%s]]\n", db_strerror(nRet));
		return NULL;
	}
	nRet =  db->set_cachesize(db, 0, nCacheSize*1024*1024, 1);
	if(nRet)
	{	
		db->close(db, 0);
		printf("[[%s]]\n", db_strerror(nRet));
		return NULL;
	}
	nFlag = DB_CREATE;
	nRet = db->open(db, NULL, sFileName, sDbName, DB_BTREE, nFlag, 0);
	if(nRet)
	{
		printf("open fail.<<%s>>\n", db_strerror(nRet));
		db->close(db, 0);
		return NULL;
	}
	
	return db;
}


int CloseDB(DB *db)
{
	int nRet;
	nRet = db->close(db, 0);
	if(nRet)
		return -1;
	return 0;
}


int CheckMailList(char *sMailboxList, int nMailboxListUsed, char *sOffsetList, int nOffsetListUsed, DB *db, DBT *key, DBT *data, int ii)
{
	int nPos = 0;
	
	
	while(nPos < nOffsetListUsed)
	{
		char* nPos2;
		int nLeng;
		int i = 0;
		
		memcpy(&nLeng, sOffsetList+nPos, 4);
		nPos2 = sMailboxList + nLeng + 4;
		key->data = (nPos2);
		key->size = strlen(nPos2) + 1;
		i = db->put(db, NULL, key, data, DB_NOOVERWRITE);
		if(i == 0)
			printf("errorAAAAAAAAAA %d  AAAAAAAAAAAa:<<%s>>\n", ii, nPos2);
		nPos += 5;
	}
	return 0;
}


int ReadTaskFile2(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID)
{
	char sLine[1024];
	FILE *pIn;
	char **sMailboxList = (char**)malloc(sizeof(char*) * (pAlias->nDomainNum+1));
	char **sOffsetList = (char**)malloc(sizeof(char*) * (pAlias->nDomainNum+1));
	int *nMailboxListSize = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nMailboxListUsed = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nOffsetListSize = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nOffsetListUsed = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nCount = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int nParamName[20];
	char sParamLine[1024];
	int nStat = 0;
	
	{
		int i = 0;
		while(i < pAlias->nDomainNum + 1)
		{
			sMailboxList[i] = (char*)malloc(10240);
			sOffsetList[i] = (char*)malloc(1024);
			nMailboxListSize[i] = 10240;
			nMailboxListUsed[i] = 0;
			nOffsetListSize[i] = 1024;
			nOffsetListUsed[i] = 0;
			nCount[i] = 0;
			++i;
		}	
	}
	
	pIn = fopen(sFileName, "r");
	
	while(fgets(sLine, 1000, pIn))
	{
		if(strncmp(sLine, "<$ID>", 5) == 0)
		{
			int n[3];
			fgets(sLine, 1000, pIn);
			sscanf(sLine, "%d.%d.%d", &(n[0]), &(n[1]), &(n[2]));
			n[0] = htonl(n[0]);
			n[1] = htonl(n[1]);
			n[2] = htonl(n[2]);
			memcpy(sID, n, 12);
			while(fgets(sLine, 1000, pIn))
			{
				if(strncmp(sLine, "</$ID>", 6) == 0)
					break;	
			}
		}
		else if(strncmp(sLine, "<$MODULA>", 9) == 0)
		{
			char *s = malloc(1024);
			int nSize = 1024;
			int nOffset = 0;
			*sModula = s;
			while(fgets(sLine, 1000, pIn))
			{
				if(strncmp(sLine, "</$MODULA>", 10) == 0)
				{
					break;
				}
				if(nOffset + strlen(sLine) + 1 > nSize)
				{
					nSize += 1024;
					s = realloc(s, nSize);
					*sModula = s;	
				}		
				snprintf(s+nOffset, strlen(sLine)+1, "%s", sLine);
				nOffset += (strlen(sLine));
			}
		}
		else if(strncmp(sLine, "<$MAILLIST>", 11) == 0)
		{
			memset(nParamName, 0x0, 20*sizeof(int));
			fgets(sParamLine, 1024, pIn);
			{
				char *sTmp;
				int i = 1;
				
				EndLine(sParamLine);
				nParamName[0] = 0;
				sTmp = sParamLine;
				while(sTmp)
				{
					sTmp = strchr(sTmp, ',');
					if(!sTmp)
						break;
					*sTmp = 0;
					sTmp++;
					nParamName[i] = (sTmp - sParamLine);
					++i;
				}		
			}
			
			while(fgets(sLine, 1000, pIn))
			{
				char *sTmp = sLine;
				int i = 0;
				int nLeng = 0;
				int nOffset = 4;
				char *sPos;
				int nWhich = pAlias->nDomainNum;
				if(strncmp(sLine, "</$MAILLIST>", 12) == 0)
					break;
				{
					char *sAliasB = strchr(sLine, '@');
					char *sAliasE;
					if(!sAliasB)
						continue;
					sAliasE = strchr(sAliasB, ',');
					if(!sAliasE)
						continue;
					*sAliasE = 0;
					nWhich = FindAliasDomain(sAliasB+1, pAlias);
					if(nWhich < 0)
						nWhich = pAlias->nDomainNum; 
					*sAliasE = ',';	
				}
				
				if(nMailboxListSize[nWhich] - nMailboxListUsed[nWhich] < 2048)
				{
					(nMailboxListSize[nWhich]) += 10240;
					sMailboxList[nWhich] = (char*)realloc(sMailboxList[nWhich], nMailboxListSize[nWhich]);	
					memset(sMailboxList[nWhich]+nMailboxListUsed[nWhich]+10, 0xff, 9024);
				}
				
				sPos = sMailboxList[nWhich] + nMailboxListUsed[nWhich];
				
				EndLine(sLine);
				i = 0;
				while(sTmp && *sTmp)
				{
					char *sTmp2 = strchr(sTmp, ',');
					if(sTmp2)
						*sTmp2 = 0;
					if(i)
					{
						snprintf(sPos+nOffset, 200, "$%s$:", sParamLine + nParamName[i]);
						nOffset += (strlen(sParamLine+nParamName[i]) + 3);
					}
					snprintf(sPos + nOffset, 200, "%s", sTmp);
					nOffset += (strlen(sTmp)+1);
					
					if(sTmp2)
						sTmp = sTmp2+1;
					else
						sTmp = 0;
					++i;
				}
				nLeng = nOffset - 4;
				//nLeng = htonl(nLeng);
				memcpy(sPos, &nLeng, 4);
				if(nOffsetListSize[nWhich] - nOffsetListUsed[nWhich] < 10)
				{
					(nOffsetListSize[nWhich]) += 1024;
					sOffsetList[nWhich] = (char*)realloc(sOffsetList[nWhich], nOffsetListSize[nWhich]);	
					memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+10, 0xff, 1000);
				}
				memcpy(sOffsetList[nWhich]+nOffsetListUsed[nWhich], &(nMailboxListUsed[nWhich]), 4);
				memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+4, 0x0, 1);
				//if(KeepOrderFast(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich]) < 0)
				{
					int iii = 0;
					while(iii < nOffsetListUsed[nWhich])
					{
						int nPos;int l = 0;
						memcpy(&nPos, sOffsetList[nWhich]+iii, 4);
						memcpy(&l, sMailboxList[nWhich]+nPos, 4);
						//l = ntohl(l);
						if(l > 500)
						{
							printf("BBBBBBBBB%d<<%s>>BBBBB\n", l, sLine);
							break;
						}
						iii += 5;	
					}	
					
				}
				
				if(KeepOrderFast(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich]) < 0)	
					printf("LLLLLLLLLLLLL   -1 -1LLLLLLLLLLLL\n");
				{
					int iii = 0;
					while(iii < nOffsetListUsed[nWhich])
					{
						int nPos;int l = 0;
						memcpy(&nPos, sOffsetList[nWhich]+iii, 4);
						memcpy(&l, sMailboxList[nWhich]+nPos, 4);
						//l = ntohl(l);
						if(l > 500)
						{
							printf("AAAAAAAAAA%d<<%s>>AAAAAAAAAA\n", l, sLine);
							break;
						}
						iii += 5;	
					}	
					
				}
				(nOffsetListUsed[nWhich]) += 5;
				(nMailboxListUsed[nWhich]) += nOffset;
				(nCount[nWhich])++;
			}	
		}
	}
	fclose(pIn);
	
	{
		int i = 0;
		while(i < pAlias->nDomainNum+1)
		{
			MailList *pMailList = pMail+i;
			pMailList->sMailboxList = sMailboxList[i];
			pMailList->sOffsetList = sOffsetList[i];
			pMailList->nMailboxListSize = nMailboxListSize[i];
			pMailList->nMailboxListUsed = nMailboxListUsed[i];
			pMailList->nOffsetListSize = nOffsetListSize[i];
			pMailList->nOffsetListUsed = nOffsetListUsed[i];
			pMailList->nCount = nCount[i];
			pMailList->nSuccess = 0;
			pMailList->nDelay = 0;
			pMailList->nFail = 0;
			pMailList->nNowPos = 0;
			memcpy(pMailList->nParamName, nParamName, 20*sizeof(int));
			memcpy(pMailList->sParamLine, sParamLine, 1024);
			++i;
		}
	}
	free(sMailboxList);
	free(sOffsetList);
	free(nMailboxListSize);
	free(nMailboxListUsed);
	free(nOffsetListSize);
	free(nOffsetListUsed);
	free(nCount);
	
	return 0;
}




int main(int argc, char *argv[])
{
	FILE *pIn;
	DB *db;
	DBT dbtKey, dbtData;
	char sLine[1024];
	int nRet = 0;
	DomainAlias *pAlias = (DomainAlias*)malloc(sizeof(DomainAlias));
	MailList *pMail1, *pMail2, *pMail3, *pMail4; 
		
	memset(pAlias, 0x0, sizeof(DomainAlias));
	
	
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	db = OpenDB(NULL, NULL, 64, 512, NULL);
	
	pIn = fopen(argv[1], "r");
	while(fgets(sLine, sizeof(sLine), pIn))
	{
		char *sTmp;
		if(strchr(sLine, '@') == NULL)
			continue;	
		sTmp = strchr(sLine, ',');
		if(!sTmp)
			sTmp = strchr(sLine, '\r');
		if(!sTmp)
			sTmp = strchr(sLine, '\n');
		if(sTmp)
			*sTmp = 0;
		dbtKey.data = sLine;
		dbtKey.size = strlen(sLine) + 1;
		dbtData.data = NULL;
		dbtData.size = 0;
		nRet = db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
		if(nRet)
			printf("error!!!<%s>AAAAAAAAAAAAAAAAAAAAA\n");
	}
	fclose(pIn);
	
	pIn = fopen(argv[2], "r");
	while(fgets(sLine, sizeof(sLine), pIn))
	{
		char *sTmp;
		if(strchr(sLine, '@') == NULL)
			continue;	
		sTmp = strchr(sLine, ',');
		if(!sTmp)
			sTmp = strchr(sLine, '\r');
		if(!sTmp)
			sTmp = strchr(sLine, '\n');
		if(sTmp)
			*sTmp = 0;
		dbtKey.data = sLine;
		dbtKey.size = strlen(sLine) + 1;
		dbtData.data = NULL;
		dbtData.size = 0;
		nRet = db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
		if(nRet)
			printf("error!!!<%s>AAAAAAAAAAAAAAAAAAAAA\n");
	}
	fclose(pIn);
	
	int i = 0;
	
	while(i < 4)
	{
		++i;
	{
		char sID[12];
		char *sModula;
		MailList *pMail;
		pMail1 = (MailList*)malloc(sizeof(MailList)); 
		pMail = pMail1;
		pMail->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		ReadTaskFile2(argv[1], pMail, pAlias, &sModula, sID);
		CheckMailList(pMail->sMailboxList, pMail->nMailboxListUsed, pMail->sOffsetList, pMail->nOffsetListUsed, db, &dbtKey, &dbtData, i);
		free(sModula);
	}
	
	{
		char sID[12];
		char *sModula;
		MailList *pMail;
		pMail2 = (MailList*)malloc(sizeof(MailList));
		pMail = pMail2;
		pMail->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		ReadTaskFile2(argv[2], pMail, pAlias, &sModula, sID);
		CheckMailList(pMail->sMailboxList, pMail->nMailboxListUsed, pMail->sOffsetList, pMail->nOffsetListUsed, db, &dbtKey, &dbtData, i);
		free(sModula);	
	}
	
	{
		char sID[12];
		char *sModula;
		MailList *pMail;
		pMail3 = (MailList*)malloc(sizeof(MailList)); 
		pMail = pMail3;
		pMail->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		ReadTaskFile2(argv[1], pMail, pAlias, &sModula, sID);
		CheckMailList(pMail->sMailboxList, pMail->nMailboxListUsed, pMail->sOffsetList, pMail->nOffsetListUsed, db, &dbtKey, &dbtData, i);
		free(sModula);
	}
	
	{
		char sID[12];
		char *sModula;
		MailList *pMail;
		pMail4 = (MailList*)malloc(sizeof(MailList)); 
		pMail = pMail4;
		pMail->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		
		ReadTaskFile2(argv[2], pMail, pAlias, &sModula, sID);
		CheckMailList(pMail->sMailboxList, pMail->nMailboxListUsed, pMail->sOffsetList, pMail->nOffsetListUsed, db, &dbtKey, &dbtData, i);
		free(sModula);
	}
	FreeMailList(pMail1);
	free(pMail1);
	FreeMailList(pMail2);
	free(pMail2);
	FreeMailList(pMail3);
	free(pMail3);
	FreeMailList(pMail4);
	free(pMail4);
	printf("%d end\n", i);
	}
	
	return 0;
}











