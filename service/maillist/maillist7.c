#include "maillist7.h"
#include "maillist_daemon.h"
#include "daemon_conf.h"
#include "writelog.h"
#include "yxlist.h"


static inline int CompareMailbox(char *sMailbox1, char *sMailbox2)
{
	if(sMailbox1[0] != sMailbox2[0])
		return sMailbox1[0] - sMailbox2[0];	
	if(sMailbox1[2]+sMailbox1[5] != sMailbox2[2]+sMailbox2[5])
		return (sMailbox1[2]+sMailbox1[5] - (sMailbox2[2]+sMailbox2[5]));	
	if(sMailbox1[4] != sMailbox2[4])
		return (sMailbox1[4]-sMailbox2[4] );
	return strcmp(sMailbox1, sMailbox2);
	
}





/*
static inline int CompareMailbox(char *sMailbox1, char *sMailbox2)
{
	
	return strcmp(sMailbox1, sMailbox2);	
}
*/






int KeepOrder(char *sOffset, int nUsed, char *sList)
{
	int nPrev = nUsed-5;
	int nThis = nUsed;
	
	while(nPrev >= 0)
	{
		char s[5];
		int nPosPrev, nPosThis;
		memcpy(&nPosPrev, sOffset+nPrev, 4);
		memcpy(&nPosThis, sOffset+nThis, 4);
		if(CompareMailbox(sList+nPosPrev+4, sList+nPosThis+4) < 0)
			break;
		memcpy(s, sOffset+nThis, 5);
		memcpy(sOffset+nThis, sOffset+nPrev, 5);
		memcpy(sOffset+nPrev, s, 5);
		nThis = nPrev;
		nPrev -= 5;
	}
	return 0;
}


int PrintMailList(char *sMailboxList, int nMailboxListUsed, char *sOffsetList, int nOffsetListUsed)
{
	int nPos = 0;
	
	while(nPos < nOffsetListUsed)
	{
		char* nPos2;
		int nLeng;
		int i = 0;
		
		memcpy(&nLeng, sOffsetList+nPos, 4);
		nPos2 = sMailboxList + nLeng + 4;
		memcpy(&nLeng, nPos2-4, 4);
		while(i < nLeng)
		{
			printf("%s,", nPos2+i);
			i = i + (strlen(nPos2+i)+1);	
		}
		printf("\n");
		nPos += 5;
	}
	return 0;
}


int PrintMailList2(char *sMailboxList, int nMailboxListUsed, char *sOffsetList, int nOffsetListUsed)
{
	int nPos = 0;
	
	while(nPos < nOffsetListUsed)
	{
		char* nPos2;
		int nLeng;
		int i = 0;
		
		memcpy(&nLeng, sOffsetList+nPos, 4);
		nPos2 = sMailboxList + nLeng + 4;
		memcpy(&nLeng, nPos2-4, 4);
		if(*(sOffsetList+nPos+4) == D_MailList_Success)
			printf("[s]");
		else if(*(sOffsetList+nPos+4) == D_MailList_Delay)
			printf("[d]");
		else if(*(sOffsetList+nPos+4) == D_MailList_Fail)
			printf("[f]");
		else
			printf("[o]");
		while(i < nLeng)
		{
			printf("%s,", nPos2+i);
			i = i + (strlen(nPos2+i)+1);	
		}
		printf("\n");
		nPos += 5;
	}
	return 0;
}



int ReadMailList(char *sFileName, MailList *pMailList)
{
	char sLine[1024];
	FILE *pIn;
	char *sMailboxList = (char*)malloc(10240);
	char *sOffsetList = (char*)malloc(1024);
	int nMailboxListSize = 10240;
	int nMailboxListUsed = 0;
	int nOffsetListSize = 1024;
	int nOffsetListUsed = 0;
	int nParamName[20];
	char sParamLine[1024];
	
	pIn = fopen(sFileName, "r");
	
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
		
		if(nMailboxListSize - nMailboxListUsed < 2048)
		{
			nMailboxListSize += 10240;
			sMailboxList = (char*)realloc(sMailboxList, nMailboxListSize);	
		}
		
		sPos = sMailboxList + nMailboxListUsed;
		
		EndLine(sLine);
		while(sTmp && *sTmp)
		{
			char *sTmp2 = strchr(sTmp, ',');
			if(sTmp2)
				*sTmp2 = 0;
			if(i)
			{
				snprintf(sPos+nOffset, 400, "$%s$", sParamLine + nParamName[i]);
				nOffset += (strlen(sParamLine+nParamName[i]) + 2);
			}
			snprintf(sPos + nOffset, 400, "%s", sTmp);
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
		if(nOffsetListSize - nOffsetListUsed < 10)
		{
			nOffsetListSize += 1024;
			sOffsetList = (char*)realloc(sOffsetList, nOffsetListSize);	
		}
		memcpy(sOffsetList+nOffsetListUsed, &nMailboxListUsed, 4);
		memset(sOffsetList+nOffsetListUsed+4, 0x0, 1);
		KeepOrder(sOffsetList, nOffsetListUsed, sMailboxList);
		nOffsetListUsed += 5;
		nMailboxListUsed += nOffset;
		
	}
	
	fclose(pIn);
	
	pMailList->sMailboxList = sMailboxList;
	pMailList->sOffsetList = sOffsetList;
	pMailList->nMailboxListSize = nMailboxListSize;
	pMailList->nMailboxListUsed = nMailboxListUsed;
	pMailList->nOffsetListSize = nOffsetListSize;
	pMailList->nOffsetListUsed = nOffsetListUsed;
	memcpy(pMailList->nParamName, nParamName, 20*sizeof(int));
	memcpy(pMailList->sParamLine, sParamLine, 1024);
	
	
	return 0;
}


int ReadDomainAlias(char *sFileName, DomainAlias *p)
{
	char sLine[1024];
	FILE *pIn;
	
	pIn = fopen(sFileName, "r");
	if(pIn == NULL)
		return 0;
	while(fgets(sLine, sizeof(sLine), pIn))
	{
		char *sTmp;
		char *sAlias;
		char *sDomain;
		int i;
		char *sPos;
		if(TreatConfLine(sLine, &sTmp))
			continue;
		p->sAlias = (char*)realloc(p->sAlias, (p->nAliasNum + 1) * 34);
		sPos = p->sAlias + (p->nAliasNum)*34;
		snprintf(sPos, 30, "%s", sLine);
		i = 0;
		while(i < p->nDomainNum)
		{
			sDomain = p->sDomain + 34*i;
			if(strcmp(sDomain, sTmp) == 0)
				break;
			++i;	
		}
		if(i == p->nDomainNum)
		{
			p->sDomain = (char*)realloc(p->sDomain, (p->nDomainNum+1)*34);	
			snprintf(p->sDomain + p->nDomainNum * 34, 30, "%s", sTmp);
			(p->nDomainNum)++;
		}
		memcpy(sPos+30, &i, 4);
		(p->nAliasNum)++;
		i = p->nAliasNum - 1;
		while(i > 0)
		{
			char sBak[34];
			if(strcmp(p->sAlias + i*34, p->sAlias + (i-1)*34) > 0)
				break;
			memcpy(sBak, p->sAlias+i*34, 34);
			memcpy(p->sAlias+i*34, p->sAlias+(i-1)*34, 34);
			memcpy(p->sAlias+(i-1)*34, sBak, 34);			
			--i;	
		}
	}
		
	fclose(pIn);
	return 0;
}


int PrintAlias(DomainAlias *p)
{
	int i = 0;
	int j = 0;
	
	while(i < p->nAliasNum)
	{
		printf("%s:", (p->sAlias)+34*i);
		memcpy(&j, p->sAlias+34*i+30, 4);
		if(j >= p->nDomainNum)
		{
			printf("domain error!\n");	
		}
		else
			printf("%s\n", p->sDomain+34*j);
		++i;
	}
	
	
	return 0;
}


int FindAliasDomain(char *sAlias, DomainAlias *p)
{
	int i = -1;
	int nB = 0;
	int nE = p->nAliasNum - 1;
	
	while(nB <= nE)
	{
		int nM = (nB+nE)/2;
		int k = strcmp(p->sAlias + nM*34, sAlias); //门户，每个占用34个字节，前面30个为域名，后4个没用
		if(k == 0)
		{
			memcpy(&i, p->sAlias + nM*34 + 30, 4);
			break;
		}
		else if(k > 0)
			nE = nM - 1;
		else
			nB = nM + 1;
	}
	
	return i;
}




int ReadMailListWithAlias(char *sFileName, MailList *pMail, DomainAlias *pAlias)
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
				snprintf(sPos+nOffset, 400, "$%s$", sParamLine + nParamName[i]);
				nOffset += (strlen(sParamLine+nParamName[i]) + 2);
			}
			snprintf(sPos + nOffset, 400, "%s", sTmp);
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
		}
		memcpy(sOffsetList[nWhich]+nOffsetListUsed[nWhich], &(nMailboxListUsed[nWhich]), 4);
		memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+4, 0x0, 1);
		KeepOrder(sOffsetList[nWhich], nOffsetListUsed[nWhich], sMailboxList[nWhich]);
		(nOffsetListUsed[nWhich]) += 5;
		(nMailboxListUsed[nWhich]) += nOffset;
		(nCount[nWhich])++;
		
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




int FreeMailList(MailList *p)
{
	free(p->sMailboxList);
	p->sMailboxList = 0;
	free(p->sOffsetList);
	p->sOffsetList = 0;
	//pthread_mutex_destroy(p->pLock);
	free(p->pLock);
	
	return 0;
}


int FreeDomainAlias(DomainAlias *p)
{
	free(p->sDomain);
	p->sDomain = 0;
	free(p->sAlias);
	p->sAlias = 0;
	return 0;
	
}

/*
static char* PopMailbox(MailList *p)
{
	int nPos;
	
	if(p->nCount == 0)
		return NULL;
		
	nPos = p->nNowPos;
	
	while(nPos < p->nOffsetUsed)
	{
		if(*(p->sOffset+nPos+4) != D_MailList_Success)
			
		nPos += 5;	
	}
	
	
	return NULL;
}
*/


