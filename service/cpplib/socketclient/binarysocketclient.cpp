/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#include "socketclient/binarysocketclient.h"
#include "socketclient/socketstreamencoder.h"

RFC_NAMESPACE_BEGIN

RequestHeader::RequestHeader(void)  : m_nProtocolPrefix(PROTOCOL_PREFIX), m_nProtocolVersion(PROTOCOL_VERSION_1)
, m_nCommandVersion(COMMAND_VERSION_1), m_nClientID(0), m_nCmd(0)
{
}

bool RequestHeader::isValid(void) const
{
	return ( m_nProtocolPrefix == PROTOCOL_PREFIX && m_nProtocolVersion == PROTOCOL_VERSION_1  && m_nCommandVersion >= COMMAND_VERSION_1 );
}

bool RequestHeader::saveToStream(BinaryStream & binBuffer) const
{
	return ( SocketStreamEncoder::smartWrite(m_nProtocolVersion, binBuffer)
		&& SocketStreamEncoder::smartWrite(m_nCommandVersion, binBuffer)
		&& SocketStreamEncoder::smartWrite(m_nClientID, binBuffer)
		&& SocketStreamEncoder::smartWrite(m_nCmd, binBuffer) );
}

bool RequestHeader::loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd)
{
	return ( SocketStreamEncoder::smartRead(pBegin, pEnd, m_nProtocolVersion)
		&& SocketStreamEncoder::smartRead(pBegin, pEnd, m_nCommandVersion)
		&& SocketStreamEncoder::smartRead(pBegin, pEnd, m_nClientID)
		&& SocketStreamEncoder::smartRead(pBegin, pEnd, m_nCmd) );
}

bool RespondHeader::saveToStream(BinaryStream & binBuffer) const
{
	return ( SocketStreamEncoder::smartWrite(m_nRetCode, binBuffer)
		&& SocketStreamEncoder::smartWrite(m_strResultMessage, binBuffer) );
}

bool RespondHeader::loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd)
{
	return ( SocketStreamEncoder::smartRead(pBegin, pEnd, m_nRetCode)
		&& SocketStreamEncoder::smartRead(pBegin, pEnd, m_strResultMessage) );
}

//////////////////////////////////////////////////////////////////////////

BinarySocketClient::BinarySocketClient(ReConnectTcpSocket * pTcpConnector) : typeSocketClient(INVALID_SOCKET), m_pTcpConnector(pTcpConnector)
{
	if ( m_pTcpConnector != NULL )
		setSocket( m_pTcpConnector->getHandle() );
}

SocketClientRetCode BinarySocketClient::onSend(const BinaryStream & varRequest)
{
	if ( writeWithSize(varRequest) != varRequest.size() )
		return SOCKET_CLIENT_RET_SEND_NET_ERROR;
	return SOCKET_CLIENT_RET_OK;
}

SocketClientRetCode BinarySocketClient::onRecv(BinaryStream & varRespond)
{
	if ( readWithSize(varRespond) != varRespond.size() )
		return SOCKET_CLIENT_RET_RECV_NET_ERROR;
	return SOCKET_CLIENT_RET_OK;
}

bool BinarySocketClient::onReConnect(void)
{
	setSocket(INVALID_SOCKET);
	if ( m_pTcpConnector != NULL )
		setSocket( m_pTcpConnector->onReConnect() );
	return ( getSocket() != INVALID_SOCKET );
}

void BinarySocketClient::disconnect(void)
{
	setSocket(INVALID_SOCKET);
	if ( m_pTcpConnector != NULL )
		m_pTcpConnector->shutdownAndClose();
}

void BinarySocketClient::setTcpConnector(ReConnectTcpSocket * pTcpConnector)
{
	setSocket(INVALID_SOCKET);
	m_pTcpConnector = pTcpConnector;
	if ( m_pTcpConnector != NULL )
		setSocket( m_pTcpConnector->getHandle() );
}

bool BinarySocketClient::varifyHeartBeat(void)
{
	RespondBody varRetBody;
	return varifyHeartBeat(varRetBody);
}

bool BinarySocketClient::varifyHeartBeat(RespondBody & varRetBody)
{
	return ( onExchange(RequestHeader::CMD_VARIFY_HEART_BEAT, RequestBody(), varRetBody) == SOCKET_CLIENT_RET_OK );
}

int BinarySocketClient::onExchange(int nCmd, const RequestBody & varReqBody, RespondBody & varRetBody)
{
	RequestHeader varReqHeader;
	varReqHeader.m_nCmd = nCmd;
	RespondHeader varRetHeader;
	return onExchange(varReqHeader, varReqBody, varRetHeader, varRetBody);
}

int BinarySocketClient::onExchange(const RequestHeader & varReqHeader, const RequestBody & varReqBody, RespondBody & varRetBody)
{
	RespondHeader varRetHeader;
	return onExchange(varReqHeader, varReqBody, varRetHeader, varRetBody);
}

int BinarySocketClient::onExchange(const RequestHeader & varReqHeader, const RequestBody & varReqBody,
								   RespondHeader & varRetHeader, RespondBody & varRetBody)
{
	BinaryStream binReq, binRet;
	if ( !varReqHeader.saveToStream(binReq) )
		return SOCKET_CLIENT_RET_SEND_FORMAT_ERROR;
	if ( !varReqBody.saveToStream(binReq) )
		return SOCKET_CLIENT_RET_SEND_FORMAT_ERROR;

	varRetHeader.m_nRetCode = typeSocketClient::onExchange(binReq, binRet);
	if ( varRetHeader.m_nRetCode != static_cast<int>(SOCKET_CLIENT_RET_OK) )
		return varRetHeader.m_nRetCode;

	const rfc_uint_8 * pRetData = binRet.data();
	const rfc_uint_8 * pRetDataEnd = pRetData + binRet.size();
	if ( !varRetHeader.loadFromBinary(pRetData, pRetDataEnd) )
		return SOCKET_CLIENT_RET_RECV_FORMAT_ERROR;
	if ( !varRetBody.loadFromBinary(pRetData, pRetDataEnd) )
		return SOCKET_CLIENT_RET_RECV_FORMAT_ERROR;
	return varRetHeader.m_nRetCode;
}

RFC_NAMESPACE_END
