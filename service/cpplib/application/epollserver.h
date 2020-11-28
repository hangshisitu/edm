/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#ifndef RFC_EPOLLSERVER_H_201108
#define RFC_EPOLLSERVER_H_201108

#include "network/socketselectorepoll.h"
#include "network/tcpsocket.h"
#include "thread/threadpool.h"
#include "network/networkfunc.h"
#include "stream/linestream.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

template<typename typeConnection>
class EpollServer;
class SocketStream;

static void writeLogLine(const stdstring & strLine)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("T:%(%) %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime()).arg(strLine).str();
	static MutexLock aLockForLog;
	AutoMutexLock auLock(aLockForLog);
	std::cout << strData << std::endl;
}

template<typename typeConnection>
class ConnectionThread : public Thread
{
public:
	ConnectionThread(EpollServer<typeConnection> & ListenerReactor, typeSocketHandle nClientSocketID, int nCmdIndex)
		: m_ListenerReactor(ListenerReactor), m_nClientSocketID(nClientSocketID), m_nCmdIndex(nCmdIndex) {}
	virtual int				run(void);

protected:
	virtual bool			onCmd(SocketStream & socketStream) { return false; }

private:
	EpollServer<typeConnection> &			m_ListenerReactor;
	typeSocketHandle		m_nClientSocketID;
	int						m_nCmdIndex;	// 命令的顺序号
};

template<typename typeConnection>
class TextCmdConnection : public ConnectionThread<typeConnection>
{
public:
	TextCmdConnection(EpollServer<typeConnection> & ListenerReactor, typeSocketHandle nClientSocketID, int nCmdIndex)
		: ConnectionThread<typeConnection>(ListenerReactor, nClientSocketID, nCmdIndex) {}

protected:
	virtual bool			onCmd(SocketStream & socketStream);
	virtual bool			onCmdLine(SocketStream & socketStream, const stdstring & strLine, const stdstring & strReturnSymbol) { return false; }
};

template<typename typeConnection>
class EpollServer : private NonCopyAble
{
public:
	EpollServer(void) : m_threadPool("EpollServer") {}
	bool		start(const stdstring & strIP, rfc_uint_16 uPort, int nQLen, bool bBlocking);

	bool		addSocket(typeSocketHandle nClientSocketID);

protected:
	void		onListen(void);
	//friend class ConnectionThread;

protected:
	TCPSocket				m_listenerSocket;
	SocketSelectorEpoll		m_SoekctSelector;
	ThreadPool				m_threadPool;
};

template<typename typeConnection>
int ConnectionThread<typeConnection>::run(void)
{
	writeLogLine( FormatString("Get readable event from fd:%, index:%")
		.arg(m_nClientSocketID).arg(m_nCmdIndex).str() );
	TCPSocket autoClientSocket(m_nClientSocketID);	// for auto disconnect
	SocketStream socketStream(m_nClientSocketID);
	if ( !onCmd(socketStream) )
		return 0;
	autoClientSocket.setHandle(INVALID_SOCKET);
	m_ListenerReactor.addSocket(m_nClientSocketID);		// 重新加入epoll队列监听
	return 0;
}

template<typename typeConnection>
bool TextCmdConnection<typeConnection>::onCmd(SocketStream & socketStream)
{
	stdstring strLine, strReturnSymbol;
	ReadLineStream lineStream(socketStream);
	while ( lineStream.readLine(strLine, strReturnSymbol) )
	{
		if ( !onCmdLine(socketStream, strLine, strReturnSymbol) )
			return false;
	} //while ( lineStream.readLine(strLine, strReturnSymbol) )
	return lineStream.isEndOfStream();	// 如果有数据可读,则保留长连接
}

template<typename typeConnection>
bool EpollServer<typeConnection>::addSocket(typeSocketHandle nClientSocketID)
{
	return m_SoekctSelector.addSocket(nClientSocketID, true, false);
}

template<typename typeConnection>
void EpollServer<typeConnection>::onListen(void)
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
				m_SoekctSelector.removeSocket(*it);
				m_threadPool.tryPushTask(new typeConnection(*this, *it, g_nIndex++), true);
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

template<typename typeConnection>
bool EpollServer<typeConnection>::start(const stdstring & strIP, rfc_uint_16 uPort, int nQLen, bool bBlocking)
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

		writeLogLine( FormatString("filtersvr start listen on %:%, QSize:%, Blocking:%")
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

#endif	//RFC_EPOLLSERVER_H_201108