static int AnswerMailboxRequest(char *sRequest, DomainAlias *pAlias, MailList *pList, char **sResult, int *nResultSize, 
													int *nResultLeng)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	
	*nResultLeng = 0;
	
	sTmp = sRequest;
	while(sTmp && *sTmp)
	{
		char *sTmpE;
		int nNum = 0;
		int nWhich = 0;
		sTmpE = strchr(sTmp, ':');	
		if(!sTmpE)
		{
			sTmp += strlen(sTmp);
			continue;	
		}
		nNum = atoi(sTmpE+1);
		*sTmpE = 0;
		nWhich = FindAliasDomain(sTmp, pAlias);	
		*sTmpE = ':';
		if(nWhich < 0)
		{
			nWhich = pAlias->nDomainNum;	
		}
		{
			MailList *p = pList + nWhich;
			int i = 0;
			while(i < nNum)
			{
				if(p->nCount == 0 || p->nRound > 1 || (p->nTime && tTime-(p->nTime) > 0))
					break;
				else
				{
					int nPos = p->nNowPos;
					if(p->nTime)
						p->nTime = 0;
					while(nPos < p->nOffsetListUsed && p->nRound < 2)
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success && *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 10 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							memcpy(*sResult + *nResultLeng, p->sMailboxList+nPos2, nLeng+4);
							*nResultLeng += (nLeng+4);
							p->nNowPos = nPos+5;	
							//(p->nCount)--;
							*(p->sOffsetList+nPos+4) = D_MailList_Deliver + p->nRound;
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						{
							int nPosTmp = 0;
							while(nPosTmp < p->nOffsetListUsed)
							{
								if(p->nRound == 0 && nPosTmp%3 != 0 && *(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
								{
									*(p->sOffsetList+nPosTmp+4)	= D_MailList_Success;
									(p->nCount)--;
								}	
								else if(p->nRound > 0 && nPosTmp%5 != 0 && *(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
								{
									*(p->sOffsetList+nPosTmp+4)	= D_MailList_Success;
									(p->nCount)--;
								}	
								else if(*(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
									*(p->sOffsetList+nPosTmp+4)	= 0;
								int nnn;
								memcpy(&nnn, p->sOffsetList+nPosTmp, 4);
								//printf("mailbox:%s,stat:%d\n", p->sMailboxList+nnn+4, *(p->sOffsetList+nPosTmp+4));
								nPosTmp += 5;
							}
						}
						(p->nRound)++;
						p->nTime = tTime;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						break;
					}
				}
			}	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	
	
	return nRet;
}


int AnswerMailboxRequest2(char *sRequest, int nRequestNum, DomainAlias *pAlias, MailList *pList, char **sResult, 
													int *nResultSize, int *nResultLeng)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	int nCheck = 0;
	
	*nResultLeng = 0;
	
	sTmp = sRequest;
	while(nCheck < nRequestNum && sTmp && *sTmp)
	{
		char *sTmpE;
		char *sTmpTmp = sTmp;
		int nNum = 0;
		int nWhich = 0;
		sTmp = strstr(sTmp, "domain=");
		if(!sTmp)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;	
		}
		sTmp += 7;
		sTmpE = strchr(sTmp, '&');
		if(!sTmpE)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		*sTmpE = 0;
		nWhich = FindAliasDomain(sTmp, pAlias);	
		if(nWhich < 0)
		{
			nWhich = pAlias->nDomainNum;	
		}
		*sTmpE = '&';
		if(sscanf(sTmpE, "&cnt=%d", &nNum) != 1)//nNum = atoi(sTmpE+1);
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		nCheck++;
		
		{
			MailList *p = pList + nWhich;
			int i = 0;
			while(i < nNum)
			{
				if(p->nCount == 0 || p->nRound > 1 || (p->nTime && tTime-(p->nTime) > 0))
					break;
				else
				{
					int nPos = p->nNowPos;
					if(p->nTime)
						p->nTime = 0;
					while(nPos < p->nOffsetListUsed && p->nRound < 2)
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success && *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 10 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							memcpy(*sResult + *nResultLeng, p->sMailboxList+nPos2, nLeng+4);
							*nResultLeng += (nLeng+4);
							p->nNowPos = nPos+5;	
							//(p->nCount)--;
							*(p->sOffsetList+nPos+4) = D_MailList_Deliver + p->nRound;
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						{
							int nPosTmp = 0;
							while(nPosTmp < p->nOffsetListUsed)
							{
								if(p->nRound == 0 && nPosTmp%3 != 0 && *(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
								{
									*(p->sOffsetList+nPosTmp+4)	= D_MailList_Success;
									(p->nCount)--;
								}	
								else if(p->nRound > 0 && nPosTmp%5 != 0 && *(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
								{
									*(p->sOffsetList+nPosTmp+4)	= D_MailList_Success;
									(p->nCount)--;
								}	
								else if(*(p->sOffsetList+nPosTmp+4) != D_MailList_Success)
									*(p->sOffsetList+nPosTmp+4)	= 0;
								int nnn;
								memcpy(&nnn, p->sOffsetList+nPosTmp, 4);
								//printf("mailbox:%s,stat:%d\n", p->sMailboxList+nnn+4, *(p->sOffsetList+nPosTmp+4));
								nPosTmp += 5;
							}
						}
						(p->nRound)++;
						p->nTime = tTime;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						break;
					}
				}
			}	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	
	
	return nRet;
}


int AnswerMailboxRequest3(char *sRequest, int nRequestNum, DomainAlias *pAlias, MailList *pList, char **sResult, 
													int *nResultSize, int *nResultLeng, int nResultBegin)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	int nCheck = 0;
	
	*nResultLeng = nResultBegin;
	
	sTmp = sRequest;
	while(nCheck < nRequestNum && sTmp && *sTmp)
	{
		char *sTmpE;
		char *sTmpTmp = sTmp;
		int nNum = 0;
		int nWhich = 0;
		sTmp = strstr(sTmp, "domain=");
		if(!sTmp)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;	
		}
		sTmp += 7;
		sTmpE = strchr(sTmp, '&');
		if(!sTmpE)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		*sTmpE = 0;
		nWhich = FindAliasDomain(sTmp, pAlias);	
		if(nWhich < 0)
		{
			nWhich = pAlias->nDomainNum;	
		}
		*sTmpE = '&';
		if(sscanf(sTmpE, "&cnt=%d", &nNum) != 1)//nNum = atoi(sTmpE+1);
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		nCheck++;
		
		{
			MailList *p = pList + nWhich;
			int i = 0;
			if(p->pLock)
				pthread_mutex_lock(p->pLock);
			while(i < nNum)
			{
				if(p->nCount == 0 || p->nRound > 1 || (p->nTime && tTime-(p->nTime) < 10))
					break;
				else
				{
					int nPos = p->nNowPos;
					if(p->nTime)
						p->nTime = 0;
					while(nPos < p->nOffsetListUsed && p->nRound < 2)
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success )//&& *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 50 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							{
								int nn;
								memcpy(&nn, p->sMailboxList+nPos2, 4);
								nn += 12;
								nn = htonl(nn);
								memcpy(*sResult + *nResultLeng, &nn, 4);
								memcpy(*sResult + *nResultLeng + 4, p->sID, 12);
								memcpy(*sResult + *nResultLeng + 16, p->sMailboxList+nPos2+4, nLeng);
							}
							*nResultLeng += (nLeng+16);
							p->nNowPos = nPos+5;	
							//(p->nCount)--;
							*(p->sOffsetList+nPos+4) = D_MailList_Deliver + p->nRound;
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						
						(p->nRound)++;
						p->nTime = tTime;
						//printf("ROUND END <<%s>>[%d]\n", pAlias->sDomain + nWhich*34, p->nCount);
						break;
					}
				}
			}
			if(p->pLock)
				pthread_mutex_unlock(p->pLock);	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	
	
	return nRet;
}




int ChangeMailboxStat(DomainAlias *pAlias, MailList *pMailList, char *sMailbox, int nStat)
{
	char *sDomain;
	int nWhich = 0;
	MailList *p;
	char *sOffsetList;
	char *sMailboxList;
	
	sDomain = strchr(sMailbox, '@');
	if(sDomain == NULL)
		return -1;
	
	nWhich = FindAliasDomain(sDomain+1, pAlias);
	if(nWhich < 0)
		nWhich = pAlias->nDomainNum;
	
	p = pMailList + nWhich;
	sOffsetList = p->sOffsetList;
	sMailboxList = p->sMailboxList;
	
	if(p->pLock)
		pthread_mutex_lock(p->pLock);
	{
		int nB = 0, nE = (p->nOffsetListUsed-5)/5;
		while(nB <= nE)
		{
			int nM = (nB+nE)/2;
			int nPos;
			int k;
			memcpy(&nPos, sOffsetList + nM*5, 4);	
			k = CompareMailbox(sMailboxList+nPos+4, sMailbox);
			if(k < 0)
				nB = nM+1;
			else if(k > 0)
				nE = nM-1;
			else
			{
				*(sOffsetList+nM*5+4) = nStat;
				if(nStat == D_MailList_Success)
					(p->nCount)--;
				//printf("FOUND:::");
				/*
				{
				int nLeng;
				int i = 0;
				memcpy(&nLeng, sMailboxList+nPos, 4);
				printf("FOUND:::");
				
				while(i < nLeng)
				{
					printf("%s,,", sMailboxList+nPos+4+i);
					i = i+strlen(sMailboxList+nPos+4+i)+1;	
				}
				printf("\n");
				}
				*/
				if(p->pLock)
					pthread_mutex_unlock(p->pLock);
				return 0;
			}
		}	
		
	}
	if(p->pLock)
		pthread_mutex_unlock(p->pLock);
	//printf("NOT FOUND!\n");
	
	return -1;
}



int KeepOrderFast(char *sOffset, int nUsed, char *sList)
{
	int nE = nUsed - 10;
	int nB = 0;
	int nPos1, nPos2, nPosAdd;
	
	if(nUsed == 5)
		return 0;
	
	memcpy(&nPosAdd, sOffset+nUsed-5, 4);
	
	nB = nB/5;
	nE = nE/5;
	
	memcpy(&nPos2, sOffset+nE*5, 4);
	if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) < 0)
		return 0;
	
	memcpy(&nPos1, sOffset+nB*5, 4);
	if(CompareMailbox(sList+nPos1+4, sList+nPosAdd+4) > 0)
	{
		memmove(sOffset+nB*5+5, sOffset+nB*5, (nE-nB+1)*5);
		memcpy(sOffset+nB*5, &nPosAdd, 4);
		memset(sOffset+nB*5+4, 0x0, 1);
		return 0;
	}	
	
	if(nB+1 == nE)
	{
		memmove(sOffset+nE*5+5, sOffset+nE*5, nUsed-5-nE*5);	
		memcpy(sOffset+nE*5, &nPosAdd, 4);
		memset(sOffset+nE*5+4, 0x0, 1);
		return 0;
	}
			
	while(nB < nE)
	{
		int nM = (nB+nE)/2;
		int k = 0;
		memcpy(&nPos1, sOffset+nM*5, 4);
		k = CompareMailbox(sList+nPos1+4, sList+nPosAdd+4);
		if(k < 0)
		{
			memcpy(&nPos2, sOffset+nM*5+5, 4);
			if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) > 0)
			{
				memmove(sOffset+nM*5+10, sOffset+nM*5+5, nUsed-5-(nM+1)*5);
				memcpy(sOffset+nM*5+5, &nPosAdd, 4);
				memset(sOffset+nM*5+9, 0x0, 1);
				return 0;
			}
			else
				nB = nM+1;			
		}
		else if(k > 0)
		{
			memcpy(&nPos2, sOffset+nM*5-5, 4);
			if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) < 0)
			{
				memmove(sOffset+nM*5+5, sOffset+nM*5, nUsed-5-nM*5);
				memcpy(sOffset+nM*5, &nPosAdd, 4);
				memset(sOffset+nM*5+4, 0x0, 1);
				return 0;
			}
			else
				nE = nM-1;	
		}
		else
		{
			return -1;	
		}
		
	}
	
	return -1;
	
}



