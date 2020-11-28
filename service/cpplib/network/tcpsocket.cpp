/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/tcpsocket.h"
#include "network/socketselector.h"
#include "network/networkfunc.h"
#include "base/byteorderfunc.h"
#include "base/deleter.h"

RFC_NAMESPACE_BEGIN

int TCPSocket::onlyShutdown(bool bShutdownRead, bool bShutdownWrite)
{
	return NetworkFunc::shutdown(m_nSocketID, bShutdownRead, bShutdownWrite);
}

int TCPSocket::onlyClose()
{
	int nRet = NetworkFunc::closeSocket(m_nSocketID);
	m_nSocketID = INVALID_SOCKET;
	return nRet;
}

int TCPSocket::shutdownAndClose()
{
	int nRet = onlyShutdown(true, true);
	if ( nRet < 0 )
		return nRet;
	return onlyClose();
}

typeSocketHandle TCPSocket::release(void)
{
	typeSocketHandle nOldHandle = m_nSocketID;
	m_nSocketID = INVALID_SOCKET;
	return nOldHandle;
}

typeSocketHandle TCPSocket::connectTo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
{
	shutdownAndClose();
	struct sockaddr_in remoteAddr = NetworkFunc::makeSockAddr(strIP, uPort);
	m_nSocketID = NetworkFunc::createSocket(AF_INET, SOCK_STREAM);
	if ( !isValid() )
		return INVALID_SOCKET;

	if ( !strBindIP.empty() && strBindIP != "0.0.0.0" )
	{
		struct sockaddr_in bindLocalAddr = NetworkFunc::makeSockAddr(strBindIP, static_cast<rfc_uint_16>(0));
		if ( ::bind(m_nSocketID, (struct sockaddr *)&bindLocalAddr, sizeof(bindLocalAddr)) != 0 )
		{
			onlyClose();
			return INVALID_SOCKET;
		}
	}

	bool bBlocking = ( tvTimeout == TimeValue::g_tvInfiniteTime );
	if ( !bBlocking && NetworkFunc::setSocketBlocking(m_nSocketID, false) < 0 )
	{
		onlyClose();
		return INVALID_SOCKET;
	}

	if ( ::connect(m_nSocketID, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) != 0 )
	{
		if ( bBlocking || (errno != EINPROGRESS && errno != EWOULDBLOCK) )
		{
			onlyClose();
			return INVALID_SOCKET;
		}

		int nMonitorErrno = SocketSyncMonitor::socketWriteCheckTimeout(m_nSocketID, tvTimeout);
		if ( nMonitorErrno != 0 )	// may be ETIMEDOUT
		{
			onlyClose();
			return INVALID_SOCKET;
		}

		if ( NetworkFunc::getSocketAsyncError(m_nSocketID) != 0 )
		{
			onlyClose();
			return INVALID_SOCKET;
		}

		if ( !bBlocking )
			NetworkFunc::setSocketBlocking(m_nSocketID, true);
	} // if ( ::connect(m_nSocketID, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) != 0 )

	return m_nSocketID;
}

typeSocketHandle TCPSocket::connectTo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout)
{
	return connectTo(strIP, uPort, tvTimeout, StringFunc::g_strEmptyString);
}

typeSocketHandle TCPSocket::connectTo(const stdstring & strIP, rfc_uint_16 uPort)
{
	return connectTo(strIP, uPort, TimeValue::g_tvInfiniteTime, StringFunc::g_strEmptyString);
}

typeSocketHandle TCPSocket::listen(rfc_uint_16 uPort, int nQLen, const InternetAddrV4 & ipAddr, bool bBlocking)
{
	shutdownAndClose();
	struct sockaddr_in localAddr = NetworkFunc::makeSockAddr(ipAddr.getInetAddress(), uPort);
	m_nSocketID = NetworkFunc::createSocket(AF_INET, SOCK_STREAM);
	if ( !isValid() )
		return INVALID_SOCKET;
	NetworkFunc::setTCPDelay(m_nSocketID, false);
	NetworkFunc::setSocketOption(m_nSocketID, SOL_SOCKET,SO_REUSEADDR, 1);

	if ( ::bind(m_nSocketID, (struct sockaddr *)&localAddr, sizeof(localAddr)) != 0 )
	{
		onlyClose();
		return INVALID_SOCKET;
	}

	if ( NetworkFunc::setSocketBlocking(m_nSocketID, bBlocking) < 0 )
	{
		onlyClose();
		return INVALID_SOCKET;
	}

	if ( ::listen(m_nSocketID, nQLen) != 0 )
	{
		onlyClose();
		return INVALID_SOCKET;
	}

	return m_nSocketID;
}

typeSocketHandle TCPSocket::accept(void) const
{
	InternetAddrV4		ipClientAddr;
	rfc_uint_16			nClientPort = 0;
	return accept(ipClientAddr, nClientPort);
}

