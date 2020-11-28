/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/socketselectorepoll.h"
#include "network/tcpsocket.h"
#include "thread/threadpool.h"
#include "network/socketstream.h"
#include "network/networkfunc.h"
#include "stream/linestream.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

class EpollServer;

static void writeLogLine(const stdstring & strLine)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("T:%(%) %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime()).arg(strLine).str();
	static MutexLock aLockForLog;
	AutoMutexLock auLock(aLockForLog);
	std::cout << strData << std::endl;
}

class EchoServerConnection : public Thread
{
public:
	EchoServerConnection(EpollServer & ListenerReactor, typeSocketHandle nClientSocketID, int nCmdIndex)
		: m_ListenerReactor(ListenerReactor), m_nClientSocketID(nClientSocketID), m_nCmdIndex(nCmdIndex) {}

	virtual int				run(void);

protected:
	virtual bool			onCmd(SocketStream & socketStream) { return false; }

private:
	EpollServer &			m_ListenerReactor;
	typeSocketHandle		m_nClientSocketID;
	int						m_nCmdIndex;	// 命令的顺序号
};

class EpollServer : private NonCopyAble
{
public:
	EpollServer(void) : m_threadPool("EpollServer") {}
	bool		start(const stdstring & strIP, rfc_uint_16 uPort, int nQLen, bool bBlocking);

protected:
	void		onListen(void);
	bool		addSocket(typeSocketHandle nClientSocketID);
	friend class EchoServerConnection;

protected:
	TCPSocket				m_listenerSocket;
	SocketSelectorEpoll		m_SoekctSelector;
	ThreadPool				m_threadPool;
};

int EchoServerConnection::run(void)
{
	writeLogLine( FormatString("EchoServerConnection::run begin event from fd:%, index:%").arg(m_nClientSocketID).arg(m_nCmdIndex).str() );
	stdstring strLine, strReturnSymbol, strRespond;
	TCPSocket autoClientSocket(m_nClientSocketID);	// for auto disconnect
	SocketStream socketStream(m_nClientSocketID);
	ReadLineStream lineStream(socketStream);
	while ( lineStream.readLine(strLine, strReturnSymbol) )
	{
		strRespond = strLine + strReturnSymbol;
		if( socketStream.writeBlock(strRespond.data(), strRespond.size()) != strRespond.size() )
			return 0;
	} //while ( lineStream.readLine(strLine, strReturnSymbol) )

	if ( lineStream.isEndOfStream() )	// 重新加入epoll队列监听保持长连接
	{
		autoClientSocket.setHandle(INVALID_SOCKET);
		m_ListenerReactor.addSocket(m_nClientSocketID);		
	}
	writeLogLine( FormatString("EchoServerConnection::run finish event from fd:%, need close:%")
		.arg(m_nClientSocketID).arg(!lineStream.isEndOfStream()).str() );
	return 0;
}

bool EpollServer::addSocket(typeSocketHandle nClientSocketID)
{
	return m_SoekctSelector.addSocket(nClientSocketID, true, false);
}