int KeepOrderFast2(char *sOffset, int nUsed, char *sList, int nMax)
{
	int nE = nUsed - 10;
	int nB = 0;
	int nPos1, nPos2, nPosAdd;
	
	if(nUsed == 5)
		return 0;
	
	memcpy(&nPosAdd, sOffset+nUsed-5, 4);
	
	nB = nB/5;
	nE = nE/5;
	
	memcpy(&nPos2, sOffset+nE*5, 4);
	if(nPos2 > nMax)
		return -1;
	if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) < 0)
		return 0;
	
	memcpy(&nPos1, sOffset+nB*5, 4);
	if(CompareMailbox(sList+nPos1+4, sList+nPosAdd+4) > 0)
	{
		memmove(sOffset+nB*5+5, sOffset+nB*5, (nE-nB+1)*5);
		memcpy(sOffset+nB*5, &nPosAdd, 4);
		memset(sOffset+nB*5+4, 0x0, 1);
		return 0;
	}	
	
	if(nB+1 == nE)
	{
		memmove(sOffset+nE*5+5, sOffset+nE*5, nUsed-5-nE*5);	
		memcpy(sOffset+nE*5, &nPosAdd, 4);
		memset(sOffset+nE*5+4, 0x0, 1);
		return 0;
	}
			
	while(nB < nE)
	{
		int nM = (nB+nE)/2;
		int k = 0;
		memcpy(&nPos1, sOffset+nM*5, 4);
		if(nPos1 > nMax)
			return -1;
		k = CompareMailbox(sList+nPos1+4, sList+nPosAdd+4);
		if(k < 0)
		{
			memcpy(&nPos2, sOffset+nM*5+5, 4);
			if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) > 0)
			{
				memmove(sOffset+nM*5+10, sOffset+nM*5+5, nUsed-5-(nM+1)*5);
				memcpy(sOffset+nM*5+5, &nPosAdd, 4);
				memset(sOffset+nM*5+9, 0x0, 1);
				return 0;
			}
			else
				nB = nM+1;			
		}
		else if(k > 0)
		{
			memcpy(&nPos2, sOffset+nM*5-5, 4);
			if(nPos2 > nMax)
				return -1;
			if(CompareMailbox(sList+nPos2+4, sList+nPosAdd+4) < 0)
			{
				memmove(sOffset+nM*5+5, sOffset+nM*5, nUsed-5-nM*5);
				memcpy(sOffset+nM*5, &nPosAdd, 4);
				memset(sOffset+nM*5+4, 0x0, 1);
				return 0;
			}
			else
				nE = nM-1;	
		}
		else
		{
			return -1;	
		}
		
	}
	
	return -1;
	
}


int ReadMailListWithAliasFast(char *sFileName, MailList *pMail, DomainAlias *pAlias)
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
				snprintf(sPos+nOffset, 400, "$%s$", sParamLine + nParamName[i]);
				nOffset += (strlen(sParamLine+nParamName[i]) + 2);
			}
			snprintf(sPos + nOffset, 400, "%s", sTmp);
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
		}
		memcpy(sOffsetList[nWhich]+nOffsetListUsed[nWhich], &(nMailboxListUsed[nWhich]), 4);
		memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+4, 0x0, 1);
		if(KeepOrderFast(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich]) < 0)
			printf("LLLLLLLLLLLLL   -1 -1LLLLLLLLLLLL\n");
		(nOffsetListUsed[nWhich]) += 5;
		(nMailboxListUsed[nWhich]) += nOffset;
		(nCount[nWhich])++;
		
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





int AddTaskQueue(TaskQueue **pBegin, TaskQueue **pEnd, DomainAlias *pAlias, MailList *pMailList, char *sModula, 
											int nModulaSize, char *sID, int *nNumber, char *sFileName, int nPaus)
{
	TaskQueue *pTask;
	int n1;
	
	memcpy(&n1, sID, 4);
	n1 = ntohl(n1);
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	pTask = (TaskQueue*)malloc(sizeof(TaskQueue));
	memset(pTask, 0x0, sizeof(TaskQueue));
	pTask->pAlias = pAlias;
	pTask->pMailList = pMailList;
	pTask->sModula = sModula;
	pTask->nModulaSize = nModulaSize;
	memcpy(pTask->sID, sID, 12);
	pTask->pLock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pTask->pLock, NULL);
	pTask->pNext = NULL;
	pTask->pPrev = NULL;
	pTask->tFinishTime = 0;
	//printf("\nADTASK %d*%d****************\n", nPaus, n1);
	if(nPaus && n1 >= 100)
		pTask->tPauseTime = 10;
	else
		pTask->tPauseTime = 0;
	pTask->tBeginTime = time(NULL);
	pTask->nTotalCount = 0;
	snprintf(pTask->sFileName, sizeof(pTask->sFileName), "%s", sFileName);
	{
		int i = 0;
		while(i < pAlias->nDomainNum + 1)
		{
			pTask->nTotalCount += ((pMailList+i)->nCount);
			++i;	
		}	
	}
	if(*pBegin == NULL)
	{
		*pBegin = pTask;
		*pEnd = pTask;
	}
	else
	{
		//pthread_mutex_t *pTmp = (*pBegin)->pLock;
		//pthread_mutex_lock(pTmp);
		(*pEnd)->pNext = pTask;
		pTask->pPrev = *pEnd;
		*pEnd = pTask;
		//pthread_mutex_unlock(pTmp);	
	}
	(*nNumber)++;
	
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	return 0;
	
}




int AddWaterTask(CONFIGURE* pConf, DABUDAI *pDabudai, int *nn)
{
	//int ReadTaskFile(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID);
	char sFileName[200];
	MailList *pMailList;
	DomainAlias *pAlias;
	char *sModula = NULL; 
	int nModulaSize = 0;
	//char *sID;
	FILE *pFile;
	int nOffset = 0;
	TaskQueue *pTask;
	int i = 0;
	char sID[12];
	char sTaskFileName[100];
	char sIedmSubject[300];
	
	sIedmSubject[0] = 0;
	snprintf(sFileName, 200, "%s/%d.%d.%d.water", pConf->sTaskFileDir, nn[0], nn[1], nn[2]);
	pFile = fopen(sFileName, "w");
	if(pFile == NULL)
		return 0;
	fprintf(pFile, "<$ID>\r\n%d.%d.%d\r\n</$ID>\r\n", nn[0], nn[1], nn[2]);
	//pthread_rwlock_rdlock(pDabudai->pLockRWTask);
	if(pDabudai->nTaskNum)
		nOffset = (pDabudai->nWaterOffset) % (pDabudai->nTaskNum);
	else
		nOffset = -1;
	i = 0;
	pTask = pDabudai->pBegin;
	while(i < nOffset)
	{
		pTask = pTask->pNext;	
		++i;	
	}
	if(nOffset < 0)
		pTask = NULL;
	if(pTask)
	{
		char *sTmp;
		int n[3];
		nModulaSize = pTask->nModulaSize;
		memcpy(n, pTask->sID, 12);
		sModula = (char*)malloc(nModulaSize + 1);
		memcpy(sModula, pTask->sModula, nModulaSize);
		sModula[nModulaSize] = 0;	
		sTmp = sModula;
		while(sTmp)
		{
			sTmp = strstr(sTmp, "test");
			if(sTmp)
				*(sTmp+1) = 'a';
		}
		n[0] = ntohl(n[0]);
		n[1] = ntohl(n[1]);
		n[2] = ntohl(n[2]);
		snprintf(sTaskFileName, 100, "%s/%d.%d.%d.task", pConf->sTaskFileDir, n[0], n[1], n[2]);
	}
	//pthread_rwlock_unlock(pDabudai->pLockRWTask);
	{
		FILE *pTaskFile = fopen(sTaskFileName, "r");
		char s[1030];
		char *sTmpB = NULL, *sTmpE = NULL;
		
		if(pTaskFile)
		{	
			fread(s, 1024, 1, pTaskFile);	
		}
		if(pTaskFile)
			fclose(pTaskFile);
		s[1024] = 0;
		sTmpB = strstr(s, "<$SUBJECT>");
		if(sTmpB)
		{
			sTmpB = strchr(sTmpB, '\n');
		}
		if(sTmpB)
			sTmpB++;
		if(sTmpB)
		{
			sTmpE = strchr(sTmpB, '\r');
			if(!sTmpE)
				sTmpE = strchr(sTmpB, '\n');
		}
		if(sTmpE)
			*sTmpE = 0;
		if(sTmpB && sTmpE)
			snprintf(sIedmSubject, 300, "%s", sTmpB);
	}
	
	if(sModula)
	{
		fprintf(pFile, "<$MODULA>\r\n");	
		fprintf(pFile, "%s", sModula);
		free(sModula);
		sModula = NULL;
		fprintf(pFile, "\r\n</$MODULA>\r\n");
	}
	{
		FILE *pFile2 = fopen(pConf->sWaterListFile, "r");
		char sLine[1024];
		int nLine;
		if(pFile2)
		{
			while(fgets(sLine, sizeof(sLine), pFile2))
			{
				if(strchr(sLine, '@'))
					nLine++;
			}
			fclose(pFile2);
		}
		pFile2 = fopen(pConf->sWaterListFile, "r");
		if(pFile2 == NULL)
			fprintf(pFile, "<$MAILLIST>\r\nmail,iedm_subject,iedm_ran\r\n</$MAILLIST>\r\n");	
		else if(nLine == 0)
			fprintf(pFile, "<$MAILLIST>\r\nmail,iedm_subject,iedm_ran\r\n</$MAILLIST>\r\n");	
		else
		{
			int n1 = nLine / 10;
			int n2;
			int n3 = 0;
			char sBak[300];
			size_t nOutLeng;
			if(n1 == 0)
				n2 = 0;
			else
				n2 = (pDabudai->nWaterOffset) % n1;
			if(sIedmSubject[0] == 0)
				snprintf(sIedmSubject, 100, "%s", sFileName);
			if(encode_base64(sIedmSubject, strlen(sIedmSubject), sBak, 290, &nOutLeng) >= 0)
			{ 
				char *ss = strchr(sBak, '\r');
				if(!ss)
					ss = strchr(sBak, '\n');
				if(ss)
					*ss = 0;
				snprintf(sIedmSubject, sizeof(sIedmSubject), "=?utf-8?B?%s?=", sBak);
			}
			else
			{
				snprintf(sIedmSubject, 100, "%s", sFileName);
			}
			fprintf(pFile, "<$MAILLIST>\r\nmail,iedm_ran2,iedm_subject,iedm_ran\r\n");
			while(fgets(sLine, sizeof(sLine), pFile2))
			{
				char *sTmpE;
				sTmpE = strchr(sLine, '\r');
				if(!sTmpE)
					sTmpE = strchr(sLine, '\n');
				if(sTmpE)
					*sTmpE = 0;
				if(strchr(sLine, '@'))
					n3++;
				else
					continue;
				if(n3 > n2*10 && n3-10 <= n2*10)
					fprintf(pFile, "%s,%s,100\r\n", sLine, sIedmSubject);	
			}
			fprintf(pFile, "</$MAILLIST>\r\n");
		}
		if(pFile2)
			fclose(pFile2);
	}
	
	fclose(pFile);
	
	pAlias = (DomainAlias*)malloc(sizeof(DomainAlias));	
	memset(pAlias, 0x0, sizeof(DomainAlias));
	ReadDomainAlias(pConf->sAliasFile, pAlias);
	pMailList = (MailList*)malloc(sizeof(MailList) * (pAlias->nDomainNum+1));
	memset(pMailList, 0x0, sizeof(MailList)*(pAlias->nDomainNum+1));
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
	
	{
		char sLog[2048];
		char sLogTag[100];
		snprintf(sLogTag, 100, "[WATER]");
		ReadTaskFileWithLevel(sFileName, pMailList, pAlias, &sModula, sID, pConf, pDabudai, sLog, sLogTag);
	}
	//ReadTaskFile(sFileName, pMailList, pAlias, &sModula, sID);
	/*
	{
		int kk[3];
		kk[0] = htonl(nn[0]);
		kk[1] = htonl(nn[1]);
		kk[2] = htonl(nn[2]); 	
		memcpy(sID, kk, 12);
	}
	*/
	if(pAlias && pMailList && sModula)
	{
		//pthread_rwlock_wrlock(pDabudai->pLockRWTask);
		AddTaskQueue(&(pDabudai->pBegin), &(pDabudai->pEnd), pAlias, pMailList, sModula, strlen(sModula)+1, 
											sID, &(pDabudai->nTaskNum), sFileName, 0);
		//snprintf(sLog, sizeof(sLog), "[%s][NEWTASK]add task <%s> into queue successfully.\n", sTag, sFileName);
		//WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
		//snprintf(sMsg, 5, "OK\n");
		//pthread_rwlock_unlock(pDabudai->pLockRWTask);
	}
		
	return 0;	
	
}



