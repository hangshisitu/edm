/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/networkfunc.h"
#include "mutex/mutex.h"
#include "base/byteorderfunc.h"
#include "base/formatstream.h"
#include <stdlib.h>
#include <net/if.h>

RFC_NAMESPACE_BEGIN

bool NetworkFunc::isValidHandle(typeSocketHandle nHandleID)
{
	return ( nHandleID >= 0 && nHandleID != INVALID_SOCKET );
}

typeSocketHandle NetworkFunc::createSocket(int af, int type , int protocal)
{
	typeSocketHandle nSocketID = static_cast<typeSocketHandle>( ::socket( af, type, protocal ) );
	return ( isValidHandle(nSocketID) ? nSocketID : INVALID_SOCKET );
}

int NetworkFunc::closeSocket(typeSocketHandle nSocketID)
{
	if ( !isValidHandle(nSocketID) )
		return 0;

	int nRet = ::close(nSocketID);
	while ( nRet != 0 && errno == EINTR )
		nRet = ::close(nSocketID);
	return nRet;
}

int NetworkFunc::shutdown(typeSocketHandle nSocketID, bool bShutdownRead, bool bShutdownWrite)
{
	if ( !isValidHandle(nSocketID) )
		return 0;

	int nFlags = 0;
	if ( bShutdownRead && bShutdownWrite )
		nFlags = 2;
	else if ( bShutdownWrite )
		nFlags = 1;
	return ::shutdown(nSocketID, nFlags);
}

int NetworkFunc::getSocketOption(typeSocketHandle nSocketID, int nLevel, int nOptionName)
{
	int nOptionValue = 0;
	socklen_t nOptionLen = sizeof(nOptionValue);
	if ( getsockopt(nSocketID, nLevel, nOptionName, &nOptionValue, &nOptionLen) < 0 )
		return -1;
	return nOptionValue;
}

int NetworkFunc::setSocketOption(typeSocketHandle nSocketID, int nLevel, int nOptionName, int nOptionValue)
{
	return ::setsockopt(nSocketID, nLevel, nOptionName, (char*)&nOptionValue, sizeof(nOptionValue));
}

int NetworkFunc::getSocketAsyncError(typeSocketHandle nSocketID)
{
	return getSocketOption(nSocketID, SOL_SOCKET, SO_ERROR);
}

int NetworkFunc::setTCPDelay(typeSocketHandle nSocketID, bool bDelay)
{
	return setSocketOption(nSocketID, IPPROTO_TCP, TCP_NODELAY, bDelay ? 0 : 1);
}

int NetworkFunc::setSocketBlocking(typeSocketHandle nSocketID, bool bBlocking)
{
	//if ( !isValidHandle(nSocketID) )
	//	return -1;

	int nFlags = ::fcntl(nSocketID, F_GETFL, 0);
	if ( nFlags < 0 )
		return nFlags;

#ifdef FNDELAY
#define NONBLOCK_FLAG	FNDELAY
#else
#define NONBLOCK_FLAG	O_NONBLOCK
#endif

	if ( bBlocking )
		nFlags &= ~NONBLOCK_FLAG;
	else
		nFlags |= NONBLOCK_FLAG;
	return ::fcntl(nSocketID, F_SETFL, nFlags);
}

sockaddr_in NetworkFunc::makeSockAddr(const in_addr & inetAddr, rfc_uint_16 nPort)
{
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr = inetAddr;
	sockAddr.sin_port = ByteOrderFunc::hton(nPort);
	return sockAddr;
}

sockaddr_in NetworkFunc::makeSockAddr(const stdstring & strIP, rfc_uint_16 nPort)
{
	in_addr inetAddr;
	if ( !NetworkFunc::getHostByName(strIP.c_str(), inetAddr) )
		inetAddr.s_addr = INADDR_NONE;
	return makeSockAddr(inetAddr, nPort);
}

bool NetworkFunc::getHostByName(const char * lpszHostName, struct in_addr & inetAddr)
{
	if ( lpszHostName == NULL )
		return false;
	rfc_uint_32 nIPInteger = 0;
	if ( getIPV4Integer(lpszHostName, nIPInteger) )
	{
		inetAddr.s_addr = ByteOrderFunc::hton( nIPInteger );
		return true;
	}

	static MutexLock g_lock;
	AutoMutexLock auLock(g_lock);
	struct hostent * pHostEntry = gethostbyname( lpszHostName );
	if ( pHostEntry == NULL ) //requested entry was not found
		return false;
	char ** ppAddrList = pHostEntry->h_addr_list;
	if ( ppAddrList == NULL || ppAddrList[0] == NULL ) //safe check: no address
		return false;

	memcpy(&inetAddr, ppAddrList[0], sizeof(::in_addr));
	return true;
}

void NetworkFunc::getHostByAddr(const struct in_addr & inetAddr, stdstring & strHostName)
{
	static MutexLock g_lock;
	AutoMutexLock auLock(g_lock);
	struct hostent* pHostEntry = gethostbyaddr(&inetAddr, sizeof(inetAddr), AF_INET);

	if ( pHostEntry != NULL )
		strHostName = pHostEntry->h_name;
	else
		strHostName = inet_ntoa(inetAddr);
}

