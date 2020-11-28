#include "commuteother7.h"
#include "socket_tool.h"
#include "c_error.h"



int ReadTaskFromJava(int nSocket, char *sFileName, int nFileNameSize, pthread_mutex_t *pLock, int nTimeOut,
												char *sDir1, char *sDir2)
{
	int nRet = 0;	
	char *sMsg = (char*)malloc(1032);
	int nLeng = 0;
	int k = 0;
	int nRead = 0;
	int nFinish = 0;
	char sFileNameTmp[200];
	char sFileNameBak[200];
	int nOpenTmpFile = 0;
	int nOpenTaskFile = 0;
	
	k = SocketRead2(nSocket, nTimeOut, sMsg, 104);
	if(k != 104)
	{
		//printf("\n%d\n\n", __LINE__);
		free(sMsg);
		nRet = D_NetReadIntFail;
		return nRet;	
	}
	memcpy(&nLeng, sMsg+100, 4);
	nLeng = ntohl(nLeng);
	snprintf(sFileNameTmp, 200, "%s/%s.tmp", sDir1, sMsg);
	snprintf(sFileName, nFileNameSize, "%s/%s", sDir1, sMsg);
	snprintf(sFileNameBak, 200, "%s/%s", sDir2, sMsg);
	if(pLock)
		pthread_mutex_lock(pLock);
	if(access(sFileNameTmp, F_OK) == 0)
	{
		nRet = D_TransNotFinish;
	}
	else if(access(sFileName, F_OK) == 0 || access(sFileNameBak, F_OK) == 0)
	{
		nRet = D_TransHasFinish;	
	}
	else if(nLeng <= 0)
	{
		nRet = D_PackLengError;	
	}
	if(nRet == 0)
	{
		FILE *p = fopen(sFileNameTmp, "w");
		if(p)
		{
			fprintf(p, "do not delete this file.\n");
			nOpenTmpFile = 1;
			fclose(p);
		}	
		else
			nRet = D_OpenFileError;
	}
	if(pLock)
		pthread_mutex_unlock(pLock);
	if(nRet)
	{
		free(sMsg);
		if(pLock)
			pthread_mutex_lock(pLock);
		if(nOpenTmpFile)
			unlink(sFileNameTmp);
		if(nOpenTaskFile)
			unlink(sFileName);
		if(pLock)
			pthread_mutex_unlock(pLock);
		return nRet;	
	}
	//sleep(2);
	
	k = SocketWrite(nSocket, nTimeOut, "GO\n", 3);
	//sleep(2);
	if(nRet == 0)
	{
		FILE *pTask;
		if(pLock)
			pthread_mutex_lock(pLock);
		if(access(sFileName, F_OK) == 0)
			nRet = D_TransNotFinish;
		if(nRet == 0)
		{
			pTask = fopen(sFileName, "w");
			if(pTask)
				nOpenTaskFile = 1;
			else
				nRet = D_OpenFileError;
		}
		if(pLock)
			pthread_mutex_unlock(pLock);
		if(nRet)
		{
			free(sMsg);
			if(pLock)
				pthread_mutex_lock(pLock);
			if(nOpenTmpFile)
				unlink(sFileNameTmp);
			if(nOpenTaskFile)
				unlink(sFileName);
			if(pLock)
				pthread_mutex_unlock(pLock);
			return nRet;	
		}
		
		while(nFinish < nLeng && nRet == 0)
		{
			if(nLeng - nFinish > 1024)
				nRead = 1024;
			else
				nRead = nLeng - nFinish;
			k = SocketRead2(nSocket, nTimeOut, sMsg, nRead);
			if(k > 0)
			{
				fwrite(sMsg, k, 1, pTask);
				nFinish += k;
			}
			else
			{
				nRet = D_NetReadMsgFail;
				//printf("eeeee  <%d>  <%d>  <%d>\n", nFinish, nLeng, k);
				fclose(pTask);
				pTask = NULL;	
			}
		}
		if(pTask)
			fclose(pTask);
		if(nRet)
		{
			free(sMsg);
			if(pLock)
				pthread_mutex_lock(pLock);
			if(nOpenTmpFile)
				unlink(sFileNameTmp);
			if(nOpenTaskFile)
				unlink(sFileName);
			if(pLock)
				pthread_mutex_unlock(pLock);
			return nRet;	
		}
		
	}
	
	if(pLock)
		pthread_mutex_lock(pLock);
	if(nOpenTmpFile)
		unlink(sFileNameTmp);
	if(pLock)
		pthread_mutex_unlock(pLock);
	
	free(sMsg);
	return nRet;
}

