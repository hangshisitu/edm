/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#ifndef APPLIB_BINARYSOCKETCLIENT_H_201110
#define APPLIB_BINARYSOCKETCLIENT_H_201110

#include "socketclient/socketclient.h"
#include "network/tcpsocket.h"

RFC_NAMESPACE_BEGIN

class RequestHeader
{
public:
	enum
	{
		PROTOCOL_PREFIX			= 0xFFFE,
		PROTOCOL_VERSION_1		= 1,
		COMMAND_VERSION_1		= 1,

		CMD_VARIFY_HEART_BEAT	= 0xFFFF,
	};

	RequestHeader(void);
	virtual ~RequestHeader(void) {}

	bool					isValid(void) const;
	virtual bool			saveToStream(BinaryStream & binBuffer) const;
	virtual bool			loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd);

	rfc_uint_16				m_nProtocolPrefix;			// 传输二进制流的前缀标志
	rfc_uint_16				m_nProtocolVersion;			// 针对整个网络协议的版本
	rfc_uint_16				m_nCommandVersion;			// 针对相应命令号的版本
	rfc_uint_16				m_nClientID;				// 客户端对应的id,告知服务端该连接是哪个应用程序发起的
	rfc_uint_32				m_nCmd;
};

class RequestBody
{
public:
	virtual ~RequestBody(void) {}
	virtual bool			saveToStream(BinaryStream & binBuffer) const { return true; }
	virtual bool			loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd) { return true; }
};

class RespondHeader
{
public:
	RespondHeader(void) : m_nRetCode(-1) {}
	virtual ~RespondHeader(void) {}

	virtual bool			saveToStream(BinaryStream & binBuffer) const;
	virtual bool			loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd);

	int						m_nRetCode;
	stdstring				m_strResultMessage;
};

class RespondBody
{
public:
	virtual ~RespondBody(void) {}
	virtual bool			saveToStream(BinaryStream & binBuffer) const { return true; }
	virtual bool			loadFromBinary(const rfc_uint_8 * & pBegin, const rfc_uint_8 * pEnd) { return true; }
};

//////////////////////////////////////////////////////////////////////////

class BinarySocketClient : public SocketClient<BinaryStream, BinaryStream>
{
public:
	typedef SocketClient<BinaryStream, BinaryStream>		typeSocketClient;
	BinarySocketClient(void) : typeSocketClient(INVALID_SOCKET), m_pTcpConnector(NULL) {}
	BinarySocketClient(typeSocketHandle nSocketID) : typeSocketClient(nSocketID), m_pTcpConnector(NULL) {}
	BinarySocketClient(const SocketStream & s) : typeSocketClient(s), m_pTcpConnector(NULL) {}
	BinarySocketClient(ReConnectTcpSocket * pTcpConnector);

	SocketClientRetCode		onSend(const BinaryStream & varRequest);
	SocketClientRetCode		onRecv(BinaryStream & varRespond);

	bool					onReConnect(void);
	void					disconnect(void);
	void					setTcpConnector(ReConnectTcpSocket * pTcpConnector);

	template<typename typeStruct>
	static stdstring		toString(const typeStruct & aStruct)
	{
		BinaryStream binData;
		aStruct.saveToStream(binData);
		stdstring strHexData;
		StringFunc::addIntToString(strHexData, binData.size());
		strHexData += '[';
		GlobalFunc::binaryToHex(binData.data(), binData.size(), strHexData);
		strHexData += ']';
		return strHexData;
	}

	bool					varifyHeartBeat(void);
	bool					varifyHeartBeat(RespondBody & varRetBody);

protected:
	int		onExchange(int nCmd, const RequestBody & varReqBody, RespondBody & varRetBody);
	int		onExchange(const RequestHeader & varReqHeader, const RequestBody & varReqBody, RespondBody & varRetBody);
	int		onExchange(const RequestHeader & varReqHeader, const RequestBody & varReqBody, RespondHeader & varRetHeader, RespondBody & varRetBody);

	using SocketStream::setSocket;
	ReConnectTcpSocket * m_pTcpConnector;		// 设置后可支持重新连接
};

RFC_NAMESPACE_END

#endif	//APPLIB_BINARYSOCKETCLIENT_H_201110
