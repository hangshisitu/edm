#include "onesideloveonesidefriend.h"



const char* sWeekName[] = 
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char* sMonthName[] = 
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};



char* GetNextLineFromStr(char **sLine, char **sLineEnd)
{
    char *s1 = *sLineEnd + 1;

    if(*sLineEnd == NULL)
        return NULL;
    *sLine = s1;
    *sLineEnd = strchr(s1, '\n');
    if(*sLineEnd)
    {
        *(*sLineEnd) = 0;
        if(*(*sLineEnd - 1) == '\r')
            *(*sLineEnd - 1) = 0;
    }
    //printf("<<%s>>\n", s1);

    return *sLineEnd;
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



int TreatChar(char *sChart, char *sChartVariant, int *nYRaid, int *nXRaid)
{
    char *sTmp1, *sTmp2;
    int n1 = 0;

    sTmp1 = sChart;
    while(sTmp1)
    {
        sTmp2 = NULL;
        sTmp1 = strstr(sTmp1, "<$Y>");
        if(sTmp1)
        {
            sTmp2 = strstr(sTmp1, "</$Y>");
            if(sTmp2)
            {
                char s[20];
                snprintf(s, 20, " $CH%dYYY$ ", *nYRaid);
                memcpy(sTmp1, s, strlen(s));
                (*nYRaid)++;	
                sTmp1 = sTmp2+5;
            }	
        }
        if(sTmp2 == NULL)
            sTmp1 = NULL;	
    }

    sTmp1 = sChart;
    while(sTmp1)
    {
        sTmp2 = NULL;
        sTmp1 = strstr(sTmp1, "<$COST>");	
        if(sTmp1)
        {
            char s[10];
            snprintf(s, 10, "         ");
            memcpy(sTmp1, s, 7);
            sTmp2 = strstr(sTmp1, "</$COST>");
            if(sTmp2)
            {
                *sTmp2 = 0;
                {
                    char *s1, *s2;
                    s1 = strchr(sTmp1, '$');
                    snprintf(sChartVariant+40*(*nXRaid), 40, "%s", s1);
                    s2 = strchr(sChartVariant+40*(*nXRaid)+1, '$');
                    if(s2)
                        *(s2+1) = 0;	
                }
                memcpy(sTmp2, s, 8);
                (*nXRaid)++;
                sTmp1 = sTmp2+8;
            }
        }
        if(sTmp2 == NULL)
            sTmp1 = NULL;
    }

    sTmp1 = sChart;
    while(sTmp1)
    {
        sTmp2 = NULL;
        sTmp1 = strstr(sTmp1, "<$X>");
        if(sTmp1)
        {
            sTmp2 = strstr(sTmp1, "</$X>");
            if(sTmp2)
            {
                char s[20];
                snprintf(s, 20, " $CH%dXXX$ ", n1);
                memcpy(sTmp1, s, strlen(s));
                n1++;	
                sTmp1 = sTmp2+5;
            }	
        }
        if(sTmp2 == NULL)
            sTmp1 = NULL;	
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
    char sFileName[300];
    char sID[100];
    char sTaskFileName[200];
	enum { REQ_TASK_SUCCESS = 0, REQ_TASK_NOT_FOUND = 1, REQ_TASK_NOT_REPLY = 2, REQ_TASK_NOT_ACTIVE = 3};
	int nstop_status = REQ_TASK_SUCCESS;

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

    //printf("BBBBB  ===%ld======\n", time(NULL));
    k = SocketReadDoubleLine(nSocket, 3, sMsg, 10240);
    //printf("EEEEE ====%ld======\n", time(NULL));
    if(k <= 0)
    {
        snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]receive fail!\n");
        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        close(nSocket);
        free(sMsg);
        return (void*)0;
    }

    //printf("%d[[%s]]", k, sMsg);
    //fflush(NULL);


    if(strncasecmp(sMsg, "post", 4) == 0)
    {
        char *sTmp = sMsg;
        char *sTmp2;
        int nLeng = 0;
        int k1 = 0, k2 = 0;
        int nSize = 10240;
        char sPipeName[100];

        sPipeName[0] = 0;


        //k1 = SocketReadDoubleLine(nSocket, 3, sMsg, nSize);
        // please use strcasestr
        sTmp = strstr(sMsg, "Content-Length:");
        if(!sTmp)
            sTmp = strstr(sMsg, "content-Length:");
        if(!sTmp)
            sTmp = strstr(sMsg, "Content-length:");
        if(!sTmp)
            sTmp = strstr(sMsg, "Content-length:");  // 15 == strlen(Content-length:)
        if(sTmp)
        {
            sTmp += 15;
            while(*sTmp == ' ')
                sTmp++;
            sscanf(sTmp, "%d", &nLeng);
            sTmp = strstr(sMsg, "\r\n\r\n");
            if(!sTmp) //no found
            {
                sTmp = strstr(sMsg, "\n\n");
                if(sTmp)
                {
                    sTmp += 2; //˽¾ݲ¿·ԍ
                    k2 = nLeng - (k-(sTmp-sMsg)); //ʣԠŚɝ³¤¶ȣº˽¾ݳ¤¶ƨContent-length) - (ӑ¶s¤¶Ơ- httpͷԲ³¤¶Ʃ
                    if(k2 + k + 10 > nSize)//»º³戸ӧ³�©և
                    {
                        sMsg = (char*)realloc(sMsg, k+k2+10);
                        nSize = k+k2+10;
                    }
                    if(k2 > 0)
                        k2 = SocketRead2(nSocket, 30, sMsg+k, k2); //¶£Ԡµ¬ͬ²½·ȗ�?
                }
            }
            else
            {
                sTmp += 4;	//ºΉЃ涄²՚Ԛ´ˡ£¡£¡£
                k2 = nLeng - (k-(sTmp-sMsg));
                if(k2 + k + 10 > nSize)
                {
                    sMsg = (char*)realloc(sMsg, k+k2+10);
                    nSize = k+k2+10;
                }
                if(k2 > 0)
                    k2 = SocketRead2(nSocket, 30, sMsg+k, k2);
            }
        }
        else
        {
            //Ӏ¾ܜr\n ½�?ɧ¹�א°�\n\r\nµĻ°£¬ղ˽¾ݽ«±»¸�?
            k2 = SocketReadDoubleLine(nSocket, 30, sMsg+k, nSize-k);
        }
        //printf("\nYYYYYYYYY%d  %dYYYYYYYYYYYY\n", k, k2);
        {	
            char s[100] = {0};
            snprintf(s, 100, "HTTP/1.1 200 - %d/%d\r\n\r\n",k2 + (k-(sTmp-sMsg)),nLeng); //·µ»ֲ00 ֻŜ˵ķ˕µ½ΪֻµŊ�»´�¾ފȕ�?
            SocketWrite(nSocket, 3, s, strlen(s));
            close(nSocket);
        }
		//------------req end�� handler begin ----------
        {
            char *sData = (char*)malloc(nSize*2+10240);
            int nOffset = 0;
            char *sLine, *sLineEnd;
            int nDataSize = nSize*2+10240;
			//-------msg parse
            sLine = sMsg;
            sLineEnd = sMsg-1;

            {

                char *sTmp;
                char sFileTmp[200];
                char sSubject[1024];
                int nParamInSubject = 0;
                char sNickName[100];
                char sSender[100];
                char sTime[100];
                char sTime2[100];
                char sReplyTo[100];
                char sMailFrom[1024];
                int  using_duo = 0;
                char cFlag = 0;
                int nSameFromFlag = 0;
                int nIsRandomFlag = 0;
                //char sPipeName[50];
                int nAD = 0;
                //int nNotifyType = 9;
                char sNotifiType[8] = {0};
                sNotifiType[0] = '0';
                char sHelo[50];
                char sChartVariant[800];
                char *sChart = (char*)malloc(20480);
                int nYRaid = 0, nXRaid = 0;
                int istestmail = -1;

                //V2.8.1 2014-10-14
                char *pstrModulaText = NULL;
                char *pstrModulaQVGA = NULL;
                int  nModulaLen = 10 * 1024 * 1024; //10M

                sHelo[0] = '0';
                sHelo[1] = 0;
                sSubject[0] = 0;
                sNickName[0] = 0;
                sSender[0] = 0;
                sReplyTo[0] = 0;
                sMailFrom[0] = 0;
                sTime[0] = 0;
                sTime2[0] = 0;

                {
                    time_t tTime;
                    struct tm tmTime;
                    int nZone = 8;
                    char sZone[20];
                    time(&tTime);
                    localtime_r(&tTime, &tmTime);
                    if(nZone > 0)
                        snprintf(sZone, sizeof(sZone), "+%02d00", nZone);
                    else
                        snprintf(sZone, sizeof(sZone), "-%02d00", -nZone);
                    snprintf(sTime, sizeof(sTime), "%s, %02d %s %d %02d:%02d:%02d %s (CST)",
                        sWeekName[tmTime.tm_wday], tmTime.tm_mday, sMonthName[tmTime.tm_mon], tmTime.tm_year+1900, 
                        tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, sZone);	
                    tTime -= 100;
                    localtime_r(&tTime, &tmTime);
                    snprintf(sTime2, sizeof(sTime2), "%s, %02d %s %d %02d:%02d:%02d %s",
                        sWeekName[tmTime.tm_wday], tmTime.tm_mday, sMonthName[tmTime.tm_mon], tmTime.tm_year+1900, 
                        tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, sZone);	
                }

                {
                    unsigned char *s = (unsigned char*)(&nIp);
                    snprintf(sFileTmp, sizeof(sFileTmp), "%s/%d.%d.%d.%d.%04d.%d", pConf->sTaskFileDir, s[0], s[1], s[2], s[3], n, time(NULL)); //n ˇһ¸�ì´ԏ͈߳쀚´�?
                }

                sID[0] = 0;
                //while(fgets(sLine, sizeof(sLine), pFile))
                while(GetNextLineFromStr(&sLine, &sLineEnd))
                {
                    char sBak[1024];

                    //fprintf(pFile2, "%s", sLine);
                    //printf("<<%s>>:%d\n\n", sLine, nAD);
                    snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n", sLine);
                    nOffset += strlen(sData+nOffset);
                    //printf("YYYYYYYY  %d  YYYYYYYYYY\n", nOffset);
                    if(strncmp(sLine, "<$ID>", 5) == 0)
                    {	
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n", sLine);
                        nOffset += strlen(sData+nOffset);
                        snprintf(sID, sizeof(sID), "%s", sLine);
                    }

                    else if(strncasecmp(sLine, "<$TEST>", 7) == 0)
                    {
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n", sLine);
                        nOffset += strlen(sData+nOffset);
                        if(strstr(sLine, "true"))
                            istestmail = 1;	
                    }
                    else if(strncasecmp(sLine, "<$AD>", 5) == 0)
                    {
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n", sLine);
                        nOffset += strlen(sData+nOffset);
                        if(strstr(sLine, "true"))
                            nAD = 1;	
                    }
                    else if(strncasecmp(sLine, "<$CHART>", 8) == 0)
                    {
                        int nOffsetChart = 0;
                        int nSizeChart = 20480;
                        //fprintf(pFile2, "%s", sLine);
                        while(GetNextLineFromStr(&sLine, &sLineEnd))
                        {
                            //fprintf(pFile2, "%s", sLine);
                            if(strncasecmp(sLine, "</$CHART>", 9) == 0)
                                break;	
                            snprintf(sChart+nOffsetChart, nSizeChart-nOffsetChart, "%s\r\n", sLine);
                            nOffsetChart += (strlen(sLine)+2);
                            if(nSizeChart - nOffsetChart < 1024)
                            {
                                nSizeChart += 10240;
                                sChart = (char*)realloc(sChart, nSizeChart);	
                            }
                        }
                        TreatChar(sChart, sChartVariant, &nYRaid, &nXRaid);	
                    }
                    else if(strncasecmp(sLine, "<$HELO>", 7) == 0)
                    {
                        char *sTmp;
                        //fgets(sHelo, 50, pFile);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sHelo, 50, "%s", sLine);
                        if(*sHelo == '<')
                        {
                            sHelo[0] = '0';
                            sHelo[1] = 0;	
                        }	
                        sTmp = strchr(sHelo, '\r');
                        if(!sTmp)
                            sTmp = strchr(sHelo, '\n');
                        if(sTmp)
                            *sTmp = 0;
                        if(*sHelo == 0)
                        {
                            sHelo[0] = '0';
                            sHelo[1] = 0;		
                        }
                    }
                    else if(strncasecmp(sLine, "<$ISRADOM>", 10) == 0)
                    {
                        char *sTmp;
                        sTmp = strstr(sLine, "true");	
                        if(sTmp)
                        {
                            nIsRandomFlag = 1;	
                        }
                    }
                    else if(strncmp(sLine, "<$PIPE>", 7) == 0)
                    {
                        char *sTmp;
                        //fgets(sPipeName, sizeof(sPipeName), pFile);
                        //fprintf(pFile2, "%s", sPipeName);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sPipeName, sizeof(sPipeName), "%s", sLine);
                        sTmp = strchr(sPipeName, '\r');
                        if(!sTmp)
                            sTmp = strchr(sPipeName, '\n');
                        if(sTmp)
                            *sTmp = 0;	
                    }
                    else if(strncmp(sLine, "<$SUBJECT>", 10) == 0)
                    {
                        char *sTmp;
                        size_t nOutLeng;
                        //fgets(sSubject, sizeof(sSubject)-10, pFile);
                        //printf("SSSS    (%s)\n\n", sSubject);/////////////////********************
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sSubject, sizeof(sSubject)-10, "%s", sLine);
                        if(nAD == 1)
                        {
                            memmove(sSubject+9, sSubject, strlen(sSubject)+1);
                            memcpy(sSubject, "$iedm_ad$", 9);
                            //$iedm_ad$	
                        }
                        //printf("SSSS    (%s)\n\n", sSubject);//////////////********************
                        sTmp = strchr(sSubject, '\r');
                        if(!sTmp)
                            sTmp = strchr(sSubject, '\n');
                        if(sTmp)
                            *sTmp = 0;
                        sTmp = strchr(sSubject, '$');
                        if(sTmp)
                        {
                            sTmp = strchr(sTmp+1, '$');
                            if(sTmp)
                                nParamInSubject = 1;	
                        }
                        if(nParamInSubject == 0)
                        {
                            if(encode_base64(sSubject, strlen(sSubject), sBak, 1020, &nOutLeng) >= 0)
                            { 
                                char *ss = strchr(sBak, '\r');
                                if(!ss)
                                    ss = strchr(sBak, '\n');
                                if(ss)
                                    *ss = 0;
                                snprintf(sSubject, sizeof(sSubject), "=?utf-8?B?%s?=\n", sBak);
                            }
                            else
                            {
                                *sTmp = '\n';
                                *(sTmp+1) = 0;
                            }
                        }
                        //fprintf(pFile2, "%s\r\n", sSubject);
                        //snprintf(sData+nOffset, nDataSize-nOffset, "%s", );
                        //nOffset += strlen(sData+nOffset);
                        sTmp = strchr(sSubject, '\r');
                        if(!sTmp)
                            sTmp = strchr(sSubject, '\n');
                        if(sTmp)
                            *sTmp = 0;
                    }
                    else if(strncmp(sLine, "<$SENDER>", 9) == 0)
                    {
                        char *sTmp;
                        //fgets(sSender, sizeof(sSender), pFile);
                        //fprintf(pFile2, "%s", sSender);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sSender, sizeof(sSender), "%s", sLine);
                        sTmp = strchr(sSender, '\r');
                        if(!sTmp)
                            sTmp = strchr(sSender, '\n');
                        if(sTmp)
                            *sTmp = 0;	
                    }
                    else if(strncmp(sLine, "<$NICKNAME>", 11) == 0)
                    {
                        char *sTmp;
                        size_t nOutLeng;
                        //fgets(sNickName, sizeof(sNickName)-1, pFile);
                        //fprintf(pFile2, "%s", sNickName);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sNickName, sizeof(sNickName), "%s", sLine);
                        sTmp = strchr(sNickName, '\r');
                        if(!sTmp)
                            sTmp = strchr(sNickName, '\n');
                        if(sTmp)
                            *sTmp = 0;	
                        if(encode_base64(sNickName, strlen(sNickName), sBak, 1020, &nOutLeng) >= 0)
                        { 
                            char *ss = strchr(sBak, '\r');
                            if(!ss)
                                ss = strchr(sBak, '\n');
                            if(ss)
                                *ss = 0;
                            snprintf(sNickName, sizeof(sNickName), "=?utf-8?B?%s?=\n", sBak);
                        }
                        else
                        {
                            *sTmp = '\n';
                            *(sTmp+1) = 0;
                        }
                        //fprintf(pFile2, "%s", sNickName);
                        sTmp = strchr(sNickName, '\r');
                        if(!sTmp)
                            sTmp = strchr(sNickName, '\n');
                        if(sTmp)
                            *sTmp = 0;
                    }
                    else if(strncmp(sLine, "<$REPLIER>", 10) == 0)
                    {
                        char *sTmp;
                        //fgets(sReplyTo, sizeof(sReplyTo), pFile);
                        //fprintf(pFile2, "%s", sReplyTo);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sReplyTo, sizeof(sReplyTo), "%s", sLine);
                        sTmp = strchr(sReplyTo, '\r');
                        if(!sTmp)
                            sTmp = strchr(sReplyTo, '\n');
                        if(sTmp)
                            *sTmp = 0;	
                    }	
                    else if(strncmp(sLine, "<$ROBOT>", 8) == 0)
                    {
                        char *sTmp;
                        //fgets(sMailFrom, sizeof(sMailFrom), pFile);
                        //fprintf(pFile2, "%s", sMailFrom);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sMailFrom, sizeof(sMailFrom), "%s", sLine);
                        sTmp = strchr(sMailFrom, '\r');
                        if(!sTmp)
                            sTmp = strchr(sMailFrom, '\n');
                        if(sTmp)
                            *sTmp = 0;	
                            
                        if (strchr(sMailFrom, ';')) {
                        	using_duo = 1;
                        }
                    }	
                    else if(strncmp(sLine, "<$RAND>", 7) == 0)
                    {
                        char *sTmp;
                        //fgets(sLine, sizeof(sLine), pFile);
                        //fprintf(pFile2, "%s", sLine);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        //snprintf(sNickName, sizeof(sNickName), "%s", sLine);
                        if(strstr(sLine, "true") || strstr(sLine, "TRUE"))
                            cFlag = 1;
                        else
                            cFlag = 0;	
                    }
                    else if(strncmp(sLine, "<$SAMEFROM>", 11) == 0)
                    {
                        char *sTmp;
                        //fgets(sLine, sizeof(sLine), pFile);
                        //fprintf(pFile2, "%s", sLine);
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        //snprintf(sNickName, sizeof(sNickName), "%s", sLine);
                        if(strstr(sLine, "true") || strstr(sLine, "TRUE"))
                            nSameFromFlag = 1;
                        else
                            nSameFromFlag = 0;	
                    }

                    //тն
                    else if(strncasecmp(sLine, "<$SMS>", 6) == 0)
                    {
                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sNotifiType, sizeof(sNotifiType), "%s", sLine);
                        int i;
                        for (i=0; i<sizeof(sNotifiType); i++){
                            if (sNotifiType[i] == '\r' || sNotifiType[i] == '\n')
                                sNotifiType[i] = '\0';
                        }
                    }

                    //V2.8.1 2014-10-14
                    else if(strncasecmp(sLine, "<$MODULA_TEXT>", strlen("<$MODULA_TEXT>")) == 0)
                    {
                        if (pstrModulaText){
                            free(pstrModulaText);
                            pstrModulaText = NULL;
                        }

                        int offset = 0;
                        pstrModulaText = (char*)malloc(nModulaLen); //10M
                        memset(pstrModulaText,0x00,nModulaLen);

                        while (GetNextLineFromStr(&sLine, &sLineEnd)){
                            if(strncasecmp(sLine, "</$MODULA_TEXT>", strlen("</$MODULA_TEXT>")) == 0)
                                break;
                            if (offset < nModulaLen)
                                offset += snprintf(pstrModulaText+offset, nModulaLen-offset, "%s\n",sLine);
                        };
                    }

                    //V2.8.1 2014-10-14
                    else if(strncasecmp(sLine, "<$MODULA_QVGA>", strlen("<$MODULA_QVGA>")) == 0)
                    {
                        if (pstrModulaQVGA){
                            free(pstrModulaQVGA);
                            pstrModulaQVGA = NULL;
                        }

                        int offset = 0;
                        pstrModulaQVGA = (char*)malloc(nModulaLen); //10M
                        memset(pstrModulaQVGA,0x00, nModulaLen);

                        while (GetNextLineFromStr(&sLine, &sLineEnd)){
                            if(strncasecmp(sLine, "</$MODULA_QVGA>", strlen("</$MODULA_QVGA>")) == 0)
                                break;
                            if (offset < nModulaLen)
                                offset += snprintf(pstrModulaQVGA+offset, nModulaLen-offset, "%s\n",sLine);
                        };
                    }

                    else if(strncmp(sLine, "<$MODULA>", 9) == 0)
                    {
                        int nFinish = 0;
                        char sSender2[100] = {0};
                        if(nSameFromFlag)
                        {
                            char *sTmp = strchr(sSender, '@');
                            if(sTmp)
                            {
                                *sTmp = 0; //Ј½ض͍
                                if(nIsRandomFlag == 1)
                                    snprintf(sSender2, sizeof(sSender2), "%s$iedm_ran$@%s", sSender, sTmp+1);
                                else
                                    snprintf(sSender2, sizeof(sSender2), "%s@%s", sSender, sTmp+1);
                                *sTmp = '@'; //ՙ»ָ´
                            }
                            else
                            {
                                sSender2[0] = 0;	
                            }	
                        }
                        {	
                            if(sMailFrom[0])
                            {
                                //fprintf(pFile2, "$MAIL_FROM$%s\r\n", sMailFrom);
                                snprintf(sData+nOffset, nDataSize-nOffset, "$MAIL_FROM$%s\r\n", sMailFrom);
                                nOffset += strlen(sData+nOffset);
                            }
                            else
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "$MAIL_FROM$\r\n");
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "$MAIL_FROM$\r\n", sMailFrom);
                            }
                            if(cFlag)
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "$FLAG$1\r\n");
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "$FLAG$1\r\n");
                            }
                            else
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "$FLAG$0\r\n");
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "$FLAG$0\r\n");
                            }
                            if(nSameFromFlag)
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "$SAMEFROM$1\r\n");
                                nOffset += strlen(sData+nOffset);
                            }
                            else
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "$SAMEFROM$0\r\n");
                                nOffset += strlen(sData+nOffset);
                            }
                                if(sSender2[0])
                                {
                                    snprintf(sData+nOffset, nDataSize-nOffset, "$FROM$%s\r\n", sSender2);
                                    nOffset += strlen(sData+nOffset);
                                    //fprintf(pFile2, "$FROM$%s\r\n", sSender2);
                                }
                                else
                                {
                                    //fprintf(pFile2, "$FROM$%s\r\n", sSender);
                                    snprintf(sData+nOffset, nDataSize-nOffset, "$FROM$%s\r\n", sSender);
                                    nOffset += strlen(sData+nOffset);
                                }                            
                            
                            //fprintf(pFile2, "$HELO$%s\r\n", sHelo);
                            //fprintf(pFile2, "Received: from localhost ([127.0.0.1])\r\n\tby iedm for <$iedm_to$>; %s\r\n", sTime);
                            snprintf(sData+nOffset, nDataSize-nOffset, "$HELO$%s\r\n", sHelo);
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "Received: from localhost ([127.0.0.1])\r\n\tby iedm for <$iedm_to$>; %s\r\n", sTime);
                            nOffset += strlen(sData+nOffset);

                            if(sNickName[0])
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "From: \"%s\" <$iedm_from_01$>\r\n", sNickName);
                                nOffset += strlen(sData+nOffset);
                            	/*
                                if(nSameFromFlag && sSender2[0])
                                {
                                    //fprintf(pFile2, "From: \"%s\" <%s>\r\n", sNickName, sSender2);
                                    snprintf(sData+nOffset, nDataSize-nOffset, "From: \"%s\" <%s>\r\n", sNickName, sSender2);
                                    nOffset += strlen(sData+nOffset);
                                }
                                else
                                {
                                    snprintf(sData+nOffset, nDataSize-nOffset, "From: \"%s\" <%s>\r\n", sNickName, sSender);
                                    nOffset += strlen(sData+nOffset);
                                    //fprintf(pFile2, "From: \"%s\" <%s>\r\n", sNickName, sSender);
                                }
                                */
                            }
                            else
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "From: $iedm_from_01$\r\n");
                                nOffset += strlen(sData+nOffset);
                            	/*
                                if(nSameFromFlag && sSender2[0])
                                {
                                    snprintf(sData+nOffset, nDataSize-nOffset, "From: %s\r\n", sSender2);
                                    nOffset += strlen(sData+nOffset);
                                    //fprintf(pFile2, "From: %s\r\n", sSender2);
                                }
                                else
                                {	
                                    snprintf(sData+nOffset, nDataSize-nOffset, "From: %s\r\n", sSender);
                                    nOffset += strlen(sData+nOffset);
                                    //fprintf(pFile2, "From: %s\r\n", sSender);
                                }
                                */
                            }
                            //fprintf(pFile2, "To: $iedm_to$\r\n");
                            snprintf(sData+nOffset, nDataSize-nOffset, "To: $iedm_to$\r\n");
                            nOffset += strlen(sData+nOffset);
                            if(nParamInSubject == 0)
                            {	
                                snprintf(sData+nOffset, nDataSize-nOffset, "Subject: %s\r\n", sSubject);
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "Subject: %s\r\n", sSubject);
                            }
                            else
                            {	
                                snprintf(sData+nOffset, nDataSize-nOffset, "Subject: $iedm_subject$\r\n");
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "Subject: $iedm_subject$\r\n");
                            }
                            //fprintf(pFile2, "Date: %s\r\n", sTime2);
                            snprintf(sData+nOffset, nDataSize-nOffset, "Date: %s\r\n", sTime2);
                            nOffset += strlen(sData+nOffset);
                            if(sReplyTo[0])
                            {	
                                snprintf(sData+nOffset, nDataSize-nOffset, "Reply-To: <%s>\r\n", sReplyTo);
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "Reply-To: <%s>\r\n", sReplyTo);
                            }
                            else
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "Reply-To: <%s>\r\n", pConf->sReplyTo);
                                nOffset += strlen(sData+nOffset);
                                //fprintf(pFile2, "Reply-To: <%s>\r\n", pConf->sReplyTo);
                            }
                            //fprintf(pFile2, "Return-Path: <%s>\r\n", pConf->sReturnPath);

                            //if (strlen(sNotifiType) > 0)
                            {
                                snprintf(sData+nOffset, nDataSize-nOffset, "X-RICHINFO:NOTIFYTYPE:%s;ISMULTITEMPLATE:%d;TEMPLATELIST:1_2_4\r\n",sNotifiType, pstrModulaQVGA?1:0);
                                nOffset += strlen(sData+nOffset);
                            }

                            if (istestmail != -1){ 
                                snprintf(sData+nOffset, nDataSize-nOffset, "ISTESTMAIL:%d\r\n",istestmail);
                                nOffset += strlen(sData+nOffset);
                            }

                            snprintf(sData+nOffset, nDataSize-nOffset, "X-mailer: iedm version 1.0\r\n");
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "MIME-Version: 1.0\r\n");
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "Message-ID: <%d.$iedm_ran$.$iedm_to$>\r\n", time(NULL));
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "Content-Type: multipart/alternative; boundary=\"----=_NextPart\"\r\n");
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "\r\nThis is a multi-part message in MIME format.\r\n\r\n");
                            nOffset += strlen(sData+nOffset);
                            snprintf(sData+nOffset, nDataSize-nOffset, "------=_NextPart\r\nContent-Type: text/plain;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                            nOffset += strlen(sData+nOffset);
                            //fprintf(pFile2, "X-mailer: iedm version 1.0\r\n");
                            //fprintf(pFile2, "MIME-Version: 1.0\r\n");
                            //fprintf(pFile2, "Message-ID: <%d.$iedm_ran$.$iedm_to$>\r\n", time(NULL));
                            //fprintf(pFile2, "Content-Type: multipart/alternative; boundary=\"----=_NextPart\"\r\n");
                            //fprintf(pFile2, "\r\nThis is a multi-part message in MIME format.\r\n\r\n");
                            //fprintf(pFile2, "------=_NextPart\r\nContent-Type: text/plain;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                            //fprintf(pFile2, "please see the html page.\r\n\r\n");
                            //fprintf(pFile2, "------=_NextPart\r\nContent-Type: text/html;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                        }


                        {
                            char *sss = (char*)malloc(500000);
                            int nOffset2 = 0;
                            int nLeng2 = 0;
                            memset(sss, 0x0, 500000);
                            while(nOffset2 < 400000 && nFinish == 0)
                            {
                                //if(fgets(sss+nOffset, 1024, pFile) == 0)
                                if(GetNextLineFromStr(&sLine, &sLineEnd) == NULL)	
                                    break;
                                snprintf(sss+nOffset2, 400000-nOffset2, "%s\r\n", sLine);
                                //printf("%d:<<%s>>\n", __LINE__, sLine);
                                nLeng2 = strlen(sss+nOffset2);
                                //printf("%s", sss+nOffset);
                                if(strncmp(sss+nOffset2, "</$MODULA>", 10) == 0)
                                {
                                    nFinish = 1;	
                                }
                                if(nFinish == 0)
                                    nOffset2 += nLeng2;
                                else
                                    sss[nOffset2] = 0;	
                            }

                            //V.2.8.1 2014-10-14
                            if (pstrModulaText){
                                snprintf(sData+nOffset, nDataSize-nOffset, "%s",pstrModulaText);
                                nOffset += strlen(sData+nOffset);
                            }
                            else {
                                {
                                    char *sMetaB = sss, *sMetaE;
                                    int nAdd = 1;
                                    sMetaB = strstr(sMetaB, "<meta ");
                                    while(sMetaB)
                                    {
                                        sMetaE = strchr(sMetaB, '>');
                                        if(sMetaE)
                                        {
                                            *sMetaE = 0;
                                            if(strstr(sMetaB, "charset"))
                                            {
                                                nAdd = 0;
                                                *sMetaE = '>';
                                                break;
                                            }
                                            *sMetaE = '>';	
                                        }
                                        if(sMetaE)
                                        {	
                                            sMetaB = strstr(sMetaE, "<meta ");	
                                        }
                                        else
                                            sMetaB = NULL;
                                    }	
                                    //printf("sss:%d:<<%s>>\n", __LINE__, sss);
                                    if(nAdd && *sss)
                                    {
                                        char *sPos = strstr(sss, "<head");
                                        char sAddThing[200];
                                        if(sPos)
                                        {
                                            sPos = strchr(sPos, '>');
                                            if(sPos)
                                            {
                                                sPos++;
                                                snprintf(sAddThing, 200, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />");	
                                                if(sss+nOffset2-sPos > 0)
                                                {
                                                    memmove(sPos+strlen(sAddThing), sPos, sss+nOffset2-sPos);
                                                    memcpy(sPos, sAddThing, strlen(sAddThing));
                                                    nOffset2 += strlen(sAddThing);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            sPos = strstr(sss, "<html>");
                                            if(sPos)
                                            {
                                                sPos+=6;
                                                snprintf(sAddThing, 200, "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /></head>");	
                                                if(sss+nOffset2-sPos > 0)
                                                {
                                                    memmove(sPos+strlen(sAddThing), sPos, sss+nOffset2-sPos);
                                                    memcpy(sPos, sAddThing, strlen(sAddThing));
                                                    nOffset2 += strlen(sAddThing);
                                                }
                                            }	
                                        }	
                                    }
                                    //printf("sssafter:%d:<<%s>>\n", __LINE__, sss);
                                }
                                {
                                    char *sTmpB, *sTmpE;
                                    int nLength2 = 0;
                                    sTmpB = strstr(sss, "<body>");
                                    if(!sTmpB)
                                        sTmpB = sss;
                                    while(sTmpB && *sTmpB)
                                    {
                                        if(*sTmpB == '<')
                                        {
                                            sTmpB = strchr(sTmpB, '>');
                                            if(sTmpB)
                                                sTmpB++;
                                        }
                                        if(sTmpB)
                                        {
                                            sTmpE = strchr(sTmpB, '<');
                                            if(sTmpE)
                                            {
                                                *sTmpE = 0;
                                                //fprintf(pFile2, "%s", sTmpB);
                                                {
                                                    char *s = sTmpB;

                                                    while(s && *s)
                                                    {
                                                        char *s2 = strchr(s, '&');

                                                        if(s2)
                                                            *s2 = 0;
                                                        //printf("<<%s>>\n", s);
                                                        while(*s && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'))
                                                            s++;
                                                        snprintf(sData+nOffset, nDataSize-nOffset, "%s", s);
                                                        nOffset += strlen(sData+nOffset);
                                                        nLength2 += strlen(s);
                                                        if(s2)
                                                        {
                                                            *s2 = '&';
                                                            s = strchr(s2, ';');
                                                            if(s)
                                                                s++;
                                                            else
                                                                s = s2+1;	
                                                        }
                                                        else
                                                            s = NULL;
                                                    }
                                                }
                                                //printf("text:%d<<%s>>\n", __LINE__, sData+nOffset);
                                                //fflush(NULL);

                                                *sTmpE = '<';	
                                                if(strncmp(sTmpE, "<br/>", 5) == 0)
                                                {
                                                    nLength2 = 0;
                                                    snprintf(sData+nOffset, nDataSize-nOffset, "\r\n");
                                                    nOffset += strlen(sData+nOffset);	
                                                }
                                                if(nLength2 > 100)
                                                {
                                                    nLength2 = 0;
                                                    snprintf(sData+nOffset, nDataSize-nOffset, "\r\n");
                                                    nOffset += strlen(sData+nOffset);
                                                    //fprintf(pFile2, "\r\n");
                                                }
                                                sTmpB = sTmpE;
                                            }
                                            else
                                                sTmpB = NULL;	
                                        }
                                    }	
                                }
                            }
                            //fprintf(pFile2, "\r\n\r\n------=_NextPart\r\nContent-Type: text/html;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                            snprintf(sData+nOffset, nDataSize-nOffset, "\r\n\r\n------=_NextPart\r\nContent-Type: text/html;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                            nOffset += strlen(sData+nOffset);
                            //printf("html:%d<<%s>>", __LINE__, sss);
                            {
                                char *s = strstr(sss, "<$CHART>");
                                char *s2;
                                sss[nOffset2] = 0;
                                if(s)
                                {
                                    *s = 0;
                                    snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n%s\r\n", sss, sChart);
                                    nOffset += strlen(sData+nOffset);
                                    //fwrite(sss, strlen(sss), 1, pFile2);
                                    //fprintf(pFile2, "\r\n");
                                    //fwrite(sChart, strlen(sChart), 1, pFile2);
                                    //fprintf(pFile2, "\r\n");
                                    s++;
                                    s2 = strstr(s, "</$CHART>");
                                    if(s2)
                                    {
                                        s2 += 9;
                                        snprintf(sData+nOffset, nDataSize-nOffset, "%s", s2);
                                        nOffset += strlen(sData+nOffset);
                                        //fwrite(s2, strlen(s2), 1, pFile2);
                                    }
                                    else
                                    {
                                        snprintf(sData+nOffset, nDataSize-nOffset, "%s", s2);
                                        nOffset += strlen(sData+nOffset);
                                        //fwrite(s, strlen(s), 1, pFile2);
                                    }			
                                }
                                else
                                {
                                    snprintf(sData+nOffset, nDataSize-nOffset, "%s", sss);
                                    nOffset += strlen(sData+nOffset);
                                    //fwrite(sss, nOffset, 1, pFile2);
                                }
                                //printf("[[[%s]]]\n", sss);
                                fflush(NULL);
                            }
                            free(sss);
                        }	

                        while( nFinish == 0  && GetNextLineFromStr(&sLine, &sLineEnd) )
                        {
                            if(strncmp(sLine, "</$MODULA>", 10) == 0)
                                break;
                            //printf("html2222:%d<<%s>>", __LINE__, sLine);
                            snprintf(sData+nOffset, nDataSize-nOffset, "%s\r\n", sLine);
                            nOffset += strlen(sData+nOffset);	
                            //fprintf(pFile2, "%s", sLine);
                        }
                        //fprintf(pFile2, "\r\n\r\n------=_NextPart--\r\n");

                        //V2.8.1 2014-10-14
                        if (pstrModulaQVGA){
                            snprintf(sData+nOffset, nDataSize-nOffset, 
                                "<!--\r\n<RICHINFO>\r\n<WEB TYPE=\"QVGA\">%s</WEB>\r\n</RICHINFO>\r\n-->", pstrModulaQVGA);
                            nOffset += strlen(sData+nOffset);
                        }

                        snprintf(sData+nOffset, nDataSize-nOffset, "\r\n\r\n------=_NextPart--\r\n");
                        nOffset += strlen(sData+nOffset);
                        //printf("\n\n=====================\n\n");
                        if(nFinish == 0)
                        {
                            snprintf(sData+nOffset, nDataSize-nOffset, "\r\n\r\n%s\r\n", sLine);
                            nOffset += strlen(sData+nOffset);
                            //fprintf(pFile2, "\r\n\r\n%s", sLine);
                        }
                        else
                        {
                            snprintf(sData+nOffset, nDataSize-nOffset, "\r\n\r\n</$MODULA>\r\n");
                            nOffset += strlen(sData+nOffset);
                            //fprintf(pFile2, "\r\n\r\n</$MODULA>\r\n");
                        }
                    }
                    else if(strncmp(sLine, "<$MAILLIST>", 11) == 0)
                    {
                        char *sTmp1, *sTmp2;
                        char sParam[40][40];
                        int nParam = 0;
                        char sLine2[1024];

                        GetNextLineFromStr(&sLine, &sLineEnd);
                        snprintf(sLine2, 1024, "%s", sLine);
                        //snprintf(sLine+strlen(sLine), sizeof(sLine)-strlen(sLine), ",idem_a");
                        if(nParamInSubject || nYRaid || nXRaid)
                        {
                            char *sss1 = strchr(sLine2, ','), *sss2;
                            if(sss1 == NULL)
                            {
                                snprintf(sParam[nParam], 40, "$iedm_null$");
                                nParam++;
                            }
                            while(sss1 && *sss1 && nParam < 36)
                            {
                                sss2 = strchr(sss1, ',');
                                if(sss2)
                                    *sss2 = 0;
                                if(*sss1)
                                {
                                    snprintf(sParam[nParam], 40, "$%s$", sss1);
                                    nParam++;
                                }
                                else
                                {
                                    snprintf(sParam[nParam], 40, "$iedm_null$");
                                    nParam++;	
                                }
                                if(sss2)
                                    *sss2 = ',';
                                if(sss2)
                                    sss1 = sss2+1;
                                else
                                    sss1 = NULL;
                            }	
                            if(nAD == 1)
                            {
                                snprintf(sParam[nParam], 40, "$iedm_ad$");
                                nParam++;	
                            }
                            snprintf(sParam[nParam], 40, "$iedm_ran$");
                            nParam++;
                        }
                        sTmp1 = sLine2+strlen(sLine2);
                        if(nParamInSubject == 0)
                            snprintf(sTmp1, 1024-(sTmp1-sLine2), ",iedm_ran");
                        else if(nAD == 1)
                            snprintf(sTmp1, 1024-(sTmp1-sLine2), ",iedm_ad,iedm_ran,iedm_subject");
                        else
                            snprintf(sTmp1, 1024-(sTmp1-sLine2), ",iedm_ran,iedm_subject");
                        sTmp1 = sTmp1+strlen(sTmp1);
                        {
                            int i = 0;
                            while(i < nYRaid)
                            {
                                snprintf(sTmp1, 1024-(sTmp1-sLine2), ",CH%dYYY", i);	
                                sTmp1 = sTmp1+strlen(sTmp1);
                                ++i;
                            }
                            i = 0;
                            while(i < nXRaid)
                            {
                                snprintf(sTmp1, 1024-(sTmp1-sLine2), ",CH%dXXX", i);
                                sTmp1 = sTmp1+strlen(sTmp1);
                                ++i;	
                            }
                        }
                        snprintf(sTmp1, 1024-(sTmp1-sLine2), "\n");
                        sTmp1++;

                        snprintf(sData+nOffset, nDataSize-nOffset, "%s", sLine2);
                        nOffset += strlen(sData+nOffset);
                        //fprintf(pFile2, "%s", sLine);
                        //while(fgets(sLine, sizeof(sLine)-10, pFile))
                        while(GetNextLineFromStr(&sLine, &sLineEnd))
                        {
                            int nn = sLine[0] * 100 + sLine[1] * 10 + sLine[2];
                            snprintf(sLine2, 1024, "%s", sLine);
                            //nn = nn % 2000;
                            nn = rand() % 10000;
                            if(strncmp(sLine2, "</$MAILLIST>", 12) == 0)
                                break;
                            /*sTmp1 = strchr(sLine, '\r');
                            if(!sTmp1)
                            sTmp1 = strchr(sLine, '\n');
                            if(sTmp1)
                            *sTmp1 = 0;
                            if(!sTmp1)
                            continue;*/
                            //if(nParamInSubject == 0)
                            if(nAD == 1)
                            {
                                char *sTmp11 = strchr(sLine2, '@');
                                if(sTmp11)
                                {
                                    char *sTmp22 = strchr(sTmp11, ',');
                                    char c = 0;
                                    if(sTmp22)
                                    {
                                        sTmp22++;
                                        c = *sTmp22;
                                        *sTmp22 = 0;
                                    }	
                                    else
                                    {
                                        sTmp22 = sTmp11+strlen(sTmp11);
                                        //c = *sTmp22;
                                        *sTmp22 = ',';
                                        *(sTmp22+1) = 0;	
                                    }
                                    sTmp1 = sLine2+strlen(sLine2);
                                    if(strstr(pConf->sAddAdDomain, sTmp11))
                                    {
                                        snprintf(sTmp1, 1024-(sTmp1-sLine2), "(AD),%d\n", nn);	
                                    }
                                    else
                                    {
                                        snprintf(sTmp1, 1024-(sTmp1-sLine2), ",%d\n", nn);
                                    }
                                    if(c)
                                        *sTmp22 = c;
                                }
                            }
                            else
                            {
                                sTmp1 = sLine2+strlen(sLine2);
                                snprintf(sTmp1, 1024-(sTmp1-sLine2), ",%d\n", nn);
                            }
                            if(nParamInSubject)
                            {
                                char s1[1024], s2[2000];
                                char *sReplaceB, *sReplaceE;
                                int i = 0;
                                sTmp1 = strchr(sLine2, '\n');
                                if(!sTmp1)
                                    continue;
                                *sTmp1 = 0;
                                size_t nOutLeng;
                                snprintf(s1, 500, "%s", sSubject);
                                while(i < nParam)
                                {
                                    char *sTmpB, *sTmpE = NULL;
                                    int j = 0;
                                    sTmpB = sLine2;
                                    while(j < i && sTmpB)
                                    {
                                        sTmpB = strchr(sTmpB, ',');
                                        if(sTmpB)
                                            sTmpB++;
                                        j++;
                                    }
                                    if(sTmpB)
                                    {
                                        sTmpE = strchr(sTmpB, ',');
                                        if(sTmpE)
                                            *sTmpE = 0;
                                    }
                                    //printf("SSSS  param[%d](%s) subject:(%s) BBBB\n\n", i, sParam[i], s1);///////////**************
                                    sReplaceB = strstr(s1, sParam[i]);
                                    while(sReplaceB && sTmpB && sTmpE)
                                    {
                                        sReplaceE = sReplaceB + strlen(sParam[i]);
                                        memmove(sReplaceE+strlen(sTmpB)-strlen(sParam[i]), sReplaceE, s1+strlen(s1)-sReplaceE+2);
                                        memcpy(sReplaceB, sTmpB, strlen(sTmpB));
                                        sReplaceE = sReplaceB + strlen(sTmpB);
                                        sReplaceB = strstr(sReplaceE, sParam[i]);	
                                    }
                                    //printf("SSSS  param[%d](%s) subject:(%s) AAAAA\n\n", i, sParam[i], s1);////////////**************
                                    if(sTmpE)
                                        *sTmpE = ',';	
                                    ++i;
                                }
                                if(encode_base64(s1, strlen(s1), s2, 2000, &nOutLeng) >= 0)
                                {
                                    char *ss = strchr(s2, '\r');
                                    if(!ss)
                                        ss = strchr(s2, '\n');
                                    if(ss)
                                        *ss = 0;
                                    snprintf(s1, sizeof(s1), "=?utf-8?B?%s?=", s2);
                                }
                                else
                                    *s1 = 0;
                                snprintf(sTmp1, 10240-(sTmp1-sLine2), ",%s\n", s1);
                            }
                            if(nXRaid && nYRaid)
                            {
                                int nMax = 0;
                                int nVariantValue[20];
                                int i = 0;
                                int j = 0;
                                sTmp1 = strchr(sLine2, '\n');
                                if(!sTmp1)
                                    continue;
                                *sTmp1 = 0;
                                j = 0;
                                while(j < nXRaid)
                                {
                                    char *sVariant = sChartVariant + 40*j;
                                    i = 0;
                                    while(i < nParam)
                                    {
                                        if(strcmp(sParam[i], sVariant) == 0)
                                            break;
                                        ++i;	
                                    }
                                    if(i < nParam)
                                    {
                                        int k = 0;
                                        char *sTmpB = sLine2, *sTmpE = NULL;
                                        while(k < i && sTmpB)
                                        {
                                            sTmpB = strchr(sTmpB, ',');
                                            if(sTmpB)
                                                sTmpB++;
                                            k++;	
                                        }
                                        if(sTmpB)
                                        {
                                            sscanf(sTmpB, "%d", nVariantValue+j);
                                            if(nMax < nVariantValue[j])
                                                nMax = nVariantValue[j];	
                                        }	
                                    }
                                    j++;	
                                }
                                {
                                    int nGap = 100;
                                    int nO = 0;
                                    int nMulti = 1;
                                    while(nO < 9)
                                    {
                                        if(nMax < nMulti*10 && nYRaid > 1)
                                        {
                                            if(nMax >= nMulti && nMax < nMulti*2)
                                                nGap = (2*nMulti)/(nYRaid-1);
                                            else if(nMax >= nMulti*2 && nMax < nMulti*4)
                                                nGap = (4*nMulti)/(nYRaid-1);
                                            else if(nMax >= nMulti*4 && nMax < nMulti*8)
                                                nGap = (8*nMulti)/(nYRaid-1);	
                                            else
                                                nGap = (10*nMulti)/(nYRaid-1);
                                            break;
                                        }
                                        nO++;
                                        nMulti = nMulti*10;	
                                    }	
                                    if(nGap == 0)
                                        nGap = 100;
                                    j = 0;
                                    while(j < nYRaid)
                                    {
                                        snprintf(sTmp1, 10240-(sTmp1-sLine2), ",%d", nGap*(nYRaid-j-1));
                                        sTmp1 = sTmp1+strlen(sTmp1);
                                        ++j;	
                                    }
                                    j = 0;
                                    while(j < nXRaid)
                                    {
                                        snprintf(sTmp1, 10240-(sTmp1-sLine2), ",%d", (36*(nVariantValue[j]))/nGap);
                                        sTmp1 = sTmp1+strlen(sTmp1);
                                        j++;	
                                    }
                                }

                                snprintf(sTmp1, 10240-(sTmp1-sLine2), "\n");
                            }
                            snprintf(sData+nOffset, nDataSize-nOffset, "%s", sLine2);
                            nOffset += strlen(sData+nOffset);
                            //fprintf(pFile2, "%s", sLine);	////////////////////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT	
                        }
                        snprintf(sData+nOffset, nDataSize-nOffset, "%s", sLine2);
                        nOffset += strlen(sData+nOffset);
                        //fprintf(pFile2, "%s", sLine);	
                    }
                }
                free(sChart);

                //V2.8.1 2014-10-14
                if (pstrModulaText){
                    free(pstrModulaText);
                    pstrModulaText = NULL;
                }
                if (pstrModulaQVGA){
                    free(pstrModulaQVGA);
                    pstrModulaQVGA = NULL;
                }
            }
            /*
            {
            FILE *p = fopen("/tmp/aadd.txt", "a");	
            fwrite(sData, nOffset, 1, p);
            fclose(p);
            }
            */
			//ת��mailist
            {	
                {
                    int nBackSocket;
                    int nFail = 0;
                    char s[10];
                    //if(nBackSocket >= 0)
                    while(nFail < 100)
                    {
                        sleep(1);
                        int nPipe = -1;
                        if(sPipeName[0])
                        {
                            int ii = 0;
                            while(ii < pConf->nPipeNum && ii < 50)
                            {
                                if(strcasecmp(sPipeName, (pConf->sPipeName)[ii]) == 0)
                                {	
                                    nPipe = ii;
                                    break;	
                                }
                                ++ii;	
                            }	
                        }

                        if(nPipe < 0 || nPipe >= 50)
                        {
                            nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
                        }
                        else
                        {
							//�ҵ�ǰ��ָ������Чmaillist ip
                            nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);	
                        }
                        if(nBackSocket < 0)
                        {
                            nFail++;
                            snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not connect back for send task  %s (fail:%d).\n", sID, nFail);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                            continue;	
                        }
                        snprintf(s, 10, "POST");
                        memcpy(s+4, &nOffset, 4);
                        SocketWrite(nBackSocket, 5, s, 8);
                        //k =	SocketRead(nBackSocket, 5, s, 4);
                        //if(k == 3 && strncmp(s, "GO", 2) == 0)
                        {
                            int nWrite, nFinish = 0;
                            //FILE *pFile;
                            //pFile = fopen(sTaskFileName, "r");
                            //sText = (char*)malloc(1025);
                            while(nFinish < nOffset)
                            {

                                if(nOffset-nFinish > 1024)
                                    nWrite = 1024;
                                else
                                    nWrite = nOffset-nFinish;
                                //fread(sText, nWrite, 1, pFile);
                                k = SocketWrite(nBackSocket, 5, sData+nFinish, nWrite);
                                if(k != nWrite)
                                {
                                    //printf("eeeeee:%d:%d:%d:\n", k, nWrite, nFinish);
                                    break;
                                    //return 0;
                                }
                                //printf("ssssss:%d:%d:%d:\n", k, nWrite, nFinish);
                                nFinish += k;

                            }
                            //free(sText);
                            //fclose(pFile);
                            //print modula
                            //WriteLog(pConf, pDabudai, sData, nOffset);

                            if(nFinish != nOffset)
                            {
                                nSuccess = 0;	
                                snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]send task %s to back fail, size:%d, send:%d (fail:%d).\n",  sID, nOffset, nFinish, nFail);
                                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                            }
                            else
                            {
                                SocketRead(nBackSocket, 5, s, 4);
                                nSuccess = 100;
                                if(strncmp(s, "OK\n", 3) == 0)
                                {
                                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "send task %s to back success, send:%d, (fail:%d).\n", sID, nFinish, nFail);
                                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                                }
                                else
                                    nSuccess = 0;
                            }
                        }
                        /*
                        else if(k == 3 && strncmp(s, "HH", 2) == 0)
                        {
                        nSuccess = 1;
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "this task %s has add before (fail:%d).\n", sTaskFileName, nFail);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                        }
                        else
                        {
                        nSuccess = 0;
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not send task file name %s (fail:%d).\n", sTaskFileName, nFail);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                        }
                        */
                        close(nBackSocket);
                        if(nSuccess)
                        {
                            nFail = 1000;
                        }
                        else
                        {
                            nFail++;	
                        }
                    }
                    /*
                    else 
                    {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not connect back for send task file name %s.\n", sTaskFileName);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                    }
                    */
                }
            }




            free(sData);	
        }//msg parse end 

        free(sMsg);
        return 0;
    }//post_handler

	//get ����
    k = 0;
    {
        char *sTmp;
        char sFile[200];

        sTmp = strstr(sMsg, "GET");
        if(sTmp == NULL)
        {
            k = 1;
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find GET!\n");
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }

        if(k == 0)
        {
            char *sTmp2 = strchr(sTmp, '\n');
            if(sTmp2)
            {
                *sTmp2 = 0;
                if(*(sTmp2-1) == '\r')
                    *(sTmp2-1) = 0;
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "get requeset as:<<%s>>.\n", sTmp);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                sTmp2 = strchr(sTmp, '?');
                if(sTmp2)
                {
                    char *sTmpE, *sTmpB;
                    sTmpE = strchr(sTmp2, ' ');
                    if(sTmpE)
                        *sTmpE = 0;
                    sTmpE = strchr(sTmp2, '\t');
                    if(sTmpE)
                        *sTmpE = 0;

                    if((sTmpB = strstr(sTmp2, "filePath=")))
                    {
                        sTmpE = strchr(sTmpB, '&');
                        if(sTmpE)
                            *sTmpE = 0;
                        sTmpB += 9;
                        if(*sTmpB == '/')
                            snprintf(sFileName, sizeof(sFileName), "%s%s", pConf->sWebBaseDir, sTmpB);
                        else
                            snprintf(sFileName, sizeof(sFileName), "%s/%s", pConf->sWebBaseDir, sTmpB);
                        if(access(sFileName, F_OK) == 0)
                        {
                            nSuccess = 1;
                            snprintf(sLog+nLogOffset, 10240-nLogOffset, "success get the file name:%s.\n", sFileName);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                        }
                        else
                        {
                            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not access the file:%s.\n", sFileName);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                        }
                        if(sTmpE)
                            *sTmpE = '&';	
                    }
                    else if((sTmpB = strstr(sTmp2, "pause=")))
                    {
                        sTmpB += 6;
                        sTmpE = strchr(sTmpB, '&');
                        if(sTmpE)
                            *sTmpE = 0;
                        sTmpE = strchr(sTmpB, ' ');
                        if(sTmpE)
                            *sTmpE = 0;
                        snprintf(sID, sizeof(sID), "%s", sTmpB);
                        nSuccess = 2;
                    }
                    else if((sTmpB = strstr(sTmp2, "restart=")))
                    {
                        sTmpB += 8;
                        sTmpE = strchr(sTmpB, '&');
                        if(sTmpE)
                            *sTmpE = 0;
                        sTmpE = strchr(sTmpB, ' ');
                        if(sTmpE)
                            *sTmpE = 0;
                        snprintf(sID, sizeof(sID), "%s", sTmpB);
                        nSuccess = 3;	
                    }
                    else if((sTmpB = strstr(sTmp2, "stop=")))
                    {
                        sTmpB += 5;	
                        sTmpE = strchr(sTmpB, '&');
                        if(sTmpE)
                            *sTmpE = 0;
                        sTmpE = strchr(sTmpB, ' ');
                        if(sTmpE)
                            *sTmpE = 0;
                        snprintf(sID, sizeof(sID), "%s", sTmpB);
                        nSuccess = 4;	
                    }
                    else if((sTmpB = strstr(sTmp2, "stat=")))
                    {
                        sTmpB += 5;	
                        sTmpE = strchr(sTmpB, '&');
                        if(sTmpE)
                            *sTmpE = 0;
                        sTmpE = strchr(sTmpB, ' ');
                        if(sTmpE)
                            *sTmpE = 0;
                        snprintf(sID, sizeof(sID), "%s", sTmpB);
                        nSuccess = 5;	
                    }
                    else
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find command correct.\n");
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));	
                    }
                }	
                else
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find \'?\'.\n");
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                }
            }
            else
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find \\n.\n");
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
            }	
        }	
    }
	//״̬��ѯ
    if(nSuccess == 5)
    {
        struct stat sctFile;
        char s[108];
        int nBackSocket;	
        int nPipe = 0;
        char sStat[4];

        snprintf(sStat, 4, "500");

        nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
        if(nBackSocket > 0)
        {
            snprintf(s, sizeof(s), "STAT %s\r\n", sID);
            SocketWrite(nBackSocket, 5, s, strlen(s));
            k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
            if(k > 0 && *s == '2')
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "success get stat for task:%s(pipe:default), stat is:%s.\n", sID, s);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                snprintf(sStat, 4, "%s", s);
            }
            else
            {
                if(k > 0)
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, return is:%s(pipe:default).\n", sID, s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                }
                else
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, can not get return(pipe:default).\n", sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                }
                nSuccess = 0;
            }
            close(nBackSocket);
        }
        else
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, can not connect %s:%d(pipe:default).\n", 
                sID, pConf->sBackIp, pConf->nBackPort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            nSuccess = 0;
        }

        nPipe = 0;
        while(nPipe < pConf->nPipeNum && nSuccess == 0)
        {
            //nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
            nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);
            if(nBackSocket > 0)
            {
                snprintf(s, sizeof(s), "STAT %s\r\n", sID);
                SocketWrite(nBackSocket, 5, s, strlen(s));
                k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
                if(k > 0 && *s == '2')
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "success stat task:%s(pipe:%s), stat is:%s.\n", sID, 
                        (pConf->sPipeName)[nPipe], s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    nSuccess = 5;
                    snprintf(sStat, 4, "%s", s);
                }
                else
                {
                    if(k > 0)
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, return is:%s(pipe:%s).\n", 
                            sID, s, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    }
                    else
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, can not get return(pipe:%s).\n", 
                            sID, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    }
                    nSuccess = 0;
                }
                close(nBackSocket);
            }
            else
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stat task:%s, can not connect %s:%d(pipe:%s).\n", 
                    sID, (pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], (pConf->sPipeName)[nPipe]);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                nSuccess = 0;
            }

            nPipe++;
        }
        snprintf(sMsg, 100, "HTTP/1.1 %s\r\n\r\n", sStat);
    }
    else if(nSuccess)
        snprintf(sMsg, 100, "HTTP/1.1 200\r\n\r\n");
    else
        snprintf(sMsg, 100, "HTTP/1.1 500\r\n\r\n");
    SocketWrite(nSocket, 3, sMsg, strlen(sMsg));

    close(nSocket);

	//�ʼ�
    if(nSuccess == 1)
    {
        FILE *pFile = fopen(sFileName, "r");
        FILE *pFile2;
        char sLine[10240];
        char *sTmp;
        char sFileTmp[200];
        char sSubject[1024];
        int nParamInSubject = 0;
        char sNickName[100];
        char sSender[512];
        char sTime[100];
        char sTime2[100];
        char sReplyTo[100];
        char sMailFrom[1024];
        int using_duo = 0;
        char cFlag = 0;
        int nSameFromFlag = 0;
        int nIsRandomFlag = 0;
        char sPipeName[50];
        int nAD = 0;
        //int nNotifyType = -1;
        char sNotifiType[8] = {0};
        sNotifiType[0] = '0';
        char sHelo[50];
        char sChartVariant[800];
        char *sChart = (char*)malloc(20480);
        int nYRaid = 0, nXRaid = 0;
        int nAtTime1 = 0;
        int nAtTime2 = 0;
        int istestmail = -1;

        //V2.8.1 2014-10-14
        char *pstrModulaText = NULL;
        char *pstrModulaQVGA = NULL;
        int nModulaLen = 10 * 1024 * 1024; //10M

        sHelo[0] = '0';
        sHelo[1] = 0;
        sSubject[0] = 0;
        sNickName[0] = 0;
        sSender[0] = 0;
        sReplyTo[0] = 0;
        sMailFrom[0] = 0;
        sTime[0] = 0;
        sTime2[0] = 0;
        sPipeName[0] = 0;
        {
            time_t tTime;
            struct tm tmTime;
            int nZone = 8;
            char sZone[20];
            time(&tTime);
            localtime_r(&tTime, &tmTime);
            if(nZone > 0)
                snprintf(sZone, sizeof(sZone), "+%02d00", nZone);
            else
                snprintf(sZone, sizeof(sZone), "-%02d00", -nZone);
            snprintf(sTime, sizeof(sTime), "%s, %02d %s %d %02d:%02d:%02d %s (CST)",
                sWeekName[tmTime.tm_wday], tmTime.tm_mday, sMonthName[tmTime.tm_mon], tmTime.tm_year+1900, 
                tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, sZone);	
            tTime -= 100;
            localtime_r(&tTime, &tmTime);
            snprintf(sTime2, sizeof(sTime2), "%s, %02d %s %d %02d:%02d:%02d %s",
                sWeekName[tmTime.tm_wday], tmTime.tm_mday, sMonthName[tmTime.tm_mon], tmTime.tm_year+1900, 
                tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, sZone);	
        }

        {
            unsigned char *s = (unsigned char*)(&nIp);
            snprintf(sFileTmp, sizeof(sFileTmp), "%s/%d.%d.%d.%d.%04d.%d", pConf->sTaskFileDir, s[0], s[1], s[2], s[3], n, time(NULL));
        }
        pFile2 = fopen(sFileTmp, "w");

        if (!pFile2)
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR] fopen fail:%s\n",sFileTmp);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            return -1;
        }

        sID[0] = 0;
        while(fgets(sLine, sizeof(sLine), pFile))
        {
            char sBak[1024];

            fprintf(pFile2, "%s", sLine);
            //printf("<<%s>>:%d\n\n", sLine, nAD);
            if(strncmp(sLine, "<$ID>", 5) == 0)
            {	
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                sTmp = strchr(sLine, '\r');
                if(!sTmp)
                    sTmp = strchr(sLine, '\n');
                if(sTmp)
                    *sTmp = 0;
                snprintf(sID, sizeof(sID), "%s", sLine);
            }
            else if(strncasecmp(sLine, "<$TIME>", 7) == 0)
            {
                int nYear, nMonth, nDay, nHour, nMinute, nSecond;
                fgets(sLine, sizeof(sLine), pFile);	
                sscanf(sLine, "%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
                nAtTime1 = nYear*10000 + nMonth*100 + nDay;
                nAtTime2 = nHour*10000 + nMinute*100 + nSecond;
            }
            else if(strncasecmp(sLine, "<$AD>", 5) == 0)
            {
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                if(strstr(sLine, "true"))
                    nAD = 1;	
            }

            else if(strncasecmp(sLine, "<$TEST>", 7) == 0)
            {
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                if(strstr(sLine, "true"))
                    istestmail = 1;	
            }
            else if(strncasecmp(sLine, "<$CHART>", 8) == 0)
            {
                int nOffset = 0;
                int nSize = 20480;
                //fprintf(pFile2, "%s", sLine); //ט¸´дһѐ 
                while(fgets(sLine, 1024, pFile))
                {
                    fprintf(pFile2, "%s", sLine);
                    if(strncasecmp(sLine, "</$CHART>", 9) == 0)
                        break;	
                    snprintf(sChart+nOffset, nSize-nOffset, "%s", sLine);
                    nOffset += strlen(sLine);
                    if(nSize - nOffset < 1024)
                    {
                        nSize += 10240;
                        sChart = (char*)realloc(sChart, nSize);	
                    }
                }
                TreatChar(sChart, sChartVariant, &nYRaid, &nXRaid);	
            }
            else if(strncasecmp(sLine, "<$HELO>", 7) == 0)
            {
                char *sTmp;
                fgets(sHelo, 50, pFile);
                if(*sHelo == '<')
                {
                    sHelo[0] = '0';
                    sHelo[1] = 0;	
                }	
                sTmp = strchr(sHelo, '\r');
                if(!sTmp)
                    sTmp = strchr(sHelo, '\n');
                if(sTmp)
                    *sTmp = 0;
                if(*sHelo == 0)
                {
                    sHelo[0] = '0';
                    sHelo[1] = 0;		
                }
            }
            else if(strncasecmp(sLine, "<$ISRADOM>", 10) == 0)
            {
                char *sTmp;
                sTmp = strstr(sLine, "true");	
                if(sTmp)
                {
                    nIsRandomFlag = 1;	
                }
            }
            else if(strncmp(sLine, "<$PIPE>", 7) == 0)
            {
                char *sTmp;
                fgets(sPipeName, sizeof(sPipeName), pFile);
                fprintf(pFile2, "%s", sPipeName);
                sTmp = strchr(sPipeName, '\r');
                if(!sTmp)
                    sTmp = strchr(sPipeName, '\n');
                if(sTmp)
                    *sTmp = 0;	
            }
            else if(strncmp(sLine, "<$SUBJECT>", 10) == 0)
            {
                char *sTmp;
                size_t nOutLeng;
                fgets(sSubject, sizeof(sSubject)-10, pFile);
                //printf("SSSS    (%s)\n\n", sSubject);/////////////////********************
                if(nAD == 1)
                {
                    memmove(sSubject+9, sSubject, strlen(sSubject)+1);
                    memcpy(sSubject, "$iedm_ad$", 9);
                    //$iedm_ad$	
                }
                //printf("SSSS    (%s)\n\n", sSubject);//////////////********************
                sTmp = strchr(sSubject, '\r');
                if(!sTmp)
                    sTmp = strchr(sSubject, '\n');
                if(sTmp)
                    *sTmp = 0;
                sTmp = strchr(sSubject, '$');
                if(sTmp)
                {
                    sTmp = strchr(sTmp+1, '$');
                    if(sTmp)
                        nParamInSubject = 1;	
                }
                if(nParamInSubject == 0)
                {
                    if(encode_base64(sSubject, strlen(sSubject), sBak, 1020, &nOutLeng) >= 0)
                    { 
                        char *ss = strchr(sBak, '\r');
                        if(!ss)
                            ss = strchr(sBak, '\n');
                        if(ss)
                            *ss = 0;
                        snprintf(sSubject, sizeof(sSubject), "=?utf-8?B?%s?=\n", sBak);
                    }
                    else
                    {
                        *sTmp = '\n';
                        *(sTmp+1) = 0;
                    }
                }
                fprintf(pFile2, "%s\r\n", sSubject);
                sTmp = strchr(sSubject, '\r');
                if(!sTmp)
                    sTmp = strchr(sSubject, '\n');
                if(sTmp)
                    *sTmp = 0;
            }
            else if(strncmp(sLine, "<$SENDER>", 9) == 0)
            {
                char *sTmp;
                fgets(sSender, sizeof(sSender), pFile);
                fprintf(pFile2, "%s", sSender);
                sTmp = strchr(sSender, '\r');
                if(!sTmp)
                    sTmp = strchr(sSender, '\n');
                if(sTmp)
                    *sTmp = 0;	
            }
            else if(strncmp(sLine, "<$NICKNAME>", 11) == 0)
            {
                char *sTmp;
                size_t nOutLeng;
                fgets(sNickName, sizeof(sNickName)-1, pFile);
                //fprintf(pFile2, "%s", sNickName);
                sTmp = strchr(sNickName, '\r');
                if(!sTmp)
                    sTmp = strchr(sNickName, '\n');
                if(sTmp)
                    *sTmp = 0;	
                if(encode_base64(sNickName, strlen(sNickName), sBak, 1020, &nOutLeng) >= 0)
                { 
                    char *ss = strchr(sBak, '\r');
                    if(!ss)
                        ss = strchr(sBak, '\n');
                    if(ss)
                        *ss = 0;
                    snprintf(sNickName, sizeof(sNickName), "=?utf-8?B?%s?=\n", sBak);
                }
                else
                {
                    *sTmp = '\n';
                    *(sTmp+1) = 0;
                }
                fprintf(pFile2, "%s", sNickName);
                sTmp = strchr(sNickName, '\r');
                if(!sTmp)
                    sTmp = strchr(sNickName, '\n');
                if(sTmp)
                    *sTmp = 0;
            }
            else if(strncmp(sLine, "<$REPLIER>", 10) == 0)
            {
                char *sTmp;
                fgets(sReplyTo, sizeof(sReplyTo), pFile);
                fprintf(pFile2, "%s", sReplyTo);
                sTmp = strchr(sReplyTo, '\r');
                if(!sTmp)
                    sTmp = strchr(sReplyTo, '\n');
                if(sTmp)
                    *sTmp = 0;	
            }	
            else if(strncmp(sLine, "<$ROBOT>", 8) == 0)
            {
                char *sTmp;
                fgets(sMailFrom, sizeof(sMailFrom), pFile);
                fprintf(pFile2, "%s", sMailFrom);
                sTmp = strchr(sMailFrom, '\r');
                if(!sTmp)
                    sTmp = strchr(sMailFrom, '\n');
                if(sTmp)
                    *sTmp = 0;	

	            if (strchr(sMailFrom, ';')) {
	                using_duo = 1;
	            }
            }	
            else if(strncmp(sLine, "<$RAND>", 7) == 0)
            {
                char *sTmp;
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                if(strstr(sLine, "true") || strstr(sLine, "TRUE"))
                    cFlag = 1;
                else
                    cFlag = 0;	
            }
            else if(strncmp(sLine, "<$SAMEFROM>", 11) == 0)
            {
                char *sTmp;
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                if(strstr(sLine, "true") || strstr(sLine, "TRUE"))
                    nSameFromFlag = 1;
                else
                    nSameFromFlag = 0;	
            }

            /*  SMS 2012/12/18 */
            else if(strncasecmp(sLine, "<$SMS>", 6) == 0)
            {
                fgets(sLine, sizeof(sLine), pFile);
                fprintf(pFile2, "%s", sLine);
                snprintf(sNotifiType, sizeof(sNotifiType), "%s", sLine);

                int i;
                for (i=0; i<sizeof(sNotifiType); i++){
                    if (sNotifiType[i] == '\r' || sNotifiType[i] == '\n')
                        sNotifiType[i] = '\0';
                }
            }

            //V2.8.1 2014-10-14
            else if(strncasecmp(sLine, "<$MODULA_TEXT>", strlen("<$MODULA_TEXT>")) == 0)
            {
                if (pstrModulaText){
                    free(pstrModulaText);
                    pstrModulaText = NULL;
                }

                int offset = 0;
                pstrModulaText = (char*)malloc(nModulaLen); //10M
                memset(pstrModulaText, 0x00,nModulaLen);

                while (fgets(sLine, sizeof(sLine), pFile)){
                    fprintf(pFile2, "%s", sLine);
                    if(strncasecmp(sLine, "</$MODULA_TEXT>", strlen("</$MODULA_TEXT>")) == 0)
                        break;
                    if (offset < nModulaLen)
                        offset += snprintf(pstrModulaText+offset, nModulaLen-offset, "%s",sLine);
                }
            }

            //V2.8.1 2014-10-14
            else if(strncasecmp(sLine, "<$MODULA_QVGA>", strlen("<$MODULA_QVGA>")) == 0)
            {
                if (pstrModulaQVGA){
                    free(pstrModulaQVGA);
                    pstrModulaQVGA = NULL;
                }

                int offset = 0;
                pstrModulaQVGA = (char*)malloc(nModulaLen); //10M
                memset(pstrModulaQVGA, 0x00,nModulaLen);

                while (fgets(sLine, sizeof(sLine), pFile)){
                    fprintf(pFile2, "%s", sLine);
                    if(strncasecmp(sLine, "</$MODULA_QVGA>", strlen("</$MODULA_QVGA>")) == 0)
                        break;
                    if (offset < nModulaLen)
                        offset += snprintf(pstrModulaQVGA+offset, nModulaLen-offset, "%s",sLine);
                }
            }
            //V2.9.2 wEide 2015-07-16
            else if(strncmp(sLine, "<$MODULA_EML>", 13) == 0)
            {
                fprintf(pFile2, "\r\n</$MODULA_EML>\r\n");
                fprintf(pFile2, "<$MODULA>\r\n");
                fprintf(pFile2, "$MODULA_TYPE_EML$\r\n");
                fprintf(pFile2, "$MAIL_FROM$%s\r\n", sMailFrom);
                fprintf(pFile2, "$FLAG$%d\r\n",cFlag?1:0);
                fprintf(pFile2, "$SAMEFROM$%d\r\n", nSameFromFlag?1:0);	
                fprintf(pFile2, "$FROM$%s\r\n", sSender);
                fprintf(pFile2, "$HELO$%s\r\n", sHelo);

                while( fgets(sLine, sizeof(sLine), pFile) ){
                    if(strncmp(sLine, "</$MODULA_EML>", 14) == 0)
                        break;
                    fprintf(pFile2, "%s", sLine);
                }

                fprintf(pFile2, "\r\n</$MODULA>\r\n");
            }

            else if(strncmp(sLine, "<$MODULA>", 9) == 0)
            {
                int nFinish = 0;
                char sSender2[512];
                memset(sSender2, 0x00, sizeof(sSender2));
                if(nSameFromFlag)
                {
                    char *sTmp = strchr(sSender, '@');
                    if(sTmp)
                    {
                        *sTmp = 0;
                        if(nIsRandomFlag == 1) //<$ISRADOM> for API
                            snprintf(sSender2, sizeof(sSender2), "%s$iedm_ran$@%s", sSender, sTmp+1);
                        else
                            snprintf(sSender2, sizeof(sSender2), "%s@%s", sSender, sTmp+1);
                        *sTmp = '@';
                    }
                    else
                    {
                        sSender2[0] = 0;	
                    }
                }
                {	
                    fprintf(pFile2, "$SMS$%s\r\n", sNotifiType);
                    if(sMailFrom[0])
                        fprintf(pFile2, "$MAIL_FROM$%s\r\n", sMailFrom);
                    else
                        fprintf(pFile2, "$MAIL_FROM$\r\n", sMailFrom);

                    if(cFlag) //<$RAND>
                        fprintf(pFile2, "$FLAG$1\r\n");
                    else
                        fprintf(pFile2, "$FLAG$0\r\n");

                    if(nSameFromFlag)  //<$SAMEFROM>
                    {
                        fprintf(pFile2, "$SAMEFROM$1\r\n");	
                    }
                    else
                    {
                        fprintf(pFile2, "$SAMEFROM$0\r\n");
                    }
                    
                    if(sSender2[0])
                        fprintf(pFile2, "$FROM$%s\r\n", sSender2);
                    else
                        fprintf(pFile2, "$FROM$%s\r\n", sSender);    
              
                    fprintf(pFile2, "$HELO$%s\r\n", sHelo);
                    
                    fprintf(pFile2, "Received: from localhost ([127.0.0.1])\r\n\tby iedm for <$iedm_to$>; %s\r\n", sTime);

                    if(sNickName[0]){
                        fprintf(pFile2, "From: \"%s\" <$iedm_from_01$>\r\n", sNickName);
                    }
                    else
                    {
                        fprintf(pFile2, "From: $iedm_from_01$\r\n");
                    }
                    

                    fprintf(pFile2, "To: $iedm_to$\r\n");
                    if(nParamInSubject == 0)
                        fprintf(pFile2, "Subject: %s\r\n", sSubject);
                    else
                        fprintf(pFile2, "Subject: $iedm_subject$\r\n");
                    fprintf(pFile2, "Date: %s\r\n", sTime2);

                    if(sReplyTo[0])
                        fprintf(pFile2, "Reply-To: <%s>\r\n", sReplyTo);
                    else
                        fprintf(pFile2, "Reply-To: <%s>\r\n", pConf->sReplyTo);

                    //if (strlen(sNotifiType) > 0) 
                    {
                        fprintf(pFile2, "X-RICHINFO:NOTIFYTYPE:%s;ISMULTITEMPLATE:%d;TEMPLATELIST:4\r\n",strcmp(sNotifiType,"99")==0?"0":sNotifiType,pstrModulaQVGA?1:0);
                    }

                    if (istestmail != -1)
                    {
                        fprintf(pFile2, "ISTESTMAIL:%d\r\n",istestmail);
                    }

                    fprintf(pFile2, "X-mailer: iedm version 1.0\r\n");
                    fprintf(pFile2, "MIME-Version: 1.0\r\n");
                    fprintf(pFile2, "Message-ID: <%d.$iedm_ran$.$iedm_to$>\r\n", time(NULL));
                    fprintf(pFile2, "Content-Type: multipart/alternative; boundary=\"----=_NextPart\"\r\n");
                    fprintf(pFile2, "\r\nThis is a multi-part message in MIME format.\r\n\r\n");
                    fprintf(pFile2, "------=_NextPart\r\nContent-Type: text/plain;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                    //fprintf(pFile2, "please see the html page.\r\n\r\n");
                    //fprintf(pFile2, "------=_NextPart\r\nContent-Type: text/html;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                }

                {
                    char *sss = (char*)malloc(50000);
                    int nOffset = 0;
                    int nLeng = 0;
                    memset(sss, 0x0, 50000);

                    while(nOffset < 40000 && nFinish == 0)
                    {
                        if(fgets(sss+nOffset, 1024, pFile) == 0)
                            break;

                        nLeng = strlen(sss+nOffset);

                        if(strncmp(sss+nOffset, "</$MODULA>", 10) == 0)
                            nFinish = 1;	

                        if(nFinish == 0)
                            nOffset += nLeng;
                        else
                            sss[nOffset] = 0;	
                    }

                    //V2.8.1 2014-10-14
                    if (pstrModulaText){
                        fprintf(pFile2,"%s",pstrModulaText);
                    }
                    else{
                        { //duan luo
                            char *sMetaB = sss, *sMetaE;
                            int nAdd = 1;
                            sMetaB = strstr(sMetaB, "<meta ");
                            while(sMetaB){
                                sMetaE = strchr(sMetaB, '>');
                                if(sMetaE){
                                    *sMetaE = 0;
                                    if(strstr(sMetaB, "charset")){
                                        nAdd = 0;
                                        *sMetaE = '>';
                                        break;
                                    }
                                    *sMetaE = '>';	
                                }
                                if(sMetaE)
                                    sMetaB = strstr(sMetaE, "<meta ");	
                                else
                                    sMetaB = NULL;
                            }

                            if(nAdd && *sss)
                            {
                                char *sPos = strstr(sss, "<head");
                                char sAddThing[200];
                                if(sPos)
                                {
                                    sPos = strchr(sPos, '>');
                                    if(sPos)
                                    {
                                        sPos++;
                                        snprintf(sAddThing, 200, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />");	
                                        if(sss+nOffset-sPos > 0)
                                        {
                                            memmove(sPos+strlen(sAddThing), sPos, sss+nOffset-sPos);
                                            memcpy(sPos, sAddThing, strlen(sAddThing));
                                            nOffset += strlen(sAddThing);
                                        }
                                    }
                                }
                                else
                                {
                                    sPos = strstr(sss, "<html>");
                                    if(sPos)
                                    {
                                        sPos+=6;
                                        snprintf(sAddThing, 200, "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /></head>");	
                                        if(sss+nOffset-sPos > 0)
                                        {
                                            memmove(sPos+strlen(sAddThing), sPos, sss+nOffset-sPos);
                                            memcpy(sPos, sAddThing, strlen(sAddThing));
                                            nOffset += strlen(sAddThing);
                                        }
                                    }	
                                }	
                            }
                        }
                        {
                            char *sTmpB, *sTmpE;
                            int nLength = 0;
                            sTmpB = strstr(sss, "<body>");
                            if(!sTmpB)
                                sTmpB = sss;
                            while(sTmpB && *sTmpB)
                            {
                                if(*sTmpB == '<')
                                {
                                    sTmpB = strchr(sTmpB, '>');
                                    if(sTmpB)
                                        sTmpB++;
                                }
                                if(sTmpB)
                                {
                                    sTmpE = strchr(sTmpB, '<');
                                    if(sTmpE)
                                    {
                                        *sTmpE = 0;
                                        //printf("<<%s>>\n", sTmpB);
                                        if(*sTmpB)
                                        {
                                            char *s = sTmpB;

                                            while(s && *s)
                                            {
                                                char *s2 = strchr(s, '&');
                                                if(s2)
                                                    *s2 = 0;
                                                while(*s && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'))
                                                    s++;
                                                fprintf(pFile2, "%s", s);
                                                nLength += strlen(s);
                                                if(s2)
                                                {
                                                    *s2 = '&';
                                                    s = strchr(s2, ';');
                                                    if(s)
                                                        s++;
                                                    else
                                                        s = s2+1;	
                                                }
                                                else
                                                    s = NULL;
                                            }
                                        }
                                        //fprintf(pFile2, "%s", sTmpB);
                                        //nLength += strlen(sTmpB);
                                        *sTmpE = '<';
                                        if(strncmp(sTmpE, "<br/>", 5) == 0)
                                        {
                                            nLength = 0;
                                            fprintf(pFile2, "\r\n");	
                                        }
                                        if(nLength > 100)
                                        {
                                            nLength = 0;
                                            fprintf(pFile2, "\r\n");
                                        }	
                                        sTmpB = sTmpE;
                                    }
                                    else
                                        sTmpB = NULL;	
                                }
                            }	
                        }
                    }
                    fprintf(pFile2, "\r\n\r\n------=_NextPart\r\nContent-Type: text/html;\r\n\tcharset=\"utf-8\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");
                    {
                        char *s = strstr(sss, "<$CHART>");
                        char *s2;
                        sss[nOffset] = 0;
                        if(s)
                        {
                            *s = 0;
                            fwrite(sss, strlen(sss), 1, pFile2);
                            fprintf(pFile2, "\r\n");
                            fwrite(sChart, strlen(sChart), 1, pFile2);
                            fprintf(pFile2, "\r\n");
                            s++;
                            s2 = strstr(s, "</$CHART>");
                            if(s2)
                            {
                                s2 += 9;
                                fwrite(s2, strlen(s2), 1, pFile2);
                            }
                            else
                                fwrite(s, strlen(s), 1, pFile2);

                        }
                        else
                            fwrite(sss, nOffset, 1, pFile2);
                    }
                    free(sss);
                }

                while( nFinish == 0  && fgets(sLine, sizeof(sLine), pFile) )
                {
                    if(strncmp(sLine, "</$MODULA>", 10) == 0)
                        break;
                    fprintf(pFile2, "%s", sLine);
                }

                //V2.8.1 2014-10-14
                if (pstrModulaQVGA){
                    fprintf(pFile2, 
                        "<!--\r\n"
                        "<RICHINFO>\r\n"
                        "<WEB TYPE=\"QVGA\">%s</WEB>\r\n"
                        "</RICHINFO>\r\n"
                        "-->", 
                        pstrModulaQVGA);
                }

                fprintf(pFile2, "\r\n\r\n------=_NextPart--\r\n");
                if(nFinish == 0)
                    fprintf(pFile2, "\r\n\r\n%s", sLine);
                else
                    fprintf(pFile2, "\r\n\r\n</$MODULA>\r\n");
            }
            else if(strncmp(sLine, "<$MAILLIST>", 11) == 0)
            {
                char *sTmp1, *sTmp2;
                char sParam[40][40];
                int nParam = 0;

                fgets(sLine, sizeof(sLine), pFile);
                sTmp1 = strchr(sLine, '\r');
                if(!sTmp1)
                    sTmp1 = strchr(sLine, '\n');
                if(sTmp1)
                    *sTmp1 = 0;
                if(!sTmp1)
                    continue;
                //snprintf(sLine+strlen(sLine), sizeof(sLine)-strlen(sLine), ",idem_a");
                if(nParamInSubject || nYRaid || nXRaid)
                {
                    char *sss1 = strchr(sLine, ','), *sss2;
                    if(sss1 == NULL)
                    {
                        snprintf(sParam[nParam], 40, "$iedm_null$");
                        nParam++;
                    }
                    while(sss1 && *sss1 && nParam < 36)
                    {
                        sss2 = strchr(sss1, ',');
                        if(sss2)
                            *sss2 = 0;
                        if(*sss1)
                        {
                            snprintf(sParam[nParam], 40, "$%s$", sss1);
                            nParam++;
                        }
                        else
                        {
                            snprintf(sParam[nParam], 40, "$iedm_null$");
                            nParam++;	
                        }
                        if(sss2)
                            *sss2 = ',';
                        if(sss2)
                            sss1 = sss2+1;
                        else
                            sss1 = NULL;
                    }	
                    if(nAD == 1)
                    {
                        snprintf(sParam[nParam], 40, "$iedm_ad$");
                        nParam++;	
                    }
                    snprintf(sParam[nParam], 40, "$iedm_ran$");
                    nParam++;
                }
                if(nParamInSubject == 0)
                    snprintf(sTmp1, 1024-(sTmp1-sLine), ",iedm_ran");
                else if(nAD == 1)
                    snprintf(sTmp1, 1024-(sTmp1-sLine), ",iedm_ad,iedm_ran,iedm_subject");
                else
                    snprintf(sTmp1, 1024-(sTmp1-sLine), ",iedm_ran,iedm_subject");
                sTmp1 = sTmp1+strlen(sTmp1);
                {
                    int i = 0;
                    while(i < nYRaid)
                    {
                        snprintf(sTmp1, 1024-(sTmp1-sLine), ",CH%dYYY", i);	
                        sTmp1 = sTmp1+strlen(sTmp1);
                        ++i;
                    }
                    i = 0;
                    while(i < nXRaid)
                    {
                        snprintf(sTmp1, 1024-(sTmp1-sLine), ",CH%dXXX", i);
                        sTmp1 = sTmp1+strlen(sTmp1);
                        ++i;	
                    }
                }
                snprintf(sTmp1, 1024-(sTmp1-sLine), "\n");
                sTmp1++;


                fprintf(pFile2, "%s", sLine);
                while(fgets(sLine, sizeof(sLine)-10, pFile))
                {
                    //int nn = sLine[0] * 100 + sLine[1] * 10 + sLine[2];
                    //nn = nn % 2000;
                    int nn = rand()%10000;
                    if(strncmp(sLine, "</$MAILLIST>", 12) == 0)
                        break;
                    sTmp1 = strchr(sLine, '\r');
                    if(!sTmp1)
                        sTmp1 = strchr(sLine, '\n');
                    if(sTmp1)
                        *sTmp1 = 0;
                    if(!sTmp1)
                        continue;
                    //if(nParamInSubject == 0)
                    if(nAD == 1)
                    {
                        char *sTmp11 = strchr(sLine, '@');
                        if(sTmp11)
                        {
                            char *sTmp22 = strchr(sTmp11, ',');
                            char c = 0;
                            if(sTmp22)
                            {
                                sTmp22++;
                                c = *sTmp22;
                                *sTmp22 = 0;
                            }	
                            else
                            {
                                sTmp22 = sTmp11+strlen(sTmp11);
                                //c = *sTmp22;
                                *sTmp22 = ',';
                                *(sTmp22+1) = 0;	
                            }
                            if(strstr(pConf->sAddAdDomain, sTmp11))
                            {
                                snprintf(sTmp1, 1024-(sTmp1-sLine), ",(AD),%04d\n", nn);	
                            }
                            else
                            {
                                snprintf(sTmp1, 1024-(sTmp1-sLine), ",,%04d\n", nn);
                            }
                            if(c)
                                *sTmp22 = c;
                        }
                    }
                    else
                    {
                        snprintf(sTmp1, 1024-(sTmp1-sLine), ",%04d\n", nn);
                    }
                    if(nParamInSubject)
                    {
                        char s1[1024], s2[2000];
                        char *sReplaceB, *sReplaceE;
                        int i = 0;
                        sTmp1 = strchr(sLine, '\n');
                        if(!sTmp1)
                            continue;
                        *sTmp1 = 0;
                        size_t nOutLeng;
                        snprintf(s1, 500, "%s", sSubject);
                        while(i < nParam)
                        {
                            char *sTmpB, *sTmpE = NULL;
                            int j = 0;
                            sTmpB = sLine;
                            while(j < i && sTmpB)
                            {
                                sTmpB = strchr(sTmpB, ',');
                                if(sTmpB)
                                    sTmpB++;
                                j++;
                            }
                            if(sTmpB)
                            {
                                sTmpE = strchr(sTmpB, ',');
                                if(sTmpE)
                                    *sTmpE = 0;
                            }
                            //printf("SSSS  param[%d](%s) subject:(%s) BBBB\n\n", i, sParam[i], s1);///////////**************
                            sReplaceB = strstr(s1, sParam[i]);
                            while(sReplaceB && sTmpB && sTmpE)
                            {
                                sReplaceE = sReplaceB + strlen(sParam[i]);
                                memmove(sReplaceE+strlen(sTmpB)-strlen(sParam[i]), sReplaceE, s1+strlen(s1)-sReplaceE+2);
                                memcpy(sReplaceB, sTmpB, strlen(sTmpB));
                                sReplaceE = sReplaceB + strlen(sTmpB);
                                sReplaceB = strstr(sReplaceE, sParam[i]);	
                            }
                            //printf("SSSS  param[%d](%s) subject:(%s) AAAAA\n\n", i, sParam[i], s1);////////////**************
                            if(sTmpE)
                                *sTmpE = ',';	
                            ++i;
                        }
                        if(encode_base64(s1, strlen(s1), s2, 2000, &nOutLeng) >= 0)
                        {
                            char *ss = strchr(s2, '\r');
                            if(!ss)
                                ss = strchr(s2, '\n');
                            if(ss)
                                *ss = 0;
                            snprintf(s1, sizeof(s1), "=?utf-8?B?%s?=", s2);
                        }
                        else
                            *s1 = 0;
                        snprintf(sTmp1, 10240-(sTmp1-sLine), ",%s\n", s1);
                    }
                    if(nXRaid && nYRaid)
                    {
                        int nMax = 0;
                        int nVariantValue[20];
                        int i = 0;
                        int j = 0;
                        sTmp1 = strchr(sLine, '\n');
                        if(!sTmp1)
                            continue;
                        *sTmp1 = 0;
                        j = 0;
                        while(j < nXRaid)
                        {
                            char *sVariant = sChartVariant + 40*j;
                            i = 0;
                            while(i < nParam)
                            {
                                if(strcmp(sParam[i], sVariant) == 0)
                                    break;
                                ++i;	
                            }
                            if(i < nParam)
                            {
                                int k = 0;
                                char *sTmpB = sLine, *sTmpE = NULL;
                                while(k < i && sTmpB)
                                {
                                    sTmpB = strchr(sTmpB, ',');
                                    if(sTmpB)
                                        sTmpB++;
                                    k++;	
                                }
                                if(sTmpB)
                                {
                                    sscanf(sTmpB, "%d", nVariantValue+j);
                                    if(nMax < nVariantValue[j])
                                        nMax = nVariantValue[j];	
                                }	
                            }
                            j++;	
                        }
                        {
                            int nGap = 100;
                            int nO = 0;
                            int nMulti = 1;
                            while(nO < 9)
                            {
                                if(nMax < nMulti*10 && nYRaid > 1)
                                {
                                    if(nMax >= nMulti && nMax < nMulti*2)
                                        nGap = (2*nMulti)/(nYRaid-1);
                                    else if(nMax >= nMulti*2 && nMax < nMulti*4)
                                        nGap = (4*nMulti)/(nYRaid-1);
                                    else if(nMax >= nMulti*4 && nMax < nMulti*8)
                                        nGap = (8*nMulti)/(nYRaid-1);	
                                    else
                                        nGap = (10*nMulti)/(nYRaid-1);
                                    break;
                                }
                                nO++;
                                nMulti = nMulti*10;	
                            }	
                            if(nGap == 0)
                                nGap = 100;
                            j = 0;
                            while(j < nYRaid)
                            {
                                snprintf(sTmp1, 10240-(sTmp1-sLine), ",%d", nGap*(nYRaid-j-1));
                                sTmp1 = sTmp1+strlen(sTmp1);
                                ++j;	
                            }
                            j = 0;
                            while(j < nXRaid)
                            {
                                snprintf(sTmp1, 10240-(sTmp1-sLine), ",%d", (36*(nVariantValue[j]))/nGap);
                                sTmp1 = sTmp1+strlen(sTmp1);
                                j++;	
                            }
                        }

                        snprintf(sTmp1, 10240-(sTmp1-sLine), "\n");
                    }
                    fprintf(pFile2, "%s", sLine);		
                }
                fprintf(pFile2, "%s", sLine);	
            }

        }
        //V2.8.1 2014-10-14
        if (pstrModulaText){
            free(pstrModulaText);
            pstrModulaText = NULL;
        }
        if (pstrModulaQVGA){
            free(pstrModulaQVGA);
            pstrModulaQVGA = NULL;
        }
        fclose(pFile2);
        fclose(pFile);
        if(sID[0] == 0)
        {
            unlink(sFileTmp);
            nSuccess = 0;	
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not find the task ID in file:%s.\n", sFileName);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
        }
        else
        {
            if(nAtTime1 > 0)
            {
                time_t tTime;
                struct tm sctTime;
                int nNow1, nNow2;
                while(1)
                {
                    tTime = time(NULL);
                    localtime_r(&tTime, &sctTime);
                    nNow1 = (sctTime.tm_year+1900)*10000 + (sctTime.tm_mon+1)*100 + sctTime.tm_mday;
                    nNow2 = sctTime.tm_hour * 10000 + sctTime.tm_min*100 + sctTime.tm_sec;
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "[TIMED]task %s is timed task, will begin at %d-%d, now is %d-%d.\n", 
                        sID, nAtTime1, nAtTime2, nNow1, nNow2);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    if(nNow1 > nAtTime1 || (nNow1 == nAtTime1 && nNow2 > nAtTime2))
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "[TIMED]will begin task:%s.\n", sID);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        break;						
                    }
                    sleep(60);	
                }
            }

            snprintf(sTaskFileName, sizeof(sTaskFileName), "%s/%s.task", pConf->sTaskFileDir, sID);
            rename(sFileTmp, sTaskFileName);
            {
                struct stat sctFile;
                char s[108];
                int nBackSocket;
                char *sText;
                int nFail = 0;
                stat(sTaskFileName, &sctFile);	

                //if(nBackSocket >= 0)
                while(nFail < 100)
                {
                    sleep(1);
                    int nPipe = -1;
                    if(sPipeName[0])
                    {
                        int ii = 0;
                        while(ii < pConf->nPipeNum && ii < 50)
                        {
                            if(strcasecmp(sPipeName, (pConf->sPipeName)[ii]) == 0)
                            {	
                                nPipe = ii;
                                break;	
                            }
                            ++ii;	
                        }	
                    }

                    if(nPipe < 0 || nPipe >= 50)
                    {
                        nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
                    }
                    else
                    {
                        nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);	
                    }
                    if(nBackSocket < 0)
                    {
                        nFail++;
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not connect back for send task file name %s (fail:%d).\n", sTaskFileName, nFail);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                        continue;	
                    }
                    snprintf(s, 104, "JAVA%s.task", sID); //¹̶¨³¤¶ƬϹţµĹ·қ
                    k = sctFile.st_size;
                    k = htonl(k);
                    memcpy(s+104, &k, 4);
                    SocketWrite(nBackSocket, 5, s, 108);
                    k =	SocketRead(nBackSocket, 5, s, 4);
                    if(k == 3 && strncmp(s, "GO", 2) == 0)
                    {
                        int nWrite, nFinish = 0;
                        FILE *pFile;
                        pFile = fopen(sTaskFileName, "r");
                        if (!pFile)
                        {
                            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR] fopen fail:%s\n",sTaskFileName);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                            return -1;
                        }

                        sText = (char*)malloc(1025);
                        while(nFinish < sctFile.st_size)
                        {

                            if(sctFile.st_size-nFinish > 1024)
                                nWrite = 1024;
                            else
                                nWrite = sctFile.st_size-nFinish;
                            fread(sText, nWrite, 1, pFile);
                            k = SocketWrite(nBackSocket, 5, sText, nWrite);
                            if(k != nWrite)
                            {
                                //printf("eeeeee:%d:%d:%d:\n", k, nWrite, nFinish);
                                break;
                                //return 0;
                            }
                            //printf("ssssss:%d:%d:%d:\n", k, nWrite, nFinish);
                            nFinish += k;

                        }
                        free(sText);
                        fclose(pFile);
                        if(nFinish != sctFile.st_size)
                        {
                            nSuccess = 0;	
                            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]send task %s to back fail, size:%d, send:%d (fail:%d).\n", sTaskFileName, sctFile.st_size, nFinish, nFail);
                            WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                        }
                        else
                        {
                            SocketRead(nBackSocket, 5, s, 4);
                            if(strncmp(s, "OK\n", 3) == 0)
                            {
                                snprintf(sLog+nLogOffset, 10240-nLogOffset, "send task %s to back success, send:%d, (fail:%d).\n", sTaskFileName, nFinish, nFail);
                                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                            }
                            else
                                nSuccess = 0;
                        }
                    }
                    else if(k == 3 && strncmp(s, "HH", 2) == 0)
                    {
                        nSuccess = 1;
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "this task %s has add before (fail:%d).\n", sTaskFileName, nFail);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                    }
                    else
                    {
                        nSuccess = 0;
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not send task file name %s (fail:%d).\n", sTaskFileName, nFail);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                    }
                    close(nBackSocket);
                    if(nSuccess)
                    {
                        nFail = 1000;
                    }
                    else
                    {
                        nFail++;	
                    }
                }
                /*
                else 
                {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "can not connect back for send task file name %s.\n", sTaskFileName);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
                }
                */
            }
        }	
        free(sChart);
    }
    //pause
	else if(nSuccess == 2)
    {
        struct stat sctFile;
        char s[108];
        int nBackSocket;	
        int nPipe = 0;

        nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
        if(nBackSocket > 0)
        {
            snprintf(s, sizeof(s), "PAUS %s\r\n", sID);
            SocketWrite(nBackSocket, 5, s, strlen(s));
            k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
            if(k > 0 && (*s == '2' || strstr(s, "has been paused before")))
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "success pause task:%s(pipe:default).\n", sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }
            else
            {
                if(k > 0)
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, return is:%s(pipe:default)", sID, s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_FOUND;
                }
                else
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, can not get return(pipe:default).\n", sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_REPLY;
                }
                //nSuccess = 0;
            }
            close(nBackSocket);
        }
        else
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, can not connect %s:%d(pipe:default).\n", 
                sID, pConf->sBackIp, pConf->nBackPort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            //nSuccess = 0;
			nstop_status = REQ_TASK_NOT_ACTIVE;
        }
        nPipe = 0;
        while(nPipe < pConf->nPipeNum && nSuccess == 0)
        {
            nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);
            if(nBackSocket > 0)
            {
                snprintf(s, sizeof(s), "PAUS %s\r\n", sID);
                SocketWrite(nBackSocket, 5, s, strlen(s));
                k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
                if(k > 0 && (*s == '2' || strstr(s, "has been paused before")))
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "success pause task:%s(pipe:%s).\n", sID, (pConf->sPipeName)[nPipe]);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    nSuccess = 2;
                }
                else
                {
                    if(k > 0)
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, return is:%s(pipe:%s).\n", 
                            sID, s, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_FOUND;
                    }
                    else
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, can not get return(pipe:%s).\n", 
                            sID, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_REPLY;
                    }
                    //nSuccess = 0;
                }
                close(nBackSocket);
            }
            else
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to pause task:%s, can not connect %s:%d(pipe:%s).\n", 
                    sID, (pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], (pConf->sPipeName)[nPipe]);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				//nSuccess = 0;
				nstop_status = REQ_TASK_NOT_ACTIVE;
            }
            nPipe++;	
        }
    }
    //restart
	else if(nSuccess == 3)
    {
        struct stat sctFile;
        char s[108];
        int nBackSocket;	
        int nPipe;

        nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
        if(nBackSocket > 0)
        {
            snprintf(s, sizeof(s), "REST %s\r\n", sID);
            SocketWrite(nBackSocket, 5, s, strlen(s));
            k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
            if(k > 0 && (*s == '2' || strstr(s, "in pause stat") || strstr(s, "has finished")))
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "success restart task:%s(pipe:default).\n", sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }
            else
            {
                if(k > 0)
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, return is:%s(pipe:default).\n", sID, s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_FOUND;
                }
                else
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, can not get return(pipe:default).\n", sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_REPLY;
                }
                //nSuccess = 0;
            }
            close(nBackSocket);
        }
        else
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, can not connect %s:%d(pipe:default).\n", 
                sID, pConf->sBackIp, pConf->nBackPort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			//nSuccess = 0;
			nstop_status = REQ_TASK_NOT_ACTIVE;
        }

        nPipe = 0;
        while(nPipe < pConf->nPipeNum && nSuccess == 0)
        {
            nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);
            //nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
            if(nBackSocket > 0)
            {
                snprintf(s, sizeof(s), "REST %s\r\n", sID);
                SocketWrite(nBackSocket, 5, s, strlen(s));
                k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
                if(k > 0 && (*s == '2' || strstr(s, "in pause stat") || strstr(s, "has finished")))
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "success restart task:%s(pipe:%s).\n", sID, (pConf->sPipeName)[nPipe]);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    nSuccess = 3;
                }
                else
                {
                    if(k > 0)
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, return is:%s(pipe:%s).\n", 
                            sID, s, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_FOUND;
                    }
                    else
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, can not get return(pipe:%s).\n", 
                            sID, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_REPLY;
                    }
                    //nSuccess = 0;
                }
                close(nBackSocket);
            }
            else
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to restart task:%s, can not connect %s:%d(pipe:%s).\n", 
                    sID, (pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], (pConf->sPipeName)[nPipe]);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				//nSuccess = 0;
				nstop_status = REQ_TASK_NOT_ACTIVE;
            }
            nPipe++;	
        }
    }
    //stop
	else if(nSuccess == 4)
    {
        struct stat sctFile;
        char s[108];
        int nBackSocket;	
        int nPipe = 0;

        nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
        if(nBackSocket > 0)
        {
            snprintf(s, sizeof(s), "STOP %s\r\n", sID);
            SocketWrite(nBackSocket, 5, s, strlen(s));
            k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
            if(k > 0 && *s == '2')
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "success stop task:%s(pipe:default).\n", sID);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
            }
            else
            {
                if(k > 0)
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, return is:%s(pipe:default).\n", sID, s);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_FOUND;
                }
                else
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, can not get return(pipe:default).\n", sID);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
					nstop_status = REQ_TASK_NOT_REPLY;
                }
                //nSuccess = 0;  Ҳ����״̬
            }
            close(nBackSocket);
        }
        else
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, can not connect %s:%d(pipe:default).\n", 
                sID, pConf->sBackIp, pConf->nBackPort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
			//nSuccess = 0;
			nstop_status = REQ_TASK_NOT_ACTIVE;
        }

        nPipe = 0;
        while(nPipe < pConf->nPipeNum && nSuccess == 0)
        {
            //nBackSocket = TcpConnect(pConf->sBackIp, pConf->nBackPort, NULL, 5, NULL, 0);
            nBackSocket = TcpConnect((pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], NULL, 5, NULL, 0);
            if(nBackSocket > 0)
            {
                snprintf(s, sizeof(s), "STOP %s\r\n", sID);
                SocketWrite(nBackSocket, 5, s, strlen(s));
                k = SocketReadLine(nBackSocket, 5, s, sizeof(s));
                if(k > 0 && *s == '2')
                {
                    snprintf(sLog+nLogOffset, 10240-nLogOffset, "success stop task:%s(pipe:%s).\n", sID, (pConf->sPipeName)[nPipe]);
                    WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                    nSuccess = 4;
                }
                else
                {
                    if(k > 0)
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, return is:%s(pipe:%s).\n", sID, s, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_FOUND;
                    }
                    else
                    {
                        snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, can not get return(pipe:%s).\n", sID, (pConf->sPipeName)[nPipe]);
                        WriteLog(pConf, pDabudai, sLog, strlen(sLog));
						nstop_status = REQ_TASK_NOT_REPLY;
                    }
                    //nSuccess = 0;
                }
                close(nBackSocket);
            }
            else
            {
                snprintf(sLog+nLogOffset, 10240-nLogOffset, "fail to stop task:%s, can not connect %s:%d(pipe:%s).\n", 
                    sID, (pConf->sPipeIp)[nPipe], (pConf->nPipePort)[nPipe], (pConf->sPipeName)[nPipe]);
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
				//nSuccess = 0;
				nstop_status = REQ_TASK_NOT_ACTIVE;
            }

            nPipe++;
        }

    }
	//mb 0���ǲ��ɹ�����
    if(nSuccess && nSuccess != 5)
    {
        nSocket = TcpConnect(pConf->sCallBackIp, pConf->nCallBackPort, "127.0.0.1", 5, NULL, 0);
        if(nSocket < 0)
            nSocket = TcpConnect(pConf->sCallBackIp, pConf->nCallBackPort, "127.0.0.1", 5, NULL, 0);
        if(nSocket < 0)
        {
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "[ERROR]can not connnect %s:%d.\n", pConf->sCallBackIp, pConf->nCallBackPort);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));		
        }
        else
        {
            char ssss[200];
            char *sTmpTmp;
            snprintf(ssss, 200, "%s", pConf->sCallBackUrl);
            sTmpTmp = strchr(ssss, '/');
            if(sTmpTmp)
                *sTmpTmp = 0;
            if(nSuccess == 1)
                snprintf(sMsg, 1024, "GET /task_callback?taskId=%s&action=process HTTP/1.1\r\nHost: %s\r\nConnection: Keep-Alive\r\nUser-Agent: onesideloveonesidefriend\r\n\r\n",
                sID, ssss);
            else if(nSuccess == 2)
                snprintf(sMsg, 1024, "GET /task_callback?taskId=%s&action=pause&info=%d HTTP/1.1\r\nHost: %s\r\nConnection: Keep-Alive\r\nUser-Agent: onesideloveonesidefriend\r\n\r\n",
				sID, nstop_status, ssss);
            else if(nSuccess == 3)
                snprintf(sMsg, 1024, "GET /task_callback?taskId=%s&action=restart&info=%d HTTP/1.1\r\nHost: %s\r\nConnection: Keep-Alive\r\nUser-Agent: onesideloveonesidefriend\r\n\r\n",
				sID, nstop_status, ssss);
            else if(nSuccess == 4)
                snprintf(sMsg, 1024, "GET /task_callback?taskId=%s&action=stop&info=%d HTTP/1.1\r\nHost: %s\r\nConnection: Keep-Alive\r\nUser-Agent: onesideloveonesidefriend\r\n\r\n",
                sID, nstop_status, ssss);
            //printf("SEND::::<<%s>>\n", sMsg);
            SocketWrite(nSocket, 3, sMsg, strlen(sMsg));
            SocketReadDoubleLine(nSocket, 3, sMsg, 10240);
            //printf("[[%s]]", sMsg);
            close(nSocket);
            snprintf(sLog+nLogOffset, 10240-nLogOffset, "success for task:%s.\n", sID);
            WriteLog(pConf, pDabudai, sLog, strlen(sLog));
        }
    }
    /*
    http://192.168.16.132:8080/task_callback?taskId=&status=
    GET /?filePath=/WEB-INF/static/upload/task/201112/100_88.txt HTTP/1.1
    Host: 127.0.0.1:5544
    Connection: Keep-Alive
    User-Agent: Apache-HttpClient/4.1.2 (java 1.5)
    */
    free(sMsg);

    return 0;
}