typeSocketHandle TCPSocket::accept(InternetAddrV4 & ipClientAddr, rfc_uint_16 & nClientPort) const
{
	if ( !isValid() )
		return INVALID_SOCKET;

	struct sockaddr_in sockAddr;
	socklen_t nSockAddrLen = sizeof(sockAddr);
	typeSocketHandle nAcceptHandle = ::accept(m_nSocketID, (struct sockaddr *) &sockAddr, &nSockAddrLen);
	ipClientAddr.setInetAddress(sockAddr.sin_addr);
	nClientPort = ByteOrderFunc::ntoh(sockAddr.sin_port);
	return nAcceptHandle;
}

typeSocketHandle TCPSocket::acceptFrom(const TCPSocket & socketListen)
{
	shutdownAndClose();
	m_nSocketID = socketListen.accept();
	return m_nSocketID;
}

typeSocketHandle TCPSocket::acceptFrom(const TCPSocket & socketListen, InternetAddrV4 & ipClientAddr, rfc_uint_16 & nClientPort)
{
	shutdownAndClose();
	m_nSocketID = socketListen.accept(ipClientAddr, nClientPort);
	return m_nSocketID;
}

//////////////////////////////////////////////////////////////////////////

ConnectionInfo::ConnectionInfo(void) : m_uPort(0), m_tvTimeout(TimeValue::g_tvInfiniteTime) {}
ConnectionInfo::ConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
 : m_strIP(strIP), m_uPort(0), m_tvTimeout(tvTimeout), m_strBindIP(strBindIP)
{}

void ConnectionInfo::setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
{
	m_strIP = strIP;
	m_uPort = uPort;
	m_tvTimeout = tvTimeout;
	m_strBindIP = strBindIP;
}

bool ConnectionInfo::operator ==(const ConnectionInfo & connInfo) const
{
	return (m_strIP == connInfo.m_strIP && m_uPort == connInfo.m_uPort && m_tvTimeout == connInfo.m_tvTimeout && m_strBindIP == connInfo.m_strBindIP );
}

ReConnectTcpSocket::ReConnectTcpSocket(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
: m_ConnInfo(strIP, uPort, tvTimeout, strBindIP)
{
}

void ReConnectTcpSocket::setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
{
	shutdownAndClose();
	m_ConnInfo.setConnectionInfo(strIP, uPort, tvTimeout, strBindIP);
}

typeSocketHandle ReConnectTcpSocket::onReConnect(void)
{
	return TCPSocket::connectTo(m_ConnInfo.m_strIP, m_ConnInfo.m_uPort, m_ConnInfo.m_tvTimeout, m_ConnInfo.m_strBindIP);
}

//////////////////////////////////////////////////////////////////////////

OneHostConnectionPool::OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort)
: m_ConnInfo(strIP, uPort, TimeValue::g_tvInfiniteTime, StringFunc::g_strEmptyString)
{
}

OneHostConnectionPool::OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout)
: m_ConnInfo(strIP, uPort, tvTimeout, StringFunc::g_strEmptyString)
{
}

OneHostConnectionPool::OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
: m_ConnInfo(strIP, uPort, tvTimeout, strBindIP)
{
}

OneHostConnectionPool::~OneHostConnectionPool(void)
{
	std::for_each(m_listTcpSocke.begin(), m_listTcpSocke.end(), Deleter());
}

void OneHostConnectionPool::setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP)
{
	ConnectionInfo connInfo(strIP, uPort, tvTimeout, strBindIP);
	if ( connInfo != m_ConnInfo )
	{
		std::for_each(m_listTcpSocke.begin(), m_listTcpSocke.end(), Deleter());
		m_listTcpSocke.clear();
		m_ConnInfo = connInfo;
	}
}

typeReConnectTcpSocketAutoPtr OneHostConnectionPool::getTCPSocket(void)
{
	typeReConnectTcpSocketAutoPtr auReConnectTcpSocket;
	if ( m_listTcpSocke.empty() )
	{
		auReConnectTcpSocket.reset(new ReConnectTcpSocket(m_ConnInfo.m_strIP, m_ConnInfo.m_uPort, m_ConnInfo.m_tvTimeout, m_ConnInfo.m_strBindIP));
		auReConnectTcpSocket->onReConnect();
	}
	else
	{
		auReConnectTcpSocket.reset(m_listTcpSocke.back());
		m_listTcpSocke.pop_back();
	}
	return auReConnectTcpSocket;
}

bool OneHostConnectionPool::putTCPSocket(typeReConnectTcpSocketAutoPtr & auReConnectTcpSocket)
{
	if ( auReConnectTcpSocket.get() == NULL )
		return false;
	if ( auReConnectTcpSocket->getConnectionInfo() != m_ConnInfo )
		return false;
	m_listTcpSocke.push_back(auReConnectTcpSocket.release());
	return true;
}

size_t OneHostConnectionPool::getFreeCount(const stdstring & strIP, rfc_uint_16 uPort)
{
	return m_listTcpSocke.size();
}

RFC_NAMESPACE_END