int DelOneTask(TaskQueue **pBegin, TaskQueue **pEnd, TaskQueue *pTask, int *nNumber)
{
	DomainAlias *pAlias; 
	MailList *pMailList; 
	pthread_mutex_t *pTmp;
	
	if(pTask == NULL || *pBegin == NULL)
		return 0;
	
		
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	if(*pBegin == pTask)
	{
		*pBegin = pTask->pNext;	
	}
	if(*pEnd == pTask)
	{
		*pEnd = pTask->pPrev;	
	}
	if(pTask->pPrev)
	{
		(pTask->pPrev)->pNext = pTask->pNext;	
	}
	if(pTask->pNext)
	{
		(pTask->pNext)->pPrev = pTask->pPrev;
	}
	if(*pBegin)
		(*pBegin)->pPrev = NULL;
	if(*pEnd)
		(*pEnd)->pNext = NULL;
	(*nNumber)--;
	
	
	pthread_mutex_lock(pTask->pLock);
	pAlias = pTask->pAlias;
	pMailList = pTask->pMailList;
	{
		int i = 0;
		while(i < pAlias->nDomainNum + 1)
		{
			MailList *pp = pMailList+i;
			FreeMailList(pp);	
			++i;
		}	
	}
	FreeDomainAlias(pAlias);
	free(pAlias);
	free(pMailList);
	free(pTask->sModula);
	pthread_mutex_unlock(pTask->pLock);
	free(pTask->pLock);
	free(pTask);
	
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	return 0;
}


int DelOneTask2(TaskQueue **pBegin, TaskQueue **pEnd, TaskQueue *pTask, int *nNumber, int *nFinishSum)
{
	DomainAlias *pAlias; 
	MailList *pMailList; 
	pthread_mutex_t *pTmp;
	
	if(pTask == NULL || *pBegin == NULL)
		return 0;
	
		
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	if(*pBegin == pTask)
	{
		*pBegin = pTask->pNext;	
	}
	if(*pEnd == pTask)
	{
		*pEnd = pTask->pPrev;	
	}
	if(pTask->pPrev)
	{
		(pTask->pPrev)->pNext = pTask->pNext;	
	}
	if(pTask->pNext)
	{
		(pTask->pNext)->pPrev = pTask->pPrev;
	}
	if(*pBegin)
		(*pBegin)->pPrev = NULL;
	if(*pEnd)
		(*pEnd)->pNext = NULL;
	(*nNumber)--;
	
	
	pthread_mutex_lock(pTask->pLock);
	pAlias = pTask->pAlias;
	pMailList = pTask->pMailList;
	{
		int i = 0;
		while(i < pAlias->nDomainNum + 1)
		{
			MailList *pp = pMailList+i;
			(*nFinishSum) += (pp->nSend);
			FreeMailList(pp);	
			++i;
		}	
	}
	FreeDomainAlias(pAlias);
	free(pAlias);
	free(pMailList);
	free(pTask->sModula);
	pthread_mutex_unlock(pTask->pLock);
	free(pTask->pLock);
	free(pTask);
	
	/*if(*pBegin && *pEnd == NULL)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if(*pBegin == NULL && *pEnd)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}
	if((*pBegin == NULL || *pEnd == NULL) && *nNumber)
	{
		printf("HHHHHHHHHHHHEEEEERRRR<%d>\n", __LINE__);
		fflush(NULL);	
	}*/
	
	return 0;
}


/*
int FreeTaskQueue()
{
		
	
	
	
	
}
*/



int AnswerMailboxRequest4(char *sRequest, int nRequestNum, TaskQueue *pBegin, TaskQueue *pEnd, char **sResult, 
													int *nResultSize, int *nResultLeng, int nResultBegin, int nTaskNum, int *nTaskOffset,
													pthread_mutex_t *pLockTask)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	int nCheck = 0;
	TaskQueue *pTaskUsed;
	
	*nResultLeng = nResultBegin;
	
	//if(*nTaskOffset >= nTaskNum)
		//*nTaskOffset = 0;
	sTmp = sRequest;
	
	{
		int j = 0;
		pthread_mutex_lock(pLockTask);
		pTaskUsed = pBegin;
		if(*nTaskOffset >= nTaskNum)
			*nTaskOffset = 0;
		while(j < *nTaskOffset && pTaskUsed)
		{
			pTaskUsed = pTaskUsed->pNext;
			++j;
		}
		if(pTaskUsed == NULL)
			pTaskUsed = pBegin;
		if(nTaskNum)	
			*nTaskOffset = (*nTaskOffset+1) % nTaskNum;
		else
			*nTaskOffset = 0;
		pthread_mutex_unlock(pLockTask);
	}
	
	while(nCheck < nRequestNum && sTmp && *sTmp)
	{
		char *sTmpE;
		char *sTmpTmp = sTmp;
		int nNum = 0;
		int nWhich = 0;
		
		
		sTmp = strstr(sTmp, "domain=");
		if(!sTmp)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;	
		}
		sTmp += 7;
		sTmpE = strchr(sTmp, '&');
		if(!sTmpE)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		/*
		*sTmpE = 0;
		nWhich = FindAliasDomain(sTmp, pAlias);	
		if(nWhich < 0)
		{
			nWhich = pAlias->nDomainNum;	
		}
		*sTmpE = '&';
		*/
		if(sscanf(sTmpE, "&cnt=%d", &nNum) != 1)//nNum = atoi(sTmpE+1);
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		nCheck++;
		
		{
			/*
			pthread_mutex_lock(pLockTask);
			pTaskUsed = pBegin;
			int j = 0;
			if(*nTaskOffset >= nTaskNum)
				*nTaskOffset = 0;
			while(j < *nTaskOffset && pTaskUsed)
				pTaskUsed = pTaskUsed->pNext;
			if(pTaskUsed == NULL)
				pTaskUsed = pBegin;
			*nTaskOffset = (*nTaskOffset+1) % nTaskNum;
			pthread_mutex_unlock(pLockTask);
			*/
			/*
			if(pTaskUsed)
			{
				pthread_mutex_lock(pTaskUsed->pLock);
				pTaskUsed->cInUse = 1;
				pthread_mutex_unlock(pTaskUsed->pLock);
			}
			*/
		}
		
		if(pTaskUsed)
		{
			MailList *p, *pList;
			DomainAlias *pAlias;
			int i = 0;
			
			pList = pTaskUsed->pMailList;
			pAlias = pTaskUsed->pAlias;
			*sTmpE = 0;
			nWhich = FindAliasDomain(sTmp, pAlias);	
			if(nWhich < 0)
			{
				nWhich = pAlias->nDomainNum;	
			}
			*sTmpE = '&';
			p = pList + nWhich;
			if(p->pLock)
				pthread_mutex_lock(p->pLock);
			while(i < nNum)
			{
				if(p->nCount == 0 || p->nRound > 1 || (p->nTime && tTime-(p->nTime) < 100))
					break;
				else
				{
					int nPos = p->nNowPos;
					if(p->nTime)
						p->nTime = 0;
					while(nPos < p->nOffsetListUsed && p->nRound < 2)
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success )//&& *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 50 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							{
								int nn;
								memcpy(&nn, p->sMailboxList+nPos2, 4);
								nn += 12;
								nn = htonl(nn);
								memcpy(*sResult + *nResultLeng, &nn, 4);
								memcpy(*sResult + *nResultLeng + 4, pTaskUsed->sID, 12);
								/*{
									int i = 0;
									printf("TTTTTTTTT");
									while(i < 12)
									{
										printf("[%d]<%02x>", i, *(pTaskUsed->sID+i));
										++i;
									}	
									printf("\n");
								}*/
								memcpy(*sResult + *nResultLeng + 16, p->sMailboxList+nPos2+4, nLeng);
								//printf("VVVVVVVVVVVAAAAAAAAAAAAAAA<%s>\n", p->sMailboxList+nPos2+4);
							}
							*nResultLeng += (nLeng+16);
							p->nNowPos = nPos+5;	
							//(p->nCount)--;
							//*(p->sOffsetList+nPos+4) = D_MailList_Deliver + p->nRound;
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						//printf("ROUND END <<%s>>[%d][[[%d]]]\n", pAlias->sDomain + nWhich*34, p->nCount, p->nRound);
						
						(p->nRound)++;
						p->nTime = tTime;
						//printf("ROUND END <<%s>>[%d][[[%d]]]\n", pAlias->sDomain + nWhich*34, p->nCount, p->nRound);
						break;
					}
				}
			}
			if(p->pLock)
				pthread_mutex_unlock(p->pLock);	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	/*
	pthread_mutex_lock(pLockTask);
	*nTaskOffet = (*nTaskOffset+1)%(*nTaskNum);
	pthread_mutex_unlock(pLockTask);
	*/
	return nRet;
}