int ReceivePackage(int nSocket, char **sMsg, int *nMsgSize, int *nMsgLeng, int nTimeOut)
{
	int nRet = 0;
	int k = 0;
	char *s = *sMsg;
	int nLeng = 0;
	
	k = SocketRead2(nSocket, nTimeOut, (char*)(&nLeng), 4);
	if(k != 4)
	{
		//printf("read int %d, %d\n", k, nLeng);
		nRet = D_NetReadIntFail;	
		return nRet;
	}
	
	{
		char s[5];
		memcpy(s, &nLeng, 4);
		s[4] = 0;
		if(strcmp(s, "JAVA") == 0)
		{
			nRet = D_JavaPackage;
			return nRet;	
		}
		else if(strcmp(s, "STOP") == 0)
		{
			nRet = D_StopPackage;
			return nRet;	
		}
		else if(strcmp(s, "REST") == 0)
		{
			nRet = D_RestPackage;
			return nRet;	
		}
		else if(strcmp(s, "PAUS") == 0)
		{
			nRet = D_PausPackage;
			return nRet;	
		}
		else if(strcmp(s, "SHOW") == 0)
		{
			nRet = D_ShowPackage;
			return nRet;	
		}
		else if(strcmp(s, "STAT") == 0)
		{
			nRet = D_StatPackage;
			return nRet;	
		}
		else if(strcasecmp(s, "post") == 0)
		{
			nRet = D_PostPackage;
			return nRet;	
		}
	}
	
	nLeng = ntohl(nLeng);
	if(nLeng <= 0 || nLeng > 1024000)
	{
		//printf("receive leng is:%d.\n", nLeng);
		nRet = D_PackLengError;
		return nRet;	
	}
	
	*nMsgLeng = nLeng;
	
	if(nLeng+10 > *nMsgSize)
	{
		free(*sMsg);
		*sMsg = (char*)malloc(nLeng+10);
		s = *sMsg;
		*nMsgSize = nLeng+10;
	}
	
	k = SocketRead2(nSocket, nTimeOut, s, nLeng);
	if(k != nLeng)
	{
		//printf("read msg %d::%d\n", k, nLeng);
		nRet = D_NetReadMsgFail;
		return nRet;	
	}	
	else
	{
		s[k] = 0;
		s[k+1] = 0;
	}
	return nRet;
}


int SendPackage(int nSocket, char *sMsg, int nLeng, int nTimeOut)
{
	int nFinish = 0;
	int nWrite = 0;
	int nRet = 0;
	
	while(nFinish < nLeng)
	{
		int k = 0;
		if(nLeng - nFinish > 1024)
			nWrite = 1024;
		else
			nWrite = nLeng - nFinish;
		
		k = SocketWrite(nSocket, nTimeOut, sMsg+nFinish, nWrite);
		if(k != nWrite)
		{
			//printf("write  %d::%d::%d:%d\n", k, nWrite, nFinish, nLeng);
			nRet = D_NetWriteFail;
			break;	
		}
		nFinish += nWrite;
	}	
	return nRet;
}

/*
int ReadTaskFromJava(int nSocket, char *sFileName, int nFileNameSize, pthread_mutex_t *pLock)
{
	int nRet = 0;	
	char *sMsg = (char*)malloc(1032);
	int nLeng = 0;
	int k = 0;
	int nRead = 0;
	int nFinish = 0;
	char sFileNameTmp[200];
	int nOpenTmpFile = 0;
	int nOpenTaskFile = 0;
	
	k = SocketRead2(nSocket, 5, sMsg, 104);
	if(k != 104)
	{
		free(sMsg);
		nRet = D_NetReadIntFail;
		return nRet;	
	}
	memcpy(&nLeng, sMsg+100, 4);
	nLeng = ntohl(nLeng);
	snprintf(sFileNameTmp, 200, "%s.tmp", sMsg);
	snprintf(sFileName, nFileNameSize, "%s", sMsg);
	if(pLock)
		pthread_mutex_lock(pLock);
	if(access(sFileNameTmp, F_OK) == 0)	
	{
		nRet = D_TransNotFinish;
	}
	else if(access(sFileName, F_OK) == 0)
	{
		nRet = D_TransHasFinish;	
	}
	if(nRet == 0)
	{
		FILE *p = fopen(sFileNameTmp, "w");
		if(p)
		{
			fprintf(p, "do not delete this file.\n");
			nOpenTmpFile = 1;
			fclose(p);
		}	
		else
			nRet = D_OpenFileError;
	}
	if(pLock)
		pthread_mutex_unlock(pLock);
	if(nRet)
	{
		free(sMsg);
		if(pLock)
			pthread_mutex_lock(pLock);
		if(nOpenTmpFile)
			unlink(sFileNameTmp);
		if(nOpenTaskFile)
			unlink(sFileName);
		if(pLock)
			pthread_mutex_unlock(pLock);
		return nRet;	
	}
	
	if(nRet == 0)
	{
		FILE *pTask;
		if(pLock)
			pthread_mutex_lock(pLock);
		if(access(sFileName, F_OK) == 0)
			nRet = D_TransNotFinish;
		if(nRet == 0)
		{
			pTask = fopen(sFileName, "w");
			if(pTask)
				nOpenTaskFile = 1;
			else
				nRet = D_OpenFileError;
		}
		if(pLock)
			pthread_mutex_unlock(pLock);
		if(nRet)
		{
			free(sMsg);
			if(pLock)
				pthread_mutex_lock(pLock);
			if(nOpenTmpFile)
				unlink(sFileNameTmp);
			if(nOpenTaskFile)
				unlink(sFileName);
			if(pLock)
				pthread_mutex_unlock(pLock);
			return nRet;	
		}
		while(nFinish < nLeng && nRet == 0)
		{
			if(nLeng - nFinish > 1024)
				k = 1024;
			else
				nRead = nLeng - nFinish;
			k = SocketRead2(nSocket, 5, sMsg, nRead);
			if(k > 0)
			{
				fwrite(sMsg, k, 1, pTask);
				nFinish += k;
			}
			else
			{
				nRet = D_NetReadMsgFail;
				fclose(pTask);
				pTask = NULL;	
			}
		}
		if(pTask)
			fclose(pTask);
		if(nRet)
		{
			free(sMsg);
			if(pLock)
				pthread_mutex_lock(pLock);
			if(nOpenTmpFile)
				unlink(sFileNameTmp);
			if(nOpenTaskFile)
				unlink(sFileName);
			if(pLock)
				pthread_mutex_unlock(pLock);
			return nRet;	
		}
		
	}
	
	return k;
}
*/


