/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "emlsender.h"
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

	if(sBindIp && *sBindIp)
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

EmlSender::EmlSender()
{

}

EmlSender::~EmlSender()
{

}

void EmlSender::Set(const char * mail_text,
	 const char * to, 
	 const char * ip, 
	 rfc_uint_16 port,
	 int random,
	 string sender,
	 bool same_flag,
	 const char * task_id, 
	 string helo)
{
	memset(m_to,0,sizeof(m_to));
	strncpy(m_to, to, strlen(to));

	memset(m_ip,0,sizeof(m_ip));
	strncpy(m_ip, ip, strlen(ip));

	m_port = port; 
	m_sender = sender; 
	m_random = random;
	m_same_flag = same_flag;

	snprintf(m_taskid,sizeof(m_taskid),"%s",task_id);
	snprintf(m_helo,sizeof(m_helo),"%s",(char *)helo.c_str());
	return;
}

int EmlSender::SendAuth2DP(const int nSocket, string auth_info, char * err)
{
	char usr[32]={0};
	char pwd[32]={0};
	char sMsg[128]={0};
	sscanf((char *)auth_info.c_str(), "%[^|]|%[^|]", usr,pwd);

	//step 1
	snprintf(sMsg, sizeof(sMsg), "%s\r\n","auth login");
	unsigned int i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		snprintf(err,256,"%s","step 5.1-->");
		return -1;
	}
	fileLog.writeLog(( FormatString("test SendAuth2DP 1 send [%]\n").arg(sMsg).str()));

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		snprintf(err,256,"%s","step 5.2-->");
		return -1;
	}	

	if(sMsg[0] == '3' && sMsg[1] == '3' && sMsg[2] == '4')
	{
		snprintf(err,256,"%s","auth1ok");
	}
	else
	{
		snprintf(err,256,"%s%s","step 5.3-->",sMsg);
		return -1;
	}
	fileLog.writeLog(( FormatString("test SendAuth2DP 2 read [%]\n").arg(sMsg).str()));

	//step 2
	snprintf(sMsg, sizeof(sMsg), "%s\r\n",usr);
	i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		snprintf(err,256,"%s","step 5.4-->");
		return -1;
	}

	fileLog.writeLog(( FormatString("test SendAuth2DP 3 send [%]\n").arg(sMsg).str()));
	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		snprintf(err,256,"%s","step 5.5-->");
		return -1;
	}

	if(sMsg[0] == '3' && sMsg[1] == '3' && sMsg[2] == '4')
	{
		snprintf(err,256,"%s","auth2ok");
	}
	else
	{
		snprintf(err,256,"%s%s","step 5.6-->",sMsg);
		return -1;
	}
	fileLog.writeLog(( FormatString("test SendAuth2DP 4 read [%]\n").arg(sMsg).str()));

	//step 3
	snprintf(sMsg, sizeof(sMsg), "%s\r\n",pwd);
	i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		snprintf(err,256,"%s","step 5.7-->");
		return -1;
	}

	fileLog.writeLog(( FormatString("test SendAuth2DP 5 send [%]\n").arg(sMsg).str()));
	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		snprintf(err,256,"%s","step 5.8-->");
		return -1;
	}

	if(sMsg[0] == '2' && sMsg[1] == '3' && sMsg[2] == '5')
	{
		snprintf(err,256,"%s","auth3ok");
	}
	else
	{
		snprintf(err,256,"%s%s","step 5.9-->",sMsg);
		return -1;
	}
	fileLog.writeLog(( FormatString("test SendAuth2DP 6 read [%]\n").arg(sMsg).str()));


	return 0;	
}

