/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "sender.h"
#include <errno.h>



RFC_NAMESPACE_BEGIN
extern FileLog fileLog;

int SelectSocket2(int nSocket, struct timeval *pTimeVal, int nStatus)
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
				snprintf(sErrMsg, nErrMsgSize, "can't resolve hostname[%s].", sDestIp);
				return -1;
			}  
		}
	}
	nSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(nSocket < 0)
	{
		snprintf(sErrMsg, nErrMsgSize, "creat socket fail:<%s>.", strerror(errno));
		return -1;
	}

	if(sBindIp && *sBindIp && (strcmp(sBindIp,"123") != 0))
	{
		memset(&sctClient, 0x0, sizeof(sctClient));
		sctClient.sin_family = AF_INET;
		sctClient.sin_addr.s_addr = inet_addr(sBindIp); 
		if(bind(nSocket, (struct sockaddr *)&sctClient, sizeof(sctClient)) < 0)
		{
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
				if(getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, (socklen_t *)&nLeng) != 0)
				{
					snprintf(sErrMsg, nErrMsgSize, "solaris pending error.");
					close(nSocket);
					return -1;
				}
				if(nError)
				{
					snprintf(sErrMsg, nErrMsgSize, "getsockopt find error.");
					close(nSocket);
					return -1;
				}
			}
			else
			{
				snprintf(sErrMsg, nErrMsgSize, "select error:<%s>.", strerror(errno));
				close(nSocket);
				return -1;
			}
			return nSocket;
		}
		else
		{
			snprintf(sErrMsg, nErrMsgSize, "select time out while connect:<%s>.", strerror(errno));
			close(nSocket);
			return -1;
		}
	}
	else
	{
		snprintf(sErrMsg, nErrMsgSize, "connect fail:<%s>.", strerror(errno));
		close(nSocket);
		return -1;
	}

	snprintf(sErrMsg, nErrMsgSize, "unknown error.");
	close(nSocket);
	return -1;
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

Sender::Sender()
{

}

Sender::~Sender()
{

}

void Sender::Set(const char * vip, const char * ip, rfc_uint_16 port)
{
	memset(m_vip,0,sizeof(m_vip));
	strncpy(m_vip, vip, strlen(vip));

	memset(m_ip,0,sizeof(m_ip));
	strncpy(m_ip, ip, strlen(ip));

	m_port = port; 
	return;
}

int Sender::Send2Domain(vector<string> & in,map<string,int> & outmap)
{
	vector<string>::iterator it;
	int nSocket;
	char sMsg[1024];
	unsigned int i = 0;

	nSocket = TcpConnect(m_ip, m_port, m_vip, 2, sMsg, sizeof(sMsg));
	if(nSocket < 0)
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		return -1;
	}

	int ret = fcntl(nSocket, F_GETFL, 0);
	fcntl(nSocket, F_SETFL, ret | O_NONBLOCK);

	i = SocketReadLine(nSocket, 10, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		close(nSocket);
		return -1;
	}

	snprintf(sMsg, sizeof(sMsg), "EHLO tmp\r\n");
	i = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		close(nSocket);
		return -1;
	}

	i = SocketReadSmtp(nSocket, 10, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		close(nSocket);
		return -1;
	}

	snprintf(sMsg, sizeof(sMsg), "MAIL FROM:<abc@ddd.com>\r\n");
	i = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		close(nSocket);
		return -1;
	}

	i = SocketReadLine(nSocket, 10, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		for(it = in.begin();it!=in.end();it++)
		{
			outmap[*it]=1;
		}
		close(nSocket);
		return -1;
	}

	it = in.begin();
	for(;it != in.end();it++)
	{
		snprintf(sMsg, sizeof(sMsg), "RCPT TO:<%s>\r\n", (*it).c_str());
		i = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
		if(i != strlen(sMsg))
		{
			outmap[*it]=1;
			continue;
		}

		i = SocketReadLine(nSocket, 10, sMsg, sizeof(sMsg));
		if(i <= 0)
		{
			outmap[*it]=1;
			continue;
		}

		if(sMsg[0] != '2')
		{
			outmap[*it]=1;
			continue;
		}
	}

	close(nSocket);
	return 0;
}

RFC_NAMESPACE_END
