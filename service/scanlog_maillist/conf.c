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
	//snprintf(pConf->sScanDir, sizeof(pConf->sScanDir), "dblog");
	snprintf(pConf->sLogName, sizeof(pConf->sLogName), "log/findallworld");
	pConf->nMaxLogSize = 1024*10240;
	snprintf(pConf->sSaveDir, sizeof(pConf->sSaveDir), "backup");
	snprintf(pConf->sIpAreaFile, sizeof(pConf->sIpAreaFile), "ip_bak.txt");
	pConf->nScanNum = 1;
	snprintf((pConf->sLogDir)[0], 100, "log");
	snprintf((pConf->sTaskDir)[0], 100, "task");
	snprintf((pConf->sBackupDir)[0], 100, "backup");
	/*	char sBindIp[20];
	int nBindPort;
	
	int nScanNum;	
	char sLogDir[50][100];
	char sTaskDir[50][100];
	char sBackupDir[50][100];
	
	char sLogName[100];
	int nMaxLogSize;
	char sSaveDir[100];
	
	char sIpAreaFile[100];
	*/
	return 0;
}


int ReadConf(char *sFileName, CONFIGURE *pConf)
{
	FILE *pIn = fopen(sFileName, "r");
	char sLine[1024];
	int i = 0;
	
	pConf->nScanNum = 0;
	while(fgets(sLine, 1024, pIn))
	{
		char *sTmp;
		
		if(TreatConfLine(sLine, &sTmp))
			continue;
		if(strcasecmp(sLine, "scanlog_bind_ip") == 0)
		{
			snprintf(pConf->sBindIp, sizeof(pConf->sBindIp), "%s", sTmp);
		}	
		else if(strcasecmp(sLine, "scanlog_bind_port") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nBindPort = n;
		}
		else if(strcasecmp(sLine, "log_name") == 0)
		{
			snprintf(pConf->sLogName, sizeof(pConf->sLogName), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "max_log_size") == 0)
		{
			int n = atoi(sTmp);
			if(n > 0 && n < 60000)
				pConf->nMaxLogSize = n * 1024;
		}
		else if(strcasecmp(sLine, "save_dir") == 0)
		{
			snprintf(pConf->sSaveDir, sizeof(pConf->sSaveDir), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "ip_area_file") == 0)
		{
			snprintf(pConf->sIpAreaFile, sizeof(pConf->sIpAreaFile), "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "number") == 0)
		{
			i = pConf->nScanNum;
			if(i < 50)
			{
				snprintf((pConf->sLogDir)[i], 100, "log");
				snprintf((pConf->sBackupDir)[i], 100, "backup");
				snprintf((pConf->sTaskDir)[i], 100, "task");
				(pConf->nScanNum)++;
			}
		}
		else if(strcasecmp(sLine, "scan_log_dir") == 0)
		{
			printf("scan_log_dir <<%d>>\n", i);
			snprintf((pConf->sLogDir)[i], 100, "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "scan_task_dir") == 0)
		{
			snprintf((pConf->sTaskDir)[i], 100, "%s", sTmp);	
		}
		else if(strcasecmp(sLine, "scan_backup_dir") == 0)
		{
			snprintf((pConf->sBackupDir)[i], 100, "%s", sTmp);	
		}
		/*	char sBindIp[20];
	int nBindPort;
	
	int nScanNum;	
	char sLogDir[50][100];
	char sTaskDir[50][100];
	char sBackupDir[50][100];
	
	char sLogName[100];
	int nMaxLogSize;
	char sSaveDir[100];
	
	char sIpAreaFile[100];
	*/
	
	}
	
	
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