int AnswerMailboxRequest5(char *sRequest, int nRequestNum, DABUDAI *pDabudai, char **sResult, int *nResultSize, 
															int *nResultLeng, int nResultBegin, CONFIGURE *pConf, char *sLog, int nLogSize, char *sTag)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	int nCheck = 0;
	TaskQueue *pTaskUsed;
	TaskQueue *pBegin = pDabudai->pBegin;
	TaskQueue *pEnd = pDabudai->pEnd;
	int nTaskNum = pDabudai->nTaskNum;
	int *nTaskOffset = &(pDabudai->nTaskOffset);
	pthread_mutex_t *pLockTask = pDabudai->pLockTask;
	
	*nResultLeng = nResultBegin;
	
	//if(*nTaskOffset >= nTaskNum)
		//*nTaskOffset = 0;
	sTmp = sRequest;
	
	{
		int j = 0;
		pthread_mutex_lock(pLockTask);
		pTaskUsed = pBegin;
		if(*nTaskOffset >= nTaskNum)
			*nTaskOffset = 0;
		while(j < *nTaskOffset && pTaskUsed)
		{
			pTaskUsed = pTaskUsed->pNext;
			++j;
		}

		if(pTaskUsed == NULL)
			pTaskUsed = pBegin;
		if(nTaskNum)	
			*nTaskOffset = (*nTaskOffset+1) % nTaskNum;
		else
			*nTaskOffset = 0;
		if(pTaskUsed && nTaskNum)
		{
			if(pTaskUsed->tFinishTime || pTaskUsed->tPauseTime)
			{
				pTaskUsed = pTaskUsed->pNext;
				if(pTaskUsed == NULL)
					pTaskUsed = pBegin;
				*nTaskOffset = (*nTaskOffset+1) % nTaskNum;	
			}
		}
		pthread_mutex_unlock(pLockTask);
	}
	
	while(nCheck < nRequestNum && sTmp && *sTmp)
	{
		char *sTmpE;
		char *sTmpTmp = sTmp;
		int nNum = 0;
		int nWhich = 0;
		
		
		sTmp = strstr(sTmp, "domain=");
		if(!sTmp)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;	
		}
		sTmp += 7;
		sTmpE = strchr(sTmp, '&');
		if(!sTmpE)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		/*
		*sTmpE = 0;
		nWhich = FindAliasDomain(sTmp, pAlias);	
		if(nWhich < 0)
		{
			nWhich = pAlias->nDomainNum;	
		}
		*sTmpE = '&';
		*/
		if(sscanf(sTmpE, "&cnt=%d", &nNum) != 1)//nNum = atoi(sTmpE+1);
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		nCheck++;
		
		{
			/*
			pthread_mutex_lock(pLockTask);
			pTaskUsed = pBegin;
			int j = 0;
			if(*nTaskOffset >= nTaskNum)
				*nTaskOffset = 0;
			while(j < *nTaskOffset && pTaskUsed)
				pTaskUsed = pTaskUsed->pNext;
			if(pTaskUsed == NULL)
				pTaskUsed = pBegin;
			*nTaskOffset = (*nTaskOffset+1) % nTaskNum;
			pthread_mutex_unlock(pLockTask);
			*/
			/*
			if(pTaskUsed)
			{
				pthread_mutex_lock(pTaskUsed->pLock);
				pTaskUsed->cInUse = 1;
				pthread_mutex_unlock(pTaskUsed->pLock);
			}
			*/
		}
		
		if(pTaskUsed && pTaskUsed->tFinishTime == 0 && pTaskUsed->tPauseTime == 0)
		{
			MailList *p, *pList;
			DomainAlias *pAlias;
			int i = 0;
			int nLogOffset = 0;
			
			pList = pTaskUsed->pMailList;
			pAlias = pTaskUsed->pAlias;
			*sTmpE = 0;
			nWhich = FindAliasDomain(sTmp, pAlias);	
			if(nWhich < 0)
			{
				nWhich = pAlias->nDomainNum;	
			}
			{
				int *nn = (int*)(pTaskUsed->sID);
				snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d]", sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]));
			}
			nLogOffset = strlen(sLog);
			if(nWhich < pAlias->nDomainNum)
			{
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "(%s:%d)", pAlias->sDomain+34*nWhich, nNum);	
				nLogOffset = strlen(sLog);
			}
			else
			{
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "(other:%d)", nNum);
				nLogOffset = strlen(sLog);	
			}
			*sTmpE = '&';
			p = pList + nWhich;
			if(p->pLock)
				pthread_mutex_lock(p->pLock);
			while(i < nNum)
			{
				if(p->nCount == 0 || p->nRound >= pConf->nRoundNum || (p->nTime && tTime-(p->nTime) < pConf->nRoundPeriod))
					break;
				else
				{
					int nPos = p->nNowPos;
					if(p->nTime)
						p->nTime = 0;
					while(nPos < p->nOffsetListUsed && p->nRound < pConf->nRoundNum)
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success )//&& *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 50 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							{
								int nn;
								memcpy(&nn, p->sMailboxList+nPos2, 4);
								nn += 12;
								nn = htonl(nn);
								memcpy(*sResult + *nResultLeng, &nn, 4);
								memcpy(*sResult + *nResultLeng + 4, pTaskUsed->sID, 12);
								memcpy(*sResult + *nResultLeng + 16, p->sMailboxList+nPos2+4, nLeng);
								//printf("VVVVVVVVVVVAAAAAAAAAAAAAAA<%s>\n", p->sMailboxList+nPos2+4);
								if(p->nRound == 0 && *(p->sOffsetList+nPos+4) != D_MailList_Success 
											&& *(p->sOffsetList+nPos+4) != D_MailList_Delay && *(p->sOffsetList+nPos+4) != D_MailList_Fail)
								{
									(p->nSend)++;
								}	
								//printf("VVVVAAAAATASK:%d:%d:%d::::\n", p->nCount, p->nSend, p->nSuccess);
								if(nLogOffset > nLogSize-200)
								{
									if(nLogOffset <= nLogSize-2)
									{	
										sLog[nLogOffset] = '\n';	
										sLog[nLogOffset+1] = 0;
										//WriteLog(pConf, pDabudai, sLog, nLogOffset+1);
									}
									else
									{
										sLog[nLogSize-2] = '\n';
										sLog[nLogSize-1] = 0;	
										//WriteLog(pConf, pDabudai, sLog, nLogSize-1);
									}
									{
										int *nn = (int*)(pTaskUsed->sID);
										snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d]", sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]));
									}
									nLogOffset = strlen(sLog);
								}
								snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "%s,", p->sMailboxList+nPos2+4);
								nLogOffset += (strlen(p->sMailboxList+nPos2+4)+1);
							}
							*nResultLeng += (nLeng+16);
							p->nNowPos = nPos+5;	
							//(p->nCount)--;
							//*(p->sOffsetList+nPos+4) = D_MailList_Deliver + p->nRound;
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						//printf("ROUND END <<%s>>[%d][[[%d]]]\n", pAlias->sDomain + nWhich*34, p->nCount, p->nRound);
						if(nLogOffset < nLogSize)
							snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "ROUND END,");
						nLogOffset += 10;
						(p->nRound)++;
						p->nTime = tTime;
						//printf("ROUND END <<%s>>[%d][[[%d]]]\n", pAlias->sDomain + nWhich*34, p->nCount, p->nRound);
						break;
					}
				}
			}
			if(p->pLock)
				pthread_mutex_unlock(p->pLock);	
			if(nLogSize > nLogOffset)
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "\n");
			nLogOffset += 1;
			//if(nLogSize > nLogOffset)
				//WriteLog(pConf, pDabudai, sLog, nLogOffset);
			//else
			{
				//sLog[nLogSize-2] = '\n';
				//WriteLog(pConf, pDabudai, sLog, nLogSize-1);
			}
		}
		else
		{
			//snprintf(sLog, nLogSize, "[%s][DELIVER]no task in queue.\n", sTag);
			//WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	/*
	pthread_mutex_lock(pLockTask);
	*nTaskOffet = (*nTaskOffset+1)%(*nTaskNum);
	pthread_mutex_unlock(pLockTask);
	*/
	return nRet;
}


int AnswerMailboxRequestWithLevel(char *sRequest, 
                                  int nRequestNum, 
                                  DABUDAI *pDabudai, 
                                  char **sResult, 
                                  int *nResultSize, 
                                  int *nResultLeng, 
                                  int nResultBegin, 
                                  CONFIGURE *pConf, 
                                  char *sLog,
                                  int nLogSize, 
                                  char *sTag)
{
	char *sTmp;
	int nFinish = 0;
	int nRet = 0;
	int nResultOffset = 0;
	int tTime = time(NULL);
	int nCheck = 0;
	TaskQueue *pTaskUsed;
	TaskQueue *pBegin = pDabudai->pBegin;
	TaskQueue *pEnd = pDabudai->pEnd;
	int nTaskNum = pDabudai->nTaskNum;
	int *nTaskOffset = &(pDabudai->nTaskOffset);
	pthread_mutex_t *pLockTask = pDabudai->pLockTask;
	
	*nResultLeng = nResultBegin;
	
	//if(*nTaskOffset >= nTaskNum)
		//*nTaskOffset = 0;
	sTmp = sRequest;
	
	{
		int j = 0;
		pthread_mutex_lock(pLockTask); //整个任务锁
		pTaskUsed = pBegin; //任务队列
		if(*nTaskOffset >= nTaskNum) //分配到第几个任务 > 任务数 ,则从头
			*nTaskOffset = 0;

		while(j < *nTaskOffset && pTaskUsed) //找任务，根据偏移量
		{
			pTaskUsed = pTaskUsed->pNext;
			++j;
		}

		if(pTaskUsed == NULL) //为空说明最后一个，又从头
			pTaskUsed = pBegin;

        int *nn = NULL;
        int ret = -1;

        if (pTaskUsed){ //youxian 2014/05/23
            nn = (int*)(pTaskUsed->sID);
            ret = yx_test(ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pConf->yxlevel);
        }

		if(nTaskNum) //下次起始偏移量
        {
            if (ret < 1){ //youxian 2014/05/23
                *nTaskOffset = (*nTaskOffset+1) % nTaskNum;
            }
        }
		else
			*nTaskOffset = 0;

		if(pTaskUsed && nTaskNum)
		{
			if(pTaskUsed->tFinishTime || pTaskUsed->tPauseTime) //已完成或者已暂停,取下一个
			{
				pTaskUsed = pTaskUsed->pNext;
				if(pTaskUsed == NULL)
					pTaskUsed = pBegin;

                //*nTaskOffset = (*nTaskOffset+1) % nTaskNum; /* youxian 2014/05/23  delete */

                // youxian 2014/05/23  begin 
                if (pTaskUsed){
                    nn = (int*)(pTaskUsed->sID);
                    ret = yx_test(ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), pConf->yxlevel);
                }

                if (ret < 1){
                    *nTaskOffset = (*nTaskOffset+1) % nTaskNum;
                }
                //youxian 2014/05/23  end 
			}
		}

        if (pTaskUsed) //youxian log
        {
            snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d] youxian == %d\n", 
                sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]), ret);
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }

		pthread_mutex_unlock(pLockTask);
	}
	
    //根据域的个数循环
	while(nCheck < nRequestNum && sTmp && *sTmp) //sTmp 是传入的缓冲
	{
		char *sTmpE;
		char *sTmpTmp = sTmp;
		int nNum = 0;
		int nWhich = 0;

		sTmp = strstr(sTmp, "domain=");
		if(!sTmp){
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;	
		}

		sTmp += 7; //strlen("domain=") = 7
		sTmpE = strchr(sTmp, '&'); //domain=xx&cnt=xx
		if(!sTmpE)
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}

		if(sscanf(sTmpE, "&cnt=%d", &nNum) != 1)//nNum = atoi(sTmpE+1);
		{
			sTmp = sTmpTmp + strlen(sTmpTmp) + 1;
			continue;
		}
		nCheck++;
		
        //此后，sTmp指向domain, nNum等于cnt
        if (pTaskUsed)
        {
            //int *nn = (int*)(pTaskUsed->sID);
            //snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d] pTaskUsed = %d, pTaskUsed->tFinishTime = %d, pTaskUsed->tPauseTime = %d \n",
                //sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]),pTaskUsed,pTaskUsed->tFinishTime, pTaskUsed->tPauseTime);
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
        else
        {
            //snprintf(sLog, nLogSize, "[%s][DELIVER] pTaskUsed is NULL\n", sTag);
            //WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }

		if(pTaskUsed && pTaskUsed->tFinishTime == 0 && pTaskUsed->tPauseTime == 0)
		{
			MailList *p, *pList;
			DomainAlias *pAlias;
			int i = 0;
			int nLogOffset = 0;
			
			pList = pTaskUsed->pMailList; //邮箱
			pAlias = pTaskUsed->pAlias; //域
			*sTmpE = 0;
			nWhich = FindAliasDomain(sTmp, pAlias);	//想必插入mailList队列的时候也是根据域的顺序的 ?
			if(nWhich < 0)
			{
				nWhich = pAlias->nDomainNum;	
			}
			{
				int *nn = (int*)(pTaskUsed->sID);
				snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d]", sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]));
			}
			nLogOffset = strlen(sLog);
			if(nWhich < pAlias->nDomainNum)
			{
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "(%s:%d)", pAlias->sDomain+34*nWhich, nNum);	
				nLogOffset = strlen(sLog);
			}
			else
			{
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "(other:%d)", nNum);
				nLogOffset = strlen(sLog);	
			}
			*sTmpE = '&';
			p = pList + nWhich;// 根据域的位置,跳至域邮箱列表对应的位置
			if(p->pLock)
				pthread_mutex_lock(p->pLock); //邮箱锁

			while(i < nNum) //nNum 请求邮箱的个数
			{
                //剩余数 = 0 || 分发邮箱的轮次 ||时间
				if(p->nCount == 0 
                    || p->nRound >= pConf->nRoundNum 
                    || (p->nTime && tTime-(p->nTime) < pConf->nRoundPeriod))
                {
					break;
                }
				else
				{
					int nPos = p->nNowPos; //分发邮箱进行到的位置
					if(p->nTime)
						p->nTime = 0;

					while(nPos < p->nOffsetListUsed && p->nRound < pConf->nRoundNum) //偏移的实际使用的空间大小
					{
						if(*(p->sOffsetList+nPos+4) != D_MailList_Success )//&& *(p->sOffsetList+nPos+4) < D_MailList_Deliver)
						{
							int nPos2;
							int nLeng = 0;
							
							memcpy(&nPos2, p->sOffsetList+nPos, 4);
							memcpy(&nLeng, p->sMailboxList+nPos2, 4);
							while(*nResultLeng + nLeng + 50 > *nResultSize)
							{
								*nResultSize += 1024;
								*sResult = (char*)realloc(*sResult, *nResultSize);	
							}
							{
								int nn;
								memcpy(&nn, p->sMailboxList+nPos2, 4);
								nn += 12;
								nn = htonl(nn);
								memcpy(*sResult + *nResultLeng, &nn, 4);
								memcpy(*sResult + *nResultLeng + 4, pTaskUsed->sID, 12);
								memcpy(*sResult + *nResultLeng + 16, p->sMailboxList+nPos2+5, nLeng);
								//printf("VVVVVVVVVVVAAAAAAAAAAAAAAA<%s>\n", p->sMailboxList+nPos2+4);
								if(p->nRound == 0 && *(p->sOffsetList+nPos+4) != D_MailList_Success 
											&& *(p->sOffsetList+nPos+4) != D_MailList_Delay && *(p->sOffsetList+nPos+4) != D_MailList_Fail)
								{
									(p->nSend)++;
								}	
								//printf("VVVVAAAAATASK:%d:%d:%d::::\n", p->nCount, p->nSend, p->nSuccess);
								if(nLogOffset > nLogSize-200)
								{
									if(nLogOffset <= nLogSize-2)
									{	
										sLog[nLogOffset] = '\n';	
										sLog[nLogOffset+1] = 0;
										//WriteLog(pConf, pDabudai, sLog, nLogOffset+1);
									}
									else
									{
										sLog[nLogSize-2] = '\n';
										sLog[nLogSize-1] = 0;	
										//WriteLog(pConf, pDabudai, sLog, nLogSize-1);
									}
									{
										int *nn = (int*)(pTaskUsed->sID);
										snprintf(sLog, nLogSize, "[%s][DELIVER][%d.%d.%d]", sTag, ntohl(nn[0]), ntohl(nn[1]), ntohl(nn[2]));
									}
									nLogOffset = strlen(sLog);
								}
								snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "%s,", p->sMailboxList+nPos2+5);
								nLogOffset += (strlen(p->sMailboxList+nPos2+5)+1);
							}
							*nResultLeng += (nLeng+16);
							p->nNowPos = nPos+5;	
							++nRet;
							++i;
							break;
						}
						else
							nPos += 5;	
					}
					if(nPos >= p->nOffsetListUsed)
					{
						p->nNowPos = 0;
						nLogOffset += 10;
						(p->nRound)++;
						p->nTime = tTime;
						break;
					}
				}
			}
			if(p->pLock)
				pthread_mutex_unlock(p->pLock);	
			if(nLogSize > nLogOffset)
				snprintf(sLog+nLogOffset, nLogSize-nLogOffset, "\n");
			nLogOffset += 1;
			//if(nLogSize > nLogOffset)
				//WriteLog(pConf, pDabudai, sLog, nLogOffset);
			//else
			{
				//sLog[nLogSize-2] = '\n';
				//WriteLog(pConf, pDabudai, sLog, nLogSize-1);
			}
		}
		else
		{
			//snprintf(sLog, nLogSize, "[%s][DELIVER]no task in queue.\n", sTag);
			//WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
		}
		sTmp = sTmpE+strlen(sTmpE)+1;
	}
	/*
	pthread_mutex_lock(pLockTask);
	*nTaskOffet = (*nTaskOffset+1)%(*nTaskNum);
	pthread_mutex_unlock(pLockTask);
	*/
	return nRet;
}

