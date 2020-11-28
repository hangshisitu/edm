#include "conf.h"

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
	pConf->nBindPort = 5544;
	snprintf(pConf->sBackIp, sizeof(pConf->sBackIp), "127.0.0.1");
	pConf->nBackPort = 5566; 
	snprintf(pConf->sLogName, sizeof(pConf->sLogName), "log/sideside");
	pConf->nMaxLogSize = 1024*1024*100;
	snprintf(pConf->sTaskFileDir, sizeof(pConf->sTaskFileDir), "task");
	snprintf(pConf->sTaskBakDir, sizeof(pConf->sTaskBakDir), "backup");
	snprintf(pConf->sCallBackUrl, sizeof(pConf->sCallBackUrl), "127.0.0.1:8080/task_callback");
	snprintf(pConf->sWebBaseDir, sizeof(pConf->sWebBaseDir), "/usr/local/tomcat/webapps/Webadmin");
	snprintf(pConf->sCallBackIp, sizeof(pConf->sCallBackIp), "127.0.0.1");
	snprintf(pConf->sReplyTo, sizeof(pConf->sReplyTo), "services@mtargeting.net");
	snprintf(pConf->sReturnPath, sizeof(pConf->sReturnPath), "services@mtargeting.net");
	pConf->nCallBackPort = 8080;
	pConf->tReadTime = 0;
	pConf->nPipeNum = 0;
	
	return 0;
}


int ReadConf(char *sFileName, CONFIGURE *pConf)
{
	FILE *pIn = fopen(sFileName, "r");
	char sLine[1024];
	int nPipeNum = 0;
	
	pConf->nPipeNum = 0;
	
	while(fgets(sLine, 1024, pIn))
	{
		char *sTmp;
		
		if(TreatConfLine(sLine, &sTmp))
			continue;
		if(strcasecmp(sLine, "sideside_bind_ip") == 0)
		{
			snprintf(pConf->sBindIp, sizeof(pConf->sBindIp), "%s", sTmp);
		}	
		else if(strcasecmp(sLine, "sideside_bind_port") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nBindPort = n;
		}
		else if(strcasecmp(sLine, "log_name") == 0)	
		{
			snprintf(pConf->sLogName, sizeof(pConf->sLogName), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "task_file_dir") == 0)	
		{
            if (strlen(sTmp) > 0)
            {
                snprintf(pConf->sTaskFileDir, sizeof(pConf->sTaskFileDir), "%s", sTmp);
            }
		}
		else if(strcasecmp(sLine, "max_log_size") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nMaxLogSize = n*1024;	
		}
		else if(strcasecmp(sLine, "task_backup_dir") == 0)
		{
			snprintf(pConf->sTaskBakDir, sizeof(pConf->sTaskBakDir), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "back_ip") == 0)	
		{
			snprintf(pConf->sBackIp, sizeof(pConf->sBackIp), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "back_port") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nBackPort = n;
		}
		else if(strcasecmp(sLine, "call_back_url") == 0)	
		{
			snprintf(pConf->sCallBackUrl, sizeof(pConf->sCallBackUrl), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "web_base_dir") == 0)	
		{
			snprintf(pConf->sWebBaseDir, sizeof(pConf->sWebBaseDir), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "call_back_ip") == 0)	
		{
			snprintf(pConf->sCallBackIp, sizeof(pConf->sCallBackIp), "%s", sTmp);
		}
		else if(strcasecmp(sLine, "call_back_port") == 0)	
		{
			int n = atoi(sTmp);
			if(n > 0)
				pConf->nCallBackPort = n;
		}
		else if(strcasecmp(sLine, "reply_to") == 0)
		{
			snprintf(pConf->sReplyTo, sizeof(pConf->sReplyTo), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "return_path") == 0)
		{
			snprintf(pConf->sReturnPath, sizeof(pConf->sReturnPath), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "pipe_number") == 0)
		{
			if(nPipeNum < 50)
			{
				nPipeNum = pConf->nPipeNum;
				snprintf((pConf->sPipeIp)[nPipeNum], 20, "127.0.0.1");
				snprintf((pConf->sPipeName)[nPipeNum], 50, "pipe%d", nPipeNum);
				(pConf->nPipePort)[nPipeNum] = 55660;
				//snprintf((pConf->sTaskDir)[i], 100, "task");
				(pConf->nPipeNum)++;
			}
		}
		else if(strcasecmp(sLine, "pipe_name") == 0)
		{
			snprintf((pConf->sPipeName)[nPipeNum], 50, "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "pipe_ip") == 0)
		{
			snprintf((pConf->sPipeIp)[nPipeNum], 20, "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "pipe_port") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0)
				(pConf->nPipePort)[nPipeNum] = n;
		}
		else if(strcasecmp(sLine, "add_ad_domain") == 0)
		{
			if(pConf->nAddAdDomainOffset < 1000)
			{
				snprintf(pConf->sAddAdDomain+pConf->nAddAdDomainOffset, 1024-pConf->nAddAdDomainOffset, "%s,", sTmp);	
				(pConf->nAddAdDomainOffset) += (strlen(sTmp)+1);
			}
		}
		/*
		else if(strcasecmp(sLine, "") == 0)	
		{
			
		}
		*/
	}
	pConf->tReadTime = time(NULL);
	
	fclose(pIn);
	return 0;
	
}

int ShowConf(CONFIGURE *pConf)
{
	
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