void EpollServer::onListen(void)
{
	try
	{
		typeSocketIDList listEventSocketID;
		m_SoekctSelector.getReadableEvents(listEventSocketID);	// 等待epoll事件

		TCPSocket			tcpClientSocket;
		InternetAddrV4		ipClientAddr;
		rfc_uint_16			nClientPort;
		//处理事件
		static int g_nIndex = 0;
		for ( typeSocketIDList::const_iterator it = listEventSocketID.begin(); it != listEventSocketID.end(); ++it )
		{
			// 如果事件的fd和监听的fd相同,表明有新的连接进入
			if ( m_listenerSocket.getHandle() == (*it) )
			{
				tcpClientSocket.acceptFrom(m_listenerSocket, ipClientAddr, nClientPort);
				NetworkFunc::setSocketBlocking(tcpClientSocket.getHandle(), false);
				if ( addSocket(tcpClientSocket.getHandle()) )
				{
					writeLogLine( FormatString("accept from client %:%, fd:%")
						.arg(ipClientAddr.getIPAddress()).arg(nClientPort).arg(tcpClientSocket.getHandle()).str() );
					tcpClientSocket.setHandle(INVALID_SOCKET);
				}
				else
				{
					writeLogLine( FormatString("epoll add socket fail for client %:%, fd:%")
						.arg(ipClientAddr.getIPAddress()).arg(nClientPort).arg(tcpClientSocket.getHandle()).str());
					tcpClientSocket.onlyClose();
				}
			}
			else	// 有可读的fd
			{
				writeLogLine( FormatString("EpollServer::onListen get readable event, fd:%").arg(*it).str() );
				m_SoekctSelector.removeSocket(*it);
				EchoServerConnection * aNewThread = new EchoServerConnection(*this, *it, g_nIndex++);
				aNewThread->createAndStart();
				//m_threadPool.tryPushTask(new EchoServerConnection(*this, *it, g_nIndex++), true);
			}
		}
	}
	catch( Exception & exp )
	{
		writeLogLine( FormatString("Got Exception on EpollServer::onListen:%, errorInfo:%, errcode:%")
			.arg(exp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(exp.getErrorCode()).str() );
	}
	catch( std::exception & stdExp )
	{
		writeLogLine( FormatString("Got std::exception on EpollServer::onListen:%, errorInfo:%, errcode:%")
			.arg(stdExp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() );
	}
	catch ( ... )
	{
		writeLogLine( FormatString("Got unknown exception on EpollServer::onListen, errorInfo:%, errcode:%")
			.arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() );
	}
}

bool EpollServer::start(const stdstring & strIP, rfc_uint_16 uPort, int nQLen, bool bBlocking)
{
	try
	{
		if ( m_listenerSocket.isValid() )
			return false;

		InternetAddrV4 ipAddr(InternetAddrV4::g_AddressAny);
		if ( !strIP.empty() )
			ipAddr.setIPAddress(strIP.c_str());
		if ( !ipAddr.isValid() )
			throw Exception(EADDRNOTAVAIL, FormatString("Listen address is not available:%").arg(strIP).str());

		m_listenerSocket.listen(uPort, nQLen, ipAddr, bBlocking);
		if ( !m_listenerSocket.isValid() )
			throw Exception(FormatString("listen fail for %:%").arg(ipAddr.getIPAddress()).arg(uPort).str());

		if ( !m_SoekctSelector.addSocket(m_listenerSocket.getHandle(), true, false) )
			throw Exception(FormatString("epoll add socket fail for fd:%").arg(m_listenerSocket.getHandle()).str());

		writeLogLine( FormatString("EpollServer start listen on %:%, QSize:%, Blocking:%")
			.arg(ipAddr.getIPAddress()).arg(uPort).arg(nQLen).arg(bBlocking).str() );
		while ( true )
		{
			onListen();
		} // while ( true )
		return true;
	}
	catch( Exception & exp )
	{
		writeLogLine( FormatString("Got Exception on EpollServer::start:%, errorInfo:%, errcode:%")
			.arg(exp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(exp.getErrorCode()).str() );
		return false;
	}
	catch( std::exception & stdExp )
	{
		writeLogLine( FormatString("Got std::exception on EpollServer::start:%, errorInfo:%, errcode:%")
			.arg(stdExp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() );
		return false;
	}
	catch ( ... )
	{
		writeLogLine( FormatString("Got unknown exception on EpollServer::start, errorInfo:%, errcode:%")
			.arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() );
		return false;
	}

	m_threadPool.waitUntilAllTaskFinish();
}

RFC_NAMESPACE_END

USING_RFC_NAMESPACE

int main(int argc, char * argv[])
{
	rfc_uint_16 nPort = static_cast<rfc_uint_16>(9527);
	if ( argc > 1 )
		nPort = StringFunc::stringToInt<rfc_uint_16>(argv[1]);

	EpollServer serverInstance;
	serverInstance.start(StringFunc::g_strEmptyString, nPort, 5, true);
	return 0;
}
