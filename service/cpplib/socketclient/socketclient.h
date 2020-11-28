/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#ifndef APPLIB_SOCKETCLIENT_H_201110
#define APPLIB_SOCKETCLIENT_H_201110

#include "network/socketstream.h"

RFC_NAMESPACE_BEGIN

enum	SocketClientRetCode
{
	SOCKET_CLIENT_RET_OK = static_cast<int>(0),
	SOCKET_CLIENT_RET_CONNECT_ERROR = static_cast<int>(-1),
	SOCKET_CLIENT_RET_SEND_NET_ERROR = static_cast<int>(-2),
	SOCKET_CLIENT_RET_RECV_NET_ERROR = static_cast<int>(-3),
	SOCKET_CLIENT_RET_SEND_FORMAT_ERROR = static_cast<int>(-4),
	SOCKET_CLIENT_RET_RECV_FORMAT_ERROR = static_cast<int>(-5),
};

template<typename typeRequest, typename typeRespond>
class SocketClient : public SocketStream
{
public:
	SocketClient(typeSocketHandle nSocketID) : SocketStream(nSocketID) {}
	SocketClient(const SocketStream & s) : SocketStream(s) {}
	virtual ~SocketClient(void) {}

	virtual SocketClientRetCode		onExchange(const typeRequest & varRequest, typeRespond & varRespond);

	virtual SocketClientRetCode		onSend(const typeRequest & varRequest) = 0;
	virtual SocketClientRetCode		onRecv(typeRespond & varRespond) = 0;

	virtual bool					onReConnect(void) { return false; }
	virtual void					disconnect(void) {}
};

template<typename typeRequest, typename typeRespond>
SocketClientRetCode SocketClient<typeRequest, typeRespond>::onExchange(const typeRequest & varRequest, typeRespond & varRespond)
{
	SocketClientRetCode nExchangeRet = onSend(varRequest);
	if ( nExchangeRet != SOCKET_CLIENT_RET_OK )
	{
		if ( !onReConnect() )
			return SOCKET_CLIENT_RET_CONNECT_ERROR;
		nExchangeRet = onSend(varRequest);
	}
	if ( nExchangeRet != SOCKET_CLIENT_RET_OK )
	{
		disconnect();
		return nExchangeRet;
	}

	nExchangeRet = onRecv(varRespond);
	if ( nExchangeRet != SOCKET_CLIENT_RET_OK )
	{
		if ( !onReConnect() )
			return SOCKET_CLIENT_RET_CONNECT_ERROR;
		else if ( (nExchangeRet = onSend(varRequest)) != SOCKET_CLIENT_RET_OK )
		{
			disconnect();
			return nExchangeRet;
		}
		else if ( (nExchangeRet = onRecv(varRespond)) != SOCKET_CLIENT_RET_OK )
		{
			disconnect();
			return nExchangeRet;
		}
	} //if ( nExchangeRet == SOCKET_CLIENT_RET_RECV_NET_ERROR )
	return nExchangeRet;
}

RFC_NAMESPACE_END

#endif	//APPLIB_SOCKETCLIENT_H_201110