int ChangeMailboxStat4(TaskQueue *pBegin, TaskQueue *pEnd, char *sID, char *sMailbox, int nStat)
{
	char *sDomain;
	int nWhich = 0;
	MailList *p;
	MailList *pMailList;
	DomainAlias *pAlias;
	char *sOffsetList;
	char *sMailboxList;
	TaskQueue *pTaskUsed = pBegin;
	
								/*{
									int i = 0;
									printf("tttttttttttt");
									while(i < 12)
									{
										printf("[%d]<%02x>", i, *(sID+i));
										++i;
									}	
									printf("\n");
								}*/
	
	while(pTaskUsed)
	{
		if(memcmp(pTaskUsed->sID, sID, 12) == 0)
			break;
		pTaskUsed = pTaskUsed->pNext;	
	}
	if(pTaskUsed == NULL)
		return -1;
		
	pMailList = pTaskUsed->pMailList;
	pAlias = pTaskUsed->pAlias;
	
	sDomain = strchr(sMailbox, '@');
	if(sDomain == NULL)
		return -1;
	
	nWhich = FindAliasDomain(sDomain+1, pAlias);
	if(nWhich < 0)
		nWhich = pAlias->nDomainNum;
	
	p = pMailList + nWhich;
	sOffsetList = p->sOffsetList;
	sMailboxList = p->sMailboxList;
	
	if(p->pLock)
		pthread_mutex_lock(p->pLock);
	{
		int nB = 0, nE = (p->nOffsetListUsed-5)/5;
		while(nB <= nE)
		{
			int nM = (nB+nE)/2;
			int nPos;
			int k;
			memcpy(&nPos, sOffsetList + nM*5, 4);	
			k = CompareMailbox(sMailboxList+nPos+4, sMailbox);
			if(k < 0)
				nB = nM+1;
			else if(k > 0)
				nE = nM-1;
			else
			{
				if(nStat == D_MailList_Success)
				{
					(p->nCount)--;
					(p->nSuccess)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					if(*(sOffsetList+nM*5+4) == D_MailList_Success)
						(p->nSuccess)--;
				}
				else if(nStat == D_MailList_Fail)
				{
					(p->nFail)++;
					///printf("yyyyyyyyyyyyyyyy::%d::\n", *(sOffsetList+nM*5+4));
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;	
				}
				else if(nStat == D_MailList_Delay)
				{
					(p->nDelay)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;	
				}
				*(sOffsetList+nM*5+4) = nStat;
				//printf("FOUND:::");
				/*
				{
				int nLeng;
				int i = 0;
				memcpy(&nLeng, sMailboxList+nPos, 4);
				printf("FOUND:::");
				
				while(i < nLeng)
				{
					printf("%s,,", sMailboxList+nPos+4+i);
					i = i+strlen(sMailboxList+nPos+4+i)+1;	
				}
				printf("\n");
				}
				*/
				if(p->pLock)
					pthread_mutex_unlock(p->pLock);
				return 0;
			}
		}	
		
	}
	if(p->pLock)
		pthread_mutex_unlock(p->pLock);
	//printf("NOT FOUND!\n");
	
	return -1;
}




////从任务文件获取邮件列表----废弃
int ReadTaskFile(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID)
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
	int *nSum = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int nParamName[50];
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
			nSum[i] = 0;
			++i;
		}	
	}
	
	pIn = fopen(sFileName, "r");
	if(pIn == NULL)
	{
		free(sMailboxList);///////////////没有将2369行跟2370行的内存free掉
		free(sOffsetList);
		free(nMailboxListSize);
		free(nMailboxListUsed);
		free(nOffsetListSize);
		free(nOffsetListUsed);
		free(nCount);
		free(nSum);
	
		return 0;	
	}
	
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
				{
					char *sTmpB;
					sTmpB = strrchr(sLine, '\r');
					if(!sTmpB)
					{
						sTmpB = strrchr(sLine, '\n');
						if(sTmpB)
						{
							*(sTmpB) = '\r';
							*(sTmpB+1) = '\n';
							*(sTmpB+2) = 0;	
						}	
					}	
				}		
				snprintf(s+nOffset, strlen(sLine)+1, "%s", sLine);
				nOffset += (strlen(sLine));
			}
		}
		else if(strncmp(sLine, "<$MAILLIST>", 11) == 0)
		{
			memset(nParamName, 0x0, 50*sizeof(int));
			fgets(sParamLine, 1024, pIn);
			{
				char *sTmp;
				int i = 1;
				
				EndLine(sParamLine);
				nParamName[0] = 0;
				sTmp = sParamLine;
				while(sTmp && i < 50)
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
					int nB, nE;
					if(!sAliasB)
						continue;
					sAliasE = strchr(sAliasB, ',');
					if(!sAliasE)
						continue;
					*sAliasE = 0;
					nWhich = FindAliasDomain(sAliasB+1, pAlias);
					if(nWhich < 0)
						nWhich = pAlias->nDomainNum; 
					
					nB = 0, nE = (nOffsetListUsed[nWhich]-5)/5;	
					while(nB <= nE)
					{
						int nM = (nB+nE)/2;
						int nPos;
						int k;
						memcpy(&nPos, sOffsetList[nWhich] + nM*5, 4);	
						k = CompareMailbox(sMailboxList[nWhich]+nPos+4, sLine);
						if(k < 0)
							nB = nM+1;
						else if(k > 0)
							nE = nM-1;
						else
						{
							break;
						}
					}
					*sAliasE = ',';
					if(nB <= nE)
						continue;
				}
				
				if(nMailboxListSize[nWhich] - nMailboxListUsed[nWhich] < 2048)
				{
					(nMailboxListSize[nWhich]) += 10240;
					sMailboxList[nWhich] = (char*)realloc(sMailboxList[nWhich], nMailboxListSize[nWhich]);	
				}
				
				sPos = sMailboxList[nWhich] + nMailboxListUsed[nWhich];
				
				EndLine(sLine);
				i = 0;
				while(sTmp && *sTmp)
				{
					char *sTmp2 = strchr(sTmp, ',');
					if(sTmp2)
						*sTmp2 = 0;
					if(i && i < 50)
					{
						snprintf(sPos+nOffset, 400, "$%s$", sParamLine + nParamName[i]);
						nOffset += (strlen(sParamLine+nParamName[i]) + 2);
					}
					snprintf(sPos + nOffset, 400, "%s", sTmp);
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
				}
				memcpy(sOffsetList[nWhich]+nOffsetListUsed[nWhich], &(nMailboxListUsed[nWhich]), 4);
				memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+4, 0x0, 1);
				//if(KeepOrderFast(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich]) < 0)
				if(KeepOrderFast2(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich], nMailboxListUsed[nWhich]) < 0)	
					printf("LLLLLLLLLLLLL   -1 -1LLLLLLLLLLLL\n");
				(nOffsetListUsed[nWhich]) += 5;
				(nMailboxListUsed[nWhich]) += nOffset;
				(nCount[nWhich])++;
				(nSum[nWhich])++;
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
			pMailList->nSum = nSum[i];
			pMailList->nSend = 0;
			pMailList->nSuccess = 0;
			pMailList->nDelay = 0;
			pMailList->nFail = 0;
			pMailList->nNowPos = 0;
			memcpy(pMailList->nParamName, nParamName, 50*sizeof(int));
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
	free(nSum);
	
	return 0;
}