int main(int argc, char *argv[])
{
    DABUDAI *pDabudai = (DABUDAI*)malloc(sizeof(DABUDAI));
    CONFIGURE *pConf1 = (CONFIGURE*)malloc(sizeof(CONFIGURE));
    CONFIGURE *pConf2 = (CONFIGURE*)malloc(sizeof(CONFIGURE));
    CONFIGURE *pConf = pConf1;
    char sLog[1024];
    struct sockaddr_in sctClientAddr; 
    struct sockaddr_in sctServerAddr; 
    int nListenSocket, nConnectSocket;	
    int nNum = 0;
    char sConfFile[100];

    InitialDaBuDai(pDabudai);
    InitialConf(pConf1);
    InitialConf(pConf2);
    srand(0);
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
            ReadConf(sConfFile, pConf1);
            ReadConf(sConfFile, pConf2);
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
        if(sConfFile[0])
        {
            struct stat sctFile;
            stat(sConfFile, &sctFile);
            if(sctFile.st_mtime > pConf->tReadTime)
            {
                snprintf(sLog, sizeof(sLog), "[SYSTEM]detect conf file change, will read again.\n");
                WriteLog(pConf, pDabudai, sLog, strlen(sLog));
                if(pConf == pConf1)
                {
                    //InitialConf(pConf2);
                    ReadConf(sConfFile, pConf2);
                    pConf = pConf2;	
                }
                else
                {
                    //InitialConf(pConf1)
                    ReadConf(sConfFile, pConf1);
                    pConf = pConf1;
                }	
            }	
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
    }

    return 0;
}