int EmlSender::Send2Domain(unsigned char & s_flag,
	string vip,
	const char * mail_text, 
	size_t  len,
	const char * from, 
	unsigned int seqno,
	vector<string> &vec_helo,
	string mail_from,
	unsigned char from_flag, 
	char * err,
	string is_auth,
	string auth_info)
{
	s_flag = 0;
	int nSocket;
	char * pMsg = new char [len+16];
	char sMsg[1024];
	char mailfrom[128];
	char mailfrommsg[128];

	unsigned int i = 0;
	if(!m_same_flag)                                //不依赖FROM
	{
		char user[128];
		char domain[128];
		memset(user,0,128);
		memset(domain,0,128);
		if(strcmp(mail_from.c_str(),"123") == 0)
		{
			int mid = (strlen(m_to)-1)/2;
			sscanf(from,"%[^@]@%[^@]",user,domain);
			//                      fileLog.writeLog(( FormatString("test m_to [%] [%]\n").arg(m_to).arg(mid).str() ));
			snprintf(mailfrom,sizeof(mailfrom),"%s%02x%02x%02x@%s", user, m_to[0],m_to[mid],m_to[strlen(m_to)-1],domain);
			snprintf(mailfrommsg, sizeof(mailfrommsg), "MAIL FROM:<%s>\r\n", mailfrom);
		}
		/*
		else if(from_flag == 1)
		{
			int mid = (strlen(m_to)-1)/2;
			sscanf((char *)mail_from.c_str(),"%[^@]@%[^@]",user,domain);
			snprintf(mailfrom,sizeof(mailfrom),"%s%02x%02x%02x@%s", user, m_to[0],m_to[mid],m_to[strlen(m_to)-1],domain);
			snprintf(mailfrommsg, sizeof(mailfrommsg), "MAIL FROM:<%s>\r\n", mailfrom);
		}*/
		else //if(from_flag == 0)
		{
			snprintf(mailfrom,sizeof(mailfrom),"%s",(char *) mail_from.c_str()); 
			snprintf(mailfrommsg, sizeof(mailfrommsg), "MAIL FROM:<%s>\r\n",mailfrom);
		}
	}
	else                                            //依赖信头的FROM来定MAILFROM
	{
//		              fileLog.writeLog(( FormatString("test mailfrom [%] \n").arg(m_sender).str() ));
		char o_sender[128];
		snprintf(o_sender,sizeof(o_sender),"%s",(char *)m_sender.c_str());
		char * p = NULL;
		p = strstr(o_sender,"$iedm_ran$");
		char n_sender[128];
		if(p != NULL)
		{
			*p = 0;
			snprintf(n_sender,sizeof(n_sender),"%s%d%s",o_sender,m_random,p+10);
		}
		else
		{
			snprintf(n_sender,sizeof(n_sender),"%s",o_sender);
		}

		snprintf(mailfrom,sizeof(mailfrom),"%s",n_sender); 
		snprintf(mailfrommsg, sizeof(mailfrommsg), "MAIL FROM:<%s>\r\n",mailfrom);
		              fileLog.writeLog(( FormatString("test mailfrom [%] \n").arg(mailfrommsg).str() ));
	}

	do{
		//把MAIL FROM指令中的发件人取出来
		//放到函数的参数 vector<string> &vec_helo 队列的末尾
		//为什么? 因为此参数是传入引用,唯一可以修改后返回
		//供deliverhelper使用
		char* beginptr = strstr(mailfrommsg, "@");
		if (beginptr==NULL){
		    vec_helo.push_back("localhost.com");
		    break;
		}
		char* endptr = strpbrk(++beginptr, ">\r\n");
		string rcpt;
		if (endptr){
			rcpt.assign(beginptr, endptr - beginptr);
		} else{
		    rcpt = beginptr;
		}
		vec_helo.push_back(rcpt);
	}while(0);

	nSocket = TcpConnect(m_ip, m_port, (char *)vip.c_str(), 2, sMsg, sizeof(sMsg));
	if(nSocket < 0)
	{
		fileLog.writeLog(( FormatString("[%] connect mail fail 1 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		s_flag = 4;
		snprintf(err,256,"%s","step 1-->connectfail:"); //修改字符串'step 1-->connectfail:'需要注意，其他地方需要根据此串判断失败
		delete []pMsg;
		return -1;
	}

	int ret = fcntl(nSocket, F_GETFL, 0);
	fcntl(nSocket, F_SETFL, ret | O_NONBLOCK);

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 2 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 2-->connectread:");
		delete []pMsg;
		return -1;
	}
	if(strstr(m_to,"@sohu.com") != NULL)			//sohu read 2 line
	{
		/*
		i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
		fileLog.writeLog(( FormatString("m_to [%] sohu rsp1[%]\n").arg(m_to).arg(sMsg).str() ));
		if(i <= 0)
		{
			fileLog.writeLog(( FormatString("[%] recv mail fail 2 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
			close(nSocket);
			s_flag = 4;
			snprintf(err,256,"%s","step 2-->sohu connectread:");
			delete []pMsg;
			return -1;
		}
		*/
		fileLog.writeLog(( FormatString("m_to [%] sohu rsp1[%]\n").arg(m_to).arg(sMsg).str() ));
		if(strstr(sMsg,"220 sohu") == NULL)
		{
			i = SocketReadLine(nSocket, 120, sMsg, sizeof(sMsg));
			fileLog.writeLog(( FormatString("m_to [%] sohu rsp2[%]\n").arg(m_to).arg(sMsg).str() ));
			if(i <= 0)
			{
				fileLog.writeLog(( FormatString("[%] recv mail fail 2 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
				close(nSocket);
				s_flag = 4;
				snprintf(err,256,"%s","step 2-->sohu connectread:");
				delete []pMsg;
				return -1;
			}
		}
	}


//	if(strcmp(m_helo,"0") == 0)
    if (vec_helo.size())
    {
		unsigned int index_helo;
		index_helo = seqno%(vec_helo.size());
		snprintf(sMsg, sizeof(sMsg), "HELO %s\r\n",(char *)vec_helo[index_helo].c_str());
	}
	else
	{
		snprintf(sMsg, sizeof(sMsg), "HELO %s\r\n",m_helo);
	}
	fileLog.writeLog(( FormatString("test helo [%]\n").arg(sMsg).str() ));

	i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		fileLog.writeLog(( FormatString("[%] send mail fail 3 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 3-->helowrite:");
		delete []pMsg;
		return -1;
	}

	i = SocketReadSmtp(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 4 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 4-->heloread:");
		delete []pMsg;
		return -1;
	}
	if(sMsg[0] != '2')
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 5 error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		if(sMsg[0] == '4')
			s_flag = 4;
		else
			s_flag = 5;
		snprintf(err,256,"%s%s","step 3-->heloresp:",sMsg);
		delete []pMsg;
		return -1;
	}

	if(strcmp(is_auth.c_str(),"true") == 0 && strstr(m_to,"@139.com") != NULL)				//send to delivery platform
	{
		if(SendAuth2DP(nSocket,auth_info, err) != 0)
		{
			fileLog.writeLog(( FormatString("[%] recv mail fail Auth fail 5.n error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
			close(nSocket);
				s_flag = 5;
			delete []pMsg;
			return -1;
		}
	}

	fileLog.writeLog(( FormatString("from [%] to [%]\n").arg(mailfrom).arg(m_to).str() ));
	i = SocketWrite(nSocket, 10, mailfrommsg, strlen(mailfrommsg));
	if(i != strlen(mailfrommsg))
	{
		fileLog.writeLog(( FormatString("[%] send mail fail 6 mailfromwrite error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(mailfrommsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 6-->mailfromwrite:");
		delete []pMsg;
		return -1;
	}

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 7 mailfromread error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 7-->mailfromwrite:");
		delete []pMsg;
		return -1;
	}

	if(sMsg[0] != '2')
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 8 mailfromresp error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrommsg).str() ));
		close(nSocket);
		if(sMsg[0] == '4')
			s_flag = 4;
		else
			s_flag = 5;
		snprintf(err,256,"%s%s","step 8-->mailfromresp:",sMsg);
		delete []pMsg;
		return -1;
	}

	snprintf(sMsg, sizeof(sMsg), "RCPT TO:<%s>\r\n", m_to);
	i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		printf("send erro:\n");
		fileLog.writeLog(( FormatString("[%] send mail fail 9 rcptwrite error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 9-->rcptwrite:");
		delete []pMsg;
		return -1;
	}

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 10 rcptread error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 10-->rcptread:");
		delete []pMsg;
		return -1;
	}

	if(sMsg[0] != '2')
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 11 rcptresp error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		if(sMsg[0] == '4')
			s_flag = 4;
		else
			s_flag = 5;
		snprintf(err,256,"%s%s","step 11-->rcptresp:",sMsg);
		delete []pMsg;
		return -1;
	}

	snprintf(sMsg, sizeof(sMsg), "DATA\r\n");
	i = SocketWrite(nSocket, 10, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		fileLog.writeLog(( FormatString("[%] send mail fail 12 datawrite error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 12-->datawrite:");
		delete []pMsg;
		return -1;
	}

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 13 dataread error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 13-->dataread:");
		delete []pMsg;
		return -1;
	}

	if(sMsg[0] != '3')
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 14 dataresp error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		if(sMsg[0] == '4')
			s_flag = 4;
		else
			s_flag = 5;
		snprintf(err,256,"%s%s","step 14-->dataresp:",sMsg);
		delete []pMsg;
		return -1;
	}
	snprintf(pMsg, len+16, "%s%s", mail_text, "\r\n.\r\n");
	//	fileLog.writeLog((FormatString("mail_text is [%]\n").arg(mail_text).str()));
	i = SocketWrite(nSocket, 10, pMsg, strlen(pMsg));
	if(i != strlen(pMsg))
	{
		fileLog.writeLog(( FormatString("[%] send mail fail 15 mailtextwrite error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 15-->mailtextwrite:");
		delete []pMsg;
		return -1;
	}

	i = SocketReadLine(nSocket, 60, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 16 mailtextread error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		s_flag = 4;
		snprintf(err,256,"%s","step 16-->mailtextread:");
		delete []pMsg;
		return -1;
	}

	if(sMsg[0] != '2')
	{
		fileLog.writeLog(( FormatString("[%] recv mail fail 17 mailtextresp error [%] str [%] sMsg [%] Task [%] IP [%] From [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).arg(sMsg).arg(m_taskid).arg(vip).arg(mailfrom).str() ));
		close(nSocket);
		if(sMsg[0] == '4' && strstr(sMsg,"452 filter blocked") == NULL)
			s_flag = 4;
		else
			s_flag = 5;
		snprintf(err,256,"%s%s","step 17-->mailtextresp:",sMsg);
		delete []pMsg;
		return -1;
	}

	snprintf(sMsg, sizeof(sMsg), "QUIT\r\n");
	i = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
	if(i != strlen(sMsg))
	{
		//		fileLog.writeLog(( FormatString("[%] send fail 13 error [%] str [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).str() ));
		//		close(nSocket);
		//		return -1;
	}

	i = SocketReadLine(nSocket, 10, sMsg, sizeof(sMsg));
	if(i <= 0)
	{
		//		fileLog.writeLog(( FormatString("[%] recv fail 14 error [%] str [%]\n").arg(m_to).arg(errno).arg(strerror(errno)).str() ));
		//		close(nSocket);
		//		return -1;
	}

	close(nSocket);
	fileLog.writeLog(( FormatString("to [%] from [%] task [%] smtp ok\n").arg(m_to).arg(mailfrom).arg(m_taskid).str() ));
	s_flag = 1;
	snprintf(err,256,"%s","ok");
	delete []pMsg;
	return 0;
}

RFC_NAMESPACE_END
