#include "socket_tool.h"

extern int errno;


static int SelectSocket(int nSocket, int nSecond, int nStatus)
{
	int n = 0;
	struct timeval sctTimeVal;
	fd_set sctFdSet;
	
	sctTimeVal.tv_sec = nSecond;
 	sctTimeVal.tv_usec = 0;
	FD_ZERO(&sctFdSet);
	FD_SET(nSocket, &sctFdSet);
	if(nStatus == D_READ_STATUS)
	{
		n = 1;
		while(n > 0)
		{
			n = select(nSocket+1, &sctFdSet, 0, 0, &sctTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	else if(nStatus == D_WRITE_STATUS)
	{
		n = 1;
		while(n > 0)
		{
			n = select(nSocket+1, 0, &sctFdSet, 0, &sctTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	else if(nStatus == D_EXCEPT_STATUS)
	{
		n = 1;	
		while(n > 0)
		{
			n = select(nSocket+1, 0, 0, &sctFdSet, &sctTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	
	return n;
}

static int SelectSocket2(int nSocket, struct timeval *pTimeVal, int nStatus)
{
	int n = 0;
	fd_set sctFdSet;
	
	
	FD_ZERO(&sctFdSet);
	FD_SET(nSocket, &sctFdSet);
	if(nStatus == D_READ_STATUS)
	{
		n = 1;
		while(n > 0)
		{
			n = select(nSocket+1, &sctFdSet, 0, 0, pTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	else if(nStatus == D_WRITE_STATUS)
	{
		n = 1;
		while(n > 0)
		{
			n = select(nSocket+1, 0, &sctFdSet, 0, pTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	else if(nStatus == D_EXCEPT_STATUS)
	{
		n = 1;	
		while(n > 0)
		{
			n = select(nSocket+1, 0, 0, &sctFdSet, pTimeVal);
			if(n > 0)
			{
				if(FD_ISSET(nSocket, &sctFdSet))
					break;
			}
		}	
	}
	
	return n;
}


int UnixConnect(char *sConnFileName,  int nSecond, char *sErrMsg, int nErrMsgSize)
{
	struct sockaddr_un sctDest;
	int nLeng;
	long non_blocking = 1;
	int nSocket = -1;
	int nRet;
  
	
	if(sConnFileName == NULL || *sConnFileName == 0)
	{
		if(sErrMsg && nErrMsgSize > 0)
			snprintf(sErrMsg, nErrMsgSize, "connect filename is NULL!");
		return -1;
	}
	memset(&sctDest, 0x0, sizeof(sctDest));
	sctDest.sun_family = AF_UNIX;  
	snprintf(sctDest.sun_path, sizeof(sctDest.sun_path), "%s", sConnFileName);
	
	nLeng = offsetof(struct sockaddr_un, sun_path) + strlen(sctDest.sun_path);
	
	
	nSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(nSocket < 0)
	{
		if(sErrMsg && nErrMsgSize > 0)
			snprintf(sErrMsg, nErrMsgSize, "create socket fail:<%s>.", strerror(errno));
		return -1;	
	}	
		
	ioctl(nSocket, FIONBIO, &non_blocking);
	nRet = connect(nSocket, (struct sockaddr *)&sctDest, nLeng);
	if (nRet == 0)
		return nSocket;
	else if(nRet == -1 && errno==EINPROGRESS)
	{
		struct timeval sctTimeVal; 
 	 	fd_set sctFdSet;		
		
		sctTimeVal.tv_sec = nSecond;
    sctTimeVal.tv_usec = 0;
    FD_ZERO(&sctFdSet); 
    FD_SET(nSocket, &sctFdSet);
    if(select(nSocket+1, NULL, &sctFdSet, NULL, &sctTimeVal)>0)
    { 
		  if(FD_ISSET(nSocket, &sctFdSet))
		  {
		   	int nError = 0;
			  nLeng = sizeof(nError);
			  if(getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, &nLeng) != 0)
			  {
			  	if(sErrMsg && nErrMsgSize > 0)
			  		snprintf(sErrMsg, nErrMsgSize, "getsockopt error[%s])", strerror(errno));
			    close(nSocket);
			    return -1;
			  }
			  if(nError)
    		{
    			if(sErrMsg && nErrMsgSize > 0)	
    				snprintf(sErrMsg, nErrMsgSize, "getsockopt find error.");
    			close(nSocket);
    			return -1;	
    		}
			  return nSocket;
			}
			else
			{
				if(sErrMsg && nErrMsgSize > 0)
					snprintf(sErrMsg, nErrMsgSize, "select time out.");
				close(nSocket);
				return -1;
			}
		}
		else 
		{
			if(sErrMsg && nErrMsgSize > 0)
				snprintf(sErrMsg, nErrMsgSize, "connect timedout.");
		  close(nSocket);
		  return -1; 
		}	
	}
	else 
	{
		if(sErrMsg && nErrMsgSize > 0)
			snprintf(sErrMsg, nErrMsgSize, "connect error(%s)", strerror(errno) );
		close(nSocket);
		return -1;
	}
	snprntf(sErrMsg, nErrMsgSize, "unknown error.");
	close(nSocket);
	return -1;
}


int TcpConnect(char *sDestIp, int nPort, char *sBindIp, int nSecond, char *sErrMsg, int nErrMsgSize)
{
	struct sockaddr_in sctDest;
	struct sockaddr_in sctClient;
	int nSocket = -1;
	int nTmp = 1;
	int nRet = 0;
	
	memset(&sctDest, 0x0, sizeof(sctDest));
	sctDest.sin_family = AF_INET;  
	sctDest.sin_port = htons(nPort);
	{
		int n1, n2, n3, n4;
		if(sscanf(sDestIp, "%d.%d.%d.%d", &n1, &n2, &n3, &n4) == 4 && n1 < 256 && n2 < 256 && n3 < 256 && n4 < 256)
		{	
			sctDest.sin_addr.s_addr = inet_addr(sDestIp); 
		}
		else
		{
			struct hostent *pHost;
			pHost = gethostbyname(sDestIp);
			if(pHost)
			{
				memcpy(&(sctDest.sin_addr.s_addr), pHost->h_addr, pHost->h_length);
			}
			else
			{
				if(sErrMsg && nErrMsgSize > 0)
					snprintf(sErrMsg, nErrMsgSize, "can't resolve hostname[%s].", sDestIp);
				return -1;
			}  	
		}
	}
	nSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(nSocket < 0)
	{
		if(sErrMsg && nErrMsgSize > 0)
			snprintf(sErrMsg, nErrMsgSize, "creat socket fail:<%s>.", strerror(errno));
		return -1;
	}
	
	if(sBindIp && *sBindIp)
	{
		memset(&sctClient, 0x0, sizeof(sctClient));
		sctClient.sin_family = AF_INET;
		sctClient.sin_addr.s_addr = inet_addr(sBindIp); 	
		if(bind(nSocket, (struct sockaddr *)&sctClient, sizeof(sctClient)) < 0)
		{	
			if(sErrMsg && nErrMsgSize > 0)
				snprintf(sErrMsg, nErrMsgSize, "bind [%s] fail:<%s>.", sBindIp, strerror(errno));	
			close(nSocket);
			return -1;
		}
	}
	
	ioctl(nSocket, FIONBIO, &nTmp);
	
	nRet = connect(nSocket, (struct sockaddr *)&sctDest, sizeof(sctDest));
	if (nRet == 0) 
	{
		return nSocket;
	}
	else if(nRet == -1 && errno == EINPROGRESS)
	{
		struct timeval sctTimeVal;
		fd_set sctFdSet;
		
		sctTimeVal.tv_sec = nSecond;
		sctTimeVal.tv_usec = 0;
		FD_ZERO(&sctFdSet); 
    FD_SET(nSocket, &sctFdSet);
    if(select(nSocket+1, NULL, &sctFdSet, NULL, &sctTimeVal) > 0)
    {
    	if(FD_ISSET(nSocket, &sctFdSet))
    	{
    		int nError = 0, nLeng = sizeof(nError);
    		if(getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, &nLeng) != 0)
    		{
    			if(sErrMsg && nErrMsgSize > 0)
    				snprintf(sErrMsg, nErrMsgSize, "solaris pending error.");
    			close(nSocket);
    			return -1;
    		}
    		if(nError)
    		{
    			if(sErrMsg && nErrMsgSize > 0)
    				snprintf(sErrMsg, nErrMsgSize, "getsockopt find error.");
    			close(nSocket);
    			return -1;	
    		}
    	}
    	else
    	{
    		if(sErrMsg && nErrMsgSize > 0)
    			snprintf(sErrMsg, nErrMsgSize, "select error:<%s>.", strerror(errno));
    		close(nSocket);
    		return -1;	
    	}
     	return nSocket;
		}
		else
		{
			if(sErrMsg && nErrMsgSize > 0)
				snprintf(sErrMsg, nErrMsgSize, "select time out while connect:<%s>.", strerror(errno));
			close(nSocket);
			return -1;	
		}
	}
	else
	{
		if(sErrMsg && nErrMsgSize > 0)
			snprintf(sErrMsg, nErrMsgSize, "connect fail:<%s>.", strerror(errno));
		close(nSocket);
		return -1;
	}
	
	if(sErrMsg && nErrMsgSize > 0)	
		snprintf(sErrMsg, nErrMsgSize, "unknown error.");
	close(nSocket);
	return -1;
}


int SocketRead(int nSocket, int nSecond, char *sMsg, int nMsgSize)
{
	struct timeval sctTimeVal;
	int nRead = 0;
	int nSize = nMsgSize - 1;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nRead < nSize)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_READ_STATUS) > 0)
		{
			int n = 0;
			n = recv(nSocket, sMsg+nRead, nSize-nRead, 0);
			if(n > 0)
				nRead += n;
			else
			{
				sMsg[nRead] = 0;
				return nRead;	
			}
		}
		else
		{
			sMsg[nRead] = 0;
			return nRead;
		}
	}
	
	sMsg[nRead] = 0;
	return nRead;
}

int SocketRead2(int nSocket, int nSecond, char *sMsg, int nMsgSize)
{
	struct timeval sctTimeVal;
	int nRead = 0;
	int nSize = nMsgSize;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nRead < nSize)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_READ_STATUS) > 0)
		{
			int n = 0;
			n = recv(nSocket, sMsg+nRead, nSize-nRead, 0);
			if(n > 0)
				nRead += n;
			else
			{
				//sMsg[nRead] = 0;
				return nRead;	
			}
		}
		else
		{
			//sMsg[nRead] = 0;
			return nRead;
		}
	}
	
	//sMsg[nRead] = 0;
	return nRead;
}


int SocketReadLine(int nSocket, int nSecond, char *sMsg, int nMsgSize)
{
	struct timeval sctTimeVal;
	int nRead = 0;
	int nSize = nMsgSize - 1;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nRead < nSize)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_READ_STATUS) > 0)
		{
			int n = 0;
			n = recv(nSocket, sMsg+nRead, nSize-nRead, 0);
			if(n > 0)
			{
				nRead += n;
				if(sMsg[nRead-1] == '\n')
				{
					sMsg[nRead] = 0;
					return nRead;	
				}
			}
			else
			{
				sMsg[nRead] = 0;
				return nRead;	
			}
		}
		else
		{
			sMsg[nRead] = 0;
			return nRead;
		}
	}
	
	sMsg[nRead] = 0;
	return nRead;
		
}


int SocketReadSmtp(int nSocket, int nSecond, char *sMsg, int nMsgSize)
{
	struct timeval sctTimeVal;
	int nRead = 0;
	int nSize = nMsgSize - 1;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nRead < nSize)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_READ_STATUS) > 0)
		{
			int n = 0;
			n = recv(nSocket, sMsg+nRead, nSize-nRead, 0);
			if(n > 0)
			{
				nRead += n;
				if(sMsg[nRead-1] == '\n')
				{
					char *sTmp;
					sMsg[nRead-1] = 0;
					sTmp = strrchr(sMsg, '\n');
					if( !sTmp )
						sTmp = sMsg;
					else
						sTmp++;
					{
						
						if(D_F_IsDigit(*sTmp) && D_F_IsDigit(*(sTmp+1)) && D_F_IsDigit(*(sTmp+2)) && *(sTmp+3) ==' ')
						{
							sMsg[nRead-1] = '\n';
							sMsg[nRead] = 0;
							return nRead;	
						}
					}
					sMsg[nRead-1] = '\n';	
				}
			}
			else
			{
				sMsg[nRead] = 0;
				return nRead;	
			}
		}
		else
		{
			sMsg[nRead] = 0;
			return nRead;
		}
	}
	
	sMsg[nRead] = 0;
	return nRead;

	
}


