/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_TCPSOCKET_H_201008
#define RFC_TCPSOCKET_H_201008

#include "network/internetaddr.h"
#include "base/datetime.h"
#include "base/string.h"
#include <list>
#include <memory>

RFC_NAMESPACE_BEGIN

class TCPSocket : private NonCopyAble
{
public:
	TCPSocket(typeSocketHandle nSocketID = INVALID_SOCKET) : m_nSocketID(nSocketID) {}
	~TCPSocket() { shutdownAndClose(); }

	int					onlyShutdown(bool bShutdownRead, bool bShutdownWrite);
	int					onlyClose(void);
	int					shutdownAndClose(void);

	bool				isValid(void) const { return m_nSocketID != INVALID_SOCKET; }
	bool				isClosed(void) const { return m_nSocketID == INVALID_SOCKET; }

	typeSocketHandle	getHandle(void) const { return m_nSocketID; }
	void				setHandle(typeSocketHandle nSocketID) { m_nSocketID = nSocketID; }
	typeSocketHandle	release(void);

	typeSocketHandle	connectTo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);
	typeSocketHandle	connectTo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout);
	typeSocketHandle	connectTo(const stdstring & strIP, rfc_uint_16 uPort);

	typeSocketHandle	listen(rfc_uint_16 uPort, int nQLen = 5, const InternetAddrV4 & ipAddr = InternetAddrV4::g_AddressAny, bool bBlocking = true);

	typeSocketHandle	accept(void) const;
	typeSocketHandle	accept(InternetAddrV4 & ipClientAddr, rfc_uint_16 & nClientPort) const;
	typeSocketHandle	acceptFrom(const TCPSocket & socketListen);
	typeSocketHandle	acceptFrom(const TCPSocket & socketListen, InternetAddrV4 & ipClientAddr, rfc_uint_16 & nClientPort);

private:
	typeSocketHandle	m_nSocketID;
};

//////////////////////////////////////////////////////////////////////////

class ConnectionInfo
{
public:
	ConnectionInfo(void);
	ConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);
	void				setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);

	bool operator		==(const ConnectionInfo & connInfo) const;
	bool operator		!=(const ConnectionInfo & connInfo) const { return !(*this == connInfo); }

	stdstring			m_strIP;
	rfc_uint_16			m_uPort;
	TimeValue			m_tvTimeout;
	stdstring			m_strBindIP;
};

class ReConnectTcpSocket : public TCPSocket
{
public:
	ReConnectTcpSocket(void) {}
	ReConnectTcpSocket(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);

	void				setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);
	typeSocketHandle	onReConnect(void);
	const ConnectionInfo &	getConnectionInfo(void) const { return m_ConnInfo; }

protected:
	ConnectionInfo		m_ConnInfo;
};

typedef std::auto_ptr<ReConnectTcpSocket>						typeReConnectTcpSocketAutoPtr;

// OneHostConnectionPool is not thread-safe
class OneHostConnectionPool
{
public:
	OneHostConnectionPool(void) {}
	OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort);
	OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout);
	OneHostConnectionPool(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);
	~OneHostConnectionPool(void);

	void				setConnectionInfo(const stdstring & strIP, rfc_uint_16 uPort, const TimeValue & tvTimeout, const stdstring& strBindIP);

	typeReConnectTcpSocketAutoPtr	getTCPSocket(void);
	bool							putTCPSocket(typeReConnectTcpSocketAutoPtr & auKeepAliveTcpSocket);

	size_t							getFreeCount(const stdstring & strIP, rfc_uint_16 uPort);

protected:
	ConnectionInfo					m_ConnInfo;

	typedef std::list<ReConnectTcpSocket *>	typeTcpSocketList;
	typeTcpSocketList			m_listTcpSocke;
};

RFC_NAMESPACE_END

#endif	//RFC_SOCKETFUNC_H_201008

