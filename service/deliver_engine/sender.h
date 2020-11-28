/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-15 ¹â¹÷½Ú*/

#ifndef	APP_EmlSender_H_201111 
#define APP_EmlSender_H_201111

#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stddef.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

RFC_NAMESPACE_BEGIN
using namespace std;
#define D_READ_STATUS       1
#define D_WRITE_STATUS      2
#define D_EXCEPT_STATUS     3


#ifndef _D_F_IsDigit
#define _D_F_IsDigit

#define D_F_IsDigit(x)      ( (x >= '0' && x <= '9') )

#endif

int SelectSocket2(int nSocket, struct timeval *pTimeVal, int nStatus);
int TcpConnect(char *sDestIp, int nPort, char *sBindIp, int nSecond, char *sErrMsg, int nErrMsgSize);
int SocketReadLine(int nSocket, int nSecond, char *sMsg, int nMsgSize);
int SocketReadSmtp(int nSocket, int nSecond, char *sMsg, int nMsgSize);
int SocketWrite(int nSocket, int nSecond, char *sMsg, int nLeng);


class Sender 
{
public:
	Sender();
	~Sender();
	void Set(const char*, const char*, rfc_uint_16);
	int  Send2Domain(vector<string> &,map<string,int> &);
	char m_ip[64];
	char m_vip[64];
	rfc_uint_16 m_port;
};

RFC_NAMESPACE_END

#endif	