bool NetworkFunc::getPeerInfo(typeSocketHandle nSocketID, in_addr & inetAddr, rfc_uint_16 & nPort)
{
	if ( !isValidHandle(nSocketID) )
		return false;

	struct sockaddr_in sockAddr;
	socklen_t nSockAddrLen = sizeof(sockAddr);
	if ( ::getpeername(nSocketID, (struct sockaddr *) &sockAddr, &nSockAddrLen) != 0 )
		return false;
	inetAddr = sockAddr.sin_addr;
	nPort = ByteOrderFunc::ntoh(sockAddr.sin_port);
	return true;
}

bool NetworkFunc::getPeerInfo(typeSocketHandle nSocketID, in_addr & inetAddr)
{
	rfc_uint_16 nPort = 0;
	return getPeerInfo(nSocketID, inetAddr, nPort);
}

bool NetworkFunc::getLocalInfo(typeSocketHandle nSocketID, in_addr & inetAddr, rfc_uint_16 & nPort)
{
	if ( !isValidHandle(nSocketID) )
		return false;

	struct sockaddr_in sockAddr;
	socklen_t nSockAddrLen = sizeof(sockAddr);
	if ( ::getsockname(nSocketID, (struct sockaddr *) &sockAddr, &nSockAddrLen) != 0 )
		return false;
	inetAddr = sockAddr.sin_addr;
	nPort = ByteOrderFunc::ntoh(sockAddr.sin_port);
	return true;
}

bool NetworkFunc::getLocalInfo(typeSocketHandle nSocketID, in_addr & inetAddr)
{
	rfc_uint_16 nPort = 0;
	return getLocalInfo(nSocketID, inetAddr, nPort);
}

bool NetworkFunc::getIPV4Integer(const char * lpszHostAddr, rfc_uint_32 & nIPInteger)
{
	return GlobalFunc::getIPV4Integer(lpszHostAddr, nIPInteger);
}

bool NetworkFunc::getIPV4Reverse(const stdstring & strIP, stdstring & strReverseIP)
{
	rfc_uint_32 nIP = 0;
	if ( !GlobalFunc::getIPV4Integer(strIP.data(), strIP.size(), nIP) )
		return false;
	getIPV4ReverseString(nIP, strReverseIP);
	return true;
}

void NetworkFunc::getIPV4String(rfc_uint_32 nIP, stdstring & strIP)
{
	strIP = FormatString("%.%.%.%").arg((nIP >> 24) & 0xff).arg((nIP >> 16) & 0xff).arg((nIP >> 8) & 0xff).arg(nIP & 0xff).str();
}

void NetworkFunc::getIPV4ReverseString(rfc_uint_32 nIP, stdstring & strReverseIP)
{
	strReverseIP = FormatString("%.%.%.%").arg(nIP & 0xff).arg((nIP >> 8) & 0xff).arg((nIP >> 16) & 0xff).arg((nIP >> 24) & 0xff).str();
}

bool NetworkFunc::isValidIPV4Addr(const char * lpszHostAddr)
{
	return GlobalFunc::isValidIPV4Addr(lpszHostAddr);
}

bool NetworkFunc::isValidHostAddr(const char * lpszHostAddr)
{
	return GlobalFunc::isValidHostAddr(lpszHostAddr);
}

bool NetworkFunc::isValidHostName(const char * lpszHostName)
{
	return GlobalFunc::isValidHostName(lpszHostName);
}

bool NetworkFunc::getAllLocalIP(std::set<stdstring> & setLocalIP)
{
	typeSocketHandle nSocketID = createSocket();
	if ( nSocketID < 0 )
		return false;

#define MAXINTERFACES 16

	struct ifconf ifc;
	struct ifreq buf[MAXINTERFACES];
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t) buf;
	if ( ioctl(nSocketID, SIOCGIFCONF, (char *)&ifc) != 0 )
	{
		closeSocket(nSocketID);
		return false;
	}

	int nInterface = ifc.ifc_len / sizeof(struct ifreq); 
	while ( nInterface > 0 )
	{
		--nInterface;
		if ( ioctl (nSocketID, SIOCGIFADDR, (char *) &buf[nInterface]) == 0 )
		{
			stdstring strIP = inet_ntoa(((struct sockaddr_in*)(&buf[nInterface].ifr_addr))->sin_addr);
			setLocalIP.insert(strIP);
		}
	} // while ( nInterface > 0 )

	closeSocket(nSocketID);
	return true;

#if RFC_WIN32_VER
#define	MAX_HOST_NAME_SIZE	256
	char lpszHostName[MAX_HOST_NAME_SIZE];
	memset(lpszHostName, 0, MAX_HOST_NAME_SIZE);
	if ( ::gethostname(lpszHostName, MAX_HOST_NAME_SIZE) != 0 )
		return false;

	struct hostent * pHostEntry = gethostbyname( lpszHostName );
	if ( pHostEntry == NULL ) //requested entry was not found
		return false;
	in_addr inetAddr;
	for ( char ** ppAddrList = pHostEntry->h_addr_list; ppAddrList != NULL && *ppAddrList != NULL; ++ppAddrList )
	{
		memcpy(&inetAddr, *ppAddrList, sizeof(::in_addr));
		setLocalIP.insert( inet_ntoa(inetAddr) );
	}
	setLocalIP.insert("127.0.0.1");
	return true;
#endif
}

RFC_NAMESPACE_END

