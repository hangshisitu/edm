/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SOCKETSTREAM_H_201008
#define RFC_SOCKETSTREAM_H_201008

#include "stream/binarystream.h"
#include "base/datetime.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(SocketStreamException, StreamException)

class SocketStream : public StreamBase
{
public:
	SocketStream(typeSocketHandle nSocketID = INVALID_SOCKET);
	SocketStream(const SocketStream & s);
	virtual ~SocketStream(void) {}

	typeSocketHandle	getSocket(void) const { return m_nSocketID; }
	void				setSocket(typeSocketHandle nSocketID) { m_nSocketID = nSocketID; }

	const TimeValue &	getTimeout(void) const { return m_tvTimeout; }
	void				setTimeout(const TimeValue & tvTimeout) { m_tvTimeout = tvTimeout; }

	size_t				getMaxTransferSize(void) const { return m_nMaxTransferSize; }
	void				setMaxTransferSize(size_t nSize) { m_nMaxTransferSize = nSize; }

	// 不检查超时的读写
	size_t				readWithoutTimed(void * pBuffer, size_t nSize);
	size_t				writeWithoutTimed(const void * pBuffer, size_t nSize);

	virtual size_t		read(void * pBuffer, size_t nSize);
	virtual size_t		write(const void * pBuffer, size_t nSize);

	// @bOnlyBodySize: true表示指定长度只包括包体长度;false表示指定长度包括包头长度+包体长度,即4+包体长度
	size_t				readWithSize(BinaryStream & binData, bool bOnlyBodySize);
	size_t				readWithSize(BinaryStream & binData) { return readWithSize(binData, true); }
	size_t				writeWithSize(const void * pBuffer, size_t nSize, bool bOnlyBodySize);
	size_t				writeWithSize(const void * pBuffer, size_t nSize)  { return writeWithSize(pBuffer, nSize, true); }
	size_t				writeWithSize(const BinaryStream & binData) { return writeWithSize(binData.data(), binData.size(), true); }
	size_t				writeWithSize(const BinaryStream & binData, bool bOnlyBodySize)
	{ return			writeWithSize(binData.data(), binData.size(), bOnlyBodySize); }

private:
	typeSocketHandle	m_nSocketID;
	TimeValue			m_tvTimeout;
	size_t				m_nMaxTransferSize;
};

RFC_NAMESPACE_END

#endif	//RFC_SOCKETSTREAM_H_201008