////从任务文件获取邮件列表
int ReadTaskFileWithLevel(char *sFileName, MailList *pMail, DomainAlias *pAlias, char **sModula, char *sID, 
														CONFIGURE *pConf, DABUDAI *pDabudai, char *sLog, char *sLogTag)
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
	int *nSum = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nActive = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nSilent = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nNotFound = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nNormal = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nFailNotFound = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nFailDns = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int *nFailSpam = (int*)malloc(sizeof(int) * (pAlias->nDomainNum+1));
	int nParamName[256];
	char sParamLine[1024*1024*4];
	int nStat = 0;
	char sIDBak[30];
	
	int nCheckNum = 0;/////////////
	
	{
		int i = 0;
		while(i < pAlias->nDomainNum + 1)
		{
			sMailboxList[i] = (char*)malloc(10240000);
			sOffsetList[i] = (char*)malloc(1024);
			nMailboxListSize[i] = 10240;
			nMailboxListUsed[i] = 0;
			nOffsetListSize[i] = 1024;
			nOffsetListUsed[i] = 0;
			nCount[i] = 0;
			nSum[i] = 0;
			nActive[i] = 0;
			nSilent[i] = 0;
			nNormal[i] = 0;
			nNotFound[i] = 0;
			nFailNotFound[i] = 0;
			nFailDns[i] = 0;
			nFailSpam[i] = 0;
			++i;
		}	
	}
	
	pIn = fopen(sFileName, "r");
	if(pIn == NULL)
	{
		free(sMailboxList);///////////////没有将2369行跟2370行的内存free掉
		free(sOffsetList);
		free(nMailboxListSize);
		free(nMailboxListUsed);
		free(nOffsetListSize);
		free(nOffsetListUsed);
		free(nCount);
		free(nSum);
		free(nActive);
		free(nSilent);
		free(nNormal);
		free(nNotFound);
		free(nFailNotFound);
		free(nFailDns);
		free(nFailSpam);
	
		return 0;	
	}
	
	while(fgets(sLine, sizeof(sLine)-1, pIn))
	{
		if(strncmp(sLine, "<$ID>", 5) == 0)
		{
			int n[3];
			fgets(sLine, 1000, pIn);
			sscanf(sLine, "%d.%d.%d", &(n[0]), &(n[1]), &(n[2]));
			snprintf(sIDBak, 30, "%d.%d.%d", n[0], n[1], n[2]);
			n[0] = htonl(n[0]);
			n[1] = htonl(n[1]);
			n[2] = htonl(n[2]);
			memcpy(sID, n, 12);
			while(fgets(sLine, sizeof(sLine)-1, pIn))
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
			while(fgets(sLine, sizeof(sLine)-1, pIn))
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
				{
					char *sTmpB;
					sTmpB = strrchr(sLine, '\r');
					if(!sTmpB)
					{
						sTmpB = strrchr(sLine, '\n');
						if(sTmpB)
						{
							*(sTmpB) = '\r';
							*(sTmpB+1) = '\n';
							*(sTmpB+2) = 0;	
						}	
					}	
				}		
				snprintf(s+nOffset, strlen(sLine)+1, "%s", sLine);
				nOffset += (strlen(sLine));
			}
		}
		else if(strncmp(sLine, "<$MAILLIST>", 11) == 0)
		{
			int nFinish = 0;
            int x_size = 1024000000/2;
			char *sList2 = (char*)malloc(x_size);
			char *sReturn = (char*)malloc(10240);
			int nSize = 10240;
			int nOffset = 0;
			int nSocket;// = TcpConnect(pConf->sCheckMailBoxIp, pConf->nCheckMailBoxPort, NULL, 5, NULL, 0);
			char sMsg[1024];
			char *sList = sList2+2;

			memset(nParamName, 0x0, sizeof(nParamName));
			fgets(sParamLine, sizeof(sParamLine)-1, pIn);
			{
				char *sTmp;
				int i = 1;
				
				EndLine(sParamLine);
				nParamName[0] = 0;
				sTmp = sParamLine;
				while(sTmp && i < sizeof(nParamName)/sizeof(nParamName[0]))
				{
					sTmp = strchr(sTmp, ','); //MSG:按逗号分割截取变量名称(首行是变量名称)
					if(!sTmp)
						break;
					*sTmp = 0;
					sTmp++;
					nParamName[i] = (sTmp - sParamLine);
					++i;
				}		
			}
			while(nFinish == 0)
			{
				char *sB1, *sE1, *sB2, *sE2;
				nOffset = 0;
				while(nFinish == 0 && nOffset < x_size)
				{
					if(fgets(sList+nOffset, 5*1024*1024, pIn) == NULL)
					{
						nFinish = 1;
						//////////////////////
					}
					else
					{
						if(strncmp(sList+nOffset, "</$MAILLIST>", 12) == 0)
						{	
							nFinish = 1;
							nOffset += strlen(sList+nOffset);
						}
						else if(strchr(sList+nOffset, '@'))
						{
							//nCheckNum++;
							nOffset += strlen(sList+nOffset);
						}
					}
				}
				//printf("=====%d=====\n", nCheckNum);
				//printf("aaaaaaaaaaaaaaaaaaaa((%s))", sList);///////////
				snprintf(sList+nOffset, 5, "\r\n\r\n");
				nOffset += 4;
				sReturn[0] = 0;
				nSocket = TcpConnect(pConf->sCheckMailBoxIp, pConf->nCheckMailBoxPort, NULL, 5, NULL, 0);
				if(nSocket >= 0)
				{
					int nLeng = 0;
					//printf("==BBB===%ld=====\n", time(NULL));
					snprintf(sMsg, 1024, "POST query HTTP/1.1\r\nHost: abc.com\r\nContent-Type: text/plain; charset=utf-8\r\n\r\n");
					SocketWrite(nSocket, pConf->nNetTimeOut, sMsg, strlen(sMsg));
					SocketWriteBig(nSocket, pConf->nNetTimeOut, sList, nOffset);
					ReceiveReply(&sReturn, &nSize, &nLeng, nSocket, 20);
					close(nSocket);
					//printf("==EEE===%ld=====\n", time(NULL));
					//printf("<<%s>>", sReturn);///////////////////////
				}
				sB1 = sList;
				sE1 = strchr(sList, '\n'); // 第一行 sB1---sE1
				if(sE1)
					*sE1 = 0;
				sB2 = sReturn;
				sE2 = strchr(sReturn, '\n');
				while(sB2 && sE2) //包含@字符的第一行 sB2---sE2
				{
					sB2 = sE2+1;
					sE2 = strchr(sB2, '\n');
					if(sE2)
					{
						*sE2 = 0;
						if(strchr(sB2, '@'))
							break;
					}
				}
				while(sB1 && sE1)
				{
					char *sTmp1, *sTmp2;
					int nScore = 0;
					char c = 0;
					if(*(sE1-1) == '\r')
						*(sE1-1) = 0;
					sTmp1 = strchr(sB1, ',');	
					if(sTmp1)
						*sTmp1 = 0;	
					if(strchr(sB1, '@') == NULL)
					{
						sB1 = sE1 + 1;
						sE1 = strchr(sB1, '\n');
						if(sE1)
							*sE1 = 0;
						continue;		
					}
					sTmp2 = strchr(sB2, ',');
					if(sTmp2)
						*sTmp2 = 0;
					
					//printf("[[%s]][[%s]]\n", sB1, sB2);	///////////////////
					
					if(*sB1 && strcasecmp(sB1, sB2) == 0)
					{
						nScore = atoi(sTmp2+1);
						//printf("score:%d, <%s><%s>\n", nScore, sB1, sB2);/////////////////////
						while(sB2 && sE2)
						{
							sB2 = sE2+1;
							sE2 = strchr(sB2, '\n');
							if(sE2)
							{
								*sE2 = 0;
								if(strchr(sB2, '@'))
									break;
							}
						}
					}
					if(nScore == 0)
					{
						c = 15;
						//printf("silent\n");
					}
					else if(nScore >= pConf->nActiveScore)	
					{
						c = 5;
					}
					else if(nScore < 0)
					{
						c = 0;
					}
					else
					{
						c = 10;
					}
					if(sTmp1)
						*sTmp1 = ',';
					if(sTmp2)
						*sTmp2 = ',';
					
					if(c)
					{
						int nWhich;
						char *sPos, *sPos2;
						char *sTmp, *sTmp2;
						int i = 0, nLeng = 0;
						int nOffset2 = 4;
						*(sB1-1) = c;
						{
							char *sAliasB = strchr(sB1-1, '@');
							char *sAliasE;
							int nB, nE;
							
							if(!sAliasB)
							{
								//printf("KKKK %s,%s\n", sB1, sE1);////////////////////
								sB1 = sE1 + 1;
								sE1 = strchr(sB1, '\n');
								if(sE1)
									*sE1 = 0;
								//printf("           %d   LLLLL\n", __LINE__);////////////
								continue;
							}
							sAliasE = strchr(sAliasB, ',');
							if(!sAliasE)
							{	
								//printf("KKKK %s,%s\n", sB1, sE1);////////////////////
								sB1 = sE1 + 1;
								sE1 = strchr(sB1, '\n');
								if(sE1)
									*sE1 = 0;
								//printf("            %d   LLLLL\n", __LINE__);////////////
								continue;
							}
							*sAliasE = 0;
							nWhich = FindAliasDomain(sAliasB+1, pAlias);
							if(nWhich < 0)
								nWhich = pAlias->nDomainNum; 
							
							nB = 0, nE = (nOffsetListUsed[nWhich]-5)/5;	
							while(nB <= nE)
							{
								int nM = (nB+nE)/2;
								int nPos;
								int k;
								memcpy(&nPos, sOffsetList[nWhich] + nM*5, 4);	
								k = CompareMailbox(sMailboxList[nWhich]+nPos+4, sB1-1);
								if(k < 0)
									nB = nM+1;
								else if(k > 0)
									nE = nM-1;
								else
								{
									break;
								}
							}
							*sAliasE = ',';
							if(nB <= nE)
							{
								//printf("KKKK %s,%s\n", sB1, sE1);////////////////////
								sB1 = sE1 + 1;
								sE1 = strchr(sB1, '\n');
								if(sE1)
									*sE1 = 0;
								//printf("            %d   LLLLL\n", __LINE__);////////////	
								continue;
							}
						}
						
						if(nMailboxListSize[nWhich] - nMailboxListUsed[nWhich] < 2048)
						{
							(nMailboxListSize[nWhich]) += 10240;
							sMailboxList[nWhich] = (char*)realloc(sMailboxList[nWhich], nMailboxListSize[nWhich]);	
						}
						
						sPos = sMailboxList[nWhich] + nMailboxListUsed[nWhich];
						
						i = 0;
						sTmp = sB1-1;
						while(sTmp && *sTmp)
						{
							char *sTmp2 = strchr(sTmp, ',');
							if(sTmp2)
								*sTmp2 = 0;
							if(i && i < sizeof(nParamName)/sizeof(nParamName[0]))
							{
								snprintf(sPos+nOffset2, 4*1024*1024, "$%s$", sParamLine + nParamName[i]);
								nOffset2 += (strlen(sParamLine+nParamName[i]) + 2);
							}
							snprintf(sPos + nOffset2, 4*1024*1024, "%s", sTmp);
							nOffset2 += (strlen(sTmp)+1);
							
							if(sTmp2)
								sTmp = sTmp2+1;
							else
								sTmp = 0;
							++i;
						}
						nLeng = nOffset2 - 5;
						//nLeng = htonl(nLeng);
						memcpy(sPos, &nLeng, 4);
						if(nOffsetListSize[nWhich] - nOffsetListUsed[nWhich] < 10)
						{
							(nOffsetListSize[nWhich]) += 1024;
							sOffsetList[nWhich] = (char*)realloc(sOffsetList[nWhich], nOffsetListSize[nWhich]);	
						}
						memcpy(sOffsetList[nWhich]+nOffsetListUsed[nWhich], &(nMailboxListUsed[nWhich]), 4);
						memset(sOffsetList[nWhich]+nOffsetListUsed[nWhich]+4, 0x0, 1);
						//if(KeepOrderFast(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich]) < 0)
						if(KeepOrderFast2(sOffsetList[nWhich], nOffsetListUsed[nWhich]+5, sMailboxList[nWhich], nMailboxListUsed[nWhich]) < 0)	
							printf("LLLLLLLLLLLLL   -1 -1LLLLLLLLLLLL\n");
						(nOffsetListUsed[nWhich]) += 5;
						(nMailboxListUsed[nWhich]) += nOffset2;
						(nCount[nWhich])++;
						(nSum[nWhich])++;
						if(*(sB1-1) == 5)
						{
							(nActive[nWhich])++;	
						}	
						else if(*(sB1-1) == 10)
						{
							(nNormal[nWhich])++;		
						}
						else if(*(sB1-1) == 15)
						{
							(nSilent[nWhich])++;	
						}
						
							
						//printf("offset:%d,list:%d,box:%d\n", nOffset2, nOffsetListUsed[nWhich], nMailboxListUsed[nWhich]);
						
					}
					else
					{
						char *sAliasB = strchr(sB1, '@');
						char *sAliasE;
						int nWhich = -1;	
						if(!sAliasB)
						{
							//printf("KKKK %s,%s\n", sB1, sE1);////////////////////
							sB1 = sE1 + 1;
							sE1 = strchr(sB1, '\n');
							if(sE1)
								*sE1 = 0;
							//printf("           %d   LLLLL\n", __LINE__);////////////
							continue;
						}
						sAliasE = strchr(sAliasB, ',');
						if(!sAliasE)
						{	
								//printf("KKKK %s,%s\n", sB1, sE1);////////////////////
								sB1 = sE1 + 1;
								sE1 = strchr(sB1, '\n');
								if(sE1)
									*sE1 = 0;
								//printf("            %d   LLLLL\n", __LINE__);////////////
								continue;
						}
						*sAliasE = 0;
						nWhich = FindAliasDomain(sAliasB+1, pAlias);
						if(nWhich < 0)
							nWhich = pAlias->nDomainNum; 
						//printf("BAD MAILBOX::::%s\n", sB1);////////////////
						if(sLog)
						{	
							snprintf(sLog, 1024, "[%s][REPORT][f][%s]%s(step -1 -->rcptresp:599 Address not found , judge by iedm system.)\n", 
																				sLogTag, sIDBak, sB1);
							WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						}
						(nNotFound[nWhich])++;
						(nCount[nWhich])++;
						(nSum[nWhich])++;
					}
					
					sB1 = sE1 + 1;
					sE1 = strchr(sB1, '\n');	
					if(sE1)
						*sE1 = 0;
					//printf("BeginAndEnd:<%s,%ld>", sB1, sE1);
				}
			}
			//if(nSocket >= 0)
				//close(nSocket);
			free(sReturn);
			free(sList2);
				
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
			pMailList->nSum = nSum[i];
			pMailList->nNormal = nNormal[i];
			pMailList->nActive = nActive[i];
			pMailList->nSilent = nSilent[i];
			pMailList->nNotFound = nNotFound[i];
			pMailList->nSend = 0;
			pMailList->nSuccess = 0;
			pMailList->nDelay = 0;
			pMailList->nFail = nNotFound[i];
			pMailList->nNowPos = 0;
			pMailList->nFailNotFound = 0;
			pMailList->nFailDns = 0;
			pMailList->nFailSpam = 0;
			memcpy(pMailList->nParamName, nParamName, 50*sizeof(int));
			memcpy(pMailList->sParamLine, sParamLine, 1024);
			//printf("BAD MAILBOX::::%s\n", sB1);////////////////
			if(sLog)
			{
				if(i < pAlias->nDomainNum)
					snprintf(sLog, 1024, "[%s][NEWTASK]task:%s, domain:%s, total:%d, active:%d, normal:%d, silent:%d, notfound:%d.\n", 
															sLogTag, sIDBak, (pAlias->sDomain)+34*i, nCount[i], nActive[i], nNormal[i], nSilent[i], nNotFound[i]);
				else
					snprintf(sLog, 1024, "[%s][NEWTASK]task:%s, domain:other, total:%d, active:%d, normal:%d, silent:%d, notfound:%d.\n", 
															sLogTag, sIDBak,  nCount[i], nActive[i], nNormal[i], nSilent[i], nNotFound[i]);
				//WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			}
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
	free(nSum);
	free(nActive);
	free(nSilent);
	free(nNormal);
	free(nNotFound);
	free(nFailNotFound);
	free(nFailDns);
	free(nFailSpam);
	
	return 0;
}


int ChangeMailboxStatWithLevel(TaskQueue *pBegin, TaskQueue *pEnd, char *sID, char *sMailbox, int nStat)
{
	char *sDomain;
	int nWhich = 0;
	MailList *p;
	MailList *pMailList;
	DomainAlias *pAlias;
	char *sOffsetList;
	char *sMailboxList;
	TaskQueue *pTaskUsed = pBegin;
	int iii = 0;
	
								/*{
									int i = 0;
									printf("tttttttttttt");
									while(i < 12)
									{
										printf("[%d]<%02x>", i, *(sID+i));
										++i;
									}	
									printf("\n");
								}*/
	
	while(pTaskUsed)
	{
		if(memcmp(pTaskUsed->sID, sID, 12) == 0)
			break;
		pTaskUsed = pTaskUsed->pNext;	
	}
	if(pTaskUsed == NULL)
		return -1;
		
	pMailList = pTaskUsed->pMailList;
	pAlias = pTaskUsed->pAlias;
	
	sDomain = strchr(sMailbox, '@');
	if(sDomain == NULL)
		return -1;
	
	nWhich = FindAliasDomain(sDomain+1, pAlias);
	if(nWhich < 0)
		nWhich = pAlias->nDomainNum;
	
	p = pMailList + nWhich;
	sOffsetList = p->sOffsetList;
	sMailboxList = p->sMailboxList;
	
	if(p->pLock)
		pthread_mutex_lock(p->pLock);
	while(iii < 3)
	{
		int nB = 0, nE = (p->nOffsetListUsed-5)/5;
		char ss[100];
		snprintf(ss+1, 90, "%s", sMailbox);
		++iii;
		ss[0] = iii*5;
		while(nB <= nE)
		{
			int nM = (nB+nE)/2;
			int nPos;
			int k;
			memcpy(&nPos, sOffsetList + nM*5, 4);	
			k = CompareMailbox(sMailboxList+nPos+4, ss);
			if(k < 0)
				nB = nM+1;
			else if(k > 0)
				nE = nM-1;
			else
			{
				if(nStat == D_MailList_Success)
				{
					(p->nCount)--;
					(p->nSuccess)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Success)
						(p->nSuccess)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}
				}
				else if(nStat == D_MailList_Fail)
				{
					(p->nFail)++;
					///printf("yyyyyyyyyyyyyyyy::%d::\n", *(sOffsetList+nM*5+4));
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;	
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}
				}
				else if(nStat == D_MailList_Delay)
				{
					(p->nDelay)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}	
				}
				else if(nStat == D_MailList_Dns)
				{
					(p->nDelay)++;
					(p->nFailDns)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}	
				}
				else if(nStat == D_MailList_NotFound)
				{
					(p->nFail)++;
					(p->nFailNotFound)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}	
				}
				else if(nStat == D_MailList_Spam)
				{
					(p->nFail)++;
					(p->nFailSpam)++;
					if(*(sOffsetList+nM*5+4) == D_MailList_Fail)
						(p->nFail)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_Delay)
						(p->nDelay)--;
					else if(*(sOffsetList+nM*5+4) == D_MailList_NotFound)
					{
						(p->nFail)--;
						(p->nFailNotFound)--;		
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Dns)
					{
						(p->nDelay)--;
						(p->nFailDns)--;	
					}
					else if(*(sOffsetList+nM*5+4) == D_MailList_Spam)
					{
						(p->nFail)--;
						(p->nFailSpam)--;	
					}		
				}
				*(sOffsetList+nM*5+4) = nStat;
				
				if(p->pLock)
					pthread_mutex_unlock(p->pLock);
				return 0;
			}
		}	
		
	}
	if(p->pLock)
		pthread_mutex_unlock(p->pLock);
	//printf("NOT FOUND!\n");
	
	return -1;
}



