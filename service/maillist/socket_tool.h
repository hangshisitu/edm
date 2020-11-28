#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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


#ifndef _H_socket_tool
#define _H_socket_tool


#define D_READ_STATUS			1
#define D_WRITE_STATUS		2
#define D_EXCEPT_STATUS		3


#ifndef _D_F_IsDigit
#define _D_F_IsDigit

#define D_F_IsDigit(x)		( (x >= '0' && x <= '9') )

#endif


int TcpConnect(char *sDestIp, int nPort, char *sBindIp, int nSecond, char *sErrMsg, int nErrMsgSize);

int SocketRead(int nSocket, int nSecond, char *sMsg, int nMsgSize);

int SocketReadLine(int nSocket, int nSecond, char *sMsg, int nMsgSize);

int SocketReadSmtp(int nSocket, int nSecond, char *sMsg, int nMsgSize);

int SocketWrite(int nSocket, int nSecond, char *sMsg, int nLeng);

int UnixConnect(char *sConnFileName,  int nSecond, char *sErrMsg, int nErrMsgSize);

int SocketRead2(int nSocket, int nSecond, char *sMsg, int nMsgSize);

int SocketWriteBig(int nSocket, int nSecond, char *sMsg, int nLeng);

int ReceiveReply(char **sData, int *nSize, int *nLeng, int nSocket, int nSecond);

int SocketReadDoubleLine(int nSocket, int nSecond, char *sMsg, int nMsgSize);

int ReceivePost(char **sData, int *nSize, int *nLeng, int nSocket, int nSecond);

#endif
