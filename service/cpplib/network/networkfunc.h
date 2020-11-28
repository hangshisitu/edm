/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SOCKETFUNC_H_201008
#define RFC_SOCKETFUNC_H_201008

#include "base/string.h"
#include <set>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

RFC_NAMESPACE_BEGIN

class NetworkFunc
{
public:
	static bool					isValidHandle(typeSocketHandle nHandleID);
	static typeSocketHandle		createSocket(int af = AF_INET, int type = SOCK_STREAM , int protocal = 0);
	static int					closeSocket(typeSocketHandle nSocketID);

	static int					shutdown(typeSocketHandle nSocketID, bool bShutdownRead, bool bShutdownWrite);

	//return < 0 on error
	// @nLevel: SOL_SOCKET or IPPROTO_TCP
	static int					getSocketOption(typeSocketHandle nSocketID, int nLevel, int nOptionName);
	static int					setSocketOption(typeSocketHandle nSocketID, int nLevel, int nOptionName, int nOptionValue);
	static int					getSocketAsyncError(typeSocketHandle nSocketID);
	static int					setTCPDelay(typeSocketHandle nSocketID, bool bDelay);
	static int					setSocketBlocking(typeSocketHandle nSocketID, bool bBlocking);

	static sockaddr_in			makeSockAddr(const in_addr & inetAddr, rfc_uint_16 nPort);
	static sockaddr_in			makeSockAddr(const stdstring & strIP, rfc_uint_16 nPort);

	static bool					getHostByName(const char * lpszHostName, struct in_addr & inetAddr);
	static void					getHostByAddr(const struct in_addr & inetAddr, stdstring & strHostName);

	static bool					getPeerInfo(typeSocketHandle nSocketID, in_addr & inetAddr, rfc_uint_16 & nPort);
	static bool					getPeerInfo(typeSocketHandle nSocketID, in_addr & inetAddr);
	static bool					getLocalInfo(typeSocketHandle nSocketID, in_addr & inetAddr, rfc_uint_16 & nPort);
	static bool					getLocalInfo(typeSocketHandle nSocketID, in_addr & inetAddr);

	static bool					getIPV4Integer(const char * lpszHostAddr, rfc_uint_32 & nIPInteger);
	static bool					getIPV4Reverse(const stdstring & strIP, stdstring & strReverseIP);

	static void					getIPV4String(rfc_uint_32 nIP, stdstring & strIP);
	static void					getIPV4ReverseString(rfc_uint_32 nIP, stdstring & strReverseIP);

	static bool					isValidIPV4Addr(const char * lpszHostAddr);
	static bool					isValidHostAddr(const char * lpszHostAddr);
	static bool					isValidHostName(const char * lpszHostName);

	static bool					getAllLocalIP(std::set<stdstring> & setLocalIP);
};

RFC_NAMESPACE_END

#endif	//RFC_SOCKETFUNC_H_201008