int DelOldTrigerNum(CONFIGURE *pConf, DABUDAI *pDabudai)
{
	time_t tTime;
	struct tm sctTime;
	int nDate;
	DBC *dbc;
	DBT dbtKey, dbtData;
	
	tTime = time(NULL) - 3600*2; // 2 小时前
	localtime_r(&tTime, &sctTime);
	
	nDate = (sctTime.tm_year+1900)*10000 + (sctTime.tm_mon+1)*100 + (sctTime.tm_mday);
	nDate += 1000000000;
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	//printf("deloldtriger:::%d\n", nDate);
    //printf("pDabudai = %p,  pDabudai->dbNumber=%p\n", pDabudai, pDabudai->dbNumber);

    if (pConf==NULL || pDabudai==NULL || pDabudai->dbNumber==NULL)
        return -1;
	
	if (0 != pDabudai->dbNumber->cursor(pDabudai->dbNumber, NULL, &dbc, 0))
        return -1;

	while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		int *n = (int*)(dbtKey.data);
		if(ntohl(n[1]) < nDate)
		{
		//	printf("deloldtriger:del:%d.%d.%d\n", ntohl(n[0]), ntohl(n[1]), ntohl(n[2]));
			if(pDabudai->dbNumber->del(pDabudai->dbNumber, NULL, &dbtKey, 0))
			{
		//		printf("deloldtriger fail\n");	
			}	
		}	
	}
	
	dbc->c_close(dbc);
	return 0;
}





int ChangeMailboxStatTriger(CONFIGURE *pConf, DABUDAI *pDabudai, char *sID, char *sMailbox, int nStat, DBC *dbc, char *cStat)
{
	DBT dbtKey, dbtData;
	//DBC *dbc;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	dbtKey.data = sID;
	dbtKey.size = 12;
		
	if(nStat == D_MailList_Success)
	{
		int k;
		k = dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET);
		if(k == 0)
		{
			*cStat = *((char*)(dbtData.data+1));	
		}
		if( (k = pDabudai->dbList->del(pDabudai->dbList, NULL, &dbtKey, 0)) == 0 )
		{	
			char *s = (char*)(dbtData.data);
			int *nID = (int*)(dbtKey.data);
			printf("trigger success and delete:[%d.%d.%d]%s\n", ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), sMailbox);
			return 0;
		}
		else
		{
			int *nID = (int*)(dbtKey.data);
			printf("trigger success but not find<%s>[%d.%d.%d]%s\n", db_strerror(k), ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]), sMailbox);	
			return -1;
		}
	}
	else
	{
		char *s;
		time_t *t;
		int *nID = (int*)(dbtKey.data);
		if(dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET))
		{
			printf("not find fail 222222:<%s>[%d.%d.%d]\n", sMailbox, ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]));
			return -2;
		}
		s = (char*)(dbtData.data);
		*cStat = *((char*)(dbtData.data+1));	
		*(s+1) = nStat;
		t = (time_t*)(s+2);
		*t = *t - pConf->nTrigerRetryPeriod;
		if(dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT))
		{
			printf("not find fail 333333333:<%s>[%d.%d.%d]\n", sMailbox, ntohl(nID[0]), ntohl(nID[1]), ntohl(nID[2]));
			return -3;
		}
	}
	
	return 0;
}