int SocketWrite(int nSocket, int nSecond, char *sMsg, int nLeng)
{
	int nWrite = 0;
	struct timeval sctTimeVal;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nWrite < nLeng)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_WRITE_STATUS) > 0)
		{
			int n = 0;
			n = send(nSocket, sMsg+nWrite, nLeng-nWrite, 0);
			if(n > 0)
				nWrite += n;
			else
			{
				return nWrite;	
			}
		}
		else
		{
			return nWrite;
		}
	}
	
	return nWrite;
}



int SocketWriteBig(int nSocket, int nSecond, char *sMsg, int nLeng)
{
	int nFinish = 0;
	int nSend = 1024;
	
	while(nFinish < nLeng)
	{
		int k = 0;
		if(nLeng - nFinish > 1024)
			nSend = 1024;
		else
			nSend = nLeng - nFinish;
		k = SocketWrite(nSocket, nSecond, sMsg+nFinish, nSend);
		if(k <= 0)
			break;
		nFinish += k;	
	}
	return nFinish;	
	
	
}


int ReceiveReply(char **sData, int *nSize, int *nLeng, int nSocket, int nSecond)
{
	char *sTmp;
	int k1 = 0, k2 = 0;
	char *sMsg = *sData;
	*nLeng = 0;
	
	///printf("\nRRRR%d<%d>\n", __LINE__, time(NULL));/////888888888888888888
	k1 = SocketReadDoubleLine(nSocket, nSecond, sMsg, *nSize);
	if(k1 <= 0)
		return -5;
	////printf("\nRRRR%d<%d>  %d\n", __LINE__, time(NULL), k1);/////888888888888888888
	
	sTmp = strstr(sMsg, "Content-Length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "content-Length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "Content-length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "Content-length:");
	if(sTmp)
	{
		sTmp += 15;
		while(*sTmp == ' ')
			sTmp++;
		sscanf(sTmp, "%d", nLeng);
		sTmp = strstr(sMsg, "\r\n\r\n");
		////printf("\nRRRR%d<%d>  k1:%d,leng:%d,has:%d\n", __LINE__, time(NULL), k1, nLeng, k1-(sTmp-sMsg));/////888888888888888888
		if(!sTmp)
		{
			sTmp = strstr(sMsg, "\n\n");
			if(sTmp)
			{
				sTmp += 2;
				k2 = *nLeng - (k1-(sTmp-sMsg));
				if(k2 + k1 + 10 > *nSize)
				{
					*sData = (char*)realloc(*sData, k1+k2+10);
					*nSize = k1+k2+10;
					sMsg = *sData;
				}
				if(k2 > 0)
					k2 = SocketRead2(nSocket, nSecond, sMsg+k1, k2);
			}
		}
		else
		{
			sTmp += 4;	
			k2 = *nLeng - (k1-(sTmp-sMsg));
			if(k2 + k1 + 10 > *nSize)
			{
				*sData = (char*)realloc(*sData, k1+k2+10);
				*nSize = k1+k2+10;
				sMsg = *sData;
			}
			if(k2 > 0)
				k2 = SocketRead2(nSocket, nSecond, sMsg+k1, k2);
		}
		
		////printf("\nRRRR%d<%d>  k1:%d,k2:%d\n", __LINE__, time(NULL), k1, k2);/////888888888888888888	
	}
	else
	{
		char *s1;
		int nFinish = 0;
		s1 = strstr(sMsg, "\r\n\r\n");
		if(s1)
		{
			s1 += 4;
			s1 = strstr(s1, "\r\n\r\n");
			if(s1)
				nFinish = 1;
		}
		else
		{
			s1 = strstr(sMsg, "\n\n");
			if(s1)
			{
				s1 += 2;
				s1 = strstr(s1, "\n\n");
				if(s1)
					nFinish = 1;	
			}	
		}
		if(nFinish == 0)
			k2 = SocketReadDoubleLine(nSocket, nSecond, sMsg+k1, *nSize-k1);
	}	
	
	if(k2 > 0)
		return k1+k2;
	else
		return k1;
}


int SocketReadDoubleLine(int nSocket, int nSecond, char *sMsg, int nMsgSize)
{
	struct timeval sctTimeVal;
	int nRead = 0;
	int nSize = nMsgSize - 1;
	
	sctTimeVal.tv_sec = nSecond;
	sctTimeVal.tv_usec = 0;
	
	while(nRead < nSize)
	{
		if(SelectSocket2(nSocket, &sctTimeVal, D_READ_STATUS) > 0)
		{
			int n = 0;
			n = recv(nSocket, sMsg+nRead, nSize-nRead, 0);
			if(n > 0)
			{
				nRead += n;
				if(nRead > 4)
				{
					if((sMsg[nRead-1] == '\n' && sMsg[nRead-2] == '\n') || (sMsg[nRead-1] == '\n' && sMsg[nRead-2] == '\r' && sMsg[nRead-3] == '\n'))
					{
						sMsg[nRead] = 0;
						return nRead;	
					}
				}
			}
			else
			{
				sMsg[nRead] = 0;
				return nRead;	
			}
		}
		else
		{
			sMsg[nRead] = 0;
			return nRead;
		}
	}
	
	sMsg[nRead] = 0;
	return nRead;
		
}


int ReceivePost(char **sData, int *nSize, int *nLeng, int nSocket, int nSecond)
{
	char *sTmp;
	int k1 = 0, k2 = 0;
	char *sMsg = *sData;
	*nLeng = 0;
	
	
	k1 = SocketReadDoubleLine(nSocket, nSecond, sMsg, *nSize);
	if(k1 <= 0)
		return -1;
		
	
	
	sTmp = strstr(sMsg, "Content-Length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "content-Length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "Content-length:");
	if(!sTmp)
		sTmp = strstr(sMsg, "Content-length:");
	if(sTmp)
	{
		sTmp += 15;
		while(*sTmp == ' ')
			sTmp++;
		sscanf(sTmp, "%d", nLeng);
		sTmp = strstr(sMsg, "\r\n\r\n");
		if(!sTmp)
		{
			sTmp = strstr(sMsg, "\n\n");
			if(sTmp)
			{
				sTmp += 2;
				k2 = *nLeng - (k1-(sTmp-sMsg));
				if(k2 + k1 + 10 > *nSize)
				{
					*sData = (char*)realloc(*sData, k1+k2+10);
					*nSize = k1+k2+10;
					sMsg = *sData;
				}
				if(k2 > 0)
					k2 = SocketRead2(nSocket, nSecond, sMsg+k1, k2);
			}
		}
		else
		{
			sTmp += 4;	
			k2 = *nLeng - (k1-(sTmp-sMsg));
			if(k2 + k1 + 10 > *nSize)
			{
				*sData = (char*)realloc(*sData, k1+k2+10);
				*nSize = k1+k2+10;
				sMsg = *sData;
			}
			if(k2 > 0)
				k2 = SocketRead2(nSocket, nSecond, sMsg+k1, k2);
		}
		
			
	}
	else
	{
		k2 = SocketReadDoubleLine(nSocket, nSecond, sMsg+k1, *nSize-k1);
	}	
	
	if(k2 > 0)
		return k1+k2;
	else
		return k1;
}



